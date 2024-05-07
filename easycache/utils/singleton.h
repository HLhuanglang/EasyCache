#pragma once

namespace EasyCache {
template <typename T>
class Singleton {
 public:
    static T *GetInstance() {
        static T instance;
        return &instance;
    }

 public:
    Singleton() = delete;
    Singleton(const Singleton &rhs) = delete;
    Singleton &operator=(const Singleton &rhs) = delete;
};

}  // namespace EasyCache