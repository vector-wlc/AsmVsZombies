#ifndef __AVZ_COROUTINE_H__
#define __AVZ_COROUTINE_H__

#include <coroutine>
#include <unordered_set>
#include "avz_state_hook.h"
#include "avz_time_queue.h"

class __ACoHandleManager : public AOrderedExitFightHook<-10> {
public:
    static void Add(std::coroutine_handle<> handle)
    {
        _handleSet.emplace(handle.address());
    }

    static void Remove(std::coroutine_handle<> handle)
    {
        _handleSet.erase(handle.address());
    }

protected:
    static std::unordered_set<void*> _handleSet;
    virtual void _ExitFight() override;
};

class __AWait {
public:
    __AWait() = default;
    __AWait(const ATime& time)
        : _time(time)
    {
    }

    template <typename Func>
        requires __AIsPredication<Func>
    __AWait(Func&& func)
        : _time(ATime(-1, -1))
        , _predication(std::forward<Func>(func))
    {
    }
    bool await_ready() const;
    void await_resume();
    void await_suspend(std::coroutine_handle<> handle);

private:
    ATime _time;
    APredication _predication;
};

#define __ACoNodiscard [[nodiscard("\n裸启动协程会导致内存访问错误问题, 请使用以下方式安全启动协程 " \
                                   "\n1. 立即启动 : ACoLaunch(协程函数名);"                                              \
                                   "\n2. 连接启动 : AConnect(时间/条件, 协程函数名)")]]

struct __ACoNodiscard ACoroutine {

#undef __ACoNodiscard
    struct promise_type {
        std::shared_ptr<std::function<ACoroutine()>> ptr;
        __AWait await_transform(int delayTime)
        {
            return ANowDelayTime(delayTime);
        }
        __AWait await_transform(const ATime& time)
        {
            return time;
        }
        template <typename Func>
            requires __AIsPredication<Func>
        __AWait await_transform(Func&& func)
        {
            return std::forward<Func>(func);
        }
        auto get_return_object()
        {
            return ACoroutine {std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        auto initial_suspend()
        {
            return std::suspend_always {};
        }
        auto final_suspend() noexcept
        {
            return std::suspend_never {};
        }
        void unhandled_exception() { }
        void return_void() { }
        ~promise_type()
        {
            aLogger->Info("协程退出");
        }
    };

    ACoroutine(std::coroutine_handle<promise_type> handle)
        : _handle(handle)
    {
    }

    void SetPtr(std::shared_ptr<std::function<ACoroutine()>>& ptr)
    {
        _handle.promise().ptr = ptr;
        _handle.resume();
    }

protected:
    std::coroutine_handle<promise_type> _handle;
};

inline __ACoHandleManager __aCoHandleManager;

using ACoroutineOp = std::function<ACoroutine()>;

template <typename T>
concept __AIsCoroutineOp = std::is_convertible_v<T, ACoroutineOp> && __ACheckRet<T, ACoroutine>;

// 判断此类型是协程函数或者普通函数
template <typename T>
concept __AIsCoOpOrOp = __AIsCoroutineOp<T> || __AIsOperation<T>;

class ACoFunctor {
public:
    template <typename Op>
        requires __AIsCoroutineOp<Op>
    ACoFunctor(Op&& op)
    {
        _functor = std::make_shared<ACoroutineOp>(std::forward<Op>(op));
    }

    ACoFunctor(ACoFunctor&& rhs)
        : _functor(std::move(rhs._functor))
    {
    }

    ACoFunctor(const ACoFunctor& rhs)
        : _functor(rhs._functor)
    {
    }

    ACoFunctor& operator=(ACoFunctor&& rhs)
    {
        this->_functor = std::move(rhs._functor);
        return *this;
    }

    ACoFunctor& operator=(const ACoFunctor& rhs)
    {
        this->_functor = rhs._functor;
        return *this;
    }

    void operator()();

protected:
    std::shared_ptr<ACoroutineOp> _functor = nullptr;
};

#define ACoLaunch(...) ACoFunctor {__VA_ARGS__}()

#endif