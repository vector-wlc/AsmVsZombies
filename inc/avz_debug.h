/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:40:16
 * @Description : debug api
 */
#ifndef __AVZ_DEBUG_H__
#define __AVZ_DEBUG_H__

#include <string>
#include <sstream>
#include <functional>

#include "avz_global.h"
#include "pvzstruct.h"

namespace AvZ
{
    template <typename T>
    void string_convert(std::string &content, T t)
    {
        std::stringstream conversion;
        conversion << t;
        content.replace(content.find_first_of('#'), 1, conversion.str());
    }

    // 该部分为调试功能部分，使用下面两个接口可以对脚本进行调试
    enum ErrorMode
    {
        POP_WINDOW,
        PVZ_TITLE,
        NONE
    };

    // *** Not In Queue
    // 设置错误提示方式
    // *** 使用示例：
    // SetErrorMode(AvZ::POP_WINDOW)----- 报错将会有弹窗弹出
    // SetErrorMode(AvZ::PVZ_TITLE)----- 报错将会显示在 PvZ 窗口标题
    // SetErrorMode(AvZ::NONE)----- 取消报错功能
    void SetErrorMode(ErrorMode _error_mode);

    // *** Not In Queue
    // 弹出窗口提示错误或者调试信息
    // 使用方法同 printf 但是其中的 %d %f 等需要替换为 #
    template <typename... Args>
    void ShowErrorNotInQueue(const std::string &content = "", Args... args)
    {
        extern int __error_mode;
        extern PvZ *__pvz_base;
        extern TimeWave __time_wave_run;
        extern HWND __pvz_hwnd;
        if (__pvz_base->gameUi() != 3 && __pvz_base->gameUi() != 2)
        {
            return;
        }

        if (__error_mode == NONE)
        {
            return;
        }
        std::string _content;
        if (__time_wave_run.wave == 0)
        {
            _content = "操作录入状态  \n\n" + content;
        }
        else
        {
            _content = "wave : # -- time : #   \n\n" + content;
            string_convert(_content, __time_wave_run.wave);
            string_convert(_content, __time_wave_run.time);
        }

        std::initializer_list<int>{(string_convert(_content, args), 0)...};

        void utf8_to_gbk(std::string & strUTF8);
        utf8_to_gbk(_content);
        if (__error_mode == POP_WINDOW)
        {
            MessageBoxA(NULL, _content.c_str(), "Error", 0);
        }

        if (__error_mode == PVZ_TITLE)
        {
            SetWindowTextA(__pvz_hwnd, _content.c_str());
        }
    };

    // *** In Queue
    // 弹出窗口提示错误或者调试信息
    // 使用方法同 printf 但是其中的 %d %f 等需要替换为 #
    template <typename... Args>
    void ShowError(const std::string &content = "", Args... args)
    {
        void InsertOperation(const std::function<void()> &operation, const std::string &description = "unknown");
        InsertOperation([=]() {
            ShowErrorNotInQueue(content, args...);
        });
    };

} // namespace AvZ
#endif