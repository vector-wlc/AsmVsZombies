#include "libavz.h"

void __ATickManager::RunQueue() {
    for (auto&& pool : _priQue) {
        // 不用这种遍历如果中途新增了元素可能会访问非法内存
        for (std::size_t idx = 0; idx < pool.Size(); ++idx)
            if (pool.IsAlive(idx) && pool[idx].isRunning)
                pool[idx].operation();
    }
}

void __ATickManager::Remove(int priority, std::size_t idx) {
    auto&& pool = _priQue[_PriToIdx(priority)];
    if (!pool.Remove(idx)) {
        aLogger->Error("无法移除 {}", _GetInfoStr(priority, idx));
        return;
    }
    aLogger->Info("移除 {}", _GetInfoStr(priority, idx));
}

void __ATickManager::_BeforeScript() {
    if (_runMode == ATickRunner::AFTER_INJECT)
        return; // 如果是注入之后的运行帧则什么都不做

    for (auto&& pool : _priQue)
        pool.Clear();
}

std::string __ATickManager::_GetInfoStr(int priority, std::size_t idx) {
    std::string mode;
    switch (_runMode) {
    case ATickRunner::ONLY_FIGHT:
        mode = "ONLY_FIGHT";
        break;
    case ATickRunner::GLOBAL:
        mode = "GLOBAL";
        break;
    case ATickRunner::AFTER_INJECT:
        mode = "AFTER_INJECT";
        break;
    case ATickRunner::PAINT:
        mode = "PAINT";
        break;
    default:
        mode = "UNKNOWN";
    }
    auto&& pool = _priQue[_PriToIdx(priority)];
    return std::format("帧运行 : (模式: {}) (ID: {}) (索引: {}) (优先级: {}) (池容量: {})",
        mode, pool.GetId(idx), idx, priority, pool.Size());
}

ATickHandle& ATickHandle::operator=(const ATickHandle& rhs) {
    _idx = rhs._idx;
    _id = rhs._id;
    _runMode = rhs._runMode;
    _priority = rhs._priority;
    _isStopped = rhs._isStopped;
    return *this;
}

bool ATickHandle::IsStopped() const noexcept {
    if (_isStopped)
        return true;
    return !__aig.tickManagers[_runMode].IsAlive(_priority, _idx, _id);
}

void ATickHandle::Pause() noexcept {
    if (IsStopped())
        return;
    __aig.tickManagers[_runMode].At(_priority, _idx).isRunning = false;
}

bool ATickHandle::IsPaused() const noexcept {
    if (IsStopped())
        return true;
    return !__aig.tickManagers[_runMode].At(_priority, _idx).isRunning;
}

void ATickHandle::GoOn() noexcept {
    if (IsStopped()) {
        aLogger->Error("帧运行已停止，无法继续运行");
        return;
    }
    __aig.tickManagers[_runMode].At(_priority, _idx).isRunning = true;
}

void ATickHandle::Stop() {
    // 卸载脚本行为会触发 Stop，
    // 此时不应该做任何事
    if (__aScriptManager.willBeExit || IsStopped())
        return;
    __aig.tickManagers[_runMode].Remove(_priority, _idx);
    _isStopped = true;
}
