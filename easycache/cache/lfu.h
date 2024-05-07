#pragma once

#include "policy.h"

namespace EasyCache {

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
}  // namespace EasyCache