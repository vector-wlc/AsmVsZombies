#include "avz_replay.h"
#include "avz_asm.h"
#include "avz_game_controllor.h"
#include "avz_memory.h"
#include "avz_script.h"

void AReplay::StartRecord(int fps, int maxTickCnt)
{
    if (fps < 1 || fps > 100) {
        __aInternalGlobal.loggerPtr->Error("AReplay::Play : fps 的范围为 [1, 100], 您当前的 fps 为: " + std::to_string(fps));
        return;
    }
    _recordInterval = 100 / fps;
    _maxTickCnt = maxTickCnt;
    if (_state != REST) {
        __aInternalGlobal.loggerPtr->Error("请先停止 Replay 的运行再进行记录");
        return;
    }
    _state = RECORDING;
    auto tmp = [this] {
        if (AGetMainObject()->GameClock() % _recordInterval != 0) {
            return;
        }
        for (; _tickQueue.size() >= _maxTickCnt;) {
            AAsm::FreeMemory(_tickQueue.front());
            _tickQueue.pop_front();
        }
        _tickQueue.push_back(AAsm::SaveToMemory());
    };
    _tickRunner.Start(std::move(tmp), false);
}

void AReplay::StopRecord()
{
    _tickRunner.Stop();
    _state = REST;
}

void AReplay::_SetNextPlayTick()
{
    if (_playTickCnt <= 0) {
        return;
    }
    ++_tick;
    if (_tick % _playInterval == 0) {
        _playIdx += _direction == FORWARD ? 1 : -1;
        --_playTickCnt;
    }
    if (_playIdx >= _tickQueue.size()) {
        _playIdx = _tickQueue.size() - 1;
        return;
    }
    if (_playIdx < 0) {
        _playIdx = 0;
    }
}

void AReplay::Play(Direction direction, int tickCnt, int fps)
{
    if (fps < 1 || fps > 100) {
        __aInternalGlobal.loggerPtr->Error("AReplay::Play : fps 的范围为 [1, 100], 您当前的 fps 为: " + std::to_string(fps));
        return;
    }
    if (_state != REST) {
        __aInternalGlobal.loggerPtr->Error("请先停止 Replay 的运行再进行播放");
        return;
    }
    _state = PLAYING;
    _playTickCnt = tickCnt;
    _direction = direction;
    _playInterval = 100 / fps;
    auto tmp = [this] {
        _SetNextPlayTick();
        AAsm::LoadFromMemory(_tickQueue[_playIdx]);
        AAsm::FreeMemory(_tickQueue[_playIdx]);
        __aInternalGlobal.loggerPtr->Info("播放: " + std::to_string(_playIdx));
        _tickQueue[_playIdx] = AAsm::SaveToMemory(); // load 一次之后内存就失效了，需要重新 save
        AAsm::GameTotalLoop();
    };
    _tickRunner.Start(std::move(tmp), true);
    ASetAdvancedPause(true);
}

void AReplay::Pause()
{
    if (_state != REST) {
        return;
    }
    _state = PAUSED;
}

void AReplay::GoOn()
{
    if (_state != REST) {
        return;
    }
    _state = PLAYING;
}

void AReplay::Stop()
{
    _state = REST;
    _playIdx = 0;
}

void AReplay::_ExitFight()
{
    for (auto iter = _tickQueue.begin(); iter != _tickQueue.end(); ++iter) {
        AAsm::FreeMemory(*iter);
    }
    _tickQueue.clear();
}