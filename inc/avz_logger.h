#ifndef __AVZ_LOGGER_H__
#define __AVZ_LOGGER_H__

#include "avz_painter.h"
#include <deque>
#include <format>
#include <fstream>
#include <sstream>
#include <string_view>

enum class ALogLevel {
    INFO,
    DEBUG,
    WARNING,
    ERROR,
};

class AAbstractLogger : public AOrderedBeforeScriptHook<-1> {
public:
    __ANodiscard const std::string& GetPattern() const {
        return _pattern;
    }

    void SetPattern(std::string_view pattern) {
        _pattern = pattern;
    }

    void SetHeaderStyle(std::string_view headerStyle) {
        _headerStyle = headerStyle;
    }

    void SetLevel(const std::vector<ALogLevel>& levelVec) {
        _level = 0;
        for (auto&& level : levelVec)
            _level |= (1 << int(level));
    }

    void SetSuffix(const std::string& suffix) { _suffix = suffix; }

    template <typename FormatStr, typename... Args>
    void Info(FormatStr&& formatStr, Args&&... args) {
        _Format(ALogLevel::INFO, std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
    }

    template <typename FormatStr, typename... Args>
    void Debug(FormatStr&& formatStr, Args&&... args) {
        _Format(ALogLevel::DEBUG, std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
    }

    template <typename FormatStr, typename... Args>
    void Warning(FormatStr&& formatStr, Args&&... args) {
        _Format(ALogLevel::WARNING, std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
    }

    template <typename FormatStr, typename... Args>
    void Error(FormatStr&& formatStr, Args&&... args) {
        _Format(ALogLevel::ERROR, std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
    }

protected:
    std::string _pattern;
    std::string _headerStyle;
    uint8_t _level = 0b1111; // 每一个 bit 代表这个水平的日志开不开(1是开)
    std::string _bufStr;
    std::string _suffix = "\n";
    const std::vector<std::string> _levelStr = {
        "INFO",
        "DEBUG",
        "WARNING",
        "ERROR",
    };
    virtual void _BeforeScript() override;
    virtual void _Output(ALogLevel level, std::string&& str) = 0;

    static void _Replace(std::string& content, std::string_view pattern, std::string_view replaceStr);

    std::string _CreateHeader(ALogLevel level);

    void _Format(ALogLevel level, std::string formatStr, auto&&... args) {
        if (((1 << int(level)) & _level) == 0)
            return;
        auto header = _CreateHeader(level);
        _Replace(formatStr, _pattern, "{}");
        std::string message;
        try {
            message = std::vformat(formatStr + _suffix, std::make_format_args(std::forward<decltype(args)>(args)...));
        } catch (const std::format_error& e) {
            message = "格式化错误: " + std::string(e.what());
        }
        _Output(level, header + message);
    }
};

struct AFile {
};
struct AConsole {
};
struct APvzGui {
};
struct AMsgBox {
    static void Show(const std::string& str) {
        MessageBoxW(nullptr, AStrToWstr(str).c_str(), L"AMsgBox", MB_OK);
    }
};

template <typename T>
class ALogger;

template <>
class ALogger<AFile> : public AAbstractLogger, public AOrderedExitFightHook<-1> {
public:
    ALogger(const std::string& fileName)
        : _fileName(fileName) {
    }

    // 清除文件中的所有内容
    // return true: 清除成功
    // return false: 清除失败
    bool Clear();

protected:
    virtual void _ExitFight() override;
    std::string _fileName;
    std::wofstream _outFile;
    virtual void _Output(ALogLevel level, std::string&& str) override;
};

template <>
class ALogger<AConsole> : public AAbstractLogger {
public:
    ALogger();
    ~ALogger();
    // 设置显示颜色
    // *** 使用示例:
    // SetColor(ALogLevel::INFO, FOREGROUND_BLUE) ------ 将 INFO 等级的显示颜色设置为蓝色
    void SetColor(ALogLevel level, uint32_t color) {
        _color[int(level)] = color;
    }

protected:
    uint32_t _color[4];
    static bool _isAllocateConsole;
    HANDLE _handle = nullptr;
    virtual void _Output(ALogLevel level, std::string&& str) override;
};

class ATickRunner;
template <>
class ALogger<APvzGui> : public AAbstractLogger {
public:
    ALogger();
    __ANodiscard APainter& GetPainter() { return _painter; }

    // 设置显示颜色
    // *** 使用示例:
    // SetColor(ALogLevel::INFO, AArgb(0xff, 0, 0, 0xff)) ------ 将 INFO 等级的显示颜色设置为蓝色
    // SetColor(ALogLevel::INFO, AArgb(0xff, 0, 0, 0xff), AArgb(0xff, 0, 0, 0)) ------ 将 INFO 等级的显示颜色设置为蓝色，并将背景色设置为黑色
    void SetColor(ALogLevel level, uint32_t textColor, uint32_t bkgColor = AArgb(0xaf, 0, 0, 0)) {
        _textColors[int(level)] = textColor;
        _rectColors[int(level)] = bkgColor;
    }

    // 设置背景颜色
    // *** 使用示例:
    // SetBkgColor(ALogLevel::INFO, AArgb(0xff, 0, 0, 0) ------- 将 INFO 等级的背景色设置为黑色
    void SetBkgColor(ALogLevel level, uint32_t bkgColor) {
        _rectColors[int(level)] = bkgColor;
    }

    // 设定显示持续时间
    // *** 使用示例:
    // SetRemainTime(100) ------- 将显示持续时间改为 100
    void SetRemainTime(int remainTime) {
        _remainTime = std::clamp(remainTime, 0, INT_MAX);
    }

    // 设定显示持续时间
    // *** 使用示例:
    // SetPos(50, 100) ------- 将显示位置更改为 (50, 100) [最大为 800, 600]
    void SetPos(int x, int y) {
        _pixelDisplay.x = std::clamp(x, 0, 800);
        _pixelDisplay.y = std::clamp(y, 0, 600);
    }

    // 设置平滑过渡速度
    // 默认值为 3
    void SetTransitSpeed(int speed) {
        _transitSpeed = std::clamp(speed, 1, INT_MAX);
    }

protected:
    uint32_t _textColors[4];
    uint32_t _rectColors[4];
    APainter _painter;
    int _remainTime = 500; // 控制显示的持续时间
    APixel _pixelDisplay = {10, 500};
    struct _Display {
        ALogLevel level;
        std::string str;
        int gameClock;
        int lineCnt = 0;
    };
    std::shared_ptr<ATickRunner> _tickRunner;
    std::deque<_Display> _displayList;
    int _lastTick = INT_MIN;
    int _curBottom = 0; // 为了平滑过渡使用的
    int _transitSpeed = 3;
    virtual void _BeforeScript() override;
    virtual void _Output(ALogLevel level, std::string&& str) override;
    void _ShowTick();
};

template <>
class ALogger<AMsgBox> : public AAbstractLogger {
protected:
    virtual void _Output(ALogLevel level, std::string&& str) override;
    virtual void _BeforeScript() override;
};

inline AAbstractLogger* aLogger;

// 注意这个函数返回的是对象指针
inline AAbstractLogger* AGetInternalLogger() {
    return aLogger;
}

inline void ASetInternalLogger(AAbstractLogger& logger) {
    aLogger = &logger;
}

#endif
