#ifndef __AVZ_EXCEPTION_H__
#define __AVZ_EXCEPTION_H__

#include "avz_types.h"
#include <exception>
#include <string>

class AException : public std::exception {
public:
    explicit AException(const char* message)
        : msg_(message) {}

    explicit AException(const std::string& message)
        : msg_(message) {}

    virtual ~AException() noexcept {}

    __ANodiscard virtual const char* what() const noexcept {
        return msg_.c_str();
    }

protected:
    std::string msg_;
};

// 通过抛出异常的形式让脚本立即停止运行
inline void ATerminate(const std::string& reason = "unknown") {
    throw AException("终止脚本运行，附加信息 : " + reason);
}

constexpr const char* ASTR_GAME_RET_MAIN_UI = "game return main ui";

// 通过抛出异常的形式通知 AvZ 游戏退出了战斗界面
inline void AExitFight() {
    throw AException(ASTR_GAME_RET_MAIN_UI);
}

#endif
