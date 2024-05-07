#pragma once

#include "easycache/nocopyable.h"

namespace EasyCache {
class ConfigImpl;

class Config : public Noncopyable {
 public:
    ~Config();

 public:
    std::string GetLogLevel();

 public:
    /// @brief 从INI配置文件创建配置对象
    ///
    /// @param config_file INI格式配置文件路径
    /// @param err_msg 初始化失败时通过该参数回传错误信息
    /// @return ConfigPtr 创建的配置对象，创建失败时返回NULL
    static Config *CreateFromFile(const std::string &config_file, std::string &err_msg);

 private:
    explicit Config(ConfigImpl *impl);

 private:
    ConfigImpl *m_config;
};
}  // namespace EasyCache