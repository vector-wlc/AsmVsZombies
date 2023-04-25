/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-10 19:29:18
 * @Description:
 */
#include "avz_logger.h"
#include "avz_time_queue.h"
#include "avz_memory.h"

void AAbstractLogger::_BeforeScript()
{
    _pattern = "#";
    _headerStyle = "[#wave, #time][#level]";
}

std::string AAbstractLogger::_CreatHeader(ALogLevel level)
{
    if (_headerStyle.empty()) {
        return "";
    }
    std::string header = _headerStyle;
    auto nowTime = ANowTime();
    auto idx = header.find(_pattern + "time");
    if (idx != std::string::npos) {
        header.replace(idx, _pattern.size() + 4, std::to_string(nowTime.time));
    }

    idx = header.find(_pattern + "wave");
    if (idx != std::string::npos) {
        header.replace(idx, _pattern.size() + 4, std::to_string(nowTime.wave));
    }

    idx = header.find(_pattern + "level");
    if (idx != std::string::npos) {
        header.replace(idx, _pattern.size() + 5, _levelStr[int(level)]);
    }
    return header + ' ';
}

void ALogger<AFile>::_Output(ALogLevel level, std::string&& str)
{
    if (!_outFile.is_open()) {
        _outFile.open(_fileName, std::ios::out | std::ios::app);
        if (!_outFile.good() && AGetInternalLogger() != this) {
            AGetInternalLogger()->Error("无法打开 #", _fileName);
            return;
        }
        _outFile.imbue(std::locale(""));
    }
    if (_outFile.good()) {
        _outFile << AStrToWstr(str);
        _outFile.flush();
    }
}

void ALogger<AFile>::_ExitFight()
{
    _outFile.close();
}

// 清除文件中的所有内容
bool ALogger<AFile>::Clear()
{
    if (!_outFile.good()) {
        return false;
    }
    _outFile.close();
    _outFile.open(_fileName, std::ios::out);
    if (!_outFile.good() && AGetInternalLogger() != this) {
        AGetInternalLogger()->Error("无法打开 #", _fileName);
        return false;
    }
    _outFile.imbue(std::locale(""));
    return true;
}

void ALogger<AConsole>::_Output(ALogLevel level, std::string&& str)
{
    SetConsoleTextAttribute(_handle, _color[int(level)]);
    std::wprintf(AStrToWstr(str).c_str());
    SetConsoleTextAttribute(_handle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
}

void ALogger<APvzGui>::_Output(ALogLevel level, std::string&& str)
{
    int globalClock = __aInternalGlobal.mainObject->GlobalClock();
    if (globalClock - _lastestDisplayedTime > _remainTime) { // 上一条显示已经结束
        auto oriColor = _painter.GetTextColor();
        _painter.SetTextColor(_color[int(level)]);
        _painter.Draw(AText(std::move(str), _pixelDisplay.x, _pixelDisplay.y), _remainTime);
        _painter.SetTextColor(oriColor);
        _lastestDisplayedTime = globalClock;
    }
}
void ALogger<APvzGui>::_BeforeScript()
{
    AAbstractLogger::_BeforeScript();
    _color[0] = AArgb(0xff, 0xff, 0xff, 0xff); // white
    _color[1] = AArgb(0xff, 0, 0xff, 0);       // green
    _color[2] = AArgb(0xff, 0xff, 0xff, 0);    // yellow
    _color[3] = AArgb(0xff, 0xff, 0, 0);       // red
    _remainTime = 500;                         // 控制显示的持续时间
    _lastestDisplayedTime = -1;
    _pixelDisplay = {10, 500};
}

void ALogger<AMsgBox>::_Output(ALogLevel level, std::string&& str)
{
    MessageBoxW(nullptr, AStrToWstr(str).c_str(), L"AMsgBox", MB_OK);
}

bool ALogger<AConsole>::_isAllocateConsole = false;

ALogger<AConsole>::ALogger()
{
    if (!_isAllocateConsole && AllocConsole()) {
        _isAllocateConsole = true;
        SetConsoleTitle(TEXT("AConsole"));
        freopen("CON", "w", stdout);
        setlocale(LC_ALL, "chs");
    }
    // 完成后，无需使用 CloseHandle 释放此句柄
    _handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

ALogger<AConsole>::~ALogger()
{
    fclose(stdout);
    if (_isAllocateConsole) {
        _isAllocateConsole = false;
        FreeConsole();
    }
}

void ALogger<AConsole>::_BeforeScript()
{
    AAbstractLogger::_BeforeScript();
    _color[0] = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE; // white
    _color[1] = FOREGROUND_GREEN;                                    // green
    _color[2] = FOREGROUND_RED | FOREGROUND_GREEN;                   // yellow
    _color[3] = FOREGROUND_RED;                                      // red
    Info("\n=================================\n脚本开始运行\n=================================");
}
