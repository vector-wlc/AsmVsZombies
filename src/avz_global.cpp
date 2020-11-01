/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:32:56
 * @Description:
 */
#include "avz_global.h"

namespace AvZ
{
    HWND __pvz_hwnd;
    HANDLE __pvz_handle = nullptr;
    PvZ *__pvz_base;
    MainObject *__main_object;

    bool RangeIn(int num, std::initializer_list<int> lst)
    {
        for (auto _num : lst)
        {
            if (_num == num)
            {
                return true;
            }
        }
        return false;
    }

    void InitAddress()
    {
        __pvz_base = *(PvZ **)0x6a9ec0;
        __pvz_hwnd = FindWindowW(L"MainWindow", L"Plants vs. Zombies");
        __pvz_handle = INVALID_HANDLE_VALUE;
    }
} // namespace AvZ
