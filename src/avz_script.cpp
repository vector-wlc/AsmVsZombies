#include "avz_script.h"
#include "avz_asm.h"
#include "avz_card.h"
#include "avz_connector.h"
#include "avz_exception.h"
#include "avz_game_controllor.h"
#include "avz_logger.h"
#include "avz_memory.h"
#include "avz_tick_runner.h"
#include "avz_time_queue.h"
#include <algorithm>

__AScriptManager::Var __AScriptManager::var;

bool __AScriptManager::Init()
{
    __aInternalGlobal.pvzBase = *(APvzBase**)0x6a9ec0;
    int gameUi = __aInternalGlobal.pvzBase->GameUi();
    if (gameUi == 1 || //
        (gameUi != 2 && gameUi != 3)) {
        return false;
    }
    auto mainObject = __aInternalGlobal.pvzBase->MainObject();
    __aInternalGlobal.mainObject = mainObject;

    // 假进入战斗界面直接返回
    if (mainObject->LoadDataState() == 1) {
        return false;
    }

    var.isBlocked = true;
    static ALogger<AMsgBox> logger;
    static __ATickManager tickInFight;
    static __ATickManager tickInGlobal;
    __aInternalGlobal.loggerPtr = &logger;
    __aInternalGlobal.tickInFight = &tickInFight;
    __aInternalGlobal.tickInGlobal = &tickInGlobal;
    __AStateHookManager::Init();
    return true;
}

void __AScriptManager::LoadScript()
{
    if (!__AScriptManager::Init()) {
        return;
    }
    var.isLoaded = true;
    __AStateHookManager::RunAllBeforeScript();
    void AScript();
    AScript();
    __AStateHookManager::RunAllAfterScript();
    var.isBlocked = true;

    Run();

    // 等待游戏进入战斗界面
    while (__aInternalGlobal.pvzBase->GameUi() == 2 && //
        __aInternalGlobal.pvzBase->MainObject()) {
        AAsm::GameSleepLoop();
    }

    // 等待游戏结束
    while (__aInternalGlobal.pvzBase->GameUi() == 3 && //
        __aInternalGlobal.pvzBase->MainObject()) {
        AAsm::GameSleepLoop();
    }

    __AStateHookManager::RunAllExitFight();
    ASetAdvancedPause(false);

    if (var.scriptReloadMode != AReloadMode::MAIN_UI_OR_FIGHT_UI) {
        // 如果战斗界面不允许重新注入则等待回主界面
        while (__aInternalGlobal.pvzBase->MainObject()) {
            AAsm::GameSleepLoop();
        }
    }

    // 当递归深度为 0 和 scriptReloadMode > 0 时, 才能重置 isLoaded
    var.isLoaded = !(int(var.scriptReloadMode) > 0 && var.blockDepth == 0);
    var.isExit = var.isLoaded;
}

void __AScriptManager::RunScript()
{
    int gameUi = __aInternalGlobal.pvzBase->GameUi();

    if (gameUi == 2 || __AGameControllor::isAdvancedPaused) {
        __ACardManager::ChooseSingleCard();
        __aInternalGlobal.tickInGlobal->RunQueue();
        return;
    }

    if (gameUi != 3) {
        return;
    }

    // 下面的代码只能在战斗界面运行

    auto mainObject = __aInternalGlobal.mainObject;

    static int64_t runFlag = -1;
    auto gameClock = mainObject->GameClock();
    if (runFlag == gameClock) { // 保证此函数下面的内容一帧只会运行一次
        // 但是全局运行帧依然可以运行
        __aInternalGlobal.tickInGlobal->RunQueue();
        return;
    }

    runFlag = gameClock;
    __AStateHookManager::RunAllEnterFight();
    __AOpQueueManager::UpdateRefreshTime();

    var.isBlockable = false;
    __AOpQueueManager::RunOperation();
    if (!__AGameControllor::isAdvancedPaused) {
        __aInternalGlobal.tickInGlobal->RunQueue();
    }
    __aInternalGlobal.tickInFight->RunQueue();
    var.isBlockable = true;
}

void __AScriptManager::Run()
{
    try {
        if (var.isExit) {
            return;
        }

        __AStateHookManager::RunAllAfterInject();

        FastSaveLoad();

        if (var.isLoaded) { // 运行脚本
            RunScript();
        } else { // 载入脚本
            LoadScript();
        }
    } catch (AException& exce) {
        std::string exceMsg = "catch exception: ";
        exceMsg += exce.what();
        if (exceMsg != ASTR_GAME_RET_MAIN_UI) {
            var.isExit = true;
            exceMsg += " || AvZ has stopped working !!!";
            __aInternalGlobal.loggerPtr->Info(exceMsg.c_str());
        } else {
            // 当递归深度为 0 和 scriptReloadMode > 0 时, 才能重置 isLoaded
            var.isLoaded = !(int(var.scriptReloadMode) > 0 && var.blockDepth == 0);
            var.isExit = var.isLoaded;
            __aInternalGlobal.loggerPtr->Info(exceMsg.c_str());
            __AStateHookManager::RunAllExitFight();
        }
    } catch (...) {
        __aInternalGlobal.loggerPtr->Error("脚本触发了一个未知的异常\n");
        var.isExit = true;
    }
}

void __AScriptManager::ScriptHook()
{
    Run();
    if (__AGameControllor::isAdvancedPaused) {
        return;
    }
    AAsm::GameTotalLoop();

    while (__AGameControllor::isSkipTick() //
        && __aInternalGlobal.pvzBase->MainObject()) {
        Run();
        if (__AGameControllor::isAdvancedPaused) {
            return;
        }
        if (AGameIsPaused()) { // 防止游戏暂停时开启跳帧发生死锁
            return;
        }
        if (var.isBlocked && ANowTime(var.blockTime.wave) == var.blockTime.time) {
            // 阻塞时间到达，必须通知阻塞函数释放阻塞
            return;
        }
        __aInternalGlobal.pvzBase->MjClock() += 1;
        AAsm::GameFightLoop();
        AAsm::ClearObjectMemory();
        AAsm::CheckFightExit();
    }
}

void __AScriptManager::WaitForFight()
{
    if (!var.isBlockable) {
        __aInternalGlobal.loggerPtr->Error("连接和帧运行内部不允许调用 WaitForFight");
        return;
    }

    if (var.blockDepth != 0) {
        __aInternalGlobal.loggerPtr->Error("请等待上一个阻塞函数时间到达之后再调用 WaitForFight");
        return;
    }

    ++var.blockDepth;
    while (__aInternalGlobal.pvzBase->GameUi() == 2) {
        AAsm::GameSleepLoop();
    }
    --var.blockDepth;

    if (!__aInternalGlobal.pvzBase->MainObject()) {
        --var.blockDepth;
        AExitFight();
    }
    __AOpQueueManager::UpdateRefreshTime(); // 刷新一次
    __AStateHookManager::RunAllEnterFight();
}

void __AScriptManager::WaitUntil(int wave, int time)
{
    if (!var.isBlockable) {
        __aInternalGlobal.loggerPtr->Error("连接和帧运行内部不允许调用 AWaitUntil");
        return;
    }

    if (var.blockDepth != 0) {
        __aInternalGlobal.loggerPtr->Error("请等待上一个阻塞函数时间到达之后再调用 AWaitUntil");
        return;
    }
    var.blockTime.time = time;
    var.blockTime.wave = wave;
    WaitForFight();
    auto nowTime = ANowTime(wave);
    if (nowTime > time) {
        auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
        __aInternalGlobal.loggerPtr->Warning("现在的时间点已到 (" + pattern + ", " + pattern +      //
                "), 但是您要求的阻塞结束时间点为 (" + pattern + ", " + pattern + "), 此阻塞无意义", //
            wave, nowTime, wave, time);
        return;
    }
    if (nowTime == time) {
        return;
    }
    ++var.blockDepth;
    var.isBlocked = true;

    while (ANowTime(wave) < time) {
        AAsm::GameSleepLoop();
        if (!__aInternalGlobal.pvzBase->MainObject()) {
            --var.blockDepth;
            AExitFight();
        }
        __AOpQueueManager::UpdateRefreshTime(); // 实时刷新当前时间
    }
    --var.blockDepth;
}

void __AScriptManager::FastSaveLoad()
{
    // 这里所有的 countdown 目标只有一个
    // 保证 EnterGame 点击对话框 DoBackToMain 不在同一帧运行
    // 留一定的缓冲时间归 AvZ 反应
    if (var.isNeedEnterGame && var.enterGameCountdown < 0) {
        AAsm::EnterGame(var.gameMode);
        var.isNeedEnterGame = false;
        var.continueCountdown = 3;
        var.backToMainCountdown = 5;
    }

    // 点掉继续对话框
    if (var.continueCountdown == 0) {
        auto topWindow = AMPtr<APvzBase>(0x6a9ec0)->MouseWindow()->TopWindow();
        if (topWindow) {
            AAsm::MouseClick(280, 370, 1);
        }
    }
    if (var.continueCountdown >= 0) {
        --var.continueCountdown;
    }
    if (var.backToMainCountdown >= 0) {
        --var.backToMainCountdown;
    }
    if (var.enterGameCountdown >= 0) {
        --var.enterGameCountdown;
    }

    if (var.isNeedBackToMain && var.backToMainCountdown < 0) {
        if (var.isSaveData) {
            AAsm::SaveData();
        }
        AAsm::DoBackToMain();
        var.isNeedBackToMain = false;
    }
}

void __AScriptManager::EnterGame(int gameMode, bool hasContinueDialog)
{
    gameMode = std::clamp(gameMode, 0, 73);
    var.gameMode = gameMode;
    var.hasContinueDialog = hasContinueDialog;
    var.isNeedEnterGame = true;
}

void __AScriptManager::BackToMain(bool isSaveData)
{
    var.isNeedBackToMain = true;
    var.isSaveData = isSaveData;
    var.enterGameCountdown = 5;
}
