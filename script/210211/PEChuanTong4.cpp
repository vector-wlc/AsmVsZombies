/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-07-08 22:47:20
 * @Description: PE 传统四炮
 *               打法 ：非冰即炸             
 *               在此程序中，我们不使用框架自带的修补坚果类，来自己编写自动修补坚果函数
 *               有时，橄榄红眼可能会啃到大喷，这时需要我们使用倭瓜来减压
 *               对于用炮、灰烬和冰，我们需要读取玉米炮的内存来规划
 *               对于气球，直接读取内存使用三叶草即可
 *                  
 * ****************************************************************
 * 这里需要特别注意 AvZ 中的函数是每帧执行的，所以不需要 while 循环和 Sleep
 * AvZ 没有自动垫材类，如需要请使用者自行实现
 * ****************************************************************
 */

#include "avz.h"
using namespace AvZ;

#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 210211

// 枚举卡片的对象序列
enum SeedIndex
{
    _ICE_SHROOM = 0,      // 寒冰菇
    _ICE_SHROOM_IMITATOR, // 模仿寒冰菇
    _COFFEE_BEEN,         // 咖啡豆
    _SQUASH,              // 倭瓜
    _CHERRY,              // 樱桃炸弹
    _CHILI,               // 火爆辣椒
    _PUMPKIN,             // 南瓜头
    _CLOVER,              // 三叶草
    _FUME_SHROOM,         // 大喷菇
    _PUFF_SHROOM          // 小喷菇
};

TickRunner clover_user; // 使用三叶草
TickRunner squash_user; // 使用倭瓜

// 使用三叶草
void UseClover();

// 应对波数主要逻辑函数
void DealWave(int wave);

// 使用倭瓜
void UseSquash();

// 是否还有炮
bool IsHaveCannon();

#endif
#endif

void Script()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 210211

    OpenMultipleEffective('Q', MAIN_UI_OR_FIGHT_UI);
    SetZombies({CG_3, TT_4, BC_12, XC_15, QQ_16, FT_21, TL_22, BY_23, HY_32, TT_18, WW_8});
    SelectCards({
        ICE_SHROOM,
        M_ICE_SHROOM,
        COFFEE_BEAN,
        SQUASH,
        CHERRY_BOMB,
        JALAPENO,
        PUMPKIN,
        BLOVER,
        SCAREDY_SHROOM,
        PUFF_SHROOM,
    });
    MaidCheats::dancing();
    clover_user.pushFunc(UseClover);
    squash_user.pushFunc(UseSquash);

    SetTime(-600, 1);

    ice_filler.start({{3, 6}, {4, 6}});
    pao_operator.resetPaoList({{3, 1}, {4, 1}, {3, 3}, {4, 3}});
    plant_fixer.start(NGT_30);
    plant_fixer.resetFixHp(4000 / 3 * 2);

    for (int wave = 1; wave < 21; ++wave)
    {
        SafeWaitUntil(0, wave);
        // 冰杀小偷
        if (RangeIn(wave, {10, 20}))
        {
            SafeWaitUntil(375 - 298 + 50, wave);
            ice_filler.coffee();
        }
        else
        {
            DealWave(wave);
        }

        // 第 9 19 波进行炮炸收尾
        if (RangeIn(wave, {9, 19, 20}))
        {
            // 如果发现有红眼再进行炮击
            if (IsZombieExist(HY_32))
            {
                pao_operator.recoverPao({{2, 8.5}, {5, 8.5}});
            }
        }
    }

#else
#error 您的版本号与此脚本不对应！
#endif

#else
#error 您的版本号与此脚本不对应！
#endif
}

#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 210211

void UseClover()
{
    auto _clover = GetMainObject()->seedArray() + _CLOVER; // 首先取出三叶草信息内存块
    auto zombie = GetMainObject()->zombieArray();          // 再取出僵尸的内存块

    if (_clover->isUsable())
    {
        // 遍历全场僵尸
        for (int index = 0; index < GetMainObject()->zombieTotal(); ++index)
        {
            // 如果气球僵尸快飞到家了
            if (zombie[index].isExist() &&
                zombie[index].type() == QQ_16 &&
                !zombie[index].isDead() &&
                zombie[index].abscissa() <= 2.5 * 80)
            {
                // 种植三叶草
                CardNotInQueue(_CLOVER + 1, 2, 7);
                break;
            }
        }
    }
}

void DealWave(int wave)
{
    // 此处模拟人为反应
    SafeWaitUntil(50, wave);

    // 辣椒和樱桃总是同时使用，所以只需要检测一个的就行
    auto seed = GetMainObject()->seedArray();

    if (IsHaveCannon())
    {
        pao_operator.pao({{2, 8.5}, {5, 8.5}});
    }
    else if (seed[_CHERRY].isUsable() || 5000 - seed[_CHERRY].cd() < 280)
    {
        SafeWaitUntil(440 - 10, wave);
        // 使用辣椒樱桃
        Card(_CHILI + 1, 5, 7);
        Card(_CHERRY + 1, {{2, 8}, {2, 7}});
    }
    else // 灰烬没有恢复只能冰杀
    {
        ice_filler.coffee();
    }
}

void UseSquash()
{
    auto seed = GetMainObject()->seedArray();
    auto zombie = GetMainObject()->zombieArray();

    struct
    {
        float min_abscissa;
        int min_row;
    } zombie_min_position{800, 0};

    int index = 0, zombie_count_max = 0, zombie_type = 0, zombie_row = 0;
    float zombie_abscissa = 0.0;
    Grid use_squash_grid{0, 0};

    zombie_min_position = {800, 0};
    if (seed[_SQUASH].isUsable())
    {
        zombie_count_max = GetMainObject()->zombieTotal();
        for (index = 0; index < zombie_count_max; ++index)
        {

            if (zombie[index].isExist() &&
                !zombie[index].isDead() &&
                zombie[index].hp() + zombie[index].oneHp() > 500)
            {
                zombie_type = zombie[index].type();
                zombie_abscissa = zombie[index].abscissa();
                zombie_row = zombie[index].row() + 1;
                if (zombie_type == 7)
                    zombie_abscissa += 40;
                if ((zombie_type == 23 || zombie_type == 32 || zombie_type == 7) &&
                    (zombie_row == 1 || zombie_row == 6))
                {
                    if (zombie_abscissa < zombie_min_position.min_abscissa)
                    {
                        zombie_min_position.min_abscissa = zombie_abscissa;
                        zombie_min_position.min_row = zombie_row;
                    }
                }
            }
        }

        // 如果检测到了僵尸
        if (zombie_min_position.min_row != 0)
        {
            use_squash_grid = {zombie_min_position.min_row, int(zombie_min_position.min_abscissa / 80)};

            // 规定种植范围
            if (use_squash_grid.col > 9)
                use_squash_grid.col = 9;
            if (use_squash_grid.col < 6)
                use_squash_grid.col = 6;
            ShovelNotInQueue(use_squash_grid.row, use_squash_grid.col);
            CardNotInQueue(_SQUASH + 1, use_squash_grid.row, use_squash_grid.col);
        }
    }
}

// 是否还有炮
bool IsHaveCannon()
{
    std::vector<Grid> cannon_grids = {
        {3, 1},
        {4, 1},
        {3, 3},
        {4, 3},
    };
    std::vector<int> cannon_indexs;
    GetPlantIndices(cannon_grids, YMJNP_47, cannon_indexs);

    auto cannon = GetMainObject()->plantArray();
    for (const auto &ele : cannon_indexs)
    {
        if (cannon[ele].state() == 37) // 状态为 37 代表能用
        {
            return true;
        }
    }

    return false;
}

#endif
#endif