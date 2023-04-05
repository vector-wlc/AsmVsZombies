/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2023-03-25 10:14:38
 * @Description:
 */
#ifndef __AVZ_REPLAY_H__
#define __AVZ_REPLAY_H__
#include "avz_tick_runner.h"
#include <deque>

// 此功能尚未完善, 有内存泄漏的问题, 先不要使用
class __ADeprecated AReplay : public AOrderedStateHook<-1> {
public:
    enum Direction {
        FORWARD, // 向前播放
        BACKWARD // 向后播放
    };

    enum State {
        RECORDING,
        PLAYING,
        REST,
        PAUSED
    };
    void StartRecord(int fps = 30, int maxTickCnt = 1000);
    void StopRecord();
    void Play(Direction direction = FORWARD, int tickCnt = INT_MAX, int fps = 30);
    void Pause();
    void GoOn();
    void Stop();
    State GetState() const { return _state; }

protected:
    std::deque<void*> _tickQueue;
    int _maxTickCnt = 3000;
    int _recordInterval = 30;
    int _playInterval = 30;
    int _playIdx = 0;
    int _playTickCnt = 0;
    uint64_t _tick = 0;
    ATickRunner _tickRunner;
    State _state = REST;
    Direction _direction = FORWARD;
    virtual void _ExitFight() override;
    void _SetNextPlayTick();
};
#endif // ! __AVZ_REPLAY_H__
