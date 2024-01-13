#include "avz_script.h"
#include "avz_asm.h"
#include "avz_card.h"
#include "avz_connector.h"
#include "avz_exception.h"
#include "avz_game_controllor.h"
#include "avz_global.h"
#include "avz_logger.h"
#include "avz_memory.h"
#include "avz_tick_runner.h"
#include "avz_time_queue.h"

void __AScriptManager::GlobalInit()
{
    static bool isInit = false;
    if (isInit) {
        return;
    }
    isInit = true;
    static ALogger<AMsgBox> logger;
    logger.SetLevel({ALogLevel::ERROR, ALogLevel::WARNING});
    __aig.loggerPtr = &logger;
    static __ATickManager tickManagers[ATickRunner::__COUNT];
    __aig.tickManagers = tickManagers;
    __aig.tickManagers[ATickRunner::AFTER_INJECT].SetRunMode(ATickRunner::AFTER_INJECT);
    __aig.tickManagers[ATickRunner::ONLY_FIGHT].SetRunMode(ATickRunner::ONLY_FIGHT);
    __aig.tickManagers[ATickRunner::GLOBAL].SetRunMode(ATickRunner::GLOBAL);
}

bool __AScriptManager::MemoryInit()
{
    __aig.pvzBase = *(APvzBase**)0x6a9ec0;
    int gameUi = __aig.pvzBase->GameUi();
    if (gameUi == 1 || //
        (gameUi != 2 && gameUi != 3)) {
        return false;
    }
    int gameIdx = __aig.pvzBase->MRef<int>(0x7f8); // 关卡序号
    if (gameIdx == AAsm::CHALLENGE_ZEN_GARDEN) {   // 进入禅静花园直接返回
        return false;
    }
    auto mainObject = __aig.pvzBase->MainObject();
    __aig.mainObject = mainObject;

    // 假进入战斗界面直接返回
    if (mainObject->LoadDataState() == 1) {
        return false;
    }

    isBlocked = true;
    for (auto&& initOp : __aig.GetInitOps()) {
        initOp();
    }
    return true;
}

void __AScriptManager::LoadScript()
{
    if (!__AScriptManager::MemoryInit()) {
        return;
    }
    isLoaded = true;
    __APublicBeforeScriptHook::RunAll();
    void AScript();
    AScript();
    __APublicAfterScriptHook::RunAll();
    isBlocked = true;

    RunTotal();

    // 等待游戏进入战斗界面
    while (__aig.pvzBase->GameUi() == 2 && //
        __aig.pvzBase->MainObject()) {
        AAsm::GameSleepLoop();
    }

    // 等待游戏结束
    while (__aig.pvzBase->GameUi() == 3 && //
        __aig.pvzBase->MainObject()) {
        AAsm::GameSleepLoop();
    }

    __APublicExitFightHook::RunAll();
    ASetAdvancedPause(false);

    if (scriptReloadMode != AReloadMode::MAIN_UI_OR_FIGHT_UI) {
        // 如果战斗界面不允许重新注入则等待回主界面
        while (__aig.pvzBase->MainObject()) {
            AAsm::GameSleepLoop();
        }
    }

    // 当递归深度为 0 和 scriptReloadMode > 0 时, 才能重置 isLoaded
    isLoaded = !(int(scriptReloadMode) > 0 && blockDepth == 0);
    isExit = isLoaded;
}

void __AScriptManager::RunScript()
{
    int gameUi = __aig.pvzBase->GameUi();

    if (gameUi == 2 || __aGameControllor.isAdvancedPaused) {
        __aig.tickManagers[ATickRunner::GLOBAL].RunQueue();
        return;
    }

    if (gameUi != 3) {
        return;
    }

    // 下面的代码只能在战斗界面运行

    auto mainObject = __aig.mainObject;

    static int64_t runFlag = -1;
    auto gameClock = mainObject->GameClock();
    if (runFlag == gameClock) { // 保证此函数下面的内容一帧只会运行一次
        // 但是全局运行帧依然可以运行
        __aig.tickManagers[ATickRunner::GLOBAL].RunQueue();
        return;
    }

    runFlag = gameClock;
    __APublicEnterFightHook::RunAll();
    __aOpQueueManager.UpdateRefreshTime();

    isBlockable = false;
    __aOpQueueManager.RunOperation();
    __aig.tickManagers[ATickRunner::GLOBAL].RunQueue();
    __aig.tickManagers[ATickRunner::ONLY_FIGHT].RunQueue();
    isBlockable = true;
}

void __AScriptManager::RunTotal()
{
    constexpr auto stopWorkingStr = " || AvZ has stopped working !!!";

    try {
        if (isExit) {
            return;
        }
        // 这里有最基础的全局初始化，任何功能都必须在这之后运行
        // 而且这只进行一次初始化，对性能毫无影响
        GlobalInit();

        __APublicBeforeTickHook::RunAll();
        __APublicBeforeTickHook::Reset();

        __APublicAfterInjectHook::RunAll();

        // 运行 AFTER_INJECT 运行帧
        __aig.tickManagers[ATickRunner::AFTER_INJECT].RunQueue();

        FastSaveLoad();

        if (isLoaded) { // 运行脚本
            RunScript();
        } else { // 载入脚本
            LoadScript();
        }

        __APublicAfterTickHook::RunAll();
        __APublicAfterTickHook::Reset();
    } catch (AException& exce) {
        std::string exceMsg = exce.what();
        if (exceMsg != ASTR_GAME_RET_MAIN_UI) {
            isExit = true;
            exceMsg = "catch avz exception: " + exceMsg + stopWorkingStr;
            __aig.loggerPtr->Info(exceMsg.c_str());
        } else {
            // 当递归深度为 0 和 scriptReloadMode > 0 时, 才能重置 isLoaded
            isLoaded = !(int(scriptReloadMode) > 0 && blockDepth == 0);
            isExit = isLoaded;
            __aig.loggerPtr->Info(exceMsg.c_str());
            __APublicExitFightHook::RunAll();
        }
    } catch (std::exception& exce) {
        AMsgBox::Show(std::string("catch std exception: ") + exce.what() + stopWorkingStr);
        isExit = true;
    } catch (...) {
        AMsgBox::Show(std::string("The script triggered an unknown exception. ") + stopWorkingStr);
        isExit = true;
    }
}

void __AScriptManager::ScriptHook()
{
    RunTotal();
    if (__aGameControllor.isAdvancedPaused) {
        return;
    }
    AAsm::GameTotalLoop();

    while (__aGameControllor.isSkipTick() //
        && __aig.pvzBase->MainObject()) {
        RunTotal();
        if (__aGameControllor.isAdvancedPaused) {
            return;
        }
        if (AGameIsPaused()) { // 防止游戏暂停时开启跳帧发生死锁
            return;
        }
        if (isBlocked && ANowTime(blockTime.wave) == blockTime.time) {
            // 阻塞时间到达，必须通知阻塞函数释放阻塞
            return;
        }
        __aig.pvzBase->MjClock() += 1;
        AAsm::GameFightLoop();
        AAsm::ClearObjectMemory();
        AAsm::CheckFightExit();
    }
}

void __AScriptManager::WaitForFight(bool isSkipTick)
{
    if (!isBlockable) {
        __aig.loggerPtr->Error("连接和帧运行内部不允许调用 WaitForFight");
        return;
    }

    if (blockDepth != 0) {
        __aig.loggerPtr->Error("请等待上一个阻塞函数时间到达之后再调用 WaitForFight");
        return;
    }

    ++blockDepth;

    for (int cnt = 0; __aig.pvzBase->GameUi() == 2 && cnt < 2; ++cnt) {
        // 画面刷新几帧防止被系统杀死
        AAsm::GameSleepLoop();
    }
    if (isSkipTick) {
        for (; __aig.pvzBase->GameUi() == 2;) {
            RunTotal();
            AAsm::UpdateFrame();
        }
    }
    for (; __aig.pvzBase->GameUi() == 2;) {
        AAsm::GameSleepLoop();
    }

    --blockDepth;
    if (!__aig.pvzBase->MainObject()) {
        AExitFight();
    }
    __aOpQueueManager.UpdateRefreshTime(); // 刷新一次
    __APublicEnterFightHook::RunAll();
}

void __AScriptManager::WaitUntil(int wave, int time)
{
    if (!isBlockable) {
        __aig.loggerPtr->Error("连接和帧运行内部不允许调用 AWaitUntil");
        return;
    }

    if (blockDepth != 0) {
        __aig.loggerPtr->Error("请等待上一个阻塞函数时间到达之后再调用 AWaitUntil");
        return;
    }
    blockTime.time = time;
    blockTime.wave = wave;
    WaitForFight(false);
    auto nowTime = ANowTime(wave);
    if (nowTime > time) {
        auto&& pattern = __aig.loggerPtr->GetPattern();
        __aig.loggerPtr->Warning("现在的时间点已到 (" + pattern + ", " + pattern +                  //
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
        if (!__aig.pvzBase->MainObject()) {
            --blockDepth;
            AExitFight();
        }
        __aOpQueueManager.UpdateRefreshTime(); // 实时刷新当前时间
    }
    --blockDepth;
}

void __AScriptManager::FastSaveLoad()
{
    // 这里所有的 countdown 目标只有一个
    // 保证 EnterGame 点击对话框 DoBackToMain 不在同一帧运行
    // 留一定的缓冲时间归 AvZ 反应
    if (isNeedEnterGame && enterGameCountdown < 0) {
        AAsm::EnterGame(gameMode);
        isNeedEnterGame = false;
        continueCountdown = 3;
        backToMainCountdown = 5;
    }

    // 点掉继续对话框
    if (continueCountdown == 0) {
        auto topWindow = AMPtr<APvzBase>(0x6a9ec0)->MouseWindow()->TopWindow();
        if (topWindow) {
            AAsm::MouseClick(280, 370, 1);
        }
    }
    if (continueCountdown >= 0) {
        --continueCountdown;
    }
    if (backToMainCountdown >= 0) {
        --backToMainCountdown;
    }
    if (enterGameCountdown >= 0) {
        --enterGameCountdown;
    }

    if (isNeedBackToMain && backToMainCountdown < 0) {
        if (isSaveData) {
            AAsm::SaveData();
        }
        AAsm::DoBackToMain();
        isNeedBackToMain = false;
    }
}

void __AScriptManager::EnterGame(int gameMode, bool hasContinueDialog)
{
    gameMode = std::clamp(gameMode, 0, 73);
    this->gameMode = gameMode;
    this->hasContinueDialog = hasContinueDialog;
    isNeedEnterGame = true;
}

void __AScriptManager::BackToMain(bool isSaveData)
{
    isNeedBackToMain = true;
    this->isSaveData = isSaveData;
    enterGameCountdown = 5;
}
