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
__ANodiscard bool ARangeIn(int num, std::initializer_list<int> lst);

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

struct __AInternalGlobal {
    AMainObject* mainObject;
    APvzBase* pvzBase;
    AAbstractLogger* loggerPtr;
};

extern __AInternalGlobal __aInternalGlobal;

inline void ASetInternalLogger(AAbstractLogger& logger)
{
    __aInternalGlobal.loggerPtr = &logger;
}

#endif