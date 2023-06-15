#include "avz_replay.h"
#include "avz_asm.h"
#include "avz_game_controllor.h"
#include "avz_memory.h"
#include "avz_script.h"
#include "avz_connector.h"
#include "avz_iterator.h"

#include <filesystem>

namespace stdFs = std::filesystem;

AReplay::AReplay()
{
    SetSavePath("areplay");
}

void AReplay::SetSavePath(const std::string& path)
{
    _savePath = stdFs::absolute(path).string();
    if (!stdFs::exists(_savePath)) {
        stdFs::create_directory(_savePath);
    }
}

void AReplay::StartRecord(int interval, int startIdx)
{
    if (AGetPvzBase()->GameUi() != 3) {
        __aInternalGlobal.loggerPtr->Error("AReplay 只能在战斗界面使用");
        return;
    }
    _tickCnt = startIdx % _maxSaveCnt;
    _endIdx = _tickCnt;
    _tickMs = AGetPvzBase()->TickMs();
    if (interval < 1) {
        __aInternalGlobal.loggerPtr->Error("AReplay::Play : interval 的范围为 [1, ], 您当前的 interval 为: " + std::to_string(interval));
        return;
    }
    _recordInterval = interval;
    if (_state != REST) {
        __aInternalGlobal.loggerPtr->Error("请先停止 Replay 的运行再进行记录");
        return;
    }
    _state = RECORDING;
    auto tmp = [this] {
        if (AGetMainObject()->GameClock() % _recordInterval != 0) {
            return;
        }
        ++_tickCnt;
        auto fileName = stdFs::path(std::to_string(_endIdx) + ".dat");
        _endIdx = _tickCnt % _maxSaveCnt;
        auto filePath = _savePath / fileName;
        if (stdFs::exists(filePath)) {
            stdFs::remove(filePath);
        }
        AAsm::SaveGame(filePath.string());
    };
    _tickRunner.Start(std::move(tmp), false);
    if (_isShowInfo && !__AGameControllor::isSkipTick()) {
        _infoTickRunner.Start([this] {
            if (!_isShowInfo) {
                return;
            }
            _painter.Draw(AText("AReplay : 录制第 " + std::to_string(_endIdx) + " 帧", _showPosX, _showPosY));
        });
    }
}

void AReplay::_SetNextPlayTick()
{
    switch (_direction) {
    case AReplay::FORWARD:
        _playIdx = (_playIdx + 1) % _maxSaveCnt;
        break;
    case AReplay::BACKWARD:
        _playIdx = (_playIdx - 1) % _maxSaveCnt;
        break;
    default:
        break;
    }
}

void AReplay::SetInfoPos(int x, int y)
{
    _showPosX = std::clamp(x, 0, 800);
    _showPosY = std::clamp(y, 0, 600);
}

bool AReplay::PlayOneTick(int tick)
{
    if (AGetPvzBase()->GameUi() != 3) {
        __aInternalGlobal.loggerPtr->Error("AReplay 只能在战斗界面使用");
        return false;
    }
    if (_state == RECORDING) {
        __aInternalGlobal.loggerPtr->Error("录制时不允许播放");
        return false;
    }
    __aInternalGlobal.isReplay = true;
    ASetAdvancedPause(true);
    _playIdx = tick;
    auto fileName = stdFs::path(std::to_string(tick) + ".dat");
    auto filePath = _savePath / fileName;
    if (stdFs::exists(filePath)) {
        AAsm::MakeNewBoard();
        AAsm::LoadGame(filePath.string());
        if (_isShowInfo && _infoTickRunner.IsStopped()) {
            _infoTickRunner.Start([this] {
                if (!_isShowInfo) {
                    return;
                }
                _painter.Draw(AText("AReplay : 播放第 " + std::to_string(_playIdx) + " 帧", _showPosX, _showPosY));
            },
                true);
        }
        return true;
    }
    return false;
}

void AReplay::_ReadInfo()
{
    auto infoFilePath = stdFs::path(_savePath) / "info.txt";
    std::ifstream infoFile(infoFilePath.c_str());
    if (!infoFile.good()) {
        _endIdx = 0;
        _tickCnt = 0;
    } else {
        std::unordered_map<std::string, int> table;
        std::string key;
        int val = 0;
        for (; (infoFile >> key) && (infoFile >> val);) {
            table[key] = val;
        }
        _maxSaveCnt = table[MAX_SAVE_CNT_KEY];
        _endIdx = table[END_IDX_KEY];
        _tickCnt = table[TICK_CNT_KEY];
    }
    infoFile.close();
}
void AReplay::_WriteInfo()
{
    auto infoFilePath = stdFs::path(_savePath) / "info.txt";
    std::ofstream infoFile(infoFilePath.c_str());
    if (!infoFile.good()) {
        AGetInternalLogger()->Error("保存回放文件信息失败");
    } else {
        infoFile << MAX_SAVE_CNT_KEY << " " << _maxSaveCnt << "\n"
                 << END_IDX_KEY << " " << _endIdx << "\n"
                 << TICK_CNT_KEY << " " << _tickCnt;
    }

    infoFile.close();
}

void AReplay::StartPlay(int interval, int startIdx, Direction direction)
{
    if (AGetPvzBase()->GameUi() != 3) {
        __aInternalGlobal.loggerPtr->Error("AReplay 只能在战斗界面使用");
        return;
    }
    if (interval < 1 || interval > 100) {
        __aInternalGlobal.loggerPtr->Error("AReplay::Play : interval 的范围为 [1, ], 您当前的 interval 为: " + std::to_string(interval));
        return;
    }
    if (_state != REST) {
        __aInternalGlobal.loggerPtr->Error("请先停止 Replay 的运行再进行播放");
        return;
    }
    _ReadInfo();
    _state = PLAYING;
    _direction = direction;
    _playInterval = interval;
    _playIdx = _tickCnt > _endIdx ? _endIdx + 1 : 0;
    _playIdx = (_playIdx + startIdx) % _maxSaveCnt;

    auto tmp = [this]() mutable {
        if ((_playIdx == (_endIdx - 1 + _maxSaveCnt) % _maxSaveCnt) || AGetPvzBase()->GameUi() != 3) {
            _tickRunner.Stop();
            if (AGetPvzBase()->GameUi() != 3) {
                Stop();
            }
            return;
        }

        static int counter = 0;
        ++counter;
        if (_isInterpolate) { // 如果插帧
            int updateCnt = _playInterval == 1 ? 2 : 1;
            for (int i = 0; i < updateCnt; ++i) {
                AAsm::GameTotalLoop();
            }
        }

        if (counter % _playInterval != 0) {
            return;
        }

        if (!_isInterpolate) { // 如果不插帧
            for (int i = 0; i < 2; ++i) {
                AAsm::GameTotalLoop();
            }
        }
        _SetNextPlayTick();
        if (!PlayOneTick(_playIdx)) {
            _tickRunner.Stop();
        }
    };
    _tickMs = AGetPvzBase()->TickMs();
    _tickRunner.Start(std::move(tmp), true);
}

void AReplay::Pause()
{
    _tickRunner.Pause();
}

bool AReplay::IsPaused()
{
    return _tickRunner.IsPaused();
}

void AReplay::GoOn()
{
    _tickRunner.GoOn();
}

void AReplay::Stop()
{
    if (_state == RECORDING) {
        _WriteInfo();
    }
    _state = REST;
    _infoTickRunner.Stop();
    _tickRunner.Stop();
    AGetPvzBase()->TickMs() = _tickMs;
    ASetAdvancedPause(false);
}
