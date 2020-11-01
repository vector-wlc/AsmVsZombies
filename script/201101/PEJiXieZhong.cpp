/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-14 21:09:30
 * @Description: PE 机械钟
 */

#include "avz.h"

using namespace AvZ;

#define CANNON_FODDER_1 8 // 小喷菇是垫材一号
#define CANNON_FODDER_2 9 // 大喷菇是垫材二号

struct MainObjectAdd : public MainObject
{
    // 得到冰道横坐标
    int &iceAbscissa(int row)
    {
        return (int &)((uint8_t *)this)[0x60c + 4 * (row - 1)];
    }
};

MainObjectAdd *main_object_add;

// 判断格子是否可种植
bool IsPlantable(Grid &grid, bool is_ash);

// 是否巨人正在举锤
bool IsHammering(Grid &grid);

// 得到放置垫材的格子
// 主要阻挡橄榄、红眼
Grid GetSetGrid();

// 放置垫材
void SetCannonFodder();

TickRunner cannon_fodder_seter;

void Script()
{
    OpenMultipleEffective();
    SetZombies({
        FOOTBALL_ZOMBIE,
        GIGA_GARGANTUAR,
        ZOMBONI,
        ZOMBIE,
        ZOMBIE,
        ZOMBIE,
        ZOMBIE,
        ZOMBIE,
        ZOMBIE,
        ZOMBIE,
        ZOMBIE,
        ZOMBIE,
    });

    SelectCards({
        ICE_SHROOM,
        M_ICE_SHROOM,
        COFFEE_BEAN,
        DOOM_SHROOM,
        CHERRY_BOMB,
        JALAPENO,
        PUMPKIN,
        SQUASH,
        PUFF_SHROOM,
        FUME_SHROOM,
    });

    KeyConnect('Q', [=]() {
        SetErrorMode(NONE);
    });
    KeyConnect('W', [=]() {
        SetErrorMode(POP_WINDOW);
    });

    SetInsertOperation(false);

    cannon_fodder_seter.pushFunc(SetCannonFodder);
}

// 判断格子是否可种植
// 这里只判断是否有冰车和冰道
bool IsPlantable(Grid &grid, bool is_ash)
{
    // 判断冰道
    main_object_add = (MainObjectAdd *)GetMainObject().toUnsafe();
    if (grid.col * 80 + 30 > main_object_add->iceAbscissa(grid.row))
    {
        return false;
    }

    if (is_ash)
    {
        return true;
    }

    // 判断冰车
    auto zombie_array = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie_array[index].isExist() &&
            !zombie_array[index].isDead() &&
            zombie_array[index].type() == ZOMBONI &&
            zombie_array[index].row() + 1 == grid.row &&
            zombie_array[index].abscissa() < grid.col * 80 + 30)
        {
            return false;
        }
    }

    return true;
}

// 是否巨人正在举锤
// 不是很准
bool IsHammering(Grid &grid)
{
    auto zombie_array = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (zombie_array[index].isExist() &&
            !zombie_array[index].isDead() &&
            zombie_array[index].type() == GIGA_GARGANTUAR &&
            zombie_array[index].row() + 1 == grid.row &&
            zombie_array[index].abscissa() < (grid.col + 0.6) * 80 &&
            zombie_array[index].abscissa() > (grid.col - 0.4) * 80 &&
            zombie_array[index].isHammering())
        {
            return true;
        }
    }

    return false;
}

// 得到放置垫材的格子
Grid GetSetGrid()
{
    auto zombie_array = GetMainObject()->zombieArray();

    Crood set_crood = {0, 1000};

    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
    {
        if (!zombie_array[index].isDead() &&
            !zombie_array[index].isDisappeared() &&
            (zombie_array[index].type() == GIGA_GARGANTUAR || zombie_array[index].type() == FOOTBALL_ZOMBIE))
        {
            if (zombie_array[index].abscissa() > set_crood.col ||
                zombie_array[index].row() + 1 != 1 &&
                    zombie_array[index].row() + 1 != 6 ||
                zombie_array[index].hp() + zombie_array[index].oneHp() < 400)
            {
                continue;
            }
            set_crood.row = zombie_array[index].row() + 1;
            set_crood.col = zombie_array[index].abscissa();
            if (zombie_array[index].type() == FOOTBALL_ZOMBIE)
            {
                // 橄榄球判定比较靠右
                set_crood.col += 20;
            }
        }
    }

    Grid set_grid = {-1, -1};
    if (set_crood.row != 0)
    {
        set_crood.col += 70;
        set_crood.col /= 80;
        set_grid.col = set_crood.col > 9 ? 9 : static_cast<int>(set_crood.col);
        set_grid.row = set_crood.row;
    }
    else
    {
        return set_grid;
    }

    while (!IsPlantable(set_grid, false))
    {
        if (set_grid.col < 5)
        {
            break;
        }
        --set_grid.col;
    }

    if (set_grid.col < 5 ||                                // 防守失败
        GetPlantIndex(set_grid.row, set_grid.col) != -1 || // 放置垫材的格子有植物
        IsHammering(set_grid)                              // 巨人举锤
    )
    {
        set_grid = {-1, -1};
    }

    return set_grid;
}

// 放置垫材
void SetCannonFodder()
{
    Grid set_grid = {-1, -1};
    auto cannon_fodder_1 = GetMainObject()->seedArray() + CANNON_FODDER_1;

    auto cannon_fodder_2 = GetMainObject()->seedArray() + CANNON_FODDER_2;

    if (cannon_fodder_1->isUsable())
    {
        set_grid = GetSetGrid();

        if (set_grid.row == -1)
        {
            return;
        }

        // Script 函数中调用了 setInsertOperation(false)
        Card(PUFF_SHROOM, set_grid.row, set_grid.col);
    }
}