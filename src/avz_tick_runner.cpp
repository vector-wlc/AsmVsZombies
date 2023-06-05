/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-15 18:28:35
 * @Description:
 */
#include "avz_tick_runner.h"
#include "avz_memory.h"

void __ATickManager::RunAll()
{
    if (AGameIsPaused()) {
        return;
    }
    for (int idx = 0; idx < _queue.size(); ++idx) { // 不用这种遍历如果中途新增了元素可能会访问非法内存
        if (_queue[idx].isRunning) {
            _queue[idx].operation();
        }
    }
}
void __ATickManager::RunOnlyInGlobal()
{
    if (AGameIsPaused()) {
        return;
    }
    for (int idx = 0; idx < _queue.size(); ++idx) { // 不用这种遍历如果中途新增了元素可能会访问非法内存
        if (_queue[idx].isInGlobal && _queue[idx].isRunning) {
            _queue[idx].operation();
        }
    }
}

void __ATickManager::Remove(int idx)
{
    if (_isClearing) {
        return;
    }
    if (idx >= _queue.size() || idx < 0) {
        __aInternalGlobal.loggerPtr->Error("无法移除 ID 为 " + std::to_string(idx) + " 的帧运行");
        return;
    }
    __aInternalGlobal.loggerPtr->Info("移除 ID 为 " + std::to_string(idx) + " 的帧运行");
    _queue[idx].isRunning = false;
    std::swap(_queue[idx].idx, _nextIdx);
}

void __ATickManager::_BeforeScript()
{
    __aInternalGlobal.tickManager = this;
    _isClearing = true;
    // 这里可能会调用 __ATickManager::Remove
    // 所以要标识 _isClearing
    _queue.clear();
    _isClearing = false;
    _nextIdx = 0;
}

void ATickRunner::Stop() noexcept
{
    // 卸载脚本行为会触发 Stop，
    // 此时不应该做任何事
    if (IsStopped() || !ARangeIn(AGetPvzBase()->GameUi(), {2, 3})) {
        return;
    }
    __aInternalGlobal.tickManager->Remove(_idx);
    _idx = -1;
}
