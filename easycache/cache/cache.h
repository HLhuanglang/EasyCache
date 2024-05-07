#pragma once

#include <map>
#include <mutex>
#include <utility>

#include "fifo.h"
#include "lfu.h"
#include "lru.h"

namespace EasyCache {

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
    mutable Policy<Key> m_policy;  // 淘汰策略（编译时多态）
    size_t m_max_cache_size;       // 缓存最大数量
    std::map<Key, Val> m_cache;    // 实际缓存数据(不用考虑排序)
    std::mutex m_mtx;
};
}  // namespace EasyCache
