#pragma once

#include <memory>

namespace EasyCache {
/// @brief 用于屏蔽拷贝构造函数和赋值函数
class Noncopyable {
 protected:
    Noncopyable() {}
    ~Noncopyable() {}

 private:
    Noncopyable(const Noncopyable &);
    const Noncopyable &operator=(const Noncopyable &);
};

/// @brief 创建unique_ptr
/// @note cpp11只有make_shared，缺少make_unique cpp14才有
template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace EasyCache