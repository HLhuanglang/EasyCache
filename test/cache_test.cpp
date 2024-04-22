#include "cache.h"

#include <gtest/gtest.h>
#include <random>
#include <string>

const constexpr int LV1_TEST = 10000;
const constexpr double EXPECT_RATE = 99.9 / 100;

TEST(CachePolicyTest, LRUTest) {
    CacheMgr<std::string, int, LRUPolicy> lru_cache(LV1_TEST);
    for (int i = 1; i <= LV1_TEST; i++) {
        lru_cache.Set("Holo" + std::to_string(i), i);
    }

    static std::random_device rd;                                // 非确定性随机数生成设备
    static std::mt19937 gen(rd());                               // 伪随机数产生器，用于产生高性能的随机数
    static std::uniform_int_distribution<int> dist(0, LV1_TEST); // 随机数分布

    int miss = 0;
    int hit = 0;
    for (int i = 0; i < LV1_TEST * 10; i++) {
        int rand = dist(gen);
        std::string key = "Holo" + std::to_string(rand);
        auto ret = lru_cache.TryGet(key);
        if (ret.Has()) {
            // auto val = ret.GetValue();
            hit++;
        } else {
            miss++;
            lru_cache.Set(key, rand);
        }
    }
    auto hit_rate = static_cast<double>(hit) / (hit + miss);
    EXPECT_GT(hit_rate, EXPECT_RATE);
}

TEST(CachePolicyTest, FIFOTest) {
    CacheMgr<std::string, int, FIFOPolicy> fifo_cache(LV1_TEST);
    for (int i = 1; i <= LV1_TEST; i++) {
        fifo_cache.Set("Holo" + std::to_string(i), i);
    }

    static std::random_device rd;                                // 非确定性随机数生成设备
    static std::mt19937 gen(rd());                               // 伪随机数产生器，用于产生高性能的随机数
    static std::uniform_int_distribution<int> dist(0, LV1_TEST); // 随机数分布

    int miss = 0;
    int hit = 0;
    for (int i = 0; i < LV1_TEST * 10; i++) {
        int rand = dist(gen);
        std::string key = "Holo" + std::to_string(rand);
        auto ret = fifo_cache.TryGet(key);
        if (ret.Has()) {
            // auto val = ret.GetValue();
            hit++;
        } else {
            miss++;
            fifo_cache.Set(key, rand);
        }
    }
    auto hit_rate = static_cast<double>(hit) / (hit + miss);
    EXPECT_GT(hit_rate, EXPECT_RATE);
}