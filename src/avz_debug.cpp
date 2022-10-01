/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 19:09:50
 * @Description: API debug
 */

#include "avz_debug.h"
#include <Windows.h>
#include <string>

namespace AvZ {
extern int __error_mode;

// *** Not In Queue
// 设置错误提示方式
// *** 使用示例：
// SetErrorMode(AvZ::POP_WINDOW)----- 报错将会有弹窗弹出
// SetErrorMode(AvZ::CONSOLE)----- 报错将会显示在 PvZ 窗口标题
// SetErrorMode(AvZ::NONE)----- 取消报错功能
void SetErrorMode(ErrorMode _error_mode)
{
    __error_mode = _error_mode;

    if (__error_mode == CONSOLE) {
        if (AllocConsole()) {
            SetConsoleTitle(TEXT("AvZ Debug"));
            freopen("CON", "w", stdout);
            setlocale(LC_ALL, "chs");
        }
    } else {
        fclose(stdout);
        FreeConsole();
    }
}

void Utf8ToGbk(std::string& strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    strUTF8 = szGBK;
    if (wszGBK) {
        delete[] wszGBK;
    }
    if (szGBK) {
        delete[] szGBK;
    }
}

} // namespace AvZ