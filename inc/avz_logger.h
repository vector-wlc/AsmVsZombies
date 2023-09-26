/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-06 15:37:12
 * @Description:
 */
#ifndef __AVZ_LOGGER_H__
#define __AVZ_LOGGER_H__

#include "avz_painter.h"
#include <deque>
#include <fstream>
#include <sstream>
#include <string_view>

enum class ALogLevel {
    INFO,
    DEBUG,
    WARNING,
    ERROR,
};

class AAbstractLogger : public AOrderedStateHook<-1> {
public:
    __ANodiscard const std::string& GetPattern() const { return _pattern; }

    void SetPattern(std::string_view pattern)
    {
        _pattern = pattern;
    }

    void SetHeaderStyle(std::string_view headerStyle)
    {
        _headerStyle = headerStyle;
    }

    void SetLevel(const std::vector<ALogLevel> levelVec)
    {
        _level = 0;
        for (auto&& level : levelVec) {
            _level |= (1 << int(level));
        }
    }

    template <typename FormatStr, typename... Args>
    void Info(FormatStr&& formatStr, Args&&... args)
    {
        _Format(ALogLevel::INFO, std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
    }

    template <typename FormatStr, typename... Args>
    void Debug(FormatStr&& formatStr, Args&&... args)
    {
        _Format(ALogLevel::DEBUG, std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
    }

    template <typename FormatStr, typename... Args>
    void Warning(FormatStr&& formatStr, Args&&... args)
    {
        _Format(ALogLevel::WARNING, std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
    }

    template <typename FormatStr, typename... Args>
    void Error(FormatStr&& formatStr, Args&&... args)
    {
        _Format(ALogLevel::ERROR, std::forward<FormatStr>(formatStr), std::forward<Args>(args)...);
    }

protected:
    std::string _pattern = "#";
    std::string _headerStyle = "[#wave, #time][#level]";
    uint8_t _level = 0b1111; // 每一个 bit 代表这个水平的日志开不开(1是开)
    std::string _bufStr;
    const std::vector<std::string> _levelStr = {
        "INFO",
        "DEBUG",
        "WARNING",
        "ERROR",
    };
    virtual void _BeforeScript() override;
    virtual void _Output(ALogLevel level, std::string&& str) = 0;

    template <typename T>
    std::string _ToStr(T t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

    // 递归终止
    void _StringConvert(size_t startIdx, std::string& content)
    {
    }

    std::string _CreatHeader(ALogLevel level);

    template <typename Cur, typename... Rest>
    void _StringConvert(size_t startIdx, std::string& content, Cur&& cur, Rest&&... rest)
    {
        auto idx = content.find_first_of(_pattern, startIdx);
        if (idx != std::string::npos) {
            content.replace(idx, _pattern.size(), _ToStr(std::forward<Cur>(cur)));
        }

        _StringConvert(startIdx, content, std::forward<Rest>(rest)...);
    }

    template <typename FormatStr, typename... Args>
        requires std::is_convertible_v<FormatStr, std::string>
    void _Format(ALogLevel level, FormatStr&& formatStr, Args&&... args)
    {
        if (!(_level & (1 << int(level)))) {
            return;
        }

        auto strHeader = _CreatHeader(level);
        auto startIdx = strHeader.size();
        _bufStr = std::move(strHeader) + std::forward<FormatStr>(formatStr) + '\n';
        _StringConvert(startIdx, _bufStr, std::forward<Args>(args)...);
        _Output(level, std::move(_bufStr));
    }
};

struct AFile {
};
struct AConsole {
};
struct APvzGui {
};
struct AMsgBox {
};

template <typename T>
class ALogger;

template <>
class ALogger<AFile> : public AAbstractLogger {
public:
    ALogger(const std::string& fileName)
        : _fileName(fileName)
    {
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
    void SetColor(ALogLevel level, uint32_t color)
    {
        _color[int(level)] = color;
    }

protected:
    uint32_t _color[4];
    static bool _isAllocateConsole;
    HANDLE _handle = nullptr;
    virtual void _Output(ALogLevel level, std::string&& str) override;
    virtual void _BeforeScript() override;
};

template <>
class ALogger<APvzGui> : public AAbstractLogger {
public:
    __ANodiscard APainter& GetPainter() { return _painter; }

    // 设置显示颜色
    // *** 使用示例:
    // SetColor(ALogLevel::INFO, AArgb(0xff, 0, 0, 0xff)) ------ 将 INFO 等级的显示颜色设置为蓝色
    void SetColor(ALogLevel level, uint32_t color)
    {
        _color[int(level)] = color;
    }
    // 设定显示持续时间
    // *** 使用示例:
    // SetRemainTime(100) ------- 将显示持续时间改为 100
    void SetRemainTime(int remainTime)
    {
        _remainTime = remainTime;
    }

    // 设定显示持续时间
    // *** 使用示例:
    // SetPos(50, 100) ------- 将显示位置更改为 (50, 100) [最大为 800, 600]
    void SetPos(int x, int y)
    {
        _pixelDisplay.x = x;
        _pixelDisplay.y = y;
    }

protected:
    uint32_t _color[4];
    APainter _painter;
    int _remainTime = 500; // 控制显示的持续时间
    int _lastestDisplayedTime = -1;
    APixel _pixelDisplay = {10, 500};
    virtual void _BeforeScript() override;
    virtual void _Output(ALogLevel level, std::string&& str) override;
};

template <>
class ALogger<AMsgBox> : public AAbstractLogger {
protected:
    virtual void _Output(ALogLevel level, std::string&& str) override;
    virtual void _BeforeScript() override;
};

#endif