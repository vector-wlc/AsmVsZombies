#ifndef __AVZ_CLICK_H__
#define __AVZ_CLICK_H__

#include "avz_logger.h"

struct AShovelPosition {
    int row;
    float col;
    int targetType;

    AShovelPosition(int row_, float col_, int targetType_ = -1)
        : row(row_)
        , col(col_)
        , targetType(targetType_) {}

    AShovelPosition(int row_, float col_, bool pumpkin)
        : row(row_)
        , col(col_)
        , targetType(pumpkin ? -2 : -1) {}
};

// 将格子转换成坐标
void AGridToCoordinate(int row, float col, int& x, int& y);
std::pair<int, int> AGridToCoordinate(int row, float col);

// 点击格子
// *** 使用示例：
// AClickGrid(3, 4)---- 点击格子(3, 4)
// AClickGrid(3, 4, 10)---- 向下偏移10像素点击格子(3, 4)
void AClickGrid(int row, float col, int offset = 0);

// 点击种子/卡片
// *** 使用示例：
// AClickSeed(1) ----- 点击第一个种子
void AClickSeed(int seedIndex);

// 鼠标左击
// ALeftClick(400, 300)-----点击 PVZ 窗口中央
void ALeftClick(int x, int y);

// 铲除植物函数
// *** 使用示例：
// AShovel(4, 6)--------铲除4行6列的植物,如果植物有南瓜保护默认铲除被保护植物
// AShovel(4, 6, APUMPKIN)---铲除4行6列的植物,如果植物有南瓜保护铲除南瓜
// AShovel({{3, 6},{4, 6}})------铲除3行6列，4行6列的植物
void AShovel(int row, float col, bool pumpkin);
void AShovel(int row, float col, int targetType = -1);
void AShovel(const std::vector<AShovelPosition>& lst);

#endif
