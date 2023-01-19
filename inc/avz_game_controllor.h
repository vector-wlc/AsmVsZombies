/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 15:54:19
 * @Description:
 */
#ifndef __AVZ_GAME_CONTROLLOR_H__
#define __AVZ_GAME_CONTROLLOR_H__

#include "avz_logger.h"

class __AGameControllor : public AOrderedStateHook<-1> {
public:
    static bool isAdvancedPaused;
    static APredication isSkipTick;

    template <typename Pre, typename CallBack>
        requires __AIsPredication<Pre> && __AIsOperation<CallBack>
    static void SkipTick(
        Pre&& pre, CallBack&& callback)
    {
        if (!_CheckSkipTick()) {
            return;
        }
        isSkipTick = [pre = std::forward<Pre>(pre), //
                         callback = std::forward<CallBack>(callback)]() mutable {
            auto gameUi = __aInternalGlobal.pvzBase->GameUi();
            if (gameUi == 3 && pre()) {
                return true;
            }
            isSkipTick = []() -> bool { return false; };
            if (gameUi == 3) {
                callback();
            }
            return false;
        };
    }
    template <typename Pre>
        requires __AIsPredication<Pre>
    static void SkipTick(Pre&& pre)
    {
        SkipTick(std::forward<Pre>(pre), [] {});
    }
    static void SkipTick(int wave, int time);

protected:
    virtual void _ExitFight() override
    {
        isSkipTick = []() -> bool {
            return false;
        };
    }

    static bool _CheckSkipTick();
};

// 跳到游戏指定时刻
// *** 注意使用此函数时不能使用高级暂停
// *** 使用示例
// ASkipTick(200, 1) ------ 跳到时刻点 (200, 1)
//
// 跳到指定条件为 false 的游戏帧
// *** 注意使用此函数时不能使用高级暂停
// *** 使用示例 : 直接跳到位置为 {1, 3}, {1, 5} 春哥死亡时的游戏帧
// auto condition = [=]() {
//     std::vector<int> results;
//     GetPlantIndices({{1, 3}, {1, 5}}, YMJNP_47, results);
//
//     for (auto result : results) {
//         if (result < 0) {
//             return false;
//         }
//     }
//     return true;
// };
//
// auto callback = [=]() {
//     // 写春哥没了的提示代码，比如用一个 ALogger 显示信息
// };
//
// ASkipTick(condition, callback);
template <typename... Args>
void ASkipTick(Args&&... args)
{
    __AGameControllor::SkipTick(std::forward<Args>(args)...);
}

// 设置高级暂停
// ***使用示例
// ASetAdvancedPause(true) ----- 游戏高级暂停
// ASetAdvancedPause(false) ----- 游戏不高级暂停
inline void ASetAdvancedPause(bool isAdvancedPaused)
{
    __AGameControllor::isAdvancedPaused = isAdvancedPaused;
}
#endif