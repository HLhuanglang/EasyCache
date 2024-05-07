#include "easycache/easycache.h"

#include <string>

#include "log/log.h"

namespace EasyCache {
#ifndef VERSION
#    define VERSION "0.0.1"
#endif  // !VERSION

#ifndef REVISION
#    define REVISION "NO_REVISION"
#endif  // !REVISION

const char *g_sdk_version = VERSION;
const char *g_sdk_version_info = "easycache version:" VERSION "-" REVISION "-[" __DATE__ "]-[" __TIME__ "]";

std::string GetVersion() {
    return g_sdk_version;
}

std::string GetVersionInfo() {
    return g_sdk_version_info;
}

void Init(Config *cfg) {
    // 1,初始化日志
    LogInit(GetLogLevelByString(cfg->GetLogLevel()));
}

}  // namespace EasyCache
