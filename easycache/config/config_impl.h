#pragma once

#include <string>

#include "ini.h"

namespace EasyCache {

class ConfigImpl {
    friend class Config;

 public:
    ConfigImpl(const std::string path);
    ~ConfigImpl();

 private:
    INIFile m_ini;
};

}  // namespace EasyCache