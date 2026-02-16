#include "libavz.h"

__AInternalGlobal __aig;

bool ARangeIn(int num, const std::vector<int>& lst) {
    for (auto _num : lst)
        if (_num == num)
            return true;
    return false;
}

std::wstring AStrToWstr(const std::string& input) {
    std::wstring wstr;
    wstr.resize(MultiByteToWideChar(CP_UTF8, 0, input.c_str(), input.length(), NULL, 0) + 1);
    MultiByteToWideChar(CP_UTF8, 0, input.c_str(), input.length(), wstr.data(), wstr.size());
    for (; !wstr.empty() && wstr.back() == 0;)
        wstr.pop_back();
    return wstr;
}

std::string AWStrToStr(const std::wstring& input) {
    std::string str;
    str.resize(WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.length(), NULL, 0, NULL, FALSE) + 1);
    WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.length(), str.data(), str.size(), NULL, FALSE);
    for (; !str.empty() && str.back() == 0;)
        str.pop_back();
    return str;
}

std::u32string AStrToU32str(const std::string& input) {
    std::u32string result;
    // https://en.wikipedia.org/wiki/UTF-8
    for (auto it = input.begin(); it != input.end(); ++it) {
        char32_t cp {};
        auto ch = static_cast<unsigned char>(*it);
        if (ch < 0x80) {
            // 1 byte
            result.push_back(ch);
            continue;
        }
        if (ch < 0xC0) {
            continue;
        }
        if (ch < 0xE0) {
            // 2 bytes
            cp = ch & 0x1F;
            if (++it == input.end())
                break;
            cp = (cp << 6) | (static_cast<unsigned char>(*it) & 0x3F);
        } else if (ch < 0xF0) {
            // 3 bytes
            cp = ch & 0x0F;
            if (++it == input.end())
                break;
            cp = (cp << 6) | (static_cast<unsigned char>(*it) & 0x3F);
            if (++it == input.end())
                break;
            cp = (cp << 6) | (static_cast<unsigned char>(*it) & 0x3F);
        } else {
            // 4 bytes
            cp = ch & 0x07;
            if (++it == input.end())
                break;
            cp = (cp << 6) | (static_cast<unsigned char>(*it) & 0x3F);
            if (++it == input.end())
                break;
            cp = (cp << 6) | (static_cast<unsigned char>(*it) & 0x3F);
            if (++it == input.end())
                break;
            cp = (cp << 6) | (static_cast<unsigned char>(*it) & 0x3F);
        }
        result.push_back(cp);
    }
    return result;
}

std::string AU32StrToStr(const std::u32string& input) {
    std::string result;
    for (char32_t cp : input) {
        if (cp < 0x80) {
            result.push_back(static_cast<char>(cp));
        } else if (cp < 0x800) {
            result.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp < 0x10000) {
            result.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else {
            result.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }
    return result;
}
