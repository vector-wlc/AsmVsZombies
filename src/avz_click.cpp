/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: API click
 */

#include "avz_click.h"

namespace AvZ {
extern MainObject* __main_object;

// *** Not In Queue
// 点击种子/卡片
// *** 使用示例：
// ClickSeed(1) ----- 点击第一个种子
void ClickSeed(int seed_index)
{
    extern MainObject* __main_object;
    auto seed = __main_object->seedArray() + seed_index - 1;
    Asm::mouseClick(int(seed->abscissa() + seed->width() / 2),
        int(seed->ordinate() + seed->height() / 2), 1);
}

// *** Not In Queue
// 右键安全点击
void SafeClick()
{
    // extern MainObject* __main_object;
    // Asm::mouseClick(20, 20, -1);
    Asm::releaseMouse();
}

// *** Not In Queue
// 鼠标左击
// LeftClick(400, 300)-----点击 PVZ 窗口中央
void LeftClick(int x, int y)
{
    extern MainObject* __main_object;
    Asm::mouseClick(x, y, 1);
}

// *** In Queue
// 铲除植物函数
void Shovel(int row, float col, bool pumpkin)
{
    InsertOperation([=]() { ShovelNotInQueue(row, col, pumpkin); },
        "Shovel");
}

// *** In Queue
// *** 使用示例：
// Shovel(4, 6)--------铲除4行6列的植物,如果植物有南瓜保护默认铲除被保护植物
// Shovel(4, 6, true)---铲除4行6列的植物,如果植物有南瓜保护铲除南瓜
// Shovel({{3, 6},{4, 6}})------铲除3行6列，4行6列的植物
void Shovel(const std::vector<ShovelPosition>& lst)
{
    InsertOperation([=]() {
        for (const auto& crood : lst) {
            ShovelNotInQueue(crood.row, crood.col, crood.pumpkin);
        }
    },
        "Shovel");
}

// 将格子转换成坐标
void GridToCoordinate(int row, float col, int& x, int& y)
{
    x = 80 * col;
    LimitValue(x, 0, 800);
    LimitValue(row, 1, 6);
    int t_col = int(col + 0.5);
    LimitValue(t_col, 1, 9);
    y = Asm::gridToOrdinate(row - 1, t_col - 1) + 40;
}

void ClickGrid(int row, float col, int offset)
{
    int x = 0;
    int y = 0;
    col = int(col + 0.5);
    GridToCoordinate(row, col, x, y);
    y += offset;
    LeftClick(x, y);
}

void ShovelNotInQueue(int row, float col, bool pumpkin)
{
    SafeClick();
    LeftClick(50 * 13, 70);

    int x = 0;
    int y = 0;
    col = int(col + 0.5);
    GridToCoordinate(row, col, x, y);

    if (pumpkin) {
        y += 40;
    }
    extern int __error_mode;
    if (__error_mode == CONSOLE) {
        Print("Shovel (%d, %g)\n", row, col);
    }
    Asm::shovelPlant(x, y);
    SafeClick();
}
} // namespace AvZ