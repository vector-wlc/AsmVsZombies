#include "avz_replay.h"
#include "avz_asm.h"
#include "avz_game_controllor.h"
#include "avz_memory.h"
#include "avz_script.h"
#include "avz_connector.h"
#include "avz_iterator.h"

#include <filesystem>

namespace stdFs = std::filesystem;

void A7zCompressor::_BeforeScript()
{
    _isRunning = true;
    _compressTask = std::make_unique<std::thread>([this] {
        for (; _isRunning || !_compressList.empty();) {
            std::string fileName;
            {
                std::unique_lock lk(_lock);
                if (_compressList.empty()) {
                    _compressCv.wait(lk);
                }
                if (_compressList.empty()) {
                    return;
                }
                fileName = _compressList.front();
            }
            std::string compressCmd;
            if (_compressCmd.empty()) {
                compressCmd = DEFAULT_CMD + std::to_string(_compressLevel);
            } else {
                compressCmd = _compressCmd;
            }
            _RunExe(_7zPath, std::move(compressCmd) + " " + _filePath + " " + fileName);
            {
                std::lock_guard<std::mutex> lk(_lock);
                _compressedList.push_back(std::move(fileName));
                _compressList.pop_front();
                if (_compressList.empty()) {
                    // 压缩列表为空需要唤醒 WaitForDone
                    _waitDoneCv.notify_all();
                }
            }
        }
    });
    _decompressTask = std::make_unique<std::thread>([this] {
        for (; _isRunning || !_decompressList.empty();) {
            Info info;
            {
                std::unique_lock lk(_lock);
                if (_decompressList.empty()) {
                    _decompressCv.wait(lk);
                }
                if (_decompressList.empty()) {
                    return;
                }
                info = _decompressList.front();
            }
            _RunExe(_7zPath, "x -aoa " + _filePath + " -o\"" + info.dstPath + "\" " + info.srcPath);
            {
                std::lock_guard<std::mutex> lk(_lock);
                _decompressedList.push_back(std::move(info));
                _decompressList.pop_front();
                if (_decompressList.empty()) {
                    // 解压列表为空需要唤醒 WaitForDone
                    _waitDoneCv.notify_all();
                }
            }
        }
    });
}

void A7zCompressor::_ExitFight()
{
    _isRunning = false;
    _compressCv.notify_all();
    _decompressCv.notify_all();
    _waitDoneCv.notify_all();
    _compressTask->join();
    _decompressTask->join();
    _compressTask = nullptr;
    _decompressTask = nullptr;
}

std::vector<std::string> A7zCompressor::GetCompressingList()
{
    std::lock_guard<std::mutex> lk(_lock);
    return {_compressList.begin(), _compressList.end()};
}

std::vector<A7zCompressor::Info> A7zCompressor::GetDecompressingList()
{
    std::lock_guard<std::mutex> lk(_lock);
    return {_decompressList.begin(), _decompressList.end()};
}

std::vector<std::string> A7zCompressor::GetCompressedList()
{
    std::vector<std::string> ret;
    std::lock_guard<std::mutex> lk(_lock);
    ret = std::move(_compressedList);
    _compressedList.clear();
    return ret;
}

std::vector<A7zCompressor::Info> A7zCompressor::GetDecompressedList()
{
    std::vector<Info> ret;
    std::lock_guard<std::mutex> lk(_lock);
    ret = std::move(_decompressedList);
    _decompressedList.clear();
    return ret;
}

void A7zCompressor::WaitForDone()
{
    for (;;) {
        std::unique_lock<std::mutex> lk(_lock);
        if (_compressList.empty() && _decompressList.empty()) {
            return;
        }
        _waitDoneCv.wait(lk);
    }
}

bool A7zCompressor::IsWorking()
{
    std::lock_guard<std::mutex> lk(_lock);
    return (!_compressList.empty()) || (!_decompressList.empty());
}

bool A7zCompressor::IsOk()
{
    if (_7zPath.empty()) {
        return false;
    }
    return stdFs::exists(_7zPath);
}

void A7zCompressor::Compress(const std::string& srcPath)
{
    std::lock_guard<std::mutex> lk(_lock);
    _compressList.push_back(srcPath);
    if (_compressList.size() == 1) {
        _compressCv.notify_one();
    }
}

void A7zCompressor::Decompress(const std::string& srcPath, const std::string& dstPath)
{
    std::lock_guard<std::mutex> lk(_lock);
    _decompressList.push_back({srcPath, dstPath});
    if (_decompressList.size() == 1) {
        _decompressCv.notify_one();
    }
}

void A7zCompressor::_RunExe(const std::string& exe, const std::string& cmd)
{
    SHELLEXECUTEINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SEE_MASK_NOCLOSEPROCESS;
    si.lpVerb = "open";
    si.lpFile = exe.c_str();
    si.lpParameters = cmd.c_str();
    si.nShow = SW_HIDE;

    ShellExecuteExA(&si);

    DWORD dwExitCode;
    GetExitCodeProcess(si.hProcess, &dwExitCode);
    while (dwExitCode == STILL_ACTIVE) {
        Sleep((DWORD)5);
        GetExitCodeProcess(si.hProcess, &dwExitCode);
    }

    CloseHandle(si.hProcess);
}

AReplay::AReplay()
{
    SetSaveDirPath("areplay");
}

void AReplay::SetSaveDirPath(const std::string& path)
{
    _savePath = stdFs::absolute(path).string();
    if (!stdFs::exists(_savePath)) {
        stdFs::create_directory(_savePath);
    }
}

void AReplay::SetMaxSaveCnt(int64_t maxSaveCnt)
{
    if (_state != REST) {
        AGetInternalLogger()->Error("SetMaxSaveCnt : 请先停止 Replay 的运行再设置最大保存帧数");
        return;
    }
    if (maxSaveCnt < 1) {
        AGetInternalLogger()->Error("SetMaxSaveCnt : 最大使用帧数必须为正数");
        return;
    }
    _maxSaveCnt = maxSaveCnt;
}

void AReplay::SetPackTickCnt(int packTickCnt)
{
    if (_state != REST) {
        AGetInternalLogger()->Error("SetPackTickCnt : 请先停止 Replay 的运行再设置每个包的包含帧数");
        return;
    }
    if (packTickCnt < 1) {
        AGetInternalLogger()->Error("SetPackTickCnt : 每个包的包含帧数必须为正数");
        return;
    }
    _packTickCnt = packTickCnt;
}

void AReplay::_RecordTick()
{
    int x = AGetPvzBase()->MouseWindow()->MouseAbscissa();
    int y = AGetPvzBase()->MouseWindow()->MouseOrdinate();
    int type = AGetPvzBase()->MRef<int>(0x4B0);
    int clock = AGetMainObject()->GameClock();
    _mouseInfos[clock] = ACursor(x, y, type);
    if (clock % _recordInterval != 0) {
        return;
    }
    auto dirName = stdFs::path(std::to_string((_endIdx % _maxSaveCnt) / _packTickCnt));
    auto fileName = stdFs::path(std::to_string(_endIdx % _maxSaveCnt % _packTickCnt) + ".dat");
    if (_endIdx - _startIdx >= _maxSaveCnt) {
        _startIdx = _endIdx - _maxSaveCnt + 1;
    }
    int leftClock = clock - (_endIdx - _startIdx) * _recordInterval;
    for (auto iter = _mouseInfos.begin(); iter != _mouseInfos.end(); iter = _mouseInfos.erase(iter)) {
        if (iter->first >= leftClock) {
            break;
        }
    }

    auto dirPath = _savePath / dirName;
    auto filePath = dirPath / fileName;
    if (!stdFs::exists(dirPath)) {
        stdFs::create_directory(dirPath);
    }
    if (stdFs::exists(filePath)) {
        stdFs::remove(filePath);
    }
    AAsm::SaveGame(filePath.string());

    // 如果开启了压缩
    if (_compressor != nullptr) {
        int curPack = (_endIdx % _maxSaveCnt) / _packTickCnt;
        int nextPack = ((_endIdx + 1) % _maxSaveCnt) / _packTickCnt;
        if (curPack != nextPack) {
            _compressor->Compress(dirPath.string());
        }
        auto compressedList = _compressor->GetCompressedList();
        for (auto&& path : compressedList) {
            stdFs::remove_all(path);
        }
    }
    ++_endIdx;
}

void AReplay::StartRecord(int interval, int64_t startIdx)
{
    if (AGetPvzBase()->GameUi() != 3) {
        AGetInternalLogger()->Error("StartRecord : AReplay 只能在战斗界面使用");
        return;
    }
    _startIdx = startIdx;
    _endIdx = startIdx;
    _maxSavePackCnt = _maxSaveCnt / _packTickCnt;
    _maxSavePackCnt += ((_maxSaveCnt % _packTickCnt > 0) ? 1 : 0);
    if (interval < 1) {
        AGetInternalLogger()->Error("StartRecord : interval 的范围为 [1, ], 您当前的 interval 为: " + std::to_string(interval));
        return;
    }
    _recordInterval = interval;
    if (_state != REST) {
        AGetInternalLogger()->Error("StartRecord : 请先停止 Replay 的运行再进行记录");
        return;
    }
    _state = RECORDING;
    _mouseInfos.clear();
    _tickRunner.Start([this] { _RecordTick(); }, false);
    _infoTickRunner.Start([this] {
        if (!_isShowInfo || __aGameControllor.isSkipTick()) {
            return;
        }
        _painter.Draw(AText("AReplay : 共录制 [" + std::to_string(_startIdx)
                + "-" + std::to_string(_endIdx) + "] 帧",
            _showPosX, _showPosY));
    },
        true);
}

void AReplay::SetInfoPos(int x, int y)
{
    _showPosX = std::clamp(x, 0, 800);
    _showPosY = std::clamp(y, 0, 600);
}

bool AReplay::ShowOneTick(int64_t tick)
{
    if (AGetPvzBase()->GameUi() != 3) {
        AGetInternalLogger()->Error("PlayOneTick : AReplay 只能在战斗界面使用");
        return false;
    }
    if (_state == REST) {
        AGetInternalLogger()->Error("PlayOneTick : 请先调用 StartPlay 或者 StartRecord 再使用此接口");
        return false;
    }
    if (tick < _startIdx || tick >= _endIdx) {
        return false;
    }

    // 对于播放模式，此函数会将播放的帧位设置为当前要显示的帧
    // 对于记录模式，此函数会将下一个记录的帧位设置为当前要显示的帧
    if (_state == RECORDING) {
        _endIdx = tick;
    } else if (_state == PLAYING) {
        _playIdx = tick;
    }

    tick = (tick + _maxSaveCnt) % _maxSaveCnt;
    auto dirName = stdFs::path(std::to_string((tick / _packTickCnt)));
    auto fileName = stdFs::path(std::to_string(tick % _packTickCnt) + ".dat");
    auto filePath = _savePath / dirName / fileName;
    if (stdFs::exists(filePath)) {
        AAsm::LoadGame(filePath.string());
        return true;
    }
    if (_compressor != nullptr) {
        // 如果开启了压缩模式，那么这一帧的结果可能在压缩包里面
        // 所以需要从压缩包里面提取出来
        _compressor->Decompress((dirName / fileName).string(), _savePath);
        _compressor->WaitForDone();
        if (stdFs::exists(filePath)) {
            AAsm::LoadGame(filePath.string());
            return true;
        }
    }
    return false;
}

bool AReplay::_PreparePack(int packIdx)
{
    auto dirName = std::to_string(packIdx);
    // 如果要播放的目录正在解压
    // 那么就是解压过载了
    auto decompressingList = _compressor->GetDecompressingList();
    for (auto&& path : decompressingList) {
        if (path.srcPath == dirName) {
            AGetInternalLogger()->Info("AReplay : 解压过载，请尝试更高的录制播放间隔以降低硬件的压力");
            return false;
        }
    }

    if (!decompressingList.empty()) {
        return true;
    }

    // 如果此时发现目标目录不存在在硬盘中
    // 说明还没有解压出来，需要进行解压
    // 这种情况一般出现在开始或者继续播放的时候
    // 所以需要等待压缩对象工作完毕
    if (!stdFs::exists(stdFs::path(_savePath) / dirName)) {
        _compressor->Decompress(dirName, _savePath);
        _compressor->WaitForDone();
    }

    // 如果下一个包不存在硬盘中，就提前将下一个包解压出来
    int nextPackIdx = packIdx + 1;
    if (nextPackIdx < 0) {
        nextPackIdx += _maxSavePackCnt;
    }
    nextPackIdx %= _maxSavePackCnt;
    if (nextPackIdx != packIdx) {
        dirName = std::to_string(nextPackIdx);
        auto dirPath = stdFs::path(_savePath) / dirName;
        if (!stdFs::exists(dirPath)) {
            _compressor->Decompress(dirName, _savePath);
        }
        // 删除之前存在的解压文件
        int prePackIdx = packIdx - 1;
        if (prePackIdx < 0) {
            prePackIdx += _maxSavePackCnt;
        }
        prePackIdx %= _maxSavePackCnt;
        if (prePackIdx != packIdx) {
            auto dirPath = stdFs::path(_savePath) / std::to_string(prePackIdx);
            if (stdFs::exists(dirPath)) {
                stdFs::remove_all(dirPath);
            }
        }
    }

    return true;
}

void AReplay::_PlayTick()
{
    if (AGetPvzBase()->GameUi() != 3) {
        Stop();
        return;
    }
    static int counter = 0;
    ++counter;
    bool isPlay = (counter % _playInterval) == 0;
    int clock = AGetMainObject()->GameClock();
    if (_isMouseVisible && _mouseInfos.find(clock) != _mouseInfos.end()) {
        _painter.Draw(_mouseInfos[clock], 1);
    }
    if (_isInterpolate && !isPlay) { // 如果插帧
        AAsm::GameTotalLoop();
    }
    if (!isPlay) {
        return;
    }
    AAsm::GameTotalLoop();
    // 开启了压缩功能需要预先对压缩包里的内容解压
    // 并且销毁之前解压出来的内容
    if (_compressor != nullptr) {
        int packIdx = ((_playIdx + _maxSaveCnt) % _maxSaveCnt) / _packTickCnt;
        if (!_PreparePack(packIdx)) {
            return;
        }
    }
    int tmpPlayIdx = _playIdx + 1;
    if (!ShowOneTick(tmpPlayIdx)) {
        _tickRunner.Pause();
        return;
    }
}

void AReplay::StartPlay(int interval, int64_t startIdx)
{
    if (AGetPvzBase()->GameUi() != 3) {
        AGetInternalLogger()->Error("StartPlay : AReplay 只能在战斗界面使用");
        return;
    }
    if (interval < 1) {
        AGetInternalLogger()->Error("StartPlay : interval 的范围为 [1, ], 您当前的 interval 为: " + std::to_string(interval));
        return;
    }
    if (_state != REST) {
        AGetInternalLogger()->Error("StartPlay : 请先停止 Replay 的运行再进行播放");
        return;
    }
    if (__aGameControllor.isSkipTick()) {
        AGetInternalLogger()->Error("StartPlay : 请先停止帧运行再进行播放");
        return;
    }
    ASetAdvancedPause(true);
    SavePvzState();
    _ReadInfo();
    _state = PLAYING;
    _playInterval = interval;
    _playIdx = _startIdx + startIdx;
    _compressPath.clear();

    _tickRunner.Start([this] {
        if (!AGameIsPaused()) {
            _PlayTick();
        }
    },
        ATickRunner::GLOBAL);

    _infoTickRunner.Start([this] {
        if (!_isShowInfo) {
            return;
        }
        std::string str = std::to_string(_playIdx);
        if (_endIdx != INT64_MAX) {
            str += "/" + std::to_string(_endIdx);
        }
        _painter.Draw(AText("AReplay : 播放第 " + std::move(str) + " 帧", _showPosX, _showPosY));
    },
        ATickRunner::GLOBAL);
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
    _ClearDatFiles();
    _tickRunner.GoOn();
}

void AReplay::Stop()
{
    if (_state == RECORDING) {
        _WriteInfo();
        _CompressTailFiles();
    } else if (_state == PLAYING) {
        _LoadPvzState();
    }
    _ClearAllFiles();
    _state = REST;
    _infoTickRunner.Stop();
    _tickRunner.Stop();
    ASetAdvancedPause(false);
}

void AReplay::_ReadInfo()
{
    if (_compressor != nullptr) {
        _compressor->Decompress(INFO_FILE_STR, _savePath);
        _compressor->Decompress(MOUSE_INFO_FILE_STR, _savePath);
        _compressor->WaitForDone();
    }
    auto infoFilePath = stdFs::path(_savePath) / INFO_FILE_STR;
    std::ifstream infoFile(infoFilePath.c_str());
    if (!infoFile.good()) {
        _maxSaveCnt = INT64_MAX;
        _packTickCnt = 100;
        _endIdx = INT64_MAX;
        _startIdx = 0;
    } else {
        std::unordered_map<std::string, int> table;
        std::string key;
        int val = 0;
        for (; (infoFile >> key) && (infoFile >> val);) {
            table[key] = val;
        }
        _maxSaveCnt = table[MAX_SAVE_CNT_KEY];
        _packTickCnt = table[PACK_TICK_CNT_KEY];
        _startIdx = table[START_IDX_KEY];
        _endIdx = table[END_IDX_KEY];
    }
    _maxSavePackCnt = _maxSaveCnt / _packTickCnt;
    _maxSavePackCnt += ((_maxSaveCnt % _packTickCnt > 0) ? 1 : 0);
    infoFile.close();
    _LoadMouseInfo();
}

void AReplay::_WriteInfo()
{
    auto infoFilePath = stdFs::path(_savePath) / INFO_FILE_STR;
    std::ofstream infoFile(infoFilePath.c_str());
    if (!infoFile.good()) {
        AGetInternalLogger()->Error("保存回放文件信息失败");
    } else {
        infoFile << MAX_SAVE_CNT_KEY << " " << _maxSaveCnt << "\n"
                 << PACK_TICK_CNT_KEY << " " << _packTickCnt << "\n"
                 << START_IDX_KEY << " " << _startIdx << "\n"
                 << END_IDX_KEY << " " << _endIdx;
    }

    infoFile.close();
    _SaveMouseInfo();
}

using AClockCursor = std::pair<int, ACursor>;

void AReplay::_SaveMouseInfo()
{
    auto infoFilePath = stdFs::path(_savePath) / MOUSE_INFO_FILE_STR;
    std::ofstream infoFile(infoFilePath.c_str(), std::ios_base::binary);
    if (!infoFile.good()) {
        AGetInternalLogger()->Error("保存鼠标回放文件信息失败");
    } else {
        std::vector<AClockCursor> tmp;
        for (auto&& e : _mouseInfos) {
            tmp.push_back(e);
        }
        size_t size = tmp.size();
        infoFile.write((char*)(&size), sizeof(size));
        infoFile.write((char*)tmp.data(), size * sizeof(AClockCursor));
    }
    infoFile.close();
    _mouseInfos.clear();
}

void AReplay::_LoadMouseInfo()
{
    _mouseInfos.clear();
    auto infoFilePath = stdFs::path(_savePath) / MOUSE_INFO_FILE_STR;
    std::ifstream infoFile(infoFilePath.c_str(), std::ios_base::binary);
    if (!infoFile.good()) {
        AGetInternalLogger()->Error("载入鼠标回放文件信息失败");
    } else {
        size_t size = 0;
        infoFile.read((char*)(&size), sizeof(size));
        std::vector<AClockCursor> tmp(size);
        infoFile.read((char*)tmp.data(), size * sizeof(AClockCursor));
        for (auto&& e : tmp) {
            _mouseInfos.insert(e);
        }
    }
    infoFile.close();
}

void AReplay::SavePvzState()
{
    auto filePath = _savePath / stdFs::path(RECOVER_DAT_STR);
    if (stdFs::exists(filePath)) {
        stdFs::remove(filePath);
    }
    AAsm::SaveGame(filePath.string());
}

void AReplay::_LoadPvzState()
{
    if (!AGetPvzBase() || !AGetPvzBase()->MainObject() || AGetPvzBase()->GameUi() != 3) {
        return;
    }
    auto filePath = _savePath / stdFs::path(RECOVER_DAT_STR);
    if (stdFs::exists(filePath)) {
        AAsm::LoadGame(filePath.string());
    }
}

void AReplay::_CompressTailFiles()
{
    if (_compressor == nullptr) {
        return;
    }
    std::string tailFilePaths;

    auto infoPath = _savePath / stdFs::path(INFO_FILE_STR);
    auto mouseInfoPath = _savePath / stdFs::path(MOUSE_INFO_FILE_STR);
    if (stdFs::exists(infoPath)) {
        tailFilePaths += " " + infoPath.string() + " " + mouseInfoPath.string();
    }

    auto dirName = stdFs::path(std::to_string((_endIdx % _maxSaveCnt) / _packTickCnt));
    auto dirPath = _savePath / dirName;
    if (stdFs::exists(dirPath)) {
        tailFilePaths += " " + dirPath.string();
    }
    _compressor->Compress(tailFilePaths);
    _compressor->WaitForDone();
}

void AReplay::_ClearDatFiles()
{
    if (_compressor == nullptr) {
        return;
    }
    // 等待压缩任务全部完成
    _compressor->WaitForDone();

    // 遍历所有的文件，发现以纯数字命名的目录就删除
    for (auto&& dir : std::filesystem::directory_iterator(_savePath)) {
        bool isNumber = true;
        auto str = dir.path().string();
        for (auto iter = str.rbegin(); iter != str.rend(); ++iter) {
            if ((*iter) == '\\' || (*iter) == '/') {
                break;
            }
            if (!std::isdigit(*iter)) {
                isNumber = false;
                break;
            }
        }
        if (isNumber) {
            stdFs::remove_all(dir);
        }
    }
}

void AReplay::_ClearAllFiles()
{
    if (_compressor == nullptr) {
        return;
    }
    _ClearDatFiles();
    // 删除 info.txt
    auto infoPath = _savePath / stdFs::path(INFO_FILE_STR);
    if (stdFs::exists(infoPath)) {
        stdFs::remove(infoPath);
    }
    // 删除鼠标文件
    auto mouseInfoPath = _savePath / stdFs::path(MOUSE_INFO_FILE_STR);
    if (stdFs::exists(mouseInfoPath)) {
        stdFs::remove(mouseInfoPath);
    }
}

void AReplay::SetCompressor(AAbstractCompressor& compressor)
{
    _compressor = nullptr;
    if (!compressor.IsOk()) {
        AGetInternalLogger()->Error("压缩对象未准备好，无法开启压缩模式");
        return;
    }
    _compressor = &compressor;
}
