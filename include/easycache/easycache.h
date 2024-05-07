#pragma once

#include <string>

#include "easycache/config.h"

namespace EasyCache {

///@brief 初始化
void Init(Config *cfg);

///@brief 获取版本号
std::string GetVersion();

///@brief 获取版本信息
std::string GetVersionInfo();

}  // namespace EasyCache