/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-10 14:23:50
 * @Description:
 */
#ifndef __AVZ_TIME_QUEUE_H__
#define __AVZ_TIME_QUEUE_H__
#include "avz_global.h"
#include "avz_logger.h"
#include "avz_memory.h"
#include <map>
#include <optional>

struct __ABoolOperation {
    AOperation operation;
    bool isStopped = false;

    __ABoolOperation(const AOperation& operation)
        : operation(operation)

    {
    }
    __ABoolOperation(AOperation&& operation)
        : operation(std::move(operation))

    {
    }
    __ABoolOperation(__ABoolOperation&& rhs)
        : operation(std::move(rhs.operation))

    {
    }
    __ABoolOperation(const __ABoolOperation& rhs)
        : operation(rhs.operation)
    {
    }
    __ABoolOperation& operator=(__ABoolOperation&& rhs)
    {
        operation = std::move(rhs.operation);
        return *this;
    }
    __ABoolOperation& operator=(const __ABoolOperation&& rhs)
    {
        operation = rhs.operation;
        return *this;
    }
};

struct __ATimeOperation {
    AOperation operation;
    ATime time;
    __ATimeOperation(AOperation&& operation, const ATime& time)
        : operation(std::move(operation))
        , time(time)
    {
    }
    __ATimeOperation(__ATimeOperation&& rhs)
        : operation(std::move(rhs.operation))
        , time(rhs.time)
    {
    }
};

class __AOperationQueue {
public:
    using RunOrderQueue = std::multimap<int, __ABoolOperation>;
    constexpr static int UNINIT = INT_MIN;
    RunOrderQueue queue;
    int calRefreshTime = UNINIT; // 计算得到的刷新时间，操作队列都用此时间
    int memRefreshTime = UNINIT; // 通过读内存得到的真实刷新时间，用于检查用户设定的波长是否正确
    int wavelength = -1;
};

using __ATimeIter = __AOperationQueue::RunOrderQueue::iterator;

class __AOpQueueManager : public AOrderedBeforeScriptHook<INT_MIN>, //
                          public AOrderedEnterFightHook<INT_MIN> {
public:
    using Container = std::vector<__AOperationQueue>;
    Container container;
    ATime startTime; // 脚本设定的开始时间
    std::optional<__ATimeIter> Push(const ATime& time, __ABoolOperation&& timeOp);
    void UpdateRefreshTime();
    void SetWavelength(const std::vector<ATime>& lst);
    void AssumeWavelength(const std::vector<ATime>& lst);
    void RunOperation();

protected:
    void _PrintLog(const ATime& time, int nowTime);
    bool _CheckWavelength(const ATime& time);
    void _CheckAssumeWavelength(int wave);
    void _SetRefreshTime(int wave, int refreshTime);
    virtual void _BeforeScript() override;
    virtual void _EnterFight() override;
};

inline __AOpQueueManager __aOpQueueManager;

// 得到当前游戏的波数
__ANodiscard inline int ANowWave()
{
    if (AGetPvzBase()->GameUi() != 3) {
        return 1;
    }
    return std::max(AGetMainObject()->Wave(), 1);
}

// 得到当前时间，读取失败返回 INT_MIN
// *** 注意得到的是以参数波刷新时间点为基准的相对时间
// *** 使用示例：
// ANowTime(1) -------- 得到以第一波刷新时间点为基准的当前时间
// ANowTime(2) -------- 得到以第二波刷新时间点为基准的当前时间
__ANodiscard int ANowTime(int wave);

// 得到当前时间，读取失败返回 [currentWave, INT_MIN]
__ANodiscard inline ATime ANowTime()
{
    auto wave = ANowWave();
    return ATime(wave, ANowTime(wave));
}

// 得到当前时间的延迟时间
__ANodiscard inline int ANowDelayTime(int wave, int delayTime)
{
    return ANowTime(wave) + delayTime;
}

// 得到当前时间的延迟时间
__ANodiscard inline ATime ANowDelayTime(int delayTime)
{
    auto time = ANowTime();
    time.time += delayTime;
    return time;
}

// 设定特定波的波长
// *** 注意: 对最后一波无效
// 第 9、19、... 波的合法波长范围为 1346 - 5245，其他波为 601 - 3100
// *** 使用示例：
// ASetWavelength({ATime(1, 601), ATime(4, 1000)}) ----- 将第一波的波长设置为 601，将第四波的波长设置为 1000
inline void ASetWavelength(const std::vector<ATime>& lst)
{
    __aOpQueueManager.SetWavelength(lst);
}

// 假定特定波的波长
// 本函数与 ASetWavelength 区别在于, 本函数不会对内存进行修改
// 只是可以让时间点的书写范围小于 -200, 如果真实的波长与假定的波长不一致, 则会报错
// *** 使用示例：
// AAssumeWavelength({ATime(1, 601), ATime(4, 1000)}) ----- 将第一波的波长假定为 601，将第四波的波长假定为 1000
inline void AAssumeWavelength(const std::vector<ATime>& lst)
{
    __aOpQueueManager.AssumeWavelength(lst);
}

#endif