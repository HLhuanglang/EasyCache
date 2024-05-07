#pragma once

#include <list>
#include <map>

#include "policy.h"

namespace EasyCache {

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
    std::list<Key> m_fifo_index;                                           // fifo进行操作的表
    std::map<Key, typename std::list<Key>::const_iterator> m_fifo_lookup;  // 快速定位
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
}  // namespace EasyCache