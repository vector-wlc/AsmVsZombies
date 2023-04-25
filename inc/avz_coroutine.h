#ifndef __AVZ_COROUTINE_H__
#define __AVZ_COROUTINE_H__

#include "avz_types.h"
#include <coroutine>
#include <unordered_set>
#include "avz_state_hook.h"
#include "avz_logger.h"
#include "avz_tick_runner.h"

class __ACoHandleManager : public AOrderedStateHook<-1> {
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

struct ACoroutine {
    struct promise_type {
        auto get_return_object() { return ACoroutine {}; }
        auto initial_suspend() { return std::suspend_never {}; }
        auto final_suspend() noexcept
        {
            AGetInternalLogger()->Info("协程退出");
            return std::suspend_never {};
        }
        void unhandled_exception() { }
        void return_void() { }
    };
};

inline __ACoHandleManager __aCoHandleManager;

class __AWait {
public:
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
    bool await_ready() const { return false; }
    void await_resume();
    void await_suspend(std::coroutine_handle<> handle);

private:
    ATime _time;
    APredication _predication;
    ATickRunner _tickRunner;
};

template <typename Func>
    requires __AIsPredication<Func>
inline auto operator co_await(Func&& func)
{
    return __AWait(std::forward<Func>(func));
}

inline auto operator co_await(const ATime& time)
{
    return __AWait(time);
}

#endif