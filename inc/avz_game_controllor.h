/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 15:54:19
 * @Description:
 */
#ifndef __AVZ_GAME_CONTROLLOR_H__
#define __AVZ_GAME_CONTROLLOR_H__

#include "avz_memory.h"
#include "avz_logger.h"

class __AGameControllor : public AOrderedExitFightHook<-1> {
public:
    __AGameControllor();
    APredication isSkipTick = []() -> bool {
        return false;
    };

    template <typename Pre, typename CallBack>
        requires __AIsPredication<Pre> && __AIsOperation<CallBack>
    void SkipTick(Pre&& pre, CallBack&& callback)
    {
        if (!_CheckSkipTick()) {
            return;
        }
        isSkipTick = [pre = std::forward<Pre>(pre), //
                         callback = std::forward<CallBack>(callback), this]() mutable {
            auto gameUi = AGetPvzBase()->GameUi();
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
    void SkipTick(Pre&& pre)
    {
        SkipTick(std::forward<Pre>(pre), [] {});
    }
    void SkipTick(int wave, int time);

    void SetAdvancedPause(bool isAdvancedPaused, bool isPlaySound = true, DWORD rectColor = AArgb(0x7f, 0, 0, 0));

    void SetUpdateWindow(bool isUpdateWindow);

    // 用于在高级暂停下的游戏刷新处理
    // 让高级暂停更加丝滑
    void UpdateAdvancedPause();

    bool isAdvancedPaused = false;

    bool isUpdateWindow = true;

    // 确保能否刷新游戏主要对象
    class UpdateGameObjGuard {
    public:
        UpdateGameObjGuard()
        {
            _asmBackup = AMRef<uint16_t>(_UPDATE_ASM_ADDR_BEGIN);
            AMRef<uint16_t>(_UPDATE_ASM_ADDR_BEGIN) = _oriAsm;
        }
        ~UpdateGameObjGuard()
        {
            AMRef<uint16_t>(_UPDATE_ASM_ADDR_BEGIN) = _asmBackup;
        }

    private:
        uint16_t _asmBackup = 0;
    };

protected:
    virtual void _ExitFight() override;

    bool _CheckSkipTick();

    // pvz 每帧的更新汇编代码起始地址
    static constexpr uintptr_t _UPDATE_ASM_ADDR_BEGIN = 0x41600E;

    // 跳过游戏更新的汇编代码
    // jmp 2a
    static constexpr uint16_t _JMP_ASM = 0x2AEB;

    // 保存原本的机器码
    static uint16_t _oriAsm;

    int _pvzHeight = 0;
    int _pvzWidth = 0;

    DWORD _rectColor = AArgb(0x7f, 0, 0, 0);
};

inline __AGameControllor __aGameControllor;

// 跳到游戏指定时刻
// *** 注意使用此函数时不能使用高级暂停
// *** 使用示例
// ASkipTick(1, 200) ------ 跳到时刻点 (1, 200)
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
    __aGameControllor.SkipTick(std::forward<Args>(args)...);
}

// 设定高级暂停
// *** 注意开启高级暂停时不能使用跳帧
// *** 特别注意的是 `ASetAdvancedPause` 一旦使得程序进入高级暂停状态后，
//     除了模式为 GLOBAL 和 AFTER_INJECT 的帧运行以及 AfterTick BeforeTick 的状态钩，框架将不再执行其他代码，
// *** 使用示例
// ASetAdvancedPause(true) ------ 开启高级暂停
// ASetAdvancedPause(false) ------ 关闭高级暂停
// ASetAdvancedPause(true, true) ------ 开启高级暂停，并播放相关音效
// ASetAdvancedPause(true, AArgb(0x7f, 0, 0, 0)) ------ 开启高级暂停，并在暂停时在 pvz 顶层绘制颜色为 AArgb(0x7f, 0, 0, 0) 的全屏矩形
inline void ASetAdvancedPause(bool isAdvancedPaused, bool isPlaySound = true, DWORD rectColor = AArgb(0x7f, 0, 0, 0))
{
    __aGameControllor.SetAdvancedPause(isAdvancedPaused, isPlaySound, rectColor);
}

// 设定游戏窗口是否更新
// *** 注意此函数与高级暂停的区别是：高级暂停开启时鼠标和种植物的渲染动画还是在的，
//     但是此函数设置为 false 时，游戏都不会渲染
// *** 使用示例
// ASetUpdateWindow(true) ------ 游戏更新窗口
// ASetUpdateWindow(false) ------ 游戏不更新窗口
inline void ASetUpdateWindow(bool isUpdateWindow)
{
    __aGameControllor.SetUpdateWindow(isUpdateWindow);
}
#endif