/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 19:09:50
 * @Description: AvZ debug
 */

#include <string>
#include "avz.h"

void AvZ::utf8_to_gbk(std::string &strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
    wchar_t *wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    strUTF8 = szGBK;
    if (wszGBK)
        delete[] wszGBK;
    if (szGBK)
        delete[] szGBK;
}