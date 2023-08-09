#include <iostream>
#include <string>

#include "easy_cache.h"

int main() {
    // fifo
    easy_cache<std::string, int> fifo_cache(10);
    for (int i = 1; i <= 20; i++) {
        fifo_cache.set("Holo" + std::to_string(i), i);
    }
    auto ret = fifo_cache.try_get("Holo");
    if (ret.second) {
        std::cout << "val=" << ret.first << std::endl;
    } else {
        std::cout << "Holo don`t hit cache" << std::endl;
    }

    // lru
    easy_cache<std::string, int, lru_policy> lru_cache(10);
    for (int i = 1; i <= 20; i++) {
        lru_cache.set("Holo" + std::to_string(i), i);
    }
    ret = lru_cache.try_get("Holo");
    if (ret.second) {
        std::cout << "val=" << ret.first << std::endl;
    } else {
        std::cout << "Holo don`t hit cache" << std::endl;
    }

    return 0;
}
