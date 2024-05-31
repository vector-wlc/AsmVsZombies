/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 16:04:38
 * @Description:
 */
#include "avz_game_controllor.h"
#include "avz_time_queue.h"
#include "avz_asm.h"

static APainter rectPainter;

__AGameControllor::__AGameControllor()
{
    _updateOriAsm.resize(_UPDATE_ASM_ADDR_SIZE);
    _updateNopAsm.assign(_UPDATE_ASM_ADDR_SIZE, 0x90);
    // 保存原本的机器码
    std::copy((uint8_t*)_UPDATE_ASM_ADDR_BEGIN, (uint8_t*)_UPDATE_ASM_ADDR_END, _updateOriAsm.begin());

    // 初始化高级暂停的工具信息
    RECT rect;
    GetClientRect(AGetPvzBase()->MRef<HWND>(0x350), &rect);
    _pvzHeight = rect.bottom - rect.top;
    _pvzWidth = rect.right - rect.left;
    rectPainter.SetRectColor(AArgb(0x7f, 0, 0, 0));
}

bool __AGameControllor::_CheckSkipTick()
{
    if (isAdvancedPaused) {
        __aig.loggerPtr->Error("开启高级暂停或者暂停时不能启用跳帧");
        return false;
    }

    if (isSkipTick()) {
        __aig.loggerPtr->Error("请等待上一个跳帧条件达到后的下一帧再设定跳帧条件");
        return false;
    }

    return true;
}
void __AGameControllor::SkipTick(int wave, int time)
{
    SkipTick([=]() {
        int nowTime = ANowTime(wave);
        if (nowTime == __AOperationQueue::UNINIT || nowTime < time) { // 时间未到达
            return true;
        }
        if (nowTime > time) { // 时间已到达
            __aig.loggerPtr->Error("无法回跳时间点");
        }
        return false;
    });
}

void __AGameControllor::SetAdvancedPause(bool isAdvancedPaused)
{
    if (this->isAdvancedPaused == isAdvancedPaused) {
        return;
    }
    if (isAdvancedPaused && AGetPvzBase()->GameUi() != 3) {
        AGetInternalLogger()->Error("高级暂停仅能在战斗界面使用");
        return;
    }
    this->isAdvancedPaused = isAdvancedPaused;
    if (isAdvancedPaused) {
        std::copy(_updateNopAsm.begin(), _updateNopAsm.end(), (uint8_t*)_UPDATE_ASM_ADDR_BEGIN);
        AAsm::PlaySample(0x15); // 暂停音效
    } else {
        std::copy(_updateOriAsm.begin(), _updateOriAsm.end(), (uint8_t*)_UPDATE_ASM_ADDR_BEGIN);
        AAsm::PlaySample(0x3A); // 继续音效
    }
}

void __AGameControllor::UpdateAdvancedPause()
{
    rectPainter.Draw(ARect(0, 0, _pvzWidth, _pvzHeight));
    AAsm::UpdateCursorObjectAndPreview();
    AGetMainObject()->GlobalClock() -= 1;
    AGetPvzBase()->MjClock() -= 1;
    int gameIdx = AGetPvzBase()->MRef<int>(0x7f8); // 关卡序号
    if (gameIdx >= AAsm::PUZZLE_I_ZOMBIE_1
        && gameIdx <= AAsm::PUZZLE_I_ZOMBIE_ENDLESS) {
        // 刷新卡片数组
        AAsm::RefreshAllSeedPackets();
        auto zombieBegin = AGetMainObject()->ZombieArray();
        auto zombieEnd = zombieBegin + AGetMainObject()->ZombieTotal();

        // 因为 IZE 模式的僵尸渲染位置有问题，
        // 所以需要更新僵尸位置
        for (auto zombie = zombieBegin; zombie != zombieEnd; ++zombie) {
            if (zombie->IsDead() || zombie->IsDisappeared()) {
                continue;
            }
            zombie->MRef<int>(0x8) = zombie->Abscissa();
            zombie->MRef<int>(0xC) = zombie->Ordinate();
        }
    }
}

void __AGameControllor::_ExitFight()
{
    isSkipTick = []() -> bool {
        return false;
    };
    // 恢复原函数调用
    std::copy(_updateOriAsm.begin(), _updateOriAsm.end(), (uint8_t*)_UPDATE_ASM_ADDR_BEGIN);
}
