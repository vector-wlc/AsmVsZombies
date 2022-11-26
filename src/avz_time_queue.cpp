/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-10 15:28:33
 * @Description:
 */
#include "avz_time_queue.h"
#include "avz_connector.h"

void __AOperationQueueManager::_SetRefreshTime(int wave, int refreshTime)
{
    auto queueIter = opQueueContainer.begin() + wave;
    queueIter->memRefreshTime = refreshTime;
    queueIter->calRefreshTime = refreshTime;

    auto nextQueueIter = queueIter + 1;
    while (nextQueueIter != opQueueContainer.end() && queueIter->wavelength != -1) {
        nextQueueIter->calRefreshTime = queueIter->calRefreshTime + queueIter->wavelength;
        ++queueIter;
        ++nextQueueIter;
    }
}

__AOperationQueueManager::OperationQueueContainer __AOperationQueueManager::opQueueContainer;
ATime __AOperationQueueManager::startTime; // 脚本设定的开始时间

std::optional<__ATimeIter>
__AOperationQueueManager::Push(const ATime& time, __ABoolOperation&& timeOp)
{
    if (time.wave < startTime.wave || //
        (time.wave == startTime.wave && time.time < startTime.time)) {
        return std::nullopt;
    }

    int nowTime = ANowTime(time.wave);
    if (nowTime >= time.time) { // 现在要立即运行的操作
        _PrintLog(time, nowTime);
        timeOp.operation();
        return std::nullopt; // 无法控制这种操作的运行状态
    }

    auto&& queue = opQueueContainer[time.wave - 1].queue;
    auto ret = queue.emplace(time.time, std::move(timeOp));
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    __aInternalGlobal.loggerPtr->Info("建立 时间-操作 [" + pattern + ", " + pattern + "] 连接成功",
        time.wave, time.time);
    return ret;
}

void __AOperationQueueManager::RunOperation()
{
    int nowTime = __aInternalGlobal.mainObject->GameClock();
    auto totalWave = opQueueContainer.size();
    for (int wave = 1; wave <= totalWave; ++wave) {
        auto&& opQueue = opQueueContainer[wave - 1];
        if (opQueue.calRefreshTime == __AOperationQueue::UNINIT || opQueue.queue.empty()) { // 本波还未到达或者本波没有操作
            continue;
        }

        for (auto iter = opQueue.queue.begin(); iter != opQueue.queue.end(); iter = opQueue.queue.erase(iter)) {
            int queueNowTime = nowTime - opQueue.calRefreshTime;
            if (iter->first > queueNowTime) { // 时间未到达
                break;
            }

            if (iter->second.isStopped) { // 此时间连接已经被废除
                continue;
            }

            _PrintLog(ATime(wave, iter->first), queueNowTime);
            iter->second.operation();
        }
    }
}

void __AOperationQueueManager::UpdateRefreshTime()
{
    auto mainObject = __aInternalGlobal.mainObject;
    int wave = mainObject->Wave();

    auto operationQueueIter = opQueueContainer.begin() + wave;

    if (operationQueueIter->memRefreshTime != __AOperationQueue::UNINIT) { // 已经读取过的不再读取
        return;
    }

    int refreshCountdown = mainObject->RefreshCountdown();
    int gameClock = mainObject->GameClock();

    if (wave == 0) { // 第一波
        _SetRefreshTime(wave, refreshCountdown + gameClock);
        return;
    }

    if ((wave + 1) % 10 != 0) { // 普通波
        if (refreshCountdown <= 200) {
            _SetRefreshTime(wave, refreshCountdown + gameClock);
        }
    } else { // 大波
        if (refreshCountdown <= 200 && refreshCountdown > 5) {
            _SetRefreshTime(wave, refreshCountdown + 745 + gameClock);
        } else if (refreshCountdown <= 5) {
            _SetRefreshTime(wave, mainObject->HugeWaveCountdown() + gameClock);
        }
    }

    // 在 wave != 0 时, 可以由初始刷新倒计时得到当前已刷出波数的刷新时间点
    --operationQueueIter;
    if (operationQueueIter->memRefreshTime == __AOperationQueue::UNINIT) {
        _SetRefreshTime(wave - 1, gameClock - (mainObject->InitialCountdown() - refreshCountdown));
    }
}

void __AOperationQueueManager::SetWavelength(const std::vector<ATime>& lst)
{
    for (const auto& time : lst) {
        if (!_CheckWavelength(time)) {
            continue;
        }
        auto&& timeQueue = opQueueContainer[time.wave - 1];
        timeQueue.wavelength = time.time;
        auto setRefresh = [time]() {
            __aInternalGlobal.mainObject->ZombieRefreshHp() = 0;
            __aInternalGlobal.mainObject->RefreshCountdown() = time.time - 1;
            __aInternalGlobal.mainObject->InitialCountdown() = time.time;
        };

        AConnect(ATime(time.wave, 1), std::move(setRefresh));
    }
}

void __AOperationQueueManager::AssumeWavelength(const std::vector<ATime>& lst)
{
    for (const auto& time : lst) {
        if (!_CheckWavelength(time)) {
            continue;
        }
        auto&& timeQueue = opQueueContainer[time.wave - 1];
        timeQueue.wavelength = time.time;
        auto tmpOp = [time, &timeQueue]() {
            auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
            auto currentRefreshTime = timeQueue.memRefreshTime;
            auto nextRefreshTime = opQueueContainer[time.wave].memRefreshTime;
            std::string str;
            if (nextRefreshTime == __AOperationQueue::UNINIT) { // 下波的实际时间还未到
                str = "但实际刷新点还未知";
            } else {
                auto trueWavelength = nextRefreshTime - currentRefreshTime;
                if (trueWavelength != timeQueue.wavelength) {
                    str = "但实际波长为" + std::to_string(trueWavelength);
                    // 将已经设置的 calRefreshTime 设置回 UNINIT
                    auto totalWave = opQueueContainer.size() - 2;
                    for (int wave = time.wave; wave < totalWave; ++wave) {
                        opQueueContainer[time.wave].calRefreshTime = __AOperationQueue::UNINIT;
                    }
                }
            }

            if (!str.empty()) {
                __aInternalGlobal.loggerPtr->Error(
                    "AssumeWavelength : 您第 " +       //
                        pattern + " 波假定的波长为 " + //
                        pattern + ", " + str,
                    time.wave, timeQueue.wavelength);
            }
        };

        // 本波的波长只有到了下波才知道
        AConnect(ATime(time.wave + 1, -199), std::move(tmpOp));
    }
}

void __AOperationQueueManager::_PrintLog(const ATime& time, int nowTime)
{
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    if (nowTime > time.time) {
        __aInternalGlobal.loggerPtr->Warning("时间-操作 [" + pattern + ", " + pattern + //
                "] 但是现在时间已到 [" + pattern + ", " + pattern + "]",                //
            time.wave, time.time, time.wave, nowTime);
    }

    __aInternalGlobal.loggerPtr->Info("运行 时间-操作 [" + pattern + ", " + pattern + "]",
        time.wave, time.time);
}

bool __AOperationQueueManager::_CheckWavelength(const ATime& time)
{
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    if (time.wave < 1 || ARangeIn(time.wave, {9, 19, 20}) || time.wave > 20) {
        __aInternalGlobal.loggerPtr->Error(
            "您当前设定的 wave 参数为 " + //
                pattern + ", 超出有效范围",
            time.wave);
        return false;
    }

    if (time.time < 601 || time.time > 2510) {
        __aInternalGlobal.loggerPtr->Error(
            "您当前设定的第 " + pattern             //
                + " 波 的 time 参数为 " + pattern + //
                ", 超出有效范围",
            time.wave, time.time);
        return false;
    }

    if (opQueueContainer[time.wave - 1].wavelength != -1) {
        __aInternalGlobal.loggerPtr->Error("您第 " + __aInternalGlobal.loggerPtr->GetPattern() + " 波已经设置过波长，不允许再次设置",
            time.wave);
        return false;
    }
    return true;
}

void __AOperationQueueManager::BeforeScript()
{
    opQueueContainer.clear();
    opQueueContainer.resize(__aInternalGlobal.mainObject->TotalWave());
}

void __AOperationQueueManager::EnterFight()
{
    UpdateRefreshTime();
    startTime = ANowTime();
}

int ANowTime(int wave)
{
    auto maxWave = __AOperationQueueManager::opQueueContainer.size();
    if (wave <= 0 || wave > maxWave) {
        auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
        __aInternalGlobal.loggerPtr->Error("ANowTime 输入参数范围 [1, " + pattern + //
                "], 您输入的参数为 " + pattern + ", 已经超出了此范围",
            maxWave, wave);
        return __AOperationQueue::UNINIT;
    }
    auto refreshTime = __AOperationQueueManager::opQueueContainer[wave - 1].calRefreshTime;
    if (refreshTime == __AOperationQueue::UNINIT) {
        return __AOperationQueue::UNINIT;
    }
    return __aInternalGlobal.mainObject->GameClock() - refreshTime;
}
