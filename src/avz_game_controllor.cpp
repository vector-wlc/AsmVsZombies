/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 16:04:38
 * @Description:
 */
#include "avz_game_controllor.h"
#include "avz_time_queue.h"

bool __AGameControllor::isAdvancedPaused = false;
APredication __AGameControllor::isSkipTick = []() -> bool {
    return false;
};

void __AGameControllor::SkipTick(int wave, int time)
{
    SkipTick([=]() {
        int nowTime = ANowTime(wave);
        if (nowTime == __AOperationQueue::UNINIT || nowTime < time) { // 时间未到达
            return true;
        }
        if (nowTime > time) { // 时间已到达
            __aInternalGlobal.loggerPtr->Error("无法回跳时间点");
        }
        return false;
    });
}
