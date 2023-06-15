/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2023-03-25 10:14:38
 * @Description:
 */
#ifndef __AVZ_REPLAY_H__
#define __AVZ_REPLAY_H__
#include "avz_tick_runner.h"

class AReplay : AOrderedStateHook<-1> {
public:
    enum Direction {
        FORWARD, // 向前播放
        BACKWARD // 向后播放
    };

    enum State {
        RECORDING,
        PLAYING,
        REST
    };
    AReplay();

    // 开始记录
    // 使用示例：
    // StartRecord(); ------- 开始记录，默认每 10 帧记录一次，默认从序号为 0 的存档开始记录
    // StartRecord(3, 4); ------- 开始记录，每 3 帧记录一次，并且从序号为 4 的存档开始记录
    void StartRecord(int interval = 10, int startIdx = 0);

    // 开始播放
    // 使用示例：
    // StartPlay(); -------- 开始播放，默认每 10 帧播放一帧，默认从第 0 帧开始播放，默认正向播放，
    // StartPlay(3, 100, AReplay::BACKWARD); -------- 开始播放，每 3 帧播放一帧，从第 100 帧开始播放，反向播放
    // StartPlay(3, 100, AReplay::FORWARD); -------- 开始播放，每 3 帧播放一帧，从第 100 帧开始播放，正向播放
    void StartPlay(int interval = 10, int startIdx = 0, Direction direction = FORWARD);

    // 播放设定的帧
    // 使用示例：
    // PlayOneTick(55) ----- 播放第 55 帧
    bool PlayOneTick(int tick);

    // 设定是否使用游戏的刷新函数补帧
    // 使用示例：
    // SetInterpolate(true) ------ 使用补帧，游戏更加流畅但是可能会导致数据不一致的现象
    // SetInterpolate(false) ------ 不使用补帧，每帧播放的都是实际记录的帧但是可能会导致游戏播放不流畅
    void SetInterpolate(bool isInterpolate) { _isInterpolate = isInterpolate; }

    // 设置是否显示提示信息
    // 使用示例：
    // SetShowInfo(true) ------- 显示提示信息
    // SetShowInfo(false) ------- 不显示提示信息
    void SetShowInfo(bool isShowInfo) { _isShowInfo = isShowInfo; }

    // 设置提示信息的位置
    // 使用示例：
    // SetInfoPos(100, 100) ----- 将信息显示的位置设置在游戏窗口的 (100, 100) 处
    void SetInfoPos(int x, int y);

    // 设置存档文件的保存路径
    // 注意存档文件可能会非常大
    // 请注意硬盘的容量
    void SetSavePath(const std::string& path);

    // 设置最大保存帧数
    // 注意：如果不使用此函数，回访对象的默认值为 2000
    // 使用示例：
    // SetMaxSaveCnt(1000); ------- 最大保留 1000 帧存档
    void SetMaxSaveCnt(int maxSaveCnt) { _maxSaveCnt = maxSaveCnt; }

    // 得到目前播放的帧位
    int GetPlayIdx() { return _playIdx; }

    void Pause();
    bool IsPaused();
    void GoOn();
    void Stop();
    State GetState() const { return _state; }
    APainter& GetPainter() { return _painter; }

protected:
    static constexpr auto MAX_SAVE_CNT_KEY = "maxSaveCnt";
    static constexpr auto END_IDX_KEY = "endIdx";
    static constexpr auto TICK_CNT_KEY = "tickCnt";
    int _maxSaveCnt = 2000;
    int _recordInterval = 10;
    int _playInterval = 10;
    int _playIdx = 0;
    int _tickCnt = 0;
    int _endIdx = 0;
    int _tickMs = 10;
    int _showPosX = 0;
    int _showPosY = 0;
    bool _isInterpolate = true;
    bool _isShowInfo = true;
    uint64_t _tick = 0;
    ATickRunner _tickRunner;
    ATickRunner _infoTickRunner;
    State _state = REST;
    Direction _direction = FORWARD;
    void _SetNextPlayTick();
    std::string _savePath;
    APainter _painter;
    virtual void _ExitFight() override { Stop(); }
    void _ReadInfo();
    void _WriteInfo();
};

inline AReplay aReplay;
#endif // ! __AVZ_REPLAY_H__
