#include "config_impl.h"

#include <cstdlib>

#include "easycache/config.h"
#include "log/log.h"

namespace EasyCache {
ConfigImpl::ConfigImpl(const std::string path) {
    if (!m_ini.Init(path)) {
        LOG_ERROR("Init config file failed");
        exit(EXIT_FAILURE);
    }
}

ConfigImpl::~ConfigImpl() {}

std::string Config::GetLogLevel() {
    auto ret = m_config->m_ini.TryGet("log", "level");
    if (ret.Has()) {
        return ret.GetValue();
    }
    return "info";
}

}  // namespace EasyCache