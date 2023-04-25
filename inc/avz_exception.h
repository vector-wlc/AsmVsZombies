/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-11-15 11:06:42
 * @Description:
 */

#ifndef __AVZ_EXCEPTION_H__
#define __AVZ_EXCEPTION_H__

#include "avz_types.h"
#include <exception>
#include <string>

class AException : public std::exception { // copy from https://www.itranslater.com/qa/details/2582506091615618048
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller.
     */
    explicit AException(const char* message)
        : msg_(message)
    {
    }

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit AException(const std::string& message)
        : msg_(message)
    {
    }

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~AException() throw() { }

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    __ANodiscard virtual const char* what() const throw()
    {
        return msg_.c_str();
    }

protected:
    /** Error message.
     */
    std::string msg_;
};

// 通过抛出异常的形式让脚本立即停止运行
inline void ATerminate(const std::string& reason = "unknown")
{
    throw AException("终止脚本运行，附加信息 : " + reason);
}

constexpr const char* ASTR_GAME_RET_MAIN_UI = "game return main ui";

// 通过抛出异常的形式通知 AvZ 游戏退出了战斗界面
inline void AExitFight()
{
    throw AException(ASTR_GAME_RET_MAIN_UI);
}
#endif