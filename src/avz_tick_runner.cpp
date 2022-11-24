/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-15 18:28:35
 * @Description:
 */
#include "avz_tick_runner.h"

void __ATickQueue::RunAll()
{
    auto iter = queue.begin();
    while (iter != queue.end()) {
        if (iter->isRunning) {
            iter->operation();
            ++iter;
        } else {
            iter = queue.erase(iter);
        }
    }
}
void __ATickQueue::RunOnlyInGlobal()
{
    auto iter = queue.begin();
    while (iter != queue.end()) {
        if (!iter->isInGlobal) {
            ++iter;
            continue;
        }

        if (iter->isRunning) {
            iter->operation();
            ++iter;
        } else {
            iter = queue.erase(iter);
        }
    }
}

__ATickQueue __ATickManager::tickQueue;
