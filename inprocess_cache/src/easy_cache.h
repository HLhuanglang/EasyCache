#ifndef EASY_CACHE_H_
#define EASY_CACHE_H_

#include <sys/types.h>

#include <cstddef>
#include <list>
#include <map>
#include <mutex>
#include <utility>

// 本地缓存的淘汰策略接口规范
template <typename Key>
class cache_policy {
 public:
    virtual ~cache_policy() = default;
    virtual void insert(const Key &key) = 0;        // 插入
    virtual void erase(const Key &key) = 0;         // 删除
    virtual auto hit(const Key &key) -> bool = 0;   // 是否命中缓存 (lru为例,则会调整元素在缓存中的位置)
    virtual void adjust(const Key &key) = 0;        // 调整元素位置以此跟新节点的生命周期
    virtual auto get_update_element() -> Key & = 0; // 需要更新的元素节点
};

//-----------------------------------------------------------------------------------
// fifo策略（默认策略）
//-----------------------------------------------------------------------------------
template <typename Key>
class fifo_policy : public cache_policy<Key> {
 public:
    fifo_policy() = default;
    ~fifo_policy() = default;

    void insert(const Key &key) override;
    void erase(const Key &key) override;
    auto hit(const Key &key) -> bool override;
    void adjust(const Key &key) override;
    auto get_update_element() -> Key & override;

 private:
    std::list<Key> m_fifo_index;                                          // fifo进行操作的表
    std::map<Key, typename std::list<Key>::const_iterator> m_fifo_lookup; // 快速定位
};

template <typename Key>
void fifo_policy<Key>::insert(const Key &key) {
    m_fifo_index.push_front(key);
    m_fifo_lookup[key] = m_fifo_index.begin();
}

template <typename Key>
void fifo_policy<Key>::erase(const Key &key) {
    auto tmp = m_fifo_lookup[key];
    m_fifo_lookup.erase(key);
    m_fifo_index.erase(tmp);
}

template <typename Key>
auto fifo_policy<Key>::hit(const Key &key) -> bool {
    auto tmp = m_fifo_lookup.find(key);
    return static_cast<bool>(tmp != m_fifo_lookup.end());
}

template <typename Key>
void fifo_policy<Key>::adjust(const Key &key) {
    // fifo 无需调整元素的生命周期
}

template <typename Key>
auto fifo_policy<Key>::get_update_element() -> Key & {
    return m_fifo_index.back();
}

//-----------------------------------------------------------------------------------
// lru策略
//
//-----------------------------------------------------------------------------------
template <typename Key>
class lru_policy : public cache_policy<Key> {
 public:
    lru_policy() = default;
    ~lru_policy() = default;

    void insert(const Key &key) override;
    void erase(const Key &key) override;
    auto hit(const Key &key) -> bool override;
    void adjust(const Key &key) override;
    auto get_update_element() -> Key & override;

 private:
    std::list<Key> m_lru_index;
    std::map<Key, typename std::list<Key>::const_iterator> m_lru_lookup;
};

template <typename Key>
void lru_policy<Key>::insert(const Key &key) {
    m_lru_index.push_front(key);
    m_lru_lookup[key] = m_lru_index.begin();
}

template <typename Key>
void lru_policy<Key>::erase(const Key &key) {
    auto tmp = m_lru_lookup[key];
    m_lru_index.erase(tmp);
    m_lru_lookup.erase(key);
}

template <typename Key>
auto lru_policy<Key>::hit(const Key &key) -> bool {
    auto tmp = m_lru_lookup.find(key);
    return static_cast<bool>(tmp != m_lru_lookup.end());
}

template <typename Key>
void lru_policy<Key>::adjust(const Key &key) {
    m_lru_index.splice(m_lru_index.begin(), m_lru_index, m_lru_lookup[key]);
}

template <typename Key>
auto lru_policy<Key>::get_update_element() -> Key & {
    return m_lru_index.back();
}

//-----------------------------------------------------------------------------------
// lfu策略
//-----------------------------------------------------------------------------------
template <typename Key>
class lfu_policy : public cache_policy<Key> {
 public:
    lfu_policy() = default;
    ~lfu_policy() = default;
    // lfu_policy(const lfu_policy &) = default;
    // auto operator=(const lfu_policy &) -> lfu_policy & = default;
    // auto operator=(lfu_policy &&) noexcept -> lfu_policy & = default;
    // lfu_policy(lfu_policy &&) noexcept = default;

    void insert(const Key &key) override;
    void erase(const Key &key) override;
    auto hit(const Key &key) -> bool override;
    void adjust(const Key &key) override;
    auto get_update_element() -> Key & override;
};

//-----------------------------------------------------------------------------------
// 本地缓存组件，提供多种淘汰策略可选，唯一key
//-----------------------------------------------------------------------------------
template <typename Key, typename Val,
          template <typename> class Policy = fifo_policy>
class easy_cache {
 public:
    using iterator = typename std::map<Key, Val>::iterator;
    using const_iterator = typename std::map<Key, Val>::const_iterator;

    explicit easy_cache(size_t max_size, const Policy<Key> policy = Policy<Key>())
        : m_policy(policy), m_max_cache_size(max_size) {
    }

    // 将元素插入缓存中
    void set(const Key &key, const Val &val) {
        std::lock_guard<std::mutex> lgmt(m_mtx);
        bool is_hit_cache = m_policy.hit(key);
        if (is_hit_cache) {
            // 1,命中缓存,直接调整
            m_policy.adjust(key);
        } else {
            // 2,未命中缓存
            if (m_cache.size() >= m_max_cache_size) {
                // 缓存队列满了,淘汰生命周期结束的节点
                auto old = m_policy.get_update_element();
                m_policy.erase(old);
                m_cache.erase(old);
                m_policy.insert(key);
                m_cache.emplace(std::make_pair(key, val));
            } else {
                m_policy.insert(key);
                m_cache.emplace(std::make_pair(key, val));
            }
        }
    }

    // 从缓存中取出某个元素,需要自行判断元素是否存在
    auto try_get(const Key &key) -> std::pair<Val, bool> {
        std::lock_guard<std::mutex> lgmt(m_mtx);
        bool is_hit_cache = m_policy.hit(key);
        if (is_hit_cache) {
            return {m_cache[key], true};
        }
        return {Val(), false};
    }

    // 从缓存中删除某个元素
    void del(const Key &key) {
        std::lock_guard<std::mutex> lgmt(m_mtx);
        bool is_hit_cache = m_policy.hit(key);
        if (is_hit_cache) {
            m_policy.erase(key);
            m_cache.erase(key);
        }
    }

 private:
    mutable Policy<Key> m_policy; // 淘汰策略（编译时多态）
    size_t m_max_cache_size;      // 缓存最大数量
    std::map<Key, Val> m_cache;   // 实际缓存数据(不用考虑排序)
    std::mutex m_mtx;
};

#endif // EASY_CACHE_H_
