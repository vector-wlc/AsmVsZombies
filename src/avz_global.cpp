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
    std::wstring wstr;
    wstr.resize(MultiByteToWideChar(CP_UTF8, 0, input.c_str(), input.length(), NULL, 0) + 1);
    MultiByteToWideChar(CP_UTF8, 0, input.c_str(), input.length(), wstr.data(), wstr.size());
    return wstr;
}

std::string AWStrToStr(const std::wstring& input)
{
    std::string str;
    str.resize(WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.length(), NULL, 0, NULL, FALSE) + 1);
    WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.length(), str.data(), str.size(), NULL, FALSE);
    return str;
}
