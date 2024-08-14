#ifndef __AVZ_CONNECTOR_H__
#define __AVZ_CONNECTOR_H__

#include "avz_coroutine.h"
#include "avz_tick_runner.h"

class ATimeConnectHandle {
public:
    using TimeIter = std::optional<__ATimeIter>;

    ATimeConnectHandle() = default;
    ATimeConnectHandle(TimeIter iter, const ATime& time)
        : _iter(iter), _time(time) {}

    void Stop();

    operator bool() {
        return _iter.has_value();
    }

protected:
    TimeIter _iter;
    ATime _time;
};

// 创建一条连接, 连接创建成功之后会返回一个类型为 AConnectHandle 的对象作为连接控制器
// *** 特别注意：如果连接创建失败, 连接控制器将被赋值为 nullptr.
// AConnect 最后一个参数为运行方式
// 运行方式为 true 时, AConnect 创建的连接选卡界面和高级暂停时都生效, 反之不生效
// *** 使用示例:
// ALogger<AMsgBox> logger;
// AConnectHandle keyHandle;
//
// void AScript()
// {
//     logger.SetLevel({ALogLevel::INFO});
//
//     // 在时间点 (1, -95) 发两门炮
//     auto timeHandle = AConnect(ATime(1, -95), [=] {
//         aCobManager.Fire({{2, 9}, {5, 9}});
//     });
//     timeHandle.Stop(); // 让上面这个连接失效
//
//
//     // 在当前时间点 100cs 之后开始不断尝试种植小喷菇, 直到小喷菇种植成功
//     // 这里的 ANowDelayTime 会返回一个 ATime 对象
//     AConnect(ANowDelayTime(100), [] {
//         if (AIsSeedUsable(AXPG_8)) {
//             ACard(AXPG_8, 1, 1);
//             return false;
//         }
//         return true;
//     });
//
//
//     // 按下 0 键弹出一个窗口, 显示 hello, 注意 0 是单引号
//     keyHandle = AConnect('0', [] { logger.Info("hello"); });
//
//     // 按下 E 键控制 0 键的是否暂停
//     // 如果 0 键此时暂停生效, 按下 E 键便会继续生效, 反之相反
//     AConnect('E', [] {
//         if (keyHandle.isPaused()) {
//             keyHandle.GoOn();
//         } else {
//             keyHandle.Pause();
//         }
//     });
//
//     // 按下 Q 键控制 0 键行为, 即将 0 键的显示变为 world
//     AConnect('Q', [] {
//         keyHandle.Stop(); // 注意此时 keyHandle 已失效
//         keyHandle = AConnect('0', [] { logger.Info("world"); }); // 此时 keyHandle 重新有效
//     });
//
//     // AConnect 第一个参数还可传入一个 bool Functor(), 如果此函数返回 true, 则会执行后面的操作
//     // 这个示例就是游戏每 10 秒钟显示一个 world 的窗口
//     AConnect([] { return AGetMainObject()->GameClock() % 1000 == 0; }, [] { logger.Info("world"); });
//
// }
template <typename Op>
    requires __AIsOperation<Op>
ATimeConnectHandle AConnect(const ATime& time, Op&& op) {
    auto timeIter = __aOpQueueManager.Push(time, __ABoolOperation(std::forward<Op>(op)));
    return ATimeConnectHandle(timeIter, time);
}

template <typename Op>
    requires __AIsCoroutineOp<Op>
ATimeConnectHandle AConnect(const ATime& time, Op&& op) {
    return AConnect(time, ACoFunctor(std::forward<Op>(op)));
}

template <typename Sess>
    requires __AIsPredicate<Sess>
ATimeConnectHandle AConnect(const ATime& time, Sess&& func) {
    return AConnect(time, [func = std::forward<Sess>(func)]() mutable {
        auto tickRunner = std::make_shared<ATickRunner>();
        tickRunner->Start([func = std::move(func), tickRunner] {
            if(!func())
                tickRunner->Stop(); }, false);
    });
}

using AConnectHandle = ATickHandle;

template <typename Pre, typename Op>
    requires __AIsPredicate<Pre> && __AIsOperation<Op>
AConnectHandle AConnect(Pre&& pre, Op&& op, int runMode = ATickRunner::ONLY_FIGHT, int priority = 0) {
    auto func = [pre = std::forward<Pre>(pre), op = std::forward<Op>(op)]() mutable {
        if (pre())
            op();
    };
    auto&& ret = __aig.tickManagers[runMode].Insert(std::move(func), priority);
    return AConnectHandle(ret.idx, ret.id, runMode, priority);
}

template <typename Pre, typename Op>
    requires __AIsPredicate<Pre> && __AIsCoroutineOp<Op>
AConnectHandle AConnect(Pre&& pre, Op&& op, int runMode = ATickRunner::ONLY_FIGHT, int priority = 0) {
    return AConnect(std::forward<Pre>(pre), ACoFunctor(std::forward<Op>(op)), runMode, priority);
}

class __AKeyManager : public AOrderedExitFightHook<-1> {
public:
    enum KeyState {
        VALID,
        UNKNOWN,
        REPEAT,
    };
    __AKeyManager();
    static KeyState ToValidKey(AKey& key);
    static void AddKey(AKey key, AConnectHandle connectHandle) {
        _keyMap.emplace(key, connectHandle);
    }
    static const std::string& ToName(AKey key) {
        return _keyVec[key];
    }

protected:
    static std::vector<std::string> _keyVec;
    static std::unordered_map<AKey, AConnectHandle> _keyMap;
    virtual void _ExitFight() override {
        _keyMap.clear();
    }
};

inline __AKeyManager __akm; // AStateHook

template <typename Op>
    requires __AIsCoOpOrOp<Op>
AConnectHandle AConnect(AKey key, Op&& op, int priority = 0, int runMode = ATickRunner::GLOBAL) {
    if (__AKeyManager::ToValidKey(key) != __AKeyManager::VALID)
        return AConnectHandle();
    auto keyFunc = [key]() -> bool {
        auto pvzHandle = AGetPvzBase()->MRef<HWND>(0x350);
        return ((GetAsyncKeyState(key) & 0x8001) == 0x8001 && //
            GetForegroundWindow() == pvzHandle);              // 检测 pvz 是否为顶层窗口
    };
    auto handle = AConnect(std::move(keyFunc), std::forward<Op>(op), runMode, priority);
    __AKeyManager::AddKey(key, handle);
    return handle;
}

#endif
