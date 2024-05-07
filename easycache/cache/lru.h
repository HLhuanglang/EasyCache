#pragma once

#include <list>
#include <map>

#include "policy.h"

namespace EasyCache {

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
}  // namespace EasyCache