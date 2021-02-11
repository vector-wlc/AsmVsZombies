/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-07-02 16:09:55
 * @Description: 24炮非定态解
 *              无冰分离不刷新 :丨PPSSDD丨PPDD丨PPSSDD丨IPP-BB-PPDD丨
 *              无冰分离刷新时 :丨PPSSDD丨PPDD丨PPSSDD丨IPP-BB丨ICE2+PPDD丨
 */

#include "avz.h"

using namespace AvZ;

#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 210211 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

// 不刷新情况
void NotRefreshed()
{
    // 设定波长
    SetWavelength({
        {1, 601},
        {2, 601},
        {3, 601},
        {4, 1800},
        {5, 601},
        {6, 601},
        {7, 601},
        {8, 1800},
        {10, 601},
        {11, 601},
        {12, 601},
        {13, 1800},
        {14, 601},
        {15, 601},
        {16, 601},
        {17, 1800},
        {18, 601},
    });

    // PPSSDD
    for (auto wave : {1, 3, 5, 7, 12, 14, 16, 18})
    {
        SetTime(-95, wave);
        pao_operator.pao({{2, 9}, {2, 9}, {5, 9}, {5, 9}});
        SetTime(-95 + 108);
        pao_operator.pao({{1, 8.8}, {5, 8.8}});
    }

    // PPDD
    for (auto wave : {2, 6, 11, 15})
    {
        SetTime(-14, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
        SetTime(-14 + 108);
        pao_operator.pao({{1, 7.7}, {5, 7.7}});
    }

    // IPP-BB-PPDD
    for (auto wave : {4, 8, 13, 17})
    {
        SetTime(-298 + 40, wave);
        ice_filler.coffee();
        SetTime(-95);
        pao_operator.pao({{1, 9}, {5, 9}});
        SetTime(786);
        pao_operator.pao({{5, 8.184}, {2, 8.185}});
        SetTime(1800 - 200 - 373);
        pao_operator.pao({{2, 9}, {5, 9}});
        SetTime(1800 - 200 - 373 + 220);
        pao_operator.pao({{1, 7.2}, {5, 7.2}});
    }

    // wave 10
    SetTime(-55, 10);
    pao_operator.pao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
    SetTime(-55 + 108);
    pao_operator.pao({{1, 8.8}, {5, 8.8}});

    // wave 9
    SetTime(-95, 9);
    pao_operator.pao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
    SetTime(-95 + 108);
    pao_operator.pao({{1, 8.8}, {5, 8.8}});
    SetTime(700);
    pao_operator.pao({{2, 9}, {5, 9}});

    // wave 19
    SetTime(-14, 19);
    pao_operator.pao({{2, 9}, {5, 9}});
    SetTime(-14 + 108);
    pao_operator.pao({{1, 7.7}, {5, 7.7}});
    SetTime(470 - 298);
    ice_filler.coffee();
    SetTime(1900 - 200 - 373);
    pao_operator.pao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
    SetTime(1900 - 200 - 373 + 220);
    pao_operator.pao({{1, 8.8}, {5, 8.8}});

    // wave 20
    SetTime(-140, 20);
    pao_operator.pao({{4, 6}, {4, 8}});
    SetTime(-55);
    pao_operator.pao({{1, 9}, {2, 9}, {5, 9}, {5, 9}});
    SetTime(-55 + 108);
    pao_operator.pao({{1, 9}, {2, 9}, {5, 9}, {5, 9}});
}

// 刷新情况
void Refreshed()
{
    // 设定波长
    SetWavelength({
        {1, 601},
        {2, 601},
        {3, 601},
        {4, 1360},
        {5, 601},
        {6, 601},
        {7, 601},
        {8, 601},
        {10, 601},
        {11, 601},
        {12, 601},
        {13, 1360},
        {14, 601},
        {15, 601},
        {16, 601},
        {17, 601},
        {18, 1800},
    });

    SetWaveZombies(4, {PJ_0, TT_4, WW_8, QS_11, BC_12, XC_15, KG_17, HT_14, FT_21, BY_23});
    SetWaveZombies(13, {PJ_0, TT_4, WW_8, QS_11, BC_12, XC_15, KG_17, HT_14, FT_21, BY_23});

    // PPSSDD
    for (auto wave : {1, 3, 6, 8, 12, 15, 17})
    {
        SetTime(-95, wave);
        pao_operator.pao({{2, 9}, {2, 9}, {5, 9}, {5, 9}});
        SetTime(-95 + 108);
        pao_operator.pao({{1, 8.8}, {5, 8.8}});
    }

    // PPDD
    for (auto wave : {2, 7, 11, 16})
    {
        SetTime(-14, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
        SetTime(-14 + 108);
        pao_operator.pao({{1, 7.7}, {5, 7.7}});
    }

    // IPP-BB
    for (auto wave : {4, 13, 18})
    {
        SetTime(-298 + 40, wave);
        ice_filler.coffee();
        SetTime(-95);
        pao_operator.pao({{1, 9}, {5, 9}});
        SetTime(786);
        pao_operator.pao({{5, 8.184}, {2, 8.185}});
    }

    // wave 18
    SetTime(1800 - 200 - 373, 18);
    pao_operator.pao({{2, 9}, {5, 9}});
    SetTime(1800 - 200 - 373 + 220);
    pao_operator.pao({{1, 7.2}, {5, 7.2}});

    // ICE2+PPDD
    for (auto wave : {5, 14})
    {
        SetTime(-95 + 373 - 298, wave);
        ice_filler.coffee();
        SetTime(-95);
        pao_operator.pao({{2, 9}, {5, 9}});
        SetTime(-95 + 300);
        pao_operator.pao({{2, 9}, {5, 9}});
    }

    // wave 10
    SetTime(-55, 10);
    pao_operator.pao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
    SetTime(-55 + 108);
    pao_operator.pao({{1, 8.8}, {5, 8.8}});

    // wave 9
    SetTime(-298 + 40, 9);
    ice_filler.coffee();
    SetTime(-95);
    pao_operator.pao({{1, 9}, {5, 9}});
    SetTime(786);
    pao_operator.pao({{5, 8.184}, {2, 8.185}});
    SetTime(1800 - 200 - 373);
    pao_operator.pao({{2, 9}, {5, 9}});
    SetTime(1800 - 200 - 373 + 220);
    pao_operator.pao({{1, 8.7}, {5, 8.7}});
    SetTime(1800 - 200 - 373 + 220 + 220);
    pao_operator.pao({{1, 8.7}, {5, 8.7}});
    SetTime(1800 - 200 - 373 + 220 + 220 + 300);
    pao_operator.pao({{2, 9}, {5, 9}});

    // wave 19
    SetTime(-95, 19);
    pao_operator.pao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
    SetTime(-95 + 108);
    pao_operator.pao({{1, 8.8}, {5, 8.8}});
    SetTime(700);
    pao_operator.pao({{2, 9}, {5, 9}});

    // wave 20
    SetTime(-140, 20);
    pao_operator.pao({{4, 6}, {4, 8}});
    SetTime(-55);
    pao_operator.pao({{1, 9}, {2, 9}, {5, 9}, {5, 9}});
    SetTime(-55 + 108);
    pao_operator.pao({{1, 9}, {2, 9}, {5, 9}, {5, 9}});
}

#endif

#endif

void Script()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 210211 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

    SetZombies({PJ_0, TT_4, WW_8, QS_11, BC_12, XC_15, KG_17, HT_14, FT_21, BY_23, HY_32});
    SelectCards({ICE_SHROOM, M_ICE_SHROOM, COFFEE_BEAN, WALL_NUT, PUMPKIN, SQUASH, PUFF_SHROOM, SUN_SHROOM, SCAREDY_SHROOM, FLOWER_POT});
    SetTime(-600, 1);
    StartMaidCheats();
    ice_filler.start({{4, 9}});
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

    Refreshed();

#else
#error 您的版本号与此脚本不对应！
#endif

#else
#error 您的版本号与此脚本不对应！
#endif
}
