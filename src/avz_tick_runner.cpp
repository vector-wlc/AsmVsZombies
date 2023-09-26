/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-15 18:28:35
 * @Description:
 */
#include "avz_tick_runner.h"
#include "avz_memory.h"

void __ATickManager::RunQueue()
{
    for (auto&& que : _priQue) {
        // 不用这种遍历如果中途新增了元素可能会访问非法内存
        for (int idx = 0; idx < que.size(); ++idx) {
            if (que[idx].isRunning) {
                que[idx].operation();
            }
        }
    }
}

void __ATickManager::Remove(int priority, int idx)
{
    if (_isClearing) {
        return;
    }
    auto&& que = _priQue[_PriToIdx(priority)];
    if (idx >= que.size() || idx < 0) {
        AGetInternalLogger()->Error("无法移除优先级为 " + std::to_string(priority) + ", ID 为 " + std::to_string(idx) + " 的帧运行");
        return;
    }
    AGetInternalLogger()->Info("移除优先级为 " + std::to_string(priority) + ", ID 为 " + std::to_string(idx) + " 的帧运行");
    que[idx].isRunning = false;
    std::swap(que[idx].idx, _nextIdx);
}

void __ATickManager::_BeforeScript()
{
    _isClearing = true;
    // 这里可能会调用 __ATickManager::Remove
    // 所以要标识 _isClearing
    for (auto&& que : _priQue) {
        que.clear();
    }
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
    _tickManager->Remove(_priority, _idx);
    _idx = -1;
}
