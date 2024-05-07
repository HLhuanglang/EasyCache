#include "easycache/config.h"
#include "easycache/easycache.h"

int main() {
    std::string err_msg;
    auto cfg = EasyCache::Config::CreateFromFile("config.ini", err_msg);
    EasyCache::Init(cfg);

    return 0;
}