#include "avz_coroutine.h"
#include "avz_connector.h"
#include "avz_memory.h"
#include "avz_exception.h"

std::unordered_set<void*> __ACoHandleManager::_handleSet;

void __ACoHandleManager::_ExitFight()
{
    // 唤醒所有协程
    for (auto&& handle : _handleSet) {
        std::coroutine_handle<>::from_address(handle).resume();
    }
    _handleSet.clear();
}

void __AWait::await_resume()
{
    // 不在战斗界面直接抛出异常
    // 这会直接让协程停止运行
    if (AGetPvzBase()->GameUi() != 3) {
        AExitFight();
    }
}

void __AWait::await_suspend(std::coroutine_handle<> handle)
{
    __ACoHandleManager::Add(handle);
    if (_time.wave == -1) { // pred 形式
        _tickRunner.Start([this, handle, pred = std::move(_predication)] {
            if(pred()) {
                _tickRunner.Stop();
                __ACoHandleManager::Remove(handle);
                handle.resume();
            } }, false);
    } else { // time 形式
        AConnect(_time, [handle] {
            __ACoHandleManager::Remove(handle);
            handle.resume();
        });
    }
}
