#pragma once

#include <sys/types.h>

#include <cstddef>
#include <list>
#include <map>
#include <mutex>
#include <utility>

// 淘汰策略接口规范
template <typename Key>
class CachePolicy {
 public:
    virtual ~CachePolicy() = default;
    virtual void Insert(const Key &key) = 0; // 插入
    virtual void Erase(const Key &key) = 0;  // 删除
    virtual bool Hit(const Key &key) = 0;    // 是否命中缓存 (lru为例,则会调整元素在缓存中的位置)
    virtual void Adjust(const Key &key) = 0; // 调整元素位置以此跟新节点的生命周期
    virtual Key &GetUpdateElement() = 0;     // 需要更新的元素节点
};

//-----------------------------------------------------------------------------------
// fifo策略（默认策略）
//-----------------------------------------------------------------------------------
template <typename Key>
class FIFOPolicy : public CachePolicy<Key> {
 public:
    FIFOPolicy() = default;
    ~FIFOPolicy() = default;

    void Insert(const Key &key) override;
    void Erase(const Key &key) override;
    bool Hit(const Key &key) override;
    void Adjust(const Key &key) override;
    Key &GetUpdateElement() override;

 private:
    std::list<Key> m_fifo_index;                                          // fifo进行操作的表
    std::map<Key, typename std::list<Key>::const_iterator> m_fifo_lookup; // 快速定位
};

template <typename Key>
void FIFOPolicy<Key>::Insert(const Key &key) {
    m_fifo_index.push_front(key);
    m_fifo_lookup[key] = m_fifo_index.begin();
}

template <typename Key>
void FIFOPolicy<Key>::Erase(const Key &key) {
    auto tmp = m_fifo_lookup[key];
    m_fifo_lookup.erase(key);
    m_fifo_index.erase(tmp);
}

template <typename Key>
bool FIFOPolicy<Key>::Hit(const Key &key) {
    auto tmp = m_fifo_lookup.find(key);
    return static_cast<bool>(tmp != m_fifo_lookup.end());
}

template <typename Key>
void FIFOPolicy<Key>::Adjust(const Key &key) {
    // fifo 无需调整元素的生命周期
}

template <typename Key>
Key &FIFOPolicy<Key>::GetUpdateElement() {
    return m_fifo_index.back();
}

//-----------------------------------------------------------------------------------
// lru策略
//
//-----------------------------------------------------------------------------------
template <typename Key>
class LRUPolicy : public CachePolicy<Key> {
 public:
    LRUPolicy() = default;
    ~LRUPolicy() = default;

    void Insert(const Key &key) override;
    void Erase(const Key &key) override;
    bool Hit(const Key &key) override;
    void Adjust(const Key &key) override;
    Key &GetUpdateElement() override;

 private:
    std::list<Key> m_lru_index;
    std::map<Key, typename std::list<Key>::const_iterator> m_lru_lookup;
};

template <typename Key>
void LRUPolicy<Key>::Insert(const Key &key) {
    m_lru_index.push_front(key);
    m_lru_lookup[key] = m_lru_index.begin();
}

template <typename Key>
void LRUPolicy<Key>::Erase(const Key &key) {
    auto tmp = m_lru_lookup[key];
    m_lru_index.erase(tmp);
    m_lru_lookup.erase(key);
}

template <typename Key>
bool LRUPolicy<Key>::Hit(const Key &key) {
    auto tmp = m_lru_lookup.find(key);
    return static_cast<bool>(tmp != m_lru_lookup.end());
}

template <typename Key>
void LRUPolicy<Key>::Adjust(const Key &key) {
    m_lru_index.splice(m_lru_index.begin(), m_lru_index, m_lru_lookup[key]);
}

template <typename Key>
Key &LRUPolicy<Key>::GetUpdateElement() {
    return m_lru_index.back();
}

//-----------------------------------------------------------------------------------
// lfu策略
//-----------------------------------------------------------------------------------
template <typename Key>
class LFUPolicy : public CachePolicy<Key> {
 public:
    LFUPolicy() = default;
    ~LFUPolicy() = default;

    void Insert(const Key &key) override;
    void Erase(const Key &key) override;
    bool Hit(const Key &key) override;
    void Adjust(const Key &key) override;
    Key &GetUpdateElement() override;
};

//-----------------------------------------------------------------------------------
// 本地缓存组件，提供多种淘汰策略可选，唯一key
//-----------------------------------------------------------------------------------
template <typename Key, typename Val,
          template <typename> class Policy = FIFOPolicy>
class CacheMgr {
 public:
    class result {
     public:
        result(Val k, bool s) : m_result(k), m_has(s) {}
        bool Has() { return m_has; }
        Val GetValue() { return m_result; }

     private:
        Val m_result;
        bool m_has;
    };

 public:
    using iterator = typename std::map<Key, Val>::iterator;
    using const_iterator = typename std::map<Key, Val>::const_iterator;

    explicit CacheMgr(size_t max_size, const Policy<Key> policy = Policy<Key>())
        : m_policy(policy), m_max_cache_size(max_size) {
    }

    // 将元素插入缓存中
    void Set(const Key &key, const Val &val) {
        std::lock_guard<std::mutex> lgmt(m_mtx);
        bool is_hit_cache = m_policy.Hit(key);
        if (is_hit_cache) {
            // 1,命中缓存,直接调整
            m_policy.Adjust(key);
        } else {
            // 2,未命中缓存
            if (m_cache.size() >= m_max_cache_size) {
                // 缓存队列满了,淘汰生命周期结束的节点
                auto old = m_policy.GetUpdateElement();
                m_policy.Erase(old);
                m_cache.erase(old);
                m_policy.Insert(key);
                m_cache.emplace(std::make_pair(key, val));
            } else {
                m_policy.Insert(key);
                m_cache.emplace(std::make_pair(key, val));
            }
        }
    }

    // 从缓存中取出某个元素,需要自行判断元素是否存在
    result TryGet(const Key &key) {
        std::lock_guard<std::mutex> lgmt(m_mtx);
        bool is_hit_cache = m_policy.Hit(key);
        if (is_hit_cache) {
            m_policy.Adjust(key);
            return {m_cache[key], true};
        }
        return {Val(), false};
    }

    // 从缓存中删除某个元素
    void Del(const Key &key) {
        std::lock_guard<std::mutex> lgmt(m_mtx);
        bool is_hit_cache = m_policy.Hit(key);
        if (is_hit_cache) {
            m_policy.Erase(key);
            m_cache.erase(key);
        }
    }

 private:
    mutable Policy<Key> m_policy; // 淘汰策略（编译时多态）
    size_t m_max_cache_size;      // 缓存最大数量
    std::map<Key, Val> m_cache;   // 实际缓存数据(不用考虑排序)
    std::mutex m_mtx;
};
