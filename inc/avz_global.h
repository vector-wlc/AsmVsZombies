/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:19:50
 * @Description: include global Func
 */

#ifndef __FUNC_H__
#define __FUNC_H__

#include <Windows.h>
#include <algorithm>
#include <initializer_list>
#include <vector>

#include "pvzstruct.h"

namespace AvZ
{
#define FindInAllRange(container, goal) std::find(container.begin(), container.end(), goal)

    // *** 函数功能：判断数字范围
    // *** 使用示例：
    // RangeIn(wave, {1,2,3})------如果波数在 1 2 3 范围里返回 true
    bool RangeIn(int num, std::initializer_list<int> lst);

    // 寻找 vector 中相同的元素，返回其迭代器
    template <typename Ele>
    auto FindSameEle(const std::vector<Ele> &container, const Ele &ele_) -> std::vector<decltype(container.begin())>
    {
        std::vector<decltype(container.begin())> result;
        for (auto it = container.begin(); it != container.end(); ++it)
        {
            if ((*it) == ele_)
            {
                result.push_back(it);
            }
        }

        return result;
    }

    // 寻找 vector 中相同的元素，返回其迭代器
    template <typename Ele>
    auto FindSameEle(std::vector<Ele> &container, const Ele &ele_) -> std::vector<decltype(container.begin())>
    {
        std::vector<decltype(container.begin())> result;
        for (auto it = container.begin(); it != container.end(); ++it)
        {
            if ((*it) == ele_)
            {
                result.push_back(it);
            }
        }

        return result;
    }

    // 读取内存函数
    template <typename T, typename... Args>
    T ReadMemory(Args... args)
    {
        extern HANDLE __pvz_handle;
        std::initializer_list<uintptr_t> lst = {static_cast<uintptr_t>(args)...};
        uintptr_t buff = 0;
        T result = T();
        for (auto it = lst.begin(); it != lst.end(); ++it)
        {
            if (it != lst.end() - 1)
                ReadProcessMemory(__pvz_handle, (const void *)(buff + *it), &buff, sizeof(buff), nullptr);
            else
                ReadProcessMemory(__pvz_handle, (const void *)(buff + *it), &result, sizeof(result), nullptr);
        }
        return result;
    }

    // 改写内存函数
    template <typename T, typename... Args>
    void WriteMemory(T value, Args... args)
    {
        extern HANDLE __pvz_handle;
        std::initializer_list<uintptr_t> lst = {static_cast<uintptr_t>(args)...};
        uintptr_t buff = 0;
        for (auto it = lst.begin(); it != lst.end(); it++)
            if (it != lst.end() - 1)
                ReadProcessMemory(__pvz_handle, (const void *)(buff + *it), &buff, sizeof(buff), nullptr);
            else
                WriteProcessMemory(__pvz_handle, (void *)(buff + *it), &value, sizeof(value), nullptr);
    }

// 随时检测线程退出
#define exit_sleep(ms)                                            \
    do                                                            \
    {                                                             \
        int _ms = ms;                                             \
        do                                                        \
        {                                                         \
            extern bool is_exited;                                \
            if (is_exited)                                        \
            {                                                     \
                extern HWND __pvz_hwnd;                           \
                SetWindowTextA(__pvz_hwnd, "Plants vs. Zombies"); \
                return;                                           \
            }                                                     \
            Sleep(1);                                             \
        } while (--_ms);                                          \
    } while (false)

    struct Grid
    {
        int row;
        int col;

        friend bool operator==(const Grid &grid1, const Grid &grid2)
        {
            return grid1.row == grid2.row && grid1.col == grid2.col;
        }

        friend bool operator<(const Grid &grid1, const Grid &grid2)
        {
            if (grid1.row == grid2.row)
            {
                return grid1.col < grid2.col;
            }
            return grid1.row < grid2.row;
        }
    };

    struct Crood
    {
        int row;
        float col;
    };

    struct TimeWave
    {
        int time;
        int wave;
    };
} // namespace AvZ
#endif
