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

struct __ATickOperation {
    AOperation operation;
    bool isInGlobal;
    bool isRunning;
    int idx = 0;

    template <typename Op>
        requires __AIsOperation<Op>
    __ATickOperation(Op&& opertaion, bool isInGlobal, int idx)
        : operation(std::forward<Op>(opertaion))
        , isInGlobal(isInGlobal)
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
    void __CopyOthers(const __ATickOperation& rhs)
    {
        this->isInGlobal = rhs.isInGlobal;
        this->isRunning = rhs.isRunning;
        this->idx = rhs.idx;
    }
};

class __ATickManager : public AOrderedStateHook<INT_MIN> {
public:
    using Queue = std::vector<__ATickOperation>;

    void RunAll();
    void RunOnlyInGlobal();

    template <typename Op>
        requires __AIsOperation<Op>
    int Add(Op&& op, bool isInGlobal)
    {
        auto retIdx = _nextIdx;
        if (_nextIdx < _queue.size()) { // 直接用之前的缓存，不必开新空间
            _queue[_nextIdx].isInGlobal = isInGlobal;
            _queue[_nextIdx].isRunning = true;
            _queue[_nextIdx].operation = std::forward<Op>(op);
            std::swap(_queue[_nextIdx].idx, _nextIdx);
        } else { // 需要新的空间
            _queue.emplace_back(
                __ATickOperation(std::forward<Op>(op), isInGlobal, _nextIdx));
            ++_nextIdx;
        }
        __aInternalGlobal.loggerPtr->Info("增加 ID 为 " + std::to_string(retIdx) + " 的帧运行");
        return retIdx;
    }

    template <typename Op>
        requires __AIsCoroutineOp<Op>
    int Add(Op&& op, bool isInGlobal)
    {
        return this->Add(ACoFunctor(std::forward<Op>(op)), isInGlobal);
    }

    __ATickOperation& At(int idx) { return _queue[idx]; }
    void Remove(int idx);

protected:
    virtual void _BeforeScript() override;
    bool _isClearing = false;
    int _nextIdx;
    Queue _queue;
};

inline __ATickManager __aTickManager;

class ATickRunner : public AOrderedStateHook<-1> {
protected:
    int _idx = -1;

public:
    ATickRunner() = default;
    ~ATickRunner() { Stop(); }
    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    explicit ATickRunner(Op&& op, bool isInGlobal = false)
    {
        Start(std::forward<Op>(op), isInGlobal);
    }

    // Start 第一个参数为每帧要运行的函数
    // Start 第二个参数为运行方式
    // 运行方式为 true 时, 在选卡界面和高级暂停时都生效, 反之不生效
    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    void Start(Op&& operation, bool isInGlobal = false)
    {
        if (!IsStopped()) {
            __aInternalGlobal.loggerPtr->Error("ATickRunner 不允许同时运行两个操作");
            return;
        }
        _idx = __aInternalGlobal.tickManager->Add(std::forward<Op>(operation), isInGlobal);
    }

    void Pause() noexcept
    {
        __aInternalGlobal.tickManager->At(_idx).isRunning = false;
    }

    bool IsPaused() const noexcept
    {
        return !__aInternalGlobal.tickManager->At(_idx).isRunning;
    }

    void GoOn() noexcept
    {
        __aInternalGlobal.tickManager->At(_idx).isRunning = true;
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
    explicit ATickRunnerWithNoStart(Op&& op, bool isInGlobal = false)
    {
        Start(std::forward<Op>(op), isInGlobal);
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