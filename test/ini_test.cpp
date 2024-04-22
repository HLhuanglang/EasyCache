#include "ini.h"
#include "log.h"

#include <gtest/gtest.h>

TEST(INITest, BasicTest) {
    EasyCache::LogInit(spdlog::level::level_enum::trace);

    EXPECT_TRUE(EasyCache::INI_INIT("test.ini"));

    auto result1 = EasyCache::INI_GET("Holo_wo", "name");
    EXPECT_TRUE(result1.Has());
    EXPECT_EQ(result1.GetValue(), "holo");

    auto result2 = EasyCache::INI_GET("Holo_wo", "age");
    EXPECT_TRUE(result2.Has());
    EXPECT_EQ(result2.GetValue(), "26");

    auto result3 = EasyCache::INI_GET("CLI", "name");
    EXPECT_TRUE(result3.Has());
    EXPECT_EQ(result3.GetValue(), "cache_cli");

    auto result4 = EasyCache::INI_GET("SVR", "name");
    EXPECT_TRUE(result4.Has());
    EXPECT_EQ(result4.GetValue(), "cache_svr");
}