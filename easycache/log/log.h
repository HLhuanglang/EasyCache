#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

namespace EasyCache {
#ifdef _WIN32
#    define DIR_SEPARATOR '\\'
#    define DIR_SEPARATOR_STR "\\"
#else
#    define DIR_SEPARATOR '/'
#    define DIR_SEPARATOR_STR "/"
#endif

#ifndef __FILENAME__
// #define __FILENAME__  (strrchr(__FILE__, DIR_SEPARATOR) ? strrchr(__FILE__, DIR_SEPARATOR) + 1 : __FILE__)
#    define __FILENAME__ (strrchr(DIR_SEPARATOR_STR __FILE__, DIR_SEPARATOR) + 1)
#endif

#define LOG_TRACE(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::trace, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::warn, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) spdlog::log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::err, fmt, ##__VA_ARGS__)

inline void LogInit(spdlog::level::level_enum lv) {
    spdlog::set_level(lv);
    spdlog::set_pattern("[%D %H:%M:%S.%e][%L][%t][%s:%# %!] %^%v%$");
}

inline spdlog::level::level_enum GetLogLevelByString(const std::string &level) {
    if (level == "trace") {
        return spdlog::level::trace;
    } else if (level == "debug") {
        return spdlog::level::debug;
    } else if (level == "info") {
        return spdlog::level::info;
    } else if (level == "warn") {
        return spdlog::level::warn;
    } else if (level == "error") {
        return spdlog::level::err;
    } else {
        return spdlog::level::info;
    }
}

}  // namespace EasyCache
