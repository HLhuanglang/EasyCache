#include "easycache/config.h"

#include "config_impl.h"

namespace EasyCache {

Config::Config(ConfigImpl *impl) : m_config(impl) {}

Config::~Config() {
    if (!m_config) {
        delete m_config;
        m_config = nullptr;
    }
}

/// @brief 从INI配置文件创建配置对象
Config *Config::CreateFromFile(const std::string &config_file, std::string &err_msg) {
    auto impl = new ConfigImpl(config_file);
    return new Config(impl);
}

}  // namespace EasyCache