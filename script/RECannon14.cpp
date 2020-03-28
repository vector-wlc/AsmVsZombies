/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-03-16 17:02:50
 * @Description: RE 椭盘十四炮 代码抄袭了 https://pvz.lmintlcx.com/scripts/demos/ 
 */

#include "avz.h"

void RECannon14()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ >= 200316 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

    OpenMultipleEffective();
    SetZombies({PJ_0, CG_3, GL_7, BC_12, XC_15, QQ_16, TT_18, FT_21, TL_22, BY_23, HY_32});
    // wave 9 19 多刷红眼增加其出现在上路的概率以便拖CD，其刷怪比例为别的类型的三倍
    SetWaveZombies(9, {CG_3, BC_12, XC_15, QQ_16, TT_18, FT_21, HY_32, HY_32, HY_32});
    SetWaveZombies(19, {CG_3, BC_12, XC_15, QQ_16, TT_18, FT_21, HY_32, HY_32, HY_32});

    SelectCards({"hp", "hbg", "Mhbg", "hmg", "kfd", "ytzd", "hblj", "wg", "hbss", "jg"});

    SetTime(-600, 1);
    pao_operator.resetPaoList({
        {4, 2},
        {4, 4},
        {1, 4},
        {5, 4},
        {5, 6},
        {3, 1},
        {4, 7},
        {1, 2},
        {2, 4},
        {3, 3},
        {3, 5},
        {2, 6},
        {2, 1},
        {3, 7},
    });
    Card({{"hp", 1, 7}, {"hbg", 1, 7}});
    ice_filler.start({{4, 6}, {2, 3}, {1, 1}, {1, 6}});

    SetTime(385 - 387, 1);
    pao_operator.roofPao({{2, 8.8}, {4, 8.8}});
    SetTime(513 - 387);
    pao_operator.roofPao({{2, 8.8}, {4, 8.8}});
    SetTime(601 + 42 - 387);
    pao_operator.roofPao({{1, 8.8}, {4, 8.8}});
    SetTime(601 + 42 - 298);
    Card("kfd", 1, 7);

    SetTime(50, 2);
    Shovel(1, 7);              // 铲
    SetTime(1300 - 200 - 387); // 727
    pao_operator.roofPao(4, 8.1);
    SetTime(1780 - 200 - 387); // 1207
    pao_operator.roofPao({{2, 9}, {4, 9}});
    SetTime(1780 + 10 - 298); // 1492
    ice_filler.coffee();
    Ice3(298);

    SetTime(-15 - 14, 10);
    pao_operator.roofPao({{2, 9}, {4, 9}, {2, 9}, {4, 9}});
    SetTime(-15 - 14 + 110);      // 95
    pao_operator.roofPao(4, 7.7); // 空炸小鬼兼小偷
    SetTime(-15 - 14 + 190);      // 175
    pao_operator.roofPao(2, 5);   // 2-5? 尾炸小鬼兼小偷
    SetTime(601 + 10 - 298);      // 313
    ice_filler.coffee();
    Ice3(298);

    SetTime(10 + 400 - 100, 11);
    Card("hblj", 1, 7);
    Card("hp", 4, 9);
    Card("ytzd", 4, 9);
    SetTime(10 + 400 + 10);
    Shovel(1, 7);                  // 铲
    Shovel(4, 9);                  // 铲
    SetTime(1250 - 200 - 387);     // 1300->1250
    pao_operator.roofPao(3, 8.15); // 落点改为 3 路炸掉 2 路冰车
    SetTime(1780 - 200 - 387);
    pao_operator.roofPao({{2, 9}, {4, 9}});
    SetTime(1780 + 10 - 298);
    ice_filler.coffee();
    Ice3(298);

    for (auto wave : {3, 12})
    {
        SetTime(-200, wave);
        SetTime(10 + 400 - 387);
        pao_operator.roofPao({{2, 9}, {4, 9}});
        SetTime(10 + 400 - 387 + 220);
        pao_operator.roofPao(4, 8.5); // 空炸;
        SetTime(10 + 400 - 387 + 300);
        pao_operator.roofPao(2, 4.7); // 尾炸小鬼跳跳
        SetTime(1300 - 200 - 387);
        pao_operator.roofPao(4, 8.1);
        SetTime(1780 - 200 - 387);
        pao_operator.roofPao({{2, 9}, {4, 9}});
        SetTime(1780 + 10 - 298);
        ice_filler.coffee();
        Ice3(298);
    }

    for (auto wave : {9, 19})
    {
        SetTime(-200, wave);
        SetTime(10 + 400 - 387);
        pao_operator.roofPao({{2, 9}, {4, 9}});
        SetTime(10 + 400 - 387 + 220);
        pao_operator.roofPao({{1, 8.5}, {4, 8.5}});
        SetTime(1300 - 200 - 387);     // 727
        pao_operator.roofPao(3, 8.15); // 落点改为 3 路减少小丑炸核机率
        // 收尾
        SetTime(1680 - 200 - 298); // 1182
        Card("hp", 3, 9);
        Card("hmg", 3, 9);
        Card("kfd", 3, 9);
        SetTime(1680 - 200 + 230 - 387);
        pao_operator.roofPao({{2, 8.5}, {4, 8.5}}); // 拦截
        SetTime(1680 - 200 + 230 + 230 - 387);
        pao_operator.roofPao({{2, 8.5}, {4, 8.5}}); // 拦截
        SetTime(1680 - 200 + 230 + 230 + 230 - 387);
        pao_operator.roofPao({{3, 9}, {5, 9}}); // 留下 1 路
        Delay(50);
        Card("hbss", 1, 6);
        // 清场
        if (wave == 9)
        {
            pao_operator.skipPao(7 - 4 - 1 + 5); // 调整炮序
            SetTime(2700);
            Card("hp", 1, 8);           // 垫一下
            SetTime(4500 - 200 - 387);  // SetTime(4500 - 5)  // 出红字时
            Delay(400);                 // 等那一门炮
            pao_operator.roofPao(1, 8); // 清场
            SetTime(4500 - 200 + 100);
            Shovel(1, 6);                  // 铲掉冰豆
            SetTime(4500 - 5 + 750 - 599); // 第 10 波刷新前 599
            Card("hp", 1, 7);
        }
        else
        {
            SetTime(4500 - 200 - 387);
            pao_operator.roofPao(1, 8); // 清场
            Delay(200);
            Shovel(1, 6); // 铲掉冰豆
        }
    }

    SetTime(50 - 298, 20);
    ice_filler.coffee();
    Ice3(298); // 冰消空降
    SetTime(-150);
    pao_operator.roofPao(4, 9); // 炸 3/4 路冰车
    SetTime(75);
    pao_operator.roofPao({{1, 6}, {2, 3}, {4, 6}}); // 炸小偷
    SetTime(1250 - 200 - 387);
    pao_operator.roofPao({{1, 9}, {2, 9}, {4, 9}, {5, 9}});
    SetTime(1250 - 200 - 387 + 220);
    pao_operator.roofPao({{1, 9}, {2, 9}, {4, 9}, {5, 9}});

    int WL;
    for (auto wave : {4, 5, 6, 7, 8, 13, 14, 15, 16, 17, 18})
    {
        WL = RangeIn(wave, {8, 18}) ? 1950 : 1780;

        SetTime(-200, wave);
        SetTime(10 + 400 - 387);
        pao_operator.roofPao({{2, 9}, {4, 9}});
        SetTime(10 + 400 - 387 + 220);
        pao_operator.roofPao({{1, 8.5}, {4, 8.5}});
        SetTime(1300 - 200 - 387);
        pao_operator.roofPao(4, 8.1);
        SetTime(WL - 200 - 387); // WL-573
        pao_operator.roofPao({{2, 9}, {4, 9}});
        SetTime(WL + 10 - 298); // WL-288
        ice_filler.coffee();
        Ice3(298);
        if (RangeIn(wave, {8, 18}))
        {
            SetTime(WL - 200 - 387 + 81); // WL-492
            Card("hp", 2, 8);
            SetTime(WL - 200); // WL-200
            Shovel(2, 8);
        }
    }
#endif
#endif
}
