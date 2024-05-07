#include "ini.h"

#include <cstdio>
#include <cstring>

#include "log/log.h"

using namespace EasyCache;

ini_datatype_e INIFile::_parse_line(std::string line, parse_data_t &data) {
    line.erase(line.find_last_not_of(k_whitespace_delimiters) + 1);
    line.erase(0, line.find_first_not_of(k_whitespace_delimiters));
    data.first.clear();
    data.second.clear();
    if (line.empty()) {
        return ini_datatype_e::DATA_NONE;
    }
    char first_ch = line[0];
    if (first_ch == ';' || first_ch == '#') {
        return ini_datatype_e::DATA_COMMENT;
    }
    if (first_ch == '[') {
        auto comment_idx = line.find_first_of(';');
        if (comment_idx != std::string::npos) {
            line = line.substr(0, comment_idx);
        }
        auto close_bracket_idx = line.find_last_of(']');
        if (close_bracket_idx != std::string::npos) {
            auto section = line.substr(1, close_bracket_idx - 1);
            data.first = section;
            return ini_datatype_e::DATA_SECTION;
        }
    }
    auto equals_idx = line.find_first_of('=');
    if (equals_idx != std::string::npos) {
        auto key = line.substr(0, equals_idx);
        auto value = line.substr(equals_idx + 1);
        data.first = key;
        data.second = value;
        return ini_datatype_e::DATA_KEY_VAL;
    }
    return ini_datatype_e::DATA_UNKNOWN;
}

bool INIFile::Init(const std::string &path) {
    m_file_name = path;
    m_file.open(m_file_name);
    if (!m_file.is_open()) {
        return false;
    }
    int line_cnt = 0;
    std::string line;
    std::string section;
    parse_data_t parse_data;
    while (std::getline(m_file, line)) {
        line_cnt++;
        auto parse_ret = _parse_line(line, parse_data);
        if (parse_ret == ini_datatype_e::DATA_NONE || parse_ret == ini_datatype_e::DATA_COMMENT) {
            continue;
        }
        if (parse_ret == ini_datatype_e::DATA_SECTION) {
            section = parse_data.first;
            m_data[section] = kv_t();
        }
        if (parse_ret == ini_datatype_e::DATA_KEY_VAL) {
            m_data[section][parse_data.first] = parse_data.second;
        }
        if (parse_ret == ini_datatype_e::DATA_UNKNOWN) {
            LOG_ERROR("{} line {} error", m_file_name, line_cnt);
            return false;
        }
    }
    return true;
}

INIFile::result INIFile::TryGet(const char *section, const char *key) {
    auto _section = m_data.find(section);  // section-kv
    if (_section != m_data.end()) {
        auto _kv = _section->second.find(key);
        if (_kv != _section->second.end()) {
            return {std::string(_kv->second), true};
        }
    }
    return {std::string(), false};
}
