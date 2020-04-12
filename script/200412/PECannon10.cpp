/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-17 14:15:41
 * @Description: PE 经典十炮 非定态
 *              整体节奏 P4，若有不刷新情况则使用灰烬强制刷新
 */
#include "avz.h"

// 是否有炮可用
bool IsHaveCannon()
{
    auto plant_memory = AvZ::mainObject()->plantArray() + AvZ::getPlantIndex(3, 5);
    return plant_memory->state() == 37;

    // 你也可以使用下面的语句，虽然看起来比较麻烦，但是效率更高
    // auto plant_memory = AvZ::mainObject()->plantArray();
    // for (int i = 0; i < AvZ::mainObject()->plantCountMax(); ++i, ++plant_memory)
    // {
    //     if (!plant_memory->isCrushed() &&
    //         !plant_memory->isDisappeared() &&
    //         plant_memory->state() == 37) // 37 为炮可用状态
    //     {
    //         return true;
    //     }
    // }
    // return false;
}

void PECannon10()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 200412 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

    OpenMultipleEffective();
    SetZombies({PJ_0, TT_4, WW_8, QS_11, BC_12, XC_15, KG_17, HT_14, FT_21, BY_23, HY_32});
    SelectCards({"hbg", "ytzd", "kfd", "hmg", "hy", "wg", "Mytzd", "syc", "ygg", "xpg"});
    //刷新： 正常放炮
    //不刷新：强迫激活，调整发炮时间点
    KeyConnect('Q', [=]() {
        pao_operator.rawPao({{3, 5, 2, 9}, {4, 5, 5, 9}});
    });

    KeyConnect('W', [=]() {
        Card("xpg", MouseRow(), MouseCol());
        Delay(1);
        Shovel(MouseRow(), MouseCol());
    });

    KeyConnect('E', [=]() {
        Card("ygg", MouseRow(), MouseCol());
        Delay(1);
        Shovel(MouseRow(), MouseCol());
    });

    SetTime(-600, 1);
    pao_operator.resetPaoList({{1, 5}, {2, 5}, {3, 1}, {3, 3}, {4, 1}, {4, 3}, {5, 5}, {6, 5}});

    int fire_time = -95;
    int ytzd_row = 1;
    int block_time;

    // 20 波特殊处理
    SetTime(-135, 20);
    pao_operator.pao(4, 7);

    for (int wave = 1; wave < 21; ++wave)
    {
        if (RangeIn(wave, {1, 10, 20}))
        {
            fire_time = -95;
        }
        SetTime(fire_time, wave);
        if (fire_time == -95)
        {
            pao_operator.pao({{2, 9}, {5, 9}});
            block_time = 601 - 200 + 50;
        }
        else
        {
            pao_operator.pao({{2, 7.7}, {5, 7.7}});
            block_time = fire_time + 373 + 50;
        }

        WaitUntil(block_time, wave); //非定态如果使用递归或者循环基本必须使用阻塞函数 WaitUntil ！
        if (RangeIn(wave, {9, 19, 20}))
        {
            pao_operator.recoverPao({{2, 8.5}, {5, 8.5}, {2, 8.5}, {5, 8.5}});
            continue;
        }

        if (AvZ::mainObject()->refreshCountdown() > 200) // 如果没有刷新
        {
            WaitUntil(3480 / 4 + 100, wave);
            if (IsHaveCannon())
            {
                pao_operator.rawPao({{3, 5, 2, 9}, {4, 5, 5, 9}});
                fire_time = -95;
                continue;
            }
            Seed *seed_memory = AvZ::mainObject()->seedArray() + 4 - 1; // 核武是否刷新？
            if (seed_memory->isUsable())
            {
                Card("hy", {{3, 8}, {3, 9}, {4, 8}, {4, 9}});
                Card("hmg", {{3, 8}, {3, 9}, {4, 8}, {4, 9}});
                Card("kfd", {{3, 8}, {3, 9}, {4, 8}, {4, 9}});
                fire_time = -95;
                continue;
            }
            seed_memory = AvZ::mainObject()->seedArray() + 2 - 1; // 樱桃炸弹是否刷新？
            if (seed_memory->isUsable())
            {
                Card("hy", {{4, 8}, {4, 9}});
                Card("Mytzd", {{4, 8}, {4, 9}});
                Delay(320);
                Card("ytzd", {{1, 9}, {1, 8}});
            }
        }
        else // 刷新了
        {
            fire_time = 3480 / 4 - 373 - 200;
        }
    }

#else
    AvZ::popErrorWindowNotInQueue("您的版本号与此脚本不对应！");
#endif

#else
    AvZ::popErrorWindowNotInQueue("您的版本号与此脚本不对应！");
#endif
}