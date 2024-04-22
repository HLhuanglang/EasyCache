#pragma once

#include "singleton.h"

#include <fstream>
#include <map>
#include <string>

namespace EasyCache {

const char *const k_whitespace_delimiters = " \t\n\r\f\v";
const char *const k_crlf = "\n";

// 配合ini_datatype_e，对每一行数据进行解析
// 可以存储的是section名，也可以是k-v数据
// 当数据类型是 DATA_SECTION时，section=first
// 当数据类型是 DATA_KEY_VAL时，key=first，val=second
using parse_data_t = std::pair<std::string, std::string>;

using kv_t = std::map<std::string, std::string>;
using ini_containder_t = std::map<std::string, kv_t>; // section-kv

enum class ini_datatype_e {
    DATA_NONE,    // 空行
    DATA_COMMENT, // 评论,使用;或者#开头
    DATA_SECTION, // 段
    DATA_KEY_VAL, // key-value
    DATA_UNKNOWN  // 未知
};

// 存储ini数据
class INIFile {
 public:
    INIFile() = default;
    ~INIFile() {
        if (m_file.is_open()) {
            m_file.close();
        }
    };

 public:
    class result {
     public:
        result(std::string k, bool s) : m_result(k), m_has(s) {}
        bool Has() { return m_has; }
        std::string GetValue() { return m_result; }

     private:
        std::string m_result;
        bool m_has;
    };

 public:
    bool Init(const std::string &path);
    result Get(const char *section, const char *key);

 private:
    ini_datatype_e _parse_line(std::string line, parse_data_t &data);

 private:
    ini_containder_t m_data;
    std::string m_file_name;
    std::ifstream m_file;
};

#define INI_INIT(path) Singleton<EasyCache::INIFile>::GetInstance()->Init(path)
#define INI_GET(section, key) Singleton<EasyCache::INIFile>::GetInstance()->Get(section, key)

// c++17 support inline variable
// inline auto INI_INIT = [](const std::string &path) {
//     return Singleton<EasyCache::INIFile>::GetInstance()->Init(path);
// };
// inline auto INI_GET = [](const char *section, const char *key) {
//     return Singleton<EasyCache::INIFile>::GetInstance()->Get(section, key);
// };

} // namespace EasyCache
