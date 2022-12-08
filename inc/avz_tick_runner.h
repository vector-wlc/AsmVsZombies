/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-06 15:34:05
 * @Description:
 */
#ifndef __AVZ_TICK_RUNNER_H__
#define __AVZ_TICK_RUNNER_H__

#include "avz_logger.h"
#include "avz_memory.h"

struct __ATickOperation {
    AOperation operation;
    bool isInGlobal;
    bool isRunning = true;

    __ATickOperation(AOperation&& opertaion, bool isInGlobal)
        : operation(std::move(opertaion))
        , isInGlobal(isInGlobal)
    {
    }

    __ATickOperation(__ATickOperation&& rhs)
    {
        this->operation = std::move(rhs.operation);
        this->isInGlobal = rhs.isInGlobal;
    }

    __ATickOperation& operator=(__ATickOperation&& rhs)
    {
        this->operation = std::move(rhs.operation);
        this->isInGlobal = rhs.isInGlobal;
        return *this;
    }

    __ATickOperation& operator=(const __ATickOperation& rhs)
    {
        this->operation = rhs.operation;
        this->isInGlobal = rhs.isInGlobal;
        return *this;
    }
};

class __ATickManager : public AOrderedStateHook<INT_MIN> {
protected:
    int _idx = INT_MAX;
    bool _isPaused = false;
    bool _isRunning = false;

public:
    using Queue = std::vector<__ATickOperation>;
    static Queue queue;

    static void RunAll();
    static void RunOnlyInGlobal();

    template <typename Op>
        requires __AIsOperation<Op>
    void Start(Op&& operation, bool isInGlobal)
    {
        if (_isRunning) {
            __aInternalGlobal.loggerPtr->Error("ATickRunner 不允许同时运行两个操作");
        }
        _isRunning = true;
        _isPaused = false;
        auto&& tmp = [this, operation = std::forward<Op>(operation)]() mutable {
            if (!_isPaused && !AGameIsPaused()) {
                operation();
            }
        };
        queue.emplace_back(__ATickOperation(std::move(tmp), isInGlobal));
        _idx = queue.size() - 1;
    }

    void Pause() noexcept
    {
        _isPaused = true;
    }

    bool isPaused() const noexcept
    {
        return _isPaused;
    }

    void GoOn() noexcept
    {
        _isPaused = false;
    }

    void Stop() noexcept
    {
        if (_idx < queue.size()) {
            queue[_idx].isRunning = false;
        }
        _isRunning = false;
    }

    bool isStopped() const noexcept
    {
        return !_isRunning;
    }

protected:
    virtual void _BeforeScript() override
    {
        if (!queue.empty()) {
            queue.clear();
        }
        _isRunning = false;
        _isPaused = false;
    }
};

class ATickRunner {
public:
    ATickRunner() = default;
    explicit ATickRunner(AOperation&& operation, bool isInGlobal = false)
    {
        _tickManager.Start(std::move(operation), isInGlobal);
    }
    explicit ATickRunner(const AOperation& operation, bool isInGlobal = false)
    {
        _tickManager.Start(operation, isInGlobal);
    }

    // Start 第一个参数为每帧要运行的函数
    // Start 第二个参数为运行方式
    // 运行方式为 true 时, 在选卡界面和高级暂停时都生效, 反之不生效
    template <class Func>
    void Start(Func&& func, bool isInGlobal = false)
    {
        _tickManager.Start(std::forward<Func>(func), isInGlobal);
    }

    void Pause() noexcept
    {
        _tickManager.Pause();
    }

    bool isPaused() const noexcept
    {
        return _tickManager.isPaused();
    }

    void GoOn() noexcept
    {
        _tickManager.GoOn();
    }

    void Stop() noexcept
    {
        _tickManager.Stop();
    }

    bool isStopped() const noexcept
    {
        return _tickManager.isStopped();
    }

protected:
    __ATickManager _tickManager;
};

class ATickRunnerWithNoStart {
public:
    ATickRunnerWithNoStart() = default;
    explicit ATickRunnerWithNoStart(AOperation&& operation, bool isInGlobal = false)
    {
        _tickManager.Start(std::move(operation), isInGlobal);
    }
    explicit ATickRunnerWithNoStart(const AOperation& operation, bool isInGlobal = false)
    {
        _tickManager.Start(operation, isInGlobal);
    }

    void Pause() noexcept
    {
        _tickManager.Pause();
    }

    bool isPaused() const noexcept
    {
        return _tickManager.isPaused();
    }

    void GoOn() noexcept
    {
        _tickManager.GoOn();
    }

    void Stop() noexcept
    {
        _tickManager.Stop();
    }

    bool isStopped() const noexcept
    {
        return _tickManager.isStopped();
    }

protected:
    __ATickManager _tickManager;
};

#endif