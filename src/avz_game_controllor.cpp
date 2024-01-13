/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 16:04:38
 * @Description:
 */
#include "avz_game_controllor.h"
#include "avz_time_queue.h"

bool __AGameControllor::_CheckSkipTick()
{
    if (isAdvancedPaused) {
        __aig.loggerPtr->Error("开启高级暂停或者暂停时不能启用跳帧");
        return false;
    }

    if (isSkipTick()) {
        __aig.loggerPtr->Error("请等待上一个跳帧条件达到后的下一帧再设定跳帧条件");
        return false;
    }

    return true;
}
void __AGameControllor::SkipTick(int wave, int time)
{
    SkipTick([=]() {
        int nowTime = ANowTime(wave);
        if (nowTime == __AOperationQueue::UNINIT || nowTime < time) { // 时间未到达
            return true;
        }
        if (nowTime > time) { // 时间已到达
            __aig.loggerPtr->Error("无法回跳时间点");
        }
        return false;
    });
}
