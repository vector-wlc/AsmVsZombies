#ifndef __AVZ_SCRIPT_H__
#define __AVZ_SCRIPT_H__

#include "avz_asm.h"

class __AScriptManager {
public:
    bool isBlockable = true;
    bool isLoaded = false;
    bool willBeExit = false;
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

    void GlobalInit();
    bool MemoryInit();
    void LoadScript();
    void RunScript();
    void RunTotal();
    void ScriptHook();
    void WaitUntil(int wave, int time);
    void WaitForFight(bool isSkipTick);
    void EnterGame(int gameMode, bool hasContinueDialog);
    void BackToMain(bool isSaveData);
    void FastSaveLoad();
};

inline __AScriptManager __aScriptManager;

// 阻塞运行直到达到目标时间点
__ADeprecated("请使用 ACoroutine ACoScript() 和 co_await ATime())") inline void AWaitUntil(int wave, int time) {
    __aScriptManager.WaitUntil(wave, time);
}

// 设置脚本重新载入模式
// *** 使用示例
// ASetReloadMode(AReloadMode::NONE) ----- 脚本运行结束后不再重新载入
// ASetReloadMode(AReloadMode::MAIN_UI) ----- 脚本运行结束在返回主界面后重新载入
// ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI) ----- 脚本运行结束在返回主界面或战斗界面后重新载入
inline void ASetReloadMode(AReloadMode reloadMode) {
    __aScriptManager.scriptReloadMode = reloadMode;
}

// 等待游戏进入战斗界面释放阻塞
inline void AWaitForFight(bool isSkipTick = false) {
    __aScriptManager.WaitForFight(isSkipTick);
}

// 快速进入游戏函数
// *** 使用示例
// AEnterGame() ------ 默认进入泳池无尽生存模式，默认会自动点掉继续对话框
// AEnterGame(AAsm::SURVIVAL_ENDLESS_STAGE_1) -------- 进入白天无尽生存模式，默认会自动点掉继续对话框
// AEnterGame(AAsm::SURVIVAL_ENDLESS_STAGE_1, false) -------- 进入白天无尽生存模式，不会自动点掉继续对话框
inline void AEnterGame(int gameMode = AAsm::SURVIVAL_ENDLESS_STAGE_3, bool hasContinueDialog = false) {
    __aScriptManager.EnterGame(gameMode, hasContinueDialog);
}

// 快速回到游戏主界面
// *** 注意：此函数仅在战斗界面生效
// *** 使用示例
// ABackToMain() ----- 直接回到主界面，默认会自动存档
// ABackToMain(false) ----- 直接回到主界面，不会自动存档
inline void ABackToMain(bool isSaveData = true) {
    __aScriptManager.BackToMain(isSaveData);
}

#endif
