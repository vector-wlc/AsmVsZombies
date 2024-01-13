/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-09 10:49:00
 * @Description:
 */

#include "avz_global.h"
#include "avz_logger.h"
#include "avz_time_queue.h"
#include <codecvt>
#include <locale>

__AInternalGlobal __aig;

bool ARangeIn(int num, const std::vector<int>& lst)
{
    for (auto _num : lst) {
        if (_num == num) {
            return true;
        }
    }
    return false;
}

std::wstring AStrToWstr(const std::string& input)
{
    auto cStr = input.c_str();
    auto strLen = input.length();
    int len = MultiByteToWideChar(CP_UTF8, 0, cStr, strLen, NULL, 0);
    wchar_t* m_wchar = new wchar_t[len + 1];
    MultiByteToWideChar(CP_UTF8, 0, cStr, strLen, m_wchar, len);
    m_wchar[len] = L'\0';
    return m_wchar;
}
