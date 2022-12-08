/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-10 19:29:18
 * @Description:
 */
#include "avz_logger.h"
#include "avz_time_queue.h"

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
    std::wofstream file(_fileName, std::ios::binary | std::ios::app);
    if (file.good()) { // 不向外返回文件好坏的结果
        file << AStrToWstr(str) << "\n";
        file.close();
    }
}

void ALogger<AConsole>::_Output(ALogLevel level, std::string&& str)
{
    auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, _color[int(level)]);
    std::wprintf(AStrToWstr(str).c_str());
    SetConsoleTextAttribute(handle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
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

void ALogger<AMsgBox>::_Output(ALogLevel level, std::string&& str)
{
    MessageBoxW(nullptr, AStrToWstr(str).c_str(), L"AMsgBox", MB_OK);
}

bool ALogger<AConsole>::_isAllocateConsole = false;

void ALogger<AConsole>::_BeforeScript()
{
    if (!_isAllocateConsole && AllocConsole()) {
        _isAllocateConsole = true;
        SetConsoleTitle(TEXT("AConsole"));
        freopen("CON", "w", stdout);
        setlocale(LC_ALL, "chs");
    }
}

void ALogger<AConsole>::_ExitFight()
{
    fclose(stdout);
    if (_isAllocateConsole) {
        _isAllocateConsole = false;
        FreeConsole();
    }
}
