/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 16:22:00
 * @Description: click api
 */
#ifndef __AVZ_CLICK_H__
#define __AVZ_CLICK_H__

#include "avz_time_operation.h"
#include "pvzfunc.h"

namespace AvZ {
struct ShovelPosition {
    int row;
    float col;
    bool pumpkin = false;
};

// 将格子转换成坐标
void GridToCoordinate(int row, float col, int& x, int& y);

// *** Not In Queue
// 点击格子
// *** 使用示例：
// ClickGrid(3, 4)---- 点击格子(3, 4)
// ClickGrid(3, 4, 10)---- 向下偏移10像素点击格子(3, 4)
void ClickGrid(int row, float col, int offset = 0);

// *** Not In Queue
// 点击种子/卡片
// *** 使用示例：
// ClickSeed(1) ----- 点击第一个种子
void ClickSeed(int seed_index);

// *** Not In Queue
// 右键安全点击
void SafeClick();

// *** Not In Queue
// 鼠标左击
// LeftClick(400, 300)-----点击 PVZ 窗口中央
void LeftClick(int x, int y);

void ShovelNotInQueue(int row, float col, bool pumpkin = false);

// *** In Queue
// 铲除植物函数
void Shovel(int row, float col, bool pumpkin = false);

// *** In Queue
// *** 使用示例：
// Shovel(4, 6)--------铲除4行6列的植物,如果植物有南瓜保护默认铲除被保护植物
// Shovel(4, 6, true)---铲除4行6列的植物,如果植物有南瓜保护铲除南瓜
// Shovel({{3, 6},{4, 6}})------铲除3行6列，4行6列的植物
void Shovel(const std::vector<ShovelPosition>& lst);

} // namespace AvZ
#endif