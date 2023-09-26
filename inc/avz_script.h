/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 20:11:26
 * @Description:
 */
#ifndef __AVZ_SCRIPT_H__
#define __AVZ_SCRIPT_H__

#include "avz_global.h"
#include "avz_asm.h"

class __AScriptManager {
public:
    struct Var {
        bool isBlocked = false;
        bool isBlockable = true;
        bool isLoaded = false;
        bool isExit = false;
        AReloadMode scriptReloadMode = AReloadMode::NONE;
        int blockDepth = 0;
        ATime blockTime;

        // 用于 EnterGame
        bool isNeedEnterGame = false;
        int gameMode = 13;
        bool hasContinueDialog = false;
        int continueCountdown = -1;
        int backToMainCountdown = -1;

        // 用于 BackToMain
        bool isNeedBackToMain = false;
        bool isSaveData = true;
        int enterGameCountdown = -1;
    };

    static Var var;

    static bool Init();
    static void LoadScript();
    static void RunScript();
    static void Run();
    static void ScriptHook();
    static void WaitUntil(int wave, int time);
    static void WaitForFight();
    static void EnterGame(int gameMode, bool hasContinueDialog);
    static void BackToMain(bool isSaveData);
    static void FastSaveLoad();
};

// 阻塞运行直到达到目标时间点
// *** AWaitUntil 停止阻塞的时间点是设定的时间点的下一帧
[[deprecated("请使用 ACoroutine ACoScript() 和 co_await ATime() 代替此函数")]] inline void AWaitUntil(int wave, int time)
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
    __AScriptManager::var.scriptReloadMode = reloadMode;
}

// 等待游戏进入战斗界面释放阻塞
inline void AWaitForFight()
{
    __AScriptManager::WaitForFight();
}

// 快速进入游戏函数
// *** 使用示例
// AEnterGame() ------ 默认进入泳池无尽生存模式，默认会自动点掉继续对话框
// AEnterGame(AAsm::SURVIVAL_ENDLESS_STAGE_1) -------- 进入白天无尽生存模式，默认会自动点掉继续对话框
// AEnterGame(AAsm::SURVIVAL_ENDLESS_STAGE_1, false) -------- 进入白天无尽生存模式，不会自动点掉继续对话框
inline void AEnterGame(int gameMode = AAsm::SURVIVAL_ENDLESS_STAGE_3, bool hasContinueDialog = false)
{
    __AScriptManager::EnterGame(gameMode, hasContinueDialog);
}

// 快速回到游戏主界面
// *** 注意：此函数仅在战斗界面生效
// *** 使用示例
// ABackToMain() ----- 直接回到主界面，默认会自动存档
// ABackToMain(false) ----- 直接回到主界面，不会自动存档
inline void ABackToMain(bool isSaveData = true)
{
    __AScriptManager::BackToMain(isSaveData);
}

#endif