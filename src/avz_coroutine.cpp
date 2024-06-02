#include "avz_coroutine.h"
#include "avz_connector.h"
#include "avz_memory.h"
#include "avz_exception.h"
#include "avz_tick_runner.h"
#include "avz_script.h"

std::unordered_set<void*> __ACoHandleManager::_handleSet;

void __ACoHandleManager::_ExitFight()
{
    // 唤醒所有协程
    for (auto&& handle : _handleSet) {
        std::coroutine_handle<>::from_address(handle).destroy();
    }
    _handleSet.clear();
}

bool __AWait::await_ready() const
{
    // 检测一下是否真的需要将协程挂起
    if (_time.wave == -1) { // pred 形式
        if (_predication()) {
            return true;
        }
    } else { // time 形式
        int nowTime = ANowTime(_time.wave);
        if (nowTime > _time.time) {
            __aig.loggerPtr->Warning("co_await 等待的时间为 ["
                + std::to_string(_time.wave) + "," + std::to_string(_time.time)
                + "], 但是现在时间已到 [" + std::to_string(_time.wave) + "," + std::to_string(nowTime) + "]");
        }
        if (nowTime >= _time.time) {
            return true;
        }
    }
    return false;
}

void __AWait::await_resume()
{
    // 不在战斗界面直接抛出异常
    // 这会直接让协程停止运行
    if (AGetPvzBase()->MainObject() == nullptr || AGetPvzBase()->GameUi() != 3) {
        AExitFight();
    }
}

void __AWait::await_suspend(std::coroutine_handle<> handle)
{
    AWaitForFight();
    __ACoHandleManager::Add(handle);
    if (_time.wave == -1) { // pred 形式
        auto tickRunner = std::make_shared<ATickRunner>();
        tickRunner->Start([handle, pred = std::move(_predication), tickRunner] {
            if(pred()) {
                __ACoHandleManager::Remove(handle);
                handle.resume();
                tickRunner->Stop();
            } }, false);
    } else { // time 形式
        auto func = [handle] {
            __ACoHandleManager::Remove(handle);
            handle.resume();
        };
        if (!AConnect(_time, func)) {
            func();
        }
    }
}

void ACoFunctor::operator()()
{
    if (_functor == nullptr) {
        return;
    }
    auto co = (*_functor)();
    co.SetPtr(_functor);
}
