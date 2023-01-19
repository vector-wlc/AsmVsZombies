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

void AClickSeed(int seed_index)
{
    auto seed = __aInternalGlobal.mainObject->SeedArray() + seed_index - 1;
    AAsm::MouseClick(int(seed->Abscissa() + seed->Width() / 2),
        int(seed->Ordinate() + seed->Height() / 2), 1);
}

void ALeftClick(int x, int y)
{
    AAsm::MouseClick(x, y, 1);
}

void AShovel(int row, float col, bool pumpkin)
{
    AAsm::ReleaseMouse();

    int x = 0;
    int y = 0;
    AGridToCoordinate(row, col, x, y);
    if (pumpkin) {
        y += 25;
    }
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    __aInternalGlobal.loggerPtr->Info("Shovel (" + pattern + ", " + pattern + ")", row, col);
    AAsm::ShovelPlant(x, y);
    AAsm::ReleaseMouse();
}

void AShovel(const std::vector<AShovelPosition>& lst)
{
    for (const auto& crood : lst) {
        AShovel(crood.row, crood.col, crood.pumpkin);
    }
}

// 将格子转换成坐标
void AGridToCoordinate(int row, float col, int& x, int& y)
{
    x = 80 * col;
    ALimitValue(x, 0, 800);
    ALimitValue(row, 1, 6);
    int tCol = int(col + 0.5);
    ALimitValue(tCol, 1, 9);
    y = AAsm::GridToOrdinate(row - 1, tCol - 1) + 40;
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
