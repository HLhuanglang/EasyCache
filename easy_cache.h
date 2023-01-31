#ifndef __EASY_CACHE_H
#define __EASY_CACHE_H

#include <cstddef>
#include <list>
#include <map>
#include <mutex>
#include <sys/types.h>
#include <utility>

//本地缓存的淘汰策略接口规范
template <typename Key>
class cache_policy {
public:
    virtual ~cache_policy() = default;
    virtual void insert(const Key &key) = 0; //插入
    virtual void erase(const Key &key) = 0;  //删除
    virtual bool hit(const Key &key) = 0;    //是否命中缓存 (lru为例,则会调整元素在缓存中的位置)
    virtual void adjust(const Key &key) = 0; //调整元素位置以此跟新节点的生命周期
    virtual Key &get_update_element() = 0;   //需要更新的元素节点
};

//-----------------------------------------------------------------------------------
//fifo策略（默认策略）
//-----------------------------------------------------------------------------------
template <typename Key>
class fifo_policy : public cache_policy<Key> {
public:
    fifo_policy() = default;
    ~fifo_policy() = default;

public:
    virtual void insert(const Key &key) override;
    virtual void erase(const Key &key) override;
    virtual bool hit(const Key &key) override;
    virtual void adjust(const Key &key) override;
    virtual Key &get_update_element() override;

private:
    std::list<Key> fifo_index;                                          //fifo进行操作的表
    std::map<Key, typename std::list<Key>::const_iterator> fifo_lookup; //快速定位
};

template <typename Key>
void fifo_policy<Key>::insert(const Key &key) {
    fifo_index.push_front(key);
    fifo_lookup[key] = fifo_index.begin();
}

template <typename Key>
void fifo_policy<Key>::erase(const Key &key) {
    auto tmp = fifo_lookup[key];
    fifo_lookup.erase(key);
    fifo_index.erase(tmp);
}

template <typename Key>
bool fifo_policy<Key>::hit(const Key &key) {
    auto tmp = fifo_lookup.find(key);
    if (tmp != fifo_lookup.end()) {
        return true;
    }
    return false;
}

template <typename Key>
void fifo_policy<Key>::adjust(const Key &key) {
    //fifo 无需调整元素的生命周期
}

template <typename Key>
Key &fifo_policy<Key>::get_update_element() {
    return fifo_index.back();
}

//-----------------------------------------------------------------------------------
//lru策略
//
//-----------------------------------------------------------------------------------
template <typename Key>
class lru_policy : public cache_policy<Key> {
public:
    lru_policy() = default;
    ~lru_policy() = default;

public:
    virtual void insert(const Key &key) override;
    virtual void erase(const Key &key) override;
    virtual bool hit(const Key &key) override;
    virtual void adjust(const Key &key) override;
    virtual Key &get_update_element() override;

private:
    std::list<Key> lru_index;
    std::map<Key, typename std::list<Key>::const_iterator> lru_lookup;
};

template <typename Key>
void lru_policy<Key>::insert(const Key &key) {
    lru_index.push_front(key);
    lru_lookup[key] = lru_index.begin();
}

template <typename Key>
void lru_policy<Key>::erase(const Key &key) {
    auto tmp = lru_lookup[key];
    lru_index.erase(tmp);
    lru_lookup.erase(key);
}

template <typename Key>
bool lru_policy<Key>::hit(const Key &key) {
    auto tmp = lru_index.find(key);
    if (tmp != lru_lookup.end()) {
        return true;
    }
    return false;
}

template <typename Key>
void lru_policy<Key>::adjust(const Key &key) {
    lru_index.splice(lru_index.begin(), lru_index, lru_lookup[key]);
}

template <typename Key>
Key &lru_policy<Key>::get_update_element() {
    return lru_index.back();
}

//-----------------------------------------------------------------------------------
//lfu策略
//-----------------------------------------------------------------------------------
template <typename Key>
class lfu_policy : public cache_policy<Key> {
public:
    lfu_policy() = default;
    ~lfu_policy() = default;

public:
    virtual void insert(const Key &key) override;
    virtual void erase(const Key &key) override;
    virtual bool hit(const Key &key) override;
    virtual void adjust(const Key &key) override;
    virtual Key &get_update_element() override;
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

public:
    explicit easy_cache(size_t max_size, const Policy<Key> policy = Policy<Key>()) : policy_(policy), max_cache_size_(max_size) {
    }

public:
    //将元素插入缓存中
    void set(const Key &key, const Val &val) {
        std::lock_guard<std::mutex> lgmt(mtx_);
        bool is_hit_cache = policy_.hit(key);
        if (is_hit_cache) {
            //1,命中缓存,直接调整
            policy_.adjust(key);
        } else {
            //2,未命中缓存
            if (cache_.size() >= max_cache_size_) {
                //缓存队列满了,淘汰生命周期结束的节点
                auto old = policy_.get_update_element();
                policy_.erase(old);
                cache_.erase(old);
                policy_.insert(key);
                cache_.emplace(std::make_pair(key, val));
            } else {
                policy_.insert(key);
                cache_.emplace(std::make_pair(key, val));
            }
        }
    }

    //从缓存中取出某个元素,需要自行判断元素是否存在
    std::pair<Val, bool> try_get(const Key &key) {
        std::lock_guard<std::mutex> lgmt(mtx_);
        bool is_hit_cache = policy_.hit(key);
        if (is_hit_cache) {
            return {cache_[key], true};
        }
        return {Val(), false};
    }

    //从缓存中删除某个元素
    void del(const Key &key) {
        std::lock_guard<std::mutex> lgmt(mtx_);
        bool is_hit_cache = policy_.hit(key);
        if (is_hit_cache) {
            policy_.erase(key);
            cache_.erase(key);
        }
    }

private:
    mutable Policy<Key> policy_; //淘汰策略（编译时多态）
    size_t max_cache_size_;      //缓存最大数量
    std::map<Key, Val> cache_;   //实际缓存数据(不用考虑排序)
    std::mutex mtx_;
};

#endif