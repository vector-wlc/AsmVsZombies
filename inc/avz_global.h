/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-06 15:53:08
 * @Description:
 */

#ifndef __AVZ_GLOBAL_H__
#define __AVZ_GLOBAL_H__

#include "avz_pvz_struct.h"
#include "avz_types.h"
#include <Windows.h>
#include <map>
#include <string>

#undef min
#undef max
#undef ERROR

__ANodiscard std::wstring AStrToWstr(const std::string& input);

// *** 函数功能：判断数字范围
// *** 使用示例：
// RangeIn(wave, {1,2,3})------如果波数在 1 2 3 范围里返回 true
__ANodiscard bool ARangeIn(int num, const std::vector<int>& lst);

// 寻找 vector 中相同的元素，返回其迭代器
template <typename Ele>
__ANodiscard auto AFindSameEle(const std::vector<Ele>& container, const Ele& ele_) -> std::vector<decltype(container.begin())>
{
    std::vector<decltype(container.begin())> result;
    for (auto it = container.begin(); it != container.end(); ++it) {
        if ((*it) == ele_) {
            result.push_back(it);
        }
    }

    return result;
}

// 寻找 vector 中相同的元素，返回其迭代器
template <typename Ele>
__ANodiscard auto AFindSameEle(std::vector<Ele>& container, const Ele& ele_) -> std::vector<decltype(container.begin())>
{
    std::vector<decltype(container.begin())> result;
    for (auto it = container.begin(); it != container.end(); ++it) {
        if ((*it) == ele_) {
            result.push_back(it);
        }
    }

    return result;
}

template <typename T>
concept __AIsNumber = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <typename T>
    requires __AIsNumber<T>
void ALimitValue(T& value, T min_v, T max_v)
{
    if (value < min_v) {
        value = min_v;
    }
    if (value > max_v) {
        value = max_v;
    }
}

class AAbstractLogger;
class __ATickManager;

struct __AInternalGlobal {
    AMainObject* mainObject = nullptr;
    APvzBase* pvzBase = nullptr;
    AAbstractLogger* loggerPtr = nullptr;
    __ATickManager* tickInFight = nullptr;
    __ATickManager* tickInGlobal = nullptr;
};

extern __AInternalGlobal __aInternalGlobal;

inline void ASetInternalLogger(AAbstractLogger& logger)
{
    __aInternalGlobal.loggerPtr = &logger;
}

// 注意这个函数返回的是对象指针
inline AAbstractLogger* AGetInternalLogger()
{
    return __aInternalGlobal.loggerPtr;
}

#define __AMsgBox(msg) MessageBoxW(nullptr, (msg), L"AMsgBox", MB_OK);

template <typename Var, typename Val>
class AVarGuard {
public:
    AVarGuard(Var& var, Val val)
        : _var(var)
    {
        _tmp = std::move(var);
        _var = std::move(val);
    }

    AVarGuard(const AVarGuard&) = delete;
    AVarGuard(AVarGuard&&) = delete;

    ~AVarGuard()
    {
        _var = std::move(_tmp);
    }

protected:
    Var _tmp;
    Var& _var;
};

#endif