#pragma once

namespace EasyCache {
// 淘汰策略接口规范
template <typename Key>
class CachePolicy {
 public:
    virtual ~CachePolicy() = default;
    virtual void Insert(const Key &key) = 0;  // 插入
    virtual void Erase(const Key &key) = 0;   // 删除
    virtual bool Hit(const Key &key) = 0;     // 是否命中缓存 (lru为例,则会调整元素在缓存中的位置)
    virtual void Adjust(const Key &key) = 0;  // 调整元素位置以此跟新节点的生命周期
    virtual Key &GetUpdateElement() = 0;      // 需要更新的元素节点
};
}  // namespace EasyCache