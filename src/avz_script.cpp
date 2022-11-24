#include "avz_script.h"
#include "avz_asm.h"
#include "avz_card.h"
#include "avz_connector.h"
#include "avz_exception.h"
#include "avz_game_controllor.h"
#include "avz_logger.h"
#include "avz_tick_runner.h"
#include "avz_time_queue.h"

bool __AScriptManager::isBlocked = false;
bool __AScriptManager::isBlockable = true;
bool __AScriptManager::isLoaded = false;
bool __AScriptManager::isExit = false;
bool __AScriptManager::isRunExitFight = false;
bool __AScriptManager::isRunEnterFight = false;
const char* const __AScriptManager::STR_GAME_RET_MAIN_UI = "game return main ui";
AReloadMode __AScriptManager::scriptReloadMode = AReloadMode::NONE;
int __AScriptManager::waitUntilDepth = 0;

void __AScriptManager::LoadScript()
{
    __aInternalGlobal.pvzBase = *(APvzBase**)0x6a9ec0;
    int gameUi = __aInternalGlobal.pvzBase->GameUi();
    if (gameUi == 1 || //
        (gameUi != 2 && gameUi != 3)) {
        return;
    }
    auto mainObject = __aInternalGlobal.pvzBase->MainObject();
    __aInternalGlobal.mainObject = mainObject;

    // 假进入战斗界面直接返回
    if (mainObject->LoadDataState() == 1) {
        return;
    }
    isRunEnterFight = false;
    isRunExitFight = false;
    isLoaded = true;
    isBlocked = true;
    static ALogger<AMsgBox> logger;
    logger.SetLevel({ALogLevel::ERROR, ALogLevel::WARNING});
    __aInternalGlobal.loggerPtr = &logger;
    __AStateHookManager::RunBeforeScript();
    void AScript();
    AScript();
    __AStateHookManager::RunAfterScript();
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

    if (!isRunExitFight) {
        isRunExitFight = true;
        __AStateHookManager::RunExitFight();
    }

    if (scriptReloadMode != AReloadMode::MAIN_UI_OR_FIGHT_UI) {
        // 如果战斗界面不允许重新注入则等待回主界面
        while (__aInternalGlobal.pvzBase->MainObject()) {
            AAsm::GameSleepLoop();
        }
    }

    // 当递归深度为 0 和 scriptReloadMode > 0 时, 才能重置 isLoaded
    isLoaded = !(int(scriptReloadMode) > 0 && waitUntilDepth == 0);
}

void __AScriptManager::RunScript()
{
    int gameUi = __aInternalGlobal.pvzBase->GameUi();

    if (__AGameControllor::isAdvancedPaused || gameUi != 3) {
        // 运行全局 TickRunner
        __ATickManager::tickQueue.RunOnlyInGlobal();
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

    if (!isRunEnterFight) {
        isRunEnterFight = true;
        __AStateHookManager::RunEnterFight();
    }
    auto mainObject = __aInternalGlobal.mainObject;
    if (mainObject->Wave() != mainObject->TotalWave()) {
        __AOperationQueueManager::UpdateRefreshTime();
    }

    isBlockable = false;
    __AOperationQueueManager::RunOperation();
    __ATickManager::tickQueue.RunAll();
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
        }
        exceMsg += '\n';
        __aInternalGlobal.loggerPtr->Info(exceMsg.c_str());
        if (!isRunExitFight) {
            __AStateHookManager::RunExitFight();
        }
        // 当递归深度为 0 和 scriptReloadMode > 0 时, 才能重置 isLoaded
        isLoaded = !(int(scriptReloadMode) > 0 && waitUntilDepth == 0);
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

    // 在调用 WaitUntil 之后, WaitUntil 内部会调用 GameSleepLoop,
    // 此时如果开启了跳帧, 即使 isBlocked = true, WaitUntil 依然不会释放阻塞,
    // 因为这里的 while 在 WaitUntil 里面的 GameSleepLoop 运行的, 便导致了死循环
    while (__AGameControllor::isSkipTick() && isBlocked //
        && __aInternalGlobal.pvzBase->MainObject()) {
        Run();
        if (__AGameControllor::isAdvancedPaused) {
            return;
        }
        __aInternalGlobal.pvzBase->MjClock() += 1;
        AAsm::GameFightLoop();
        AAsm::ClearObjectMemory();
        AAsm::GameExit();
    }
}

void __AScriptManager::WaitUntil(int wave, int time)
{
    if (!isBlockable) {
        __aInternalGlobal.loggerPtr->Error("连接和帧运行内部不允许调用 AWaitUntil");
        return;
    }

    if (waitUntilDepth != 0) {
        __aInternalGlobal.loggerPtr->Error("请等待上一个 AWaitUntil 时间到达之后再调用 AWaitUntil");
        return;
    }

    auto nowTime = ANowTime(wave);
    if (nowTime > time) {
        auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
        __aInternalGlobal.loggerPtr->Warning("现在的时间点已到 (" + pattern + ", " + pattern + "), 但是您要求的阻塞结束时间点为 (" + pattern + ", " + pattern + "), 此阻塞无意义", //
            wave, nowTime, wave, time);
        return;
    }
    ++waitUntilDepth;
    isBlocked = true;
    AConnect(
        ATime(wave, time), [] { __AScriptManager::isBlocked = false; });

    Run();

    if (__aInternalGlobal.pvzBase->GameUi() == 1) {
        --waitUntilDepth;
        throw AException(STR_GAME_RET_MAIN_UI);
    }

    while (isBlocked) {
        AAsm::GameSleepLoop();
        if (!__aInternalGlobal.pvzBase->MainObject()) {
            --waitUntilDepth;
            throw AException(STR_GAME_RET_MAIN_UI);
        }
    }
    --waitUntilDepth;
}
