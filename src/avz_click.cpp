/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-13 14:54:52
 * @Description:
 */

#include "avz_click.h"
#include "avz_asm.h"
#include "avz_global.h"
#include "avz_memory.h"
#include <algorithm>

void AClickSeed(int seed_index)
{
    auto seed = AGetMainObject()->SeedArray() + seed_index - 1;
    AAsm::MouseClick(int(seed->Abscissa() + seed->Width() / 2),
        int(seed->Ordinate() + seed->Height() / 2), 1);
}

void ALeftClick(int x, int y)
{
    AAsm::MouseClick(x, y, 1);
}

void AShovel(int row, float col, int targetType)
{
    if (targetType >= AM_PEASHOOTER) {
        AShovel(row, col, AIMITATOR);
        targetType -= AM_PEASHOOTER;
    }

    AAsm::ReleaseMouse();
    auto [x, y] = AGridToCoordinate(row, col);
    if (targetType == ACOFFEE_BEAN) {
        y -= 30;
    } else if (targetType == -2 || targetType == APUMPKIN) {
        y += 30;
    }

    if (targetType >= 0 && !AGetPlantPtr(row, col, targetType)) {
        return;
    }

    auto&& pattern = __aig.loggerPtr->GetPattern();
    __aig.loggerPtr->Info("Shovel (" + pattern + ", " + pattern + ")", row, col);
    for (int i = 0; i < 10; ++i) {
        ALeftClick(x, y);
    }
    AAsm::ShovelPlant(x, y);
    AAsm::ReleaseMouse();
}

void AShovel(int row, float col, bool pumpkin)
{
    AShovel(row, col, pumpkin ? -2 : -1);
}

void AShovel(const std::vector<AShovelPosition>& lst)
{
    for (auto&& crood : lst) {
        AShovel(crood.row, crood.col, crood.targetType);
    }
}

// 将格子转换成坐标
void AGridToCoordinate(int row, float col, int& x, int& y)
{
    int tCol = std::clamp(int(col + 0.5), 1, 10);
    x = col * 80;
    y = AAsm::GridToOrdinate(row - 1, tCol - 1) + 40;
    if (col <= 0 || row <= 0 || x < 0 || x >= 800 || y < 0 || y >= 600) {
        AGetInternalLogger()->Error("您输入的格子位置参数 : ("
                + std::to_string(row) + ", " + AGetInternalLogger()->GetPattern()
                + ") 已溢出, 已帮您自动调整为边界值",
            col);
    }
    x = std::clamp(x, 0, 800 - 1);
    y = std::clamp(y, 0, 600 - 1);
}

std::pair<int, int> AGridToCoordinate(int row, float col)
{
    int x;
    int y;
    AGridToCoordinate(row, col, x, y);
    return {x, y};
}

void AClickGrid(int row, float col, int offset)
{
    int x = 0;
    int y = 0;
    col = int(col + 0.5);
    AGridToCoordinate(row, col, x, y);
    y += offset;
    ALeftClick(x, y);
}
