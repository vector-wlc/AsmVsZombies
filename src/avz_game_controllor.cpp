#include "libavz.h"

namespace {
APainter _rectPainter;
}

// 保存原本的机器码
uint16_t __AGameControllor::_oriAsm = 0;

__AGameControllor::__AGameControllor() {
    // 保存原本的机器码
    _oriAsm = AMRef<uint16_t>(_UPDATE_ASM_ADDR_BEGIN);

    // 初始化高级暂停的工具信息
    RECT rect;
    GetClientRect(AGetPvzBase()->MRef<HWND>(0x350), &rect);
    _pvzHeight = rect.bottom - rect.top;
    _pvzWidth = rect.right - rect.left;
    _rectPainter.SetRectColor(_rectColor);
}

bool __AGameControllor::_CheckSkipTick() {
    if (isAdvancedPaused) {
        aLogger->Error("开启高级暂停或者暂停时不能启用跳帧");
        return false;
    }

    if (isSkipTick()) {
        aLogger->Error("请等待上一个跳帧条件达到后的下一帧再设定跳帧条件");
        return false;
    }

    return true;
}

void __AGameControllor::SkipTick(int wave, int time) {
    SkipTick([=]() {
        int nowTime = ANowTime(wave);
        if (nowTime == __AOperationQueue::UNINIT || nowTime < time) { // 时间未到达
            return true;
        }
        if (nowTime > time) { // 时间已到达
            aLogger->Error("无法回跳时间点");
        }
        return false;
    });
}

void __AGameControllor::SetAdvancedPause(bool isAdvancedPaused, bool isPlaySound, DWORD rectColor) {
    if (this->isAdvancedPaused == isAdvancedPaused) {
        return;
    }
    if (isAdvancedPaused && AGetPvzBase()->GameUi() != 3) {
        aLogger->Error("高级暂停仅能在战斗界面使用");
        return;
    }
    this->isAdvancedPaused = isAdvancedPaused;
    auto asmCode = isAdvancedPaused ? _JMP_ASM : _oriAsm;
    auto soundIdx = isAdvancedPaused ? 0x15 : 0x3A;
    AMRef<uint16_t>(_UPDATE_ASM_ADDR_BEGIN) = asmCode;
    if (isPlaySound) {
        AAsm::PlaySample(soundIdx);
    }
    _rectPainter.SetRectColor(rectColor);
}

void __AGameControllor::SetUpdateWindow(bool isUpdateWindow) {
    this->isUpdateWindow = isUpdateWindow;
}

void __AGameControllor::UpdateAdvancedPause() {
    _rectPainter.Draw(ARect(0, 0, _pvzWidth, _pvzHeight));
    AAsm::UpdateCursorObjectAndPreview();
    --AGetMainObject()->GlobalClock();
    --AGetPvzBase()->MjClock();
    int levelId = AGetPvzBase()->LevelId(); // 关卡序号
    if (AAsm::PUZZLE_I_ZOMBIE_1 <= levelId && levelId <= AAsm::PUZZLE_I_ZOMBIE_ENDLESS) {
        // 刷新卡片数组
        AAsm::RefreshAllSeedPackets();
        // 因为 IZE 模式的僵尸渲染位置有问题，
        // 所以需要更新僵尸位置
        for (auto& zombie : aAliveZombieFilter) {
            zombie.MRef<int>(0x8) = zombie.Abscissa();
            zombie.MRef<int>(0xC) = zombie.Ordinate();
        }
    }
}

void __AGameControllor::_ExitFight() {
    isSkipTick = [] {
        return false;
    };
    // 恢复原函数调用
    AMRef<uint16_t>(_UPDATE_ASM_ADDR_BEGIN) = _oriAsm;
    isUpdateWindow = true;
}
