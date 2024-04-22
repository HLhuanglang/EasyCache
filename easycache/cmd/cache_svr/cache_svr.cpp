#include "ini.h"
#include "log.h"
#include <iostream>

int main() {
    EasyCache::LogInit(spdlog::level::level_enum::trace);

    if (!EasyCache::INI_INIT("test.ini")) {
        std::cout << "init failed\n";
        return 1;
    }
    auto ret = EasyCache::INI_GET("SVR", "name");
    if (ret.Has())
        std::cout << "hello " << ret.GetValue() << std::endl;
    else
        std::cout << "not found\n";
}