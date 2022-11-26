/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 20:11:26
 * @Description:
 */
#ifndef __AVZ_SCRIPT_H__
#define __AVZ_SCRIPT_H__

#include "avz_global.h"

class __AScriptManager {
public:
    static bool isBlocked;
    static bool isBlockable;
    static bool isLoaded;
    static bool isExit;
    static const char* const STR_GAME_RET_MAIN_UI;
    static AReloadMode scriptReloadMode;
    static int waitUntilDepth;

    static void LoadScript();
    static void RunScript();
    static void Run();
    static void ScriptHook();
    static void WaitUntil(int wave, int time);
};

// 阻塞运行直到达到目标时间点
// *** AWaitUntil 停止阻塞的时间点是设定的时间点的下一帧
// 例如 AWaitUntil(150, 1); int time = NowTime(1)
// 此时 time 的值是 151
// *** return : 阻塞是否正常结束
// ture 阻塞正常结束
// false 阻塞异常结束
inline void AWaitUntil(int wave, int time)
{
    __AScriptManager::WaitUntil(wave, time);
}

// 设置脚本重新载入模式
// *** 使用示例
// ASetReloadMode(AReloadMode::NONE) ----- 脚本运行结束后不再重新载入
// ASetReloadMode(AReloadMode::MAIN_UI) ----- 脚本运行结束在返回主界面后重新载入
// ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI) ----- 脚本运行结束在返回主界面或战斗界面后重新载入
inline void ASetReloadMode(AReloadMode reloadMode)
{
    __AScriptManager::scriptReloadMode = reloadMode;
}

#endif