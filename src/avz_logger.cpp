/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-10 19:29:18
 * @Description:
 */
#include "libavz.h"

void AAbstractLogger::_BeforeScript()
{
    _pattern = "#";
    _headerStyle = "[#wave, #time][#level] ";
}

void AAbstractLogger::_Replace(std::string& content, std::string_view pattern, std::string_view replaceStr)
{
    if (pattern.empty()) {
        return;
    }
    size_t idx = 0;
    while (true) {
        idx = content.find(pattern, idx);
        if (idx == std::string::npos) {
            break;
        }
        content.replace(idx, pattern.size(), replaceStr);
        idx += replaceStr.size();
    }
}

std::string AAbstractLogger::_CreateHeader(ALogLevel level)
{
    std::string headerFormat = _headerStyle;
    _Replace(headerFormat, _pattern + "wave", "{0}");
    _Replace(headerFormat, _pattern + "time", "{1}");
    _Replace(headerFormat, _pattern + "level", "{2}");
    auto now = ANowTime();
    return std::vformat(headerFormat, std::make_format_args(now.wave, now.time, _levelStr[int(level)]));
}

void ALogger<AFile>::_Output(ALogLevel level, std::string&& str)
{
    if (!_outFile.is_open()) {
        _outFile.open(_fileName, std::ios::out | std::ios::app);
        if (!_outFile.good() && aLogger != this) {
            aLogger->Error("无法打开 #", _fileName);
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
    if (!_outFile.good() && aLogger != this) {
        aLogger->Error("无法打开 {}", _fileName);
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

ALogger<APvzGui>::ALogger()
{
    _textColors[0] = AArgb(0xff, 0xff, 0xff, 0xff); // white
    _textColors[1] = AArgb(0xff, 0, 0xff, 0);       // green
    _textColors[2] = AArgb(0xff, 0xff, 0xff, 0);    // yellow
    _textColors[3] = AArgb(0xff, 0xff, 0, 0);       // red
    _rectColors[0] = AArgb(0xaf, 0, 0, 0);
    _rectColors[1] = AArgb(0xaf, 0, 0, 0);
    _rectColors[2] = AArgb(0xaf, 0, 0, 0);
    _rectColors[3] = AArgb(0xaf, 0, 0, 0);
    _remainTime = 500; // 控制显示的持续时间
    _pixelDisplay = {10, 500};
}

void ALogger<APvzGui>::_Output(ALogLevel level, std::string&& str)
{
    int lineCnt = 0;
    for (auto c : str) {
        lineCnt += (c == '\n');
    }
    _curBottom += lineCnt * _painter.GetFontSize();
    int globalClock = AGetMainObject()->GlobalClock();
    _displayList.push_back({level, std::move(str), globalClock, lineCnt});
    _ShowTick();
}

void ALogger<APvzGui>::_ShowTick()
{
    int globalClock = AGetMainObject()->GlobalClock();
    if (globalClock == _lastTick) {
        return; // 一帧只显示一次
    }
    _lastTick = globalClock;
    int x = _pixelDisplay.x;
    int y = _pixelDisplay.y;
    int eraseCnt = _displayList.size();
    for (auto iter = _displayList.rbegin(); iter != _displayList.rend(); ++iter, --eraseCnt) {
        if (iter->gameClock + _remainTime < globalClock) { // 持续时间到，需要删除
            break;
        }
        y -= _painter.GetFontSize() * iter->lineCnt;
        _painter.SetTextColor(_textColors[int(iter->level)]);
        _painter.SetRectColor(_rectColors[int(iter->level)]);
        _painter.Draw(AText(iter->str, x, y + _curBottom));
    }
    for (int i = 0; i < eraseCnt; ++i) {
        _displayList.pop_front();
    }
    if (_curBottom != 0) {
        _curBottom -= _transitSpeed;
        _curBottom = std::clamp(_curBottom, 0, INT_MAX);
    }
}

void ALogger<APvzGui>::_BeforeScript()
{
    AAbstractLogger::_BeforeScript();
    _curBottom = 0;
    _displayList.clear();
    _lastTick = INT_MIN;
    _tickRunner = std::make_shared<ATickRunner>([this] {
        this->_ShowTick();
    },
        ATickRunner::GLOBAL);
}

void ALogger<AMsgBox>::_Output(ALogLevel level, std::string&& str)
{
    AMsgBox::Show(str);
}

void ALogger<AMsgBox>::_BeforeScript()
{
    AAbstractLogger::_BeforeScript();
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
    _color[0] = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE; // white
    _color[1] = FOREGROUND_GREEN;                                    // green
    _color[2] = FOREGROUND_RED | FOREGROUND_GREEN;                   // yellow
    _color[3] = FOREGROUND_RED;                                      // red

    // 完成后，无需使用 CloseHandle 释放此句柄
    _handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

ALogger<AConsole>::~ALogger()
{
    if (_isAllocateConsole) {
        fclose(stdout);
        _isAllocateConsole = false;
        FreeConsole();
    }
}
