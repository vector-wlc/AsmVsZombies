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

    template <typename Op>
        requires __AIsOperation<Op>
    __ATickOperation(Op&& operation)
        : operation(std::forward<Op>(operation))
        , isRunning(true)
    {
    }

    __ATickOperation(__ATickOperation&& rhs)
    {
        this->operation = std::move(rhs.operation);
        this->isRunning = rhs.isRunning;
    }

    __ATickOperation& operator=(__ATickOperation&& rhs)
    {
        this->operation = std::move(rhs.operation);
        this->isRunning = rhs.isRunning;
        return *this;
    }

    __ATickOperation& operator=(const __ATickOperation& rhs)
    {
        this->operation = rhs.operation;
        this->isRunning = rhs.isRunning;
        return *this;
    }
};

// 一个非常简单的对象池
template <typename Obj>
class __AObjectPool {
public:
    struct ObjInfo {
        Obj obj;
        std::size_t idx;
        std::size_t id;
    };

protected:
    std::vector<ObjInfo> _pool;
    std::size_t _nextIdx = 0;
    bool _isClearing = false;

public:
    template <typename... Args>
    ObjInfo& Insert(Args&&... args)
    {
        auto newIdx = _nextIdx;
        auto newId = ++__aig.objId;
        if (_nextIdx < _pool.size()) { // 直接用之前的缓存，不必开新空间
            auto&& [obj, idx, id] = _pool[newIdx];
            obj = Obj(std::forward<Args>(args)...);
            std::swap(idx, _nextIdx);
            id = newId;
        } else { // 需要新的空间
            _pool.push_back({Obj(std::forward<Args>(args)...), newIdx, newId});
            ++_nextIdx;
        }
        return _pool[newIdx];
    }

    Obj& operator[](std::size_t idx) noexcept
    {
        return _pool[idx].obj;
    }

    const Obj& operator[](std::size_t idx) const noexcept
    {
        return _pool[idx].obj;
    }

    std::size_t Size() const noexcept
    {
        return _pool.size();
    }

    bool Remove(std::size_t idx)
    {
        if (_isClearing) {
            return true;
        }
        if (idx >= _pool.size()) {
            return false;
        }
        std::swap(_pool[idx].idx, _nextIdx);
        return true;
    }

    void Clear()
    {
        // 这里可能会调用 Remove
        // 所以要标识 _isClearing
        _isClearing = true;
        _nextIdx = 0;
        _pool.clear();
        _isClearing = false;
    }

    // 仅检查 idx 处的对象是否存活
    bool IsAlive(std::size_t idx) const noexcept
    {
        if (idx >= _pool.size()) {
            return false;
        }
        return _pool[idx].idx == idx;
    }

    // 检查 idx 处的对象是否存活并且是否与相应的 id 匹配
    bool IsAlive(std::size_t idx, std::size_t id) const noexcept
    {
        if (idx >= _pool.size()) {
            return false;
        }
        return _pool[idx].idx == idx && _pool[idx].id == id;
    }

    std::size_t GetId(std::size_t idx) const noexcept
    {
        return _pool[idx].id;
    }
};

class __ATickManager : public AOrderedBeforeScriptHook<INT_MIN> {
public:
    static constexpr int PRIORITY_SIZE = 41;
    using TickObjPool = __AObjectPool<__ATickOperation>;
    using Queue = std::array<TickObjPool, PRIORITY_SIZE>;

    void RunQueue();

    template <typename Op>
        requires __AIsOperation<Op>
    TickObjPool::ObjInfo& Insert(Op&& op, int priority)
    {
        auto&& pool = _priQue[_PriToIdx(priority)];
        auto&& ret = pool.Insert(std::forward<Op>(op));
        AGetInternalLogger()->Info("增加" + _GetInfoStr(priority, ret.idx));
        return ret;
    }

    template <typename Op>
        requires __AIsCoroutineOp<Op>
    TickObjPool::ObjInfo& Insert(Op&& op, int priority)
    {
        return this->Insert(ACoFunctor(std::forward<Op>(op)), priority);
    }

    __ATickOperation& At(int priority, std::size_t idx) { return _priQue[_PriToIdx(priority)][idx]; }
    void Remove(int priority, std::size_t idx);
    void SetRunMode(int runMode) { _runMode = runMode; }
    bool IsAlive(int priority, std::size_t idx, std::size_t id) { return _priQue[_PriToIdx(priority)].IsAlive(idx, id); }

protected:
    virtual void _BeforeScript() override;
    static int _PriToIdx(int priority)
    {
        return priority + PRIORITY_SIZE / 2;
    }
    std::string _GetInfoStr(int priority, std::size_t idx);
    Queue _priQue;
    int _runMode = 0;
};

class ATickHandle {
public:
    ATickHandle() = default;
    explicit ATickHandle(std::size_t idx, std::size_t id, int runMode, int priority)
        : _idx(idx)
        , _id(id)
        , _runMode(runMode)
        , _priority(priority)
        , _isStopped(false)
    {
    }

    ATickHandle(const ATickHandle& rhs)
        : _idx(rhs._idx)
        , _id(rhs._id)
        , _runMode(rhs._runMode)
        , _priority(rhs._priority)
        , _isStopped(rhs._isStopped)
    {
    }

    ATickHandle& operator=(const ATickHandle& rhs);

    bool IsStopped() const noexcept;

    void Pause() noexcept;

    bool IsPaused() const noexcept;

    void GoOn() noexcept;

    void Stop();

    __ADeprecated("请使用 IsStopped()") bool isStopped() const noexcept
    {
        return IsStopped();
    }
    __ADeprecated("请使用 IsPaused()") bool isPaused() const noexcept
    {
        return IsPaused();
    }

protected:
    std::size_t _idx = 0;
    std::size_t _id = 0;
    int _runMode = 0;
    int _priority = 0;
    bool _isStopped = false; // 此变量必须存在，因为中途切换了模式，会让查表失效
};

class ATickRunnerWithNoStart : public ATickHandle {
    __ADeleteCopyAndMove(ATickRunnerWithNoStart);

public:
    enum RunMode {
        ONLY_FIGHT,   // 只在战斗界面运行，大多数情况下使用此模式
        GLOBAL,       // 在选卡界面和战斗界面运行，少数情况下使用此模式
        AFTER_INJECT, // 在注入之后一直运行，除非调用 Stop 停止，注意此模式非常危险，因为框架还没有完成初始化
        __COUNT       // 计数器，没有其他作用
    };

    ATickRunnerWithNoStart() = default;
    ~ATickRunnerWithNoStart() { Stop(); }

    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    explicit ATickRunnerWithNoStart(Op&& op, int runMode = ONLY_FIGHT, int priority = 0)
    {
        _Start(std::forward<Op>(op), runMode, priority);
    }

protected:
    // Start 第一个参数为每帧要运行的函数
    // Start 第二个参数为运行方式
    // 运行方式为 true 时, 在选卡界面和高级暂停时都生效, 反之不生效
    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    void _Start(Op&& operation, int runMode = ONLY_FIGHT, int priority = 0)
    {
        constexpr auto PRI_MAX = __ATickManager::PRIORITY_SIZE / 2;
        if (_priority < -PRI_MAX || _priority > PRI_MAX) {
            AGetInternalLogger()->Error("优先级设置范围为 [{}, {}], 您设置的优先级数值为 {}, 已溢出", -PRI_MAX, PRI_MAX, priority);
            return;
        }
        _runMode = runMode;
        _priority = priority;
        auto&& ret = __aig.tickManagers[_runMode].Insert(std::forward<Op>(operation), _priority);
        _idx = ret.idx;
        _id = ret.id;
        _isStopped = false;
    }
};

class ATickRunner : public ATickRunnerWithNoStart {
    __ADeleteCopyAndMove(ATickRunner);

public:
    ATickRunner()
    {
        _isStopped = true;
    }

    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    explicit ATickRunner(Op&& op, int runMode = ONLY_FIGHT, int priority = 0)
    {
        _Start(std::forward<Op>(op), runMode, priority);
    }

    template <typename Op>
        requires __AIsCoOpOrOp<Op>
    void Start(Op&& op, int runMode = ONLY_FIGHT, int priority = 0)
    {
        if (!IsStopped()) {
            AGetInternalLogger()->Error("ATickRunner 不允许同时运行两个操作");
            return;
        }
        _Start(std::forward<Op>(op), runMode, priority);
    }
};

#endif