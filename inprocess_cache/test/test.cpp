#include <iostream>
#include <random>
#include <string>

#include "easy_cache.h"

constexpr int k_l1_test = 1000000;

int main() {
    easy_cache<std::string, int, lru_policy> lru_cache(k_l1_test);
    for (int i = 1; i <= k_l1_test; i++) {
        lru_cache.set("Holo" + std::to_string(i), i);
    }

    static std::random_device rd;  // 非确定性随机数生成设备
    static std::mt19937 gen(rd()); // 伪随机数产生器，用于产生高性能的随机数
    static std::uniform_int_distribution<int> dist(0, k_l1_test);

    int miss = 0;
    int hit = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000000; i++) {
        int rand = dist(gen);
        auto ret = lru_cache.try_get("Holo" + std::to_string(rand));
        if (ret.second) {
            hit++;
        } else {
            miss++;
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "cost time: " << duration.count() << " us" << std::endl;
    std::cout << "hit  " << hit << std::endl;
    std::cout << "miss " << miss << std::endl;
    return 0;
}
