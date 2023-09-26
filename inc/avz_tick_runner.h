/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-06 15:34:05
 * @Description:
 */
#ifndef __AVZ_TICK_RUNNER_H__
#define __AVZ_TICK_RUNNER_H__

#include "avz_logger.h"
#include "avz_coroutine.h"
#include <array>

struct __ATickOperation {
    AOperation operation;
    bool isRunning;
    int idx = 0;

    template <typename Op>
        requires __AIsOperation<Op>
    __ATickOperation(Op&& opertaion, int idx)
        : operation(std::forward<Op>(opertaion))
        , isRunning(true)
        , idx(idx)
    {
    }

    __ATickOperation(__ATickOperation&& rhs)
    {
        this->operation = std::move(rhs.operation);
        __CopyOthers(rhs);
    }

    __ATickOperation& operator=(__ATickOperation&& rhs)
    {
        this->operation = std::move(rhs.operation);
        __CopyOthers(rhs);
        return *this;
    }

    __ATickOperation& operator=(const __ATickOperation& rhs)
    {
        this->operation = rhs.operation;
        __CopyOthers(rhs);
        return *this;
    }

private:
    template <typename T>
    void __CopyOthers(T&& rhs)
    {
        this->isRunning = rhs.isRunning;
        this->idx = rhs.idx;
    }
};

class __ATickManager : public AOrderedStateHook<INT_MIN> {
public:
    static constexpr int PRIORITY_SIZE = 41;
    using Queue = std::array<std::vector<__ATickOperation>, PRIORITY_SIZE>;

    void RunQueue();

    template <typename Op>
        requires __AIsOperation<Op>
    int Add(Op&& op, int priority)
    {
        auto&& que = _priQue[_PriToIdx(priority)];
        auto retIdx = _nextIdx;
        if (_nextIdx < que.size()) { // 直接用之前的缓存，不必开新空间
            que[_nextIdx].isRunning = true;
            que[_nextIdx].operation = std::forward<Op>(op);
            std::swap(que[_nextIdx].idx, _nextIdx);
        } else { // 需要新的空间
            que.emplace_back(
                __ATickOperation(std::forward<Op>(op), _nextIdx));
            ++_nextIdx;
        }
        AGetInternalLogger()->Info("增加优先级为 " + std::to_string(priority)
            + ", ID 为 " + std::to_string(retIdx) + " 的帧运行");
        return retIdx;
    }

    template <typename Op>
        requires __AIsCoroutineOp<Op>
    int Add(Op&& op, int priority)
    {
        return this->Add(ACoFunctor(std::forward<Op>(op)), priority);
    }

    __ATickOperation& At(int priority, int idx) { return _priQue[_PriToIdx(priority)][idx]; }
    void Remove(int priority, int idx);

protected:
    virtual void _BeforeScript() override;
    static int _PriToIdx(int priority)
    {
        return priority + PRIORITY_SIZE / 2;
    }
    bool _isClearing = false;
    int _nextIdx;
    Queue _priQue;
};

class ATickRunner : public AOrderedStateHook<-1> {
protected:
    int _idx = -1;
    int _priority = 0;
    __ATickManager* _tickManager = nullptr;

public:
    ATickRunner() = default;
    ~ATickRunner() { Stop(); }
    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    explicit ATickRunner(Op&& op, bool isInGlobal = false, int priority = 0)
    {
        Start(std::forward<Op>(op), isInGlobal, priority);
    }

    // Start 第一个参数为每帧要运行的函数
    // Start 第二个参数为运行方式
    // 运行方式为 true 时, 在选卡界面和高级暂停时都生效, 反之不生效
    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    void Start(Op&& operation, bool isInGlobal = false, int priority = 0)
    {
        if (!IsStopped()) {
            AGetInternalLogger()->Error("ATickRunner 不允许同时运行两个操作");
            return;
        }
        constexpr auto PRI_MAX = __ATickManager::PRIORITY_SIZE / 2;
        if (_priority < -PRI_MAX || _priority > PRI_MAX) {
            AGetInternalLogger()->Error("优先级设置范围为 [#, #], 您设置的优先级数值为 #, 已溢出", -PRI_MAX, PRI_MAX, priority);
            return;
        }
        _priority = priority;
        _tickManager = isInGlobal ? __aInternalGlobal.tickInGlobal : __aInternalGlobal.tickInFight;
        _idx = _tickManager->Add(std::forward<Op>(operation), _priority);
    }

    void Pause() noexcept
    {
        _tickManager->At(_priority, _idx).isRunning = false;
    }

    bool IsPaused() const noexcept
    {
        return !_tickManager->At(_priority, _idx).isRunning;
    }

    void GoOn() noexcept
    {
        _tickManager->At(_priority, _idx).isRunning = true;
    }

    void Stop() noexcept;

    bool IsStopped() const noexcept
    {
        return _idx == -1;
    }

    __ADeprecated bool isStopped() const noexcept
    {
        return IsStopped();
    }
    __ADeprecated bool isPaused() const noexcept
    {
        return IsPaused();
    }

protected:
    virtual void _BeforeScript() override
    {
        _idx = -1;
    }
};

class ATickRunnerWithNoStart : protected ATickRunner {
public:
    ATickRunnerWithNoStart() = default;
    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    explicit ATickRunnerWithNoStart(Op&& op, bool isInGlobal = false, int priority = 0)
    {
        Start(std::forward<Op>(op), isInGlobal, priority);
    }

    using ATickRunner::GoOn;
    using ATickRunner::IsPaused;
    using ATickRunner::isPaused;
    using ATickRunner::IsStopped;
    using ATickRunner::isStopped;
    using ATickRunner::Pause;
    using ATickRunner::Stop;
};

#endif