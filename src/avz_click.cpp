/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: old click
 */

#include "avz_click.h"

namespace AvZ
{
    extern MainObject *__main_object;

    // *** Not In Queue
    // 点击种子/卡片
    // *** 使用示例：
    // ClickSeed(1) ----- 点击第一个种子
    void ClickSeed(int seed_index)
    {
        extern MainObject *__main_object;
        Asm::ClickScene(__main_object, 50 + 50 * seed_index, 70, 1);
    }

    // *** Not In Queue
    // 右键安全点击
    void SafeClick()
    {
        extern MainObject *__main_object;
        Asm::ClickScene(__main_object, 1, 1, -1);
    }

    // *** Not In Queue
    // 鼠标左击
    // LeftClick(400, 300)-----点击 PVZ 窗口中央
    void LeftClick(int x, int y)
    {
        extern MainObject *__main_object;
        Asm::ClickScene(__main_object, x, y, 1);
    }

    // *** In Queue
    // 铲除植物函数
    void Shovel(int row, float col, bool pumpkin)
    {
        InsertOperation([=]() {
            ShovelNotInQueue(row, col, pumpkin);
        },
                        "Shovel");
    }

    // *** In Queue
    // *** 使用示例：
    // Shovel(4, 6)--------铲除4行6列的植物,如果植物有南瓜保护默认铲除被保护植物
    // Shovel(4, 6, true)---铲除4行6列的植物,如果植物有南瓜保护铲除南瓜
    // Shovel({{3, 6},{4, 6}})------铲除3行6列，4行6列的植物
    void Shovel(const std::vector<ShovelCrood> &lst)
    {
        InsertOperation([=]() {
            for (const auto &crood : lst)
            {
                ShovelNotInQueue(crood.row, crood.col, crood.pumpkin);
            }
        },
                        "Shovel");
    }

    // 将格子转换成坐标
    void GridToCoordinate(int row, float col, int &x, int &y)
    {
        x = 80 * col;
        y = 0;

        if ((__main_object->scene() == 2) || (__main_object->scene() == 3))
        {
            y = 55 + 85 * row;
        }
        else if ((__main_object->scene() == 4) || (__main_object->scene() == 5))
        {
            int t_col = static_cast<int>(col + 0.5);
            if (t_col > 5)
                y = 45 + 85 * row;
            else
                y = 45 + 85 * row + (120 - 20 * t_col);
        }
        else
        {
            y = 40 + 100 * row;
        }
    }

    void ClickGrid(int row, float col, int offset)
    {
        int x = 0;
        int y = 0;

        GridToCoordinate(row, col, x, y);
        y += offset;
        LeftClick(x, y);
    }

    void ShovelNotInQueue(int row, float col, bool pumpkin)
    {
        SafeClick();
        LeftClick(50 * 13, 70);
        if (!pumpkin)
        {
            ClickGrid(row, col);
        }
        else
        {
            ClickGrid(row, col, 10);
        }
        SafeClick();
    }
} // namespace AvZ