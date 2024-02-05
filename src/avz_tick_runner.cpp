/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-15 18:28:35
 * @Description:
 */
#include "avz_tick_runner.h"
#include "avz_memory.h"
#include "avz_script.h"

void __ATickManager::RunQueue()
{
    for (auto&& pool : _priQue) {
        // 不用这种遍历如果中途新增了元素可能会访问非法内存
        for (std::size_t idx = 0; idx < pool.Size(); ++idx) {
            if (pool.IsAlive(idx) && pool[idx].isRunning) {
                pool[idx].operation();
            }
        }
    }
}

void __ATickManager::Remove(int priority, std::size_t idx)
{
    auto&& pool = _priQue[_PriToIdx(priority)];
    if (!pool.Remove(idx)) {
        AGetInternalLogger()->Error("无法移除" + _GetInfoStr(priority, idx));
        return;
    }
    AGetInternalLogger()->Info("移除" + _GetInfoStr(priority, idx));
}

void __ATickManager::_BeforeScript()
{
    if (_runMode == ATickRunner::AFTER_INJECT) {
        return; // 如果是注入之后的运行帧则什么都不做
    }

    for (auto&& pool : _priQue) {
        pool.Clear();
    }
}

std::string __ATickManager::_GetInfoStr(int priority, std::size_t idx)
{
    std::string info = "帧运行 : (模式: ";
    switch (_runMode) {
    case ATickRunner::ONLY_FIGHT:
        info += "ONLY_FIGHT";
        break;
    case ATickRunner::GLOBAL:
        info += "GLOBAL";
        break;
    case ATickRunner::AFTER_INJECT:
        info += "AFTER_INJECT";
        break;
    default:
        info += "UNKNOWN";
        break;
    }
    auto&& pool = _priQue[_PriToIdx(priority)];
    return info + ") (ID: " + std::to_string(pool.GetId(idx))
        + ") (索引: " + std::to_string(idx)
        + ") (优先级: " + std::to_string(priority)
        + ") (池容量: " + std::to_string(pool.Size()) + ")";
}

ATickHandle& ATickHandle::operator=(const ATickHandle& rhs)
{
    _idx = rhs._idx;
    _id = rhs._id;
    _runMode = rhs._runMode;
    _priority = rhs._priority;
    _isStopped = rhs._isStopped;
    return *this;
}

bool ATickHandle::IsStopped() const noexcept
{
    if (_isStopped) {
        return true;
    }
    return !__aig.tickManagers[_runMode].IsAlive(_priority, _idx, _id);
}

void ATickHandle::Pause() noexcept
{
    if (IsStopped()) {
        return;
    }
    __aig.tickManagers[_runMode].At(_priority, _idx).isRunning = false;
}

bool ATickHandle::IsPaused() const noexcept
{
    if (IsStopped()) {
        return true;
    }
    return !__aig.tickManagers[_runMode].At(_priority, _idx).isRunning;
}

void ATickHandle::GoOn() noexcept
{
    if (IsStopped()) {
        AGetInternalLogger()->Error("帧运行已停止，无法继续运行");
        return;
    }
    __aig.tickManagers[_runMode].At(_priority, _idx).isRunning = true;
}

void ATickHandle::Stop()
{
    // 卸载脚本行为会触发 Stop，
    // 此时不应该做任何事
    if (__aScriptManager.willBeExit || IsStopped()) {
        return;
    }
    __aig.tickManagers[_runMode].Remove(_priority, _idx);
    _isStopped = true;
}
