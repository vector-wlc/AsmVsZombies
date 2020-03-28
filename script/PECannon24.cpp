/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-23 15:17:05
 * @Description: PE 24炮
 */

#include "avz.h"

void PECannon24()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ >= 200316 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

    OpenMultipleEffective();
    SetZombies({PJ_0, TT_4, WW_8, QS_11, BC_12, XC_15, KG_17, HT_14, FT_21, BY_23, HY_32});
    SelectCards({"hbg", "Mhbg", "kfd", "jg", "ngt", "wg", "xpg", "ygg", "dxg", "hp"});
    SetTime(-600, 1);
    pao_operator.resetPaoList({
        {1, 1},
        {1, 3},
        {1, 5},
        {1, 7},
        {2, 1},
        {2, 3},
        {2, 5},
        {2, 7},
        {3, 1},
        {3, 3},
        {3, 5},
        {3, 7},
        {4, 1},
        {4, 3},
        {4, 5},
        {4, 7},
        {5, 1},
        {5, 3},
        {5, 5},
        {5, 7},
        {6, 1},
        {6, 3},
        {6, 5},
        {6, 7},
    });

    ice_filler.start({{4, 9}});

    for (auto wave : {1, 4, 7, 11, 14, 17})
    {
        // 精准之舞
        SetTime(-15, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
        Delay(107);
        pao_operator.pao({{1, 7.7}, {5, 7.7}});
    }

    for (auto wave : {2, 5, 8, 12, 15, 18})
    {
        SetTime(-95, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
        SetTime(-95 + 373 - 298);
        ice_filler.coffee();
    }

    for (auto wave : {3, 6, 9, 13, 16, 19})
    {
        SetTime(-95, wave);
        pao_operator.pao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
        Delay(108);
        pao_operator.pao({{1, 8.8}, {5, 8.8}});
    }

    for (auto wave : {9, 19})
    {
        SetTime(601 - 15, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
    }

    SetTime(-55, 10);
    pao_operator.pao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
    Delay(108);
    pao_operator.pao({{1, 8.8}, {5, 8.8}});

    SetTime(-150, 20);
    pao_operator.pao({{4, 6}, {4, 8}});
    Delay(90);
    pao_operator.pao({{1, 9}, {2, 9}, {5, 9}, {6, 9}});
    Delay(108);
    pao_operator.pao({{1, 9}, {2, 9}, {5, 9}, {6, 9}});

#endif
#endif
}