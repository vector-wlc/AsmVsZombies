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

bool __AScriptManager::isBlocked = false;
bool __AScriptManager::isBlockable = true;
bool __AScriptManager::isLoaded = false;
bool __AScriptManager::isExit = false;
const char* const __AScriptManager::STR_GAME_RET_MAIN_UI = "game return main ui";
AReloadMode __AScriptManager::scriptReloadMode = AReloadMode::NONE;
int __AScriptManager::blockDepth = 0;
ATime __AScriptManager::blockTime;

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

    isBlocked = true;
    static ALogger<AMsgBox> logger;
    logger.SetLevel({ALogLevel::ERROR, ALogLevel::WARNING});
    __aInternalGlobal.loggerPtr = &logger;
    __AStateHookManager::Init();
    return true;
}

void __AScriptManager::LoadScript()
{
    if (!__AScriptManager::Init()) {
        return;
    }
    isLoaded = true;
    __AStateHookManager::RunAllBeforeScript();
    void AScript();
    AScript();
    __AStateHookManager::RunAllAfterScript();
    isBlocked = true;

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

    if (scriptReloadMode != AReloadMode::MAIN_UI_OR_FIGHT_UI) {
        // 如果战斗界面不允许重新注入则等待回主界面
        while (__aInternalGlobal.pvzBase->MainObject()) {
            AAsm::GameSleepLoop();
        }
    }

    // 当递归深度为 0 和 scriptReloadMode > 0 时, 才能重置 isLoaded
    isLoaded = !(int(scriptReloadMode) > 0 && blockDepth == 0);
    isExit = isLoaded;
}

void __AScriptManager::RunScript()
{
    int gameUi = __aInternalGlobal.pvzBase->GameUi();

    if (__AGameControllor::isAdvancedPaused || gameUi != 3) {
        // 运行全局 TickRunner
        __aInternalGlobal.tickManager->RunOnlyInGlobal();
    }

    if (__AGameControllor::isAdvancedPaused) {
        return;
    }

    if (gameUi == 2) {
        __ACardManager::ChooseSingleCard();
    }

    if (gameUi != 3) {
        return;
    }

    // 下面的代码只能在战斗界面运行

    auto mainObject = __aInternalGlobal.mainObject;

    static int64_t runFlag = -1;
    auto gameClock = mainObject->GameClock();
    if (runFlag == gameClock) { // 保证此函数下面的内容一帧只会运行一次
        return;
    }
    runFlag = gameClock;
    __AStateHookManager::RunAllEnterFight();
    __AOperationQueueManager::UpdateRefreshTime();

    isBlockable = false;
    __AOperationQueueManager::RunOperation();
    __aInternalGlobal.tickManager->RunAll();
    isBlockable = true;
}

void __AScriptManager::Run()
{
    try {
        if (isExit) {
            return;
        }

        if (isLoaded) { // 运行脚本
            RunScript();
        } else { // 载入脚本
            LoadScript();
        }
    } catch (AException& exce) {
        std::string exceMsg = "catch exception: ";
        exceMsg += exce.what();
        if (exceMsg != STR_GAME_RET_MAIN_UI) {
            isExit = true;
            exceMsg += " || AvZ has stopped working !!!";
            __aInternalGlobal.loggerPtr->Info(exceMsg.c_str());
        } else {
            // 当递归深度为 0 和 scriptReloadMode > 0 时, 才能重置 isLoaded
            isLoaded = !(int(scriptReloadMode) > 0 && blockDepth == 0);
            isExit = isLoaded;
            __aInternalGlobal.loggerPtr->Info(exceMsg.c_str());
            __AStateHookManager::RunAllExitFight();
        }
    } catch (...) {
        __aInternalGlobal.loggerPtr->Error("脚本触发了一个未知的异常\n");
        isExit = true;
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
        if (isBlocked && ANowTime(blockTime.wave) == blockTime.time) {
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
    if (!isBlockable) {
        __aInternalGlobal.loggerPtr->Error("连接和帧运行内部不允许调用 WaitForFight");
        return;
    }

    if (blockDepth != 0) {
        __aInternalGlobal.loggerPtr->Error("请等待上一个阻塞函数时间到达之后再调用 WaitForFight");
        return;
    }

    ++blockDepth;
    while (__aInternalGlobal.pvzBase->GameUi() == 2) {
        AAsm::GameSleepLoop();
    }
    --blockDepth;

    if (!__aInternalGlobal.pvzBase->MainObject()) {
        --blockDepth;
        throw AException(STR_GAME_RET_MAIN_UI);
    }
    __AOperationQueueManager::UpdateRefreshTime(); // 刷新一次
    __AStateHookManager::RunAllEnterFight();
}

void __AScriptManager::WaitUntil(int wave, int time)
{
    if (!isBlockable) {
        __aInternalGlobal.loggerPtr->Error("连接和帧运行内部不允许调用 AWaitUntil");
        return;
    }

    if (blockDepth != 0) {
        __aInternalGlobal.loggerPtr->Error("请等待上一个阻塞函数时间到达之后再调用 AWaitUntil");
        return;
    }
    blockTime.time = time;
    blockTime.wave = wave;
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
    ++blockDepth;
    isBlocked = true;

    while (ANowTime(wave) < time) {
        AAsm::GameSleepLoop();
        if (!__aInternalGlobal.pvzBase->MainObject()) {
            --blockDepth;
            throw AException(STR_GAME_RET_MAIN_UI);
        }
        __AOperationQueueManager::UpdateRefreshTime(); // 实时刷新当前时间
    }
    --blockDepth;
}
