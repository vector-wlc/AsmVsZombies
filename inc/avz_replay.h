#ifndef __AVZ_REPLAY_H__
#define __AVZ_REPLAY_H__

#include "avz_tick_runner.h"
#include <algorithm>
#include <atomic>
#include <map>
#include <mutex>
#include <thread>

class AAbstractCompressor {
public:
    struct Info {
        std::string srcPath;
        std::string dstPath;
    };
    void SetFilePath(const std::string& filePath) { _filePath = filePath; };
    // 压缩文件
    virtual void Compress(const std::string& srcPath) = 0;
    // 解压文件
    virtual void Decompress(const std::string& srcPath, const std::string& dstPath) = 0;
    // 得到正在压缩的文件列表
    virtual std::vector<std::string> GetCompressingList() = 0;
    // 得到正在解压的文件列表
    virtual std::vector<Info> GetDecompressingList() = 0;
    // 得到已经压缩的文件列表，注意对象内部应该删除已经获取的文件列表
    virtual std::vector<std::string> GetCompressedList() = 0;
    // 得到已经解压的文件列表，注意对象内部应该删除已经获取的文件列表
    virtual std::vector<Info> GetDecompressedList() = 0;
    // 等待压缩任务做完
    virtual void WaitForDone() = 0;
    // 压缩对象是否正在工作
    virtual bool IsWorking() = 0;
    // 压缩对象是否能够正常工作
    virtual bool IsOk() = 0;
    virtual ~AAbstractCompressor() {}

protected:
    std::string _filePath = "areplay.7z";
};

class A7zCompressor : public AAbstractCompressor,
                      public AOrderedBeforeScriptHook<-1>, //
                      public AOrderedExitFightHook<-1> {
public:
    A7zCompressor(const std::string& path);
    virtual ~A7zCompressor() {};
    virtual void Compress(const std::string& inPath) override;
    virtual void Decompress(const std::string& srcPath, const std::string& dstPath) override;
    virtual std::vector<std::string> GetCompressingList() override;
    virtual std::vector<Info> GetDecompressingList() override;
    virtual std::vector<std::string> GetCompressedList() override;
    virtual std::vector<Info> GetDecompressedList() override;
    virtual void WaitForDone() override;
    virtual bool IsWorking() override;
    virtual bool IsOk() override;

    // 设置 7z 压缩命令
    // 注意不能设置路径相关的命令
    // 此接口不会检查设置的命令是否合法
    // 使用时请仔细检查设置的命令是否合法
    // 如果命令不合法，导致的一切结果需要自己负责
    // 使用示例：
    // SetCompressCmd("a -aoa -mx=4") ---- 压缩模式 覆盖模式  压缩等级为 4
    void SetCompressCmd(const std::string& cmd) { _compressCmd = cmd; }

    // 设置压缩等级
    // 注意：设置范围为 [1, 5]，当使用了 SetCompressCmd 之后，此函数的设置将会失效
    // 越低的压缩等级 CPU 占用越少，硬盘占用越大
    // 默认为 4
    void SetCompressLevel(int level) { _compressLevel = std::clamp(level, 1, 5); }

protected:
    virtual void _BeforeScript() override;
    virtual void _ExitFight() override;
    static void _RunExe(const std::string& exe, const std::string& cmd);
    static constexpr auto DEFAULT_CMD = "a -aoa -mx=";
    std::string _7zPath = "7z";
    std::deque<std::string> _compressList;
    std::deque<Info> _decompressList;
    std::vector<std::string> _compressedList;
    std::vector<Info> _decompressedList;
    int _compressLevel = 4;
    std::string _compressCmd;
    std::mutex _lock;
    std::unique_ptr<std::thread> _compressTask = nullptr;
    std::unique_ptr<std::thread> _decompressTask = nullptr;
    std::condition_variable _compressCv;
    std::condition_variable _decompressCv;
    std::condition_variable _waitDoneCv;
    std::atomic<bool> _isRunning = true;
};

class AReplay : public AOrderedExitFightHook<-2> {
public:
    struct TickInfo {
        ACursor cursor;
        int mjClock;
        uint32_t mjPhase; // 这个是记录女仆的相位的，开启了女仆秘籍此值生效
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
    void StartRecord(int interval = 10, int64_t startIdx = 0);

    // 开始播放
    // 使用示例：
    // StartPlay(); -------- 开始播放，默认从第 0 帧开始播放
    // StartPlay(3, 100); -------- 开始播放，每 3 帧播放一帧，从第 100 帧开始播放
    void StartPlay(int interval = -1, int64_t startIdx = 0);

    // 显示设定的帧
    // 注意: 对于播放模式，此函数会将播放的帧位设置为当前要显示的帧
    //      对于记录模式，此函数会将下一个记录的帧位设置为当前要显示的帧
    // 使用示例：
    // ShowOneTick(55) ----- 显示第 55 帧
    bool ShowOneTick(int64_t tick);

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
    void SetSaveDirPath(const std::string& path);

    // 设置最大保存帧数
    // 注意：如果不使用此函数，回放对象的默认值为 2000
    // 使用示例：
    // SetMaxSaveCnt(1000); ------- 最大保留 1000 帧存档
    void SetMaxSaveCnt(int64_t maxSaveCnt);

    // 设置每个包含有多少帧
    // 注意：如果不使用此函数，回放对象的默认值为 100
    // 使用示例：
    // SetPackTickCnt(100); ------- 每个包含有 100 帧存档
    void SetPackTickCnt(int packTickCnt);

    // 得到目前播放的帧位
    int64_t GetPlayIdx() const { return _playIdx; }

    // 得到帧尾位置
    int64_t GetEndIdx() const { return _endIdx; }

    // 得到要记录的帧位
    // 实际上同 GetEndIdx()
    int64_t GetRecordIdx() const { return _endIdx; }

    // 得到帧头位置
    int64_t GetStartIdx() const { return _startIdx; }

    // 设置压缩对象
    void SetCompressor(AAbstractCompressor& compressor);

    // 此函数会立即保存当前 pvz 的状态
    // 当 回放对象 停止播放时会将 pvz 的状态恢复到此函数保存的状态
    void SavePvzState();

    // 设置鼠标是否可见
    void SetMouseVisible(bool isVisible) { _isMouseVisible = isVisible; }

    void Pause();
    bool IsPaused();
    void GoOn();
    void Stop();
    State GetState() const { return _state; }
    APainter& GetPainter() { return _painter; }

protected:
    virtual void _ExitFight() override { Stop(); }
    void _ReadInfo();
    void _WriteInfo();
    void _SaveTickInfo();
    void _LoadTickInfo();
    void _LoadPvzState();
    void _RecordTick();
    void _PlayTick();
    void _ShowTickInfo();
    bool _PreparePack(int64_t tick);
    void _CompressTailFiles();
    void _ClearDatFiles();
    void _ClearAllFiles();
    static constexpr auto _MAX_SAVE_CNT_KEY = "maxSaveCnt";
    static constexpr auto _PACK_TICK_CNT_KEY = "packTickCnt";
    static constexpr auto _END_IDX_KEY = "endIdx";
    static constexpr auto _START_IDX_KEY = "startIdx";
    static constexpr auto _RECORD_INTERVAL_KEY = "recordInterval";
    static constexpr auto _RECOVER_DAT_STR = "recover.dat";
    static constexpr auto _INFO_FILE_STR = "info.txt";
    static constexpr auto _TICK_INFO_FILE_STR = "tickInfo.dat";
    int _recordInterval = 10;
    int _playInterval = 10;
    int64_t _maxSaveCnt = 2000;
    int64_t _playIdx = 0;
    int64_t _startIdx = 0;
    int64_t _endIdx = 0;
    int64_t _maxSavePackCnt = 20;
    int _lastPackIdx = INT_MIN;
    int _showPosX = 0;
    int _showPosY = 0;
    bool _isInterpolate = true;
    bool _isShowInfo = true;
    int _packTickCnt = 100;
    ATickRunner _tickRunner;
    ATickRunner _infoTickRunner;
    State _state = REST;
    std::string _savePath;
    APainter _painter;
    AAbstractCompressor* _compressor = nullptr;
    std::map<int, TickInfo> _tickInfos;
    bool _isMouseVisible = true;
    uint32_t _mjPhaseRecover = AMaidCheats::MC_STOP;
    static constexpr uint32_t _FALLING_SUN_ADDR = 0x00413B83;
    static constexpr uint8_t _NO_FALLING_SUN_CODE = 0xeb;
    uint8_t _fallingSunCodeRecover = 0x75;
    static constexpr uint32_t _ZOMBIE_SPAWN_ADDR = 0x004265dc;
    static constexpr uint8_t _STOP_ZOMBIE_SPAWN_CODE = 0xeb;
    uint8_t _zombieSpawnCodeRecover = 0x74;
    int _cursorLastPressType = INT_MIN;
    int _lastWave = INT_MIN;
};

inline AReplay aReplay;

#endif
