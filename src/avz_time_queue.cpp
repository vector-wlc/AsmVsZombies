#include "libavz.h"

std::optional<int> __AOpQueueManager::_GetNextWaveCountdown() {
    auto mo = AGetMainObject();
    int wave = mo->Wave();
    if (wave == mo->TotalWave()) {
        int countdown = mo->LevelEndCountdown();
        return countdown > 0 ? std::make_optional(countdown) : std::nullopt;
    }
    int countdown = mo->RefreshCountdown();
    if (wave == 0)
        return countdown;
    if (wave % 10 == 9) {
        if (countdown <= 5)
            return mo->HugeWaveCountdown();
        else
            return countdown <= 200 ? std::make_optional(countdown + 745) : std::nullopt;
    }
    return countdown <= 200 ? std::make_optional(countdown) : std::nullopt;
}

void __AOpQueueManager::_CalculateRefreshTime(int startWave) {
    if (queues[startWave].calRefreshTime == __AOperationQueue::UNINIT)
        return;
    for (int wave = startWave; wave <= totalWave && queues[wave].waveLength != -1; ++wave)
        queues[wave + 1].calRefreshTime = queues[wave].calRefreshTime + queues[wave].waveLength;
}

void __AOpQueueManager::_RecordRefresh(int wave, int refreshTime) {
    if (queues[wave].memRefreshTime == refreshTime)
        return;
    if (queues[wave].memRefreshTime != __AOperationQueue::UNINIT) {
        /*
        static int lastWarnTime = -100;
        if (AGetMainObject()->GameClock() - lastWarnTime >= 100) {
            lastWarnTime = AGetMainObject()->GameClock();
            aLogger->Warning("时间换算出现 {}cs 的不一致；是否开启了暂停刷新修改？", refreshTime - queues[wave].memRefreshTime);
        }
        */
        return;
    }
    queues[wave].memRefreshTime = queues[wave].calRefreshTime = refreshTime;
    if (wave > 0 && queues[wave - 1].memRefreshTime != __AOperationQueue::UNINIT) {
        int waveLength = queues[wave].memRefreshTime - queues[wave - 1].memRefreshTime;
        if (wave > 1)
            aLogger->Info("下一波即将刷新，第 {} 波的波长为 {}", wave - 1, waveLength);
        queues[wave - 1].waveLength = waveLength;
    }
    _CalculateRefreshTime(wave);
}

std::optional<__ATimeIter> __AOpQueueManager::Push(const ATime& time, __ABoolOperation&& timeOp) {
    if (time.wave < 0 || time.wave >= queues.size()) {
        aLogger->Error("连接设定的 wave 参数为 {}, 超出有效范围", time.wave);
        return std::nullopt;
    }
    if (time.wave < startTime.wave || (time.wave == startTime.wave && time.time < startTime.time)) {
        aLogger->Info("当前连接时间 {} 小于开始时间 {}, 因此未建立连接", time, startTime);
        return std::nullopt;
    }

    int nowTime = ANowTime(time.wave);
    if (nowTime >= time.time) { // 现在要立即运行的操作
        _PrintLog(time, nowTime);
        timeOp.operation();
        return std::nullopt; // 无法控制这种操作的运行状态
    }

    auto ret = queues[time.wave].queue.emplace(time.time, std::move(timeOp));
    aLogger->Info("建立 时间-操作 {} 连接成功", time);
    return ret;
}

void __AOpQueueManager::RunOperation() {
    int nowTime = AGetMainObject()->GameClock();
    for (int wave = 0; wave <= totalWave + 1; ++wave) {
        auto& opQueue = queues[wave];
        if (opQueue.calRefreshTime == __AOperationQueue::UNINIT) // 本波还未到达
            continue;
        for (auto iter = opQueue.queue.begin(); iter != opQueue.queue.end(); iter = opQueue.queue.erase(iter)) {
            int queueNowTime = nowTime - opQueue.calRefreshTime;
            if (iter->first > queueNowTime) // 时间未到达
                break;
            if (iter->second.isStopped) // 此时间连接已经被废除
                continue;
            _PrintLog(ATime(wave, iter->first), queueNowTime);
            iter->second.operation();
            if (AGetPvzBase()->GameUi() != 3)
                return; // 在非战斗界面需要立即退出
        }
    }
}

void __AOpQueueManager::UpdateRefreshTime() {
    if (AGetPvzBase()->GameUi() != 3)
        return;
    auto mo = AGetMainObject();
    int wave = mo->Wave(), gameClock = mo->GameClock();
    if (queues[wave].memRefreshTime == __AOperationQueue::UNINIT)
        _RecordRefresh(wave, gameClock + mo->RefreshCountdown() - mo->InitialCountdown());
    std::optional<int> countdown = _GetNextWaveCountdown();
    if (countdown.has_value())
        _RecordRefresh(wave + 1, gameClock + *countdown);
}

void __AOpQueueManager::SetWavelength(const std::vector<ATime>& lst) {
    for (const auto& time : lst) {
        if (time.wave == AGetMainObject()->TotalWave()) {
            aLogger->Error("SetWavelength 对最后一波无效");
            continue;
        }
        if (!_CheckWavelength(time))
            continue;
        auto&& timeQueue = queues[time.wave];
        timeQueue.waveLength = time.time;
        _CalculateRefreshTime(time.wave);

        AConnect(ATime(time.wave, 1), [=]() {
            AGetMainObject()->ZombieRefreshHp() = -1;
            int countdown = (time.wave % 10 == 9 ? time.time - 745 : time.time);
            AGetMainObject()->RefreshCountdown() = countdown - 1;
            AGetMainObject()->InitialCountdown() = countdown;
        });
    }
}

void __AOpQueueManager::AssumeWavelength(const std::vector<ATime>& lst) {
    for (const auto& time : lst) {
        if (!_CheckWavelength(time))
            continue;
        queues[time.wave].waveLength = time.time;
        queues[time.wave].isAssumed = true;
        _CalculateRefreshTime(time.wave);
        AConnect(ATime(time.wave + 1, -200), [=, this] {
            _CheckAssumeWavelength(time.wave);
        });
    }
}

void __AOpQueueManager::_PrintLog(const ATime& time, int nowTime) {
    if (nowTime > time.time)
        aLogger->Warning("时间-操作 {} 但是现在时间已到 {}", time, ATime(time.wave, nowTime));
    aLogger->Info("运行 时间-操作 {}", time);
}

bool __AOpQueueManager::_CheckWavelength(const ATime& time) {
    if (time.wave < 0 || time.wave > totalWave) {
        aLogger->Error("您当前设定的波次 {} 超出有效范围", time.wave);
        return false;
    }
    if (int wl = queues[time.wave].waveLength; wl != -1) {
        if (wl != time.time)
            aLogger->Error("第 {} 波的波长已被设置为 {}，此次设置无效", time.wave, wl);
        return false;
    }
    std::pair<int, int> bound;
    if (time.wave == totalWave)
        bound = {500, 5999};
    else if (time.wave % 10 == 9)
        bound = {1346, 5245};
    else
        bound = {601, 3100};
    if (time.time < bound.first) {
        aLogger->Error("您当前设定的第 {} 波 的 time 参数为 {}, 超出有效范围", time.wave, time.time);
        return false;
    }
    if (time.time > bound.second)
        aLogger->Warning("您当前设定的第 {} 波 的 time 参数为 {}, 超出有效范围", time.wave, time.time);
    return true;
}

void __AOpQueueManager::_CheckAssumeWavelength(int wave) {
    auto& currentQueue = queues[wave];
    auto& nextQueue = queues[wave + 1];
    auto currentRefreshTime = currentQueue.memRefreshTime;
    auto nextRefreshTime = nextQueue.memRefreshTime;
    std::string errorMsg;
    if (nextRefreshTime == __AOperationQueue::UNINIT) { // 下波的实际时间还未到
        // 计算当前僵尸的血量
        int currentHp = AAsm::ZombieTotalHp(wave - 1);
        int refreshHp = AGetMainObject()->ZombieRefreshHp();
        int totalHp = AGetMainObject()->MRef<int>(0x5598);
        double refreshRatio = double(totalHp - currentHp) / (totalHp - refreshHp);
        errorMsg = std::format("但下一波僵尸尚未刷新，目前僵尸总血量为 {} 刷新血量为 {} 刷新进度为 {:.2f}%",
            currentHp, refreshHp, refreshRatio * 100);
    } else {
        int trueWavelength = nextRefreshTime - currentRefreshTime;
        if (trueWavelength != currentQueue.waveLength)
            errorMsg = std::format("但实际波长为 {}", trueWavelength);
    }

    if (!errorMsg.empty()) { // str 不为空说明有错误
        // 将已经设置的 calRefreshTime 设置回 UNINIT
        for (int w = wave + 1; w <= totalWave + 1; ++w) {
            queues[w].calRefreshTime = __AOperationQueue::UNINIT;
            queues[w].isAssumed = false;
        }
        aLogger->Error("AssumeWavelength : 您第 {} 波假定的波长为 {}, {}", wave, currentQueue.waveLength, errorMsg);
    }
}

void __AOpQueueManager::_BeforeScript() {
    isInitialized = true;
    startTime = ATime(0, __AOperationQueue::UNINIT);
    totalWave = AGetMainObject()->TotalWave();
    queues.clear();
    queues.resize(totalWave + 2);
}

void __AOpQueueManager::_EnterFight() {
    UpdateRefreshTime();
    startTime = ANowTime(false);
}

void __AOpQueueManager::_ExitFight() {
    isInitialized = false;
}

int ANowWave() {
    if (!__aOpQueueManager.isInitialized || AGetPvzBase()->GameUi() != 3)
        return 0;
    return std::max(AGetMainObject()->Wave(), 1);
}

int ANowWave(bool allowNegativeTime) {
    if (!__aOpQueueManager.isInitialized || AGetPvzBase()->GameUi() != 3)
        return 0;
    int wave = AGetMainObject()->Wave();
    if (allowNegativeTime && __aOpQueueManager.queues[wave + 1].memRefreshTime != __AOperationQueue::UNINIT)
        wave++;
    return wave;
}

int ANowTime(int wave) {
    if (AGetPvzBase()->GameUi() != 3) {
        return __AOperationQueue::UNINIT;
    }
    if (!__aOpQueueManager.isInitialized)
        return __AOperationQueue::UNINIT;
    if (wave < 0 || wave >= __aOpQueueManager.queues.size()) {
        aLogger->Error("ANowTime : 波次的合法范围为 [0, {}], 您输入的参数为 {}", __aOpQueueManager.totalWave + 1, wave);
        return __AOperationQueue::UNINIT;
    }
    int refreshTime = __aOpQueueManager.queues[wave].calRefreshTime;
    if (refreshTime == __AOperationQueue::UNINIT)
        return __AOperationQueue::UNINIT;
    return AGetMainObject()->GameClock() - refreshTime;
}

ATime ANowTime() {
    int wave = ANowWave();
    return ATime(wave, ANowTime(wave));
}

ATime ANowTime(bool allowNegativeTime) {
    int wave = ANowWave(allowNegativeTime);
    return ATime(wave, ANowTime(wave));
}

// 得到当前时间的延迟时间
__ANodiscard ATime ANowDelayTime(int delayTime) {
    return ANowTime() + delayTime;
}
