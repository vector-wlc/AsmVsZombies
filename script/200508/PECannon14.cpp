/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-03-04 13:47:09
 * @Description: PE 最后之作十四炮
 */

#include "avz.h"

void SetDianCai()
{
    //种植垫材
    Card({{"xpg", 1, 8}, {"ygg", 2, 8}, {"dxg", 5, 8}, {"yzbhs", 6, 8}});
    Delay(50);
    //铲除垫材
    Shovel({{1, 8}, {2, 8}, {5, 8}, {6, 8}});
}

void PECannon14()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 200508 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

    OpenMultipleEffective();
    SetZombies({PJ_0, TT_4, WW_8, QS_11, BC_12, XC_15, KG_17, HT_14, FT_21, BY_23, HY_32});
    SelectCards({"hbg", "Mhbg", "kfd", "hy", "gjg", "ytzd", "xpg", "ygg", "dxg", "yzbhs"});
    KeyConnect('Q', [=]() {
        pao_operator.pao(MouseRow(), MouseCol());
    });

    SetTime(-600, 1);
    pao_operator.resetPaoList({{1, 1}, {1, 3}, {2, 1}, {2, 3}, {5, 1}, {5, 3}, {6, 1}, {6, 3}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}});
    Card("hy", 3, 3);
    ice_filler.start({{1, 5}, {6, 5}, {3, 3}});
    plant_fixer.start(GJG_23, {{3, 8}, {4, 8}});
    StartMaidCheats();

    for (auto wave : {2, 5, 8, 11, 14, 17})
    {
        SetTime(-15, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
        SetTime(-15 + 110);
        pao_operator.pao({{1, 7.5}, {5, 7.5}});
        SetTime(373 + 200 - 298 + 50);
        ice_filler.coffee();
    }

    for (auto wave : {3, 6, 9, 12, 15, 18})
    {
        SetTime(1290 - 373 - 200, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
        SetTime(1290 - 373 - 200 + 350);
        pao_operator.pao({{1, 2.4}, {5, 2.4}});
        SetTime(1290 - 298 + 30);
        ice_filler.coffee();
    }

    for (auto wave : {4, 7, 13, 16, 19})
    {
        SetTime(150, wave);
        pao_operator.pao({{1, 7.2}, {5, 7.2}});
        SetTime(1670 - 373 - 200);
        pao_operator.pao({{2, 8.5}, {5, 8.5}});
        SetDianCai();
        SetTime(1670 - 373 - 200 + 220);
        pao_operator.pao({{1, 7.3}, {5, 7.3}});
    }

    // 额外处理

    // wave 1
    SetTime(-95, 1);
    pao_operator.pao({{2, 9}, {5, 9}});
    SetTime(-95 + 110);
    pao_operator.pao({{1, 8}, {5, 8}});

    for (auto wave : {9, 19})
    {
        int time = (wave == 9 ? 1290 - 373 - 200 + 350 : 1670 + 220 - 200);
        SetTime(time, wave);

        if (wave == 19)
        {
            ice_filler.coffee();
        }
        else
        {
            Card({{"hy", 4, 3}, {"yzbhs", 4, 3}});
        }

        SetTime(time + 300 + 715);
        pao_operator.pao({{1, 8.5}, {5, 8.5}});
        SetDianCai();
        SetTime(time + 300 + 715 + 220);
        pao_operator.pao({{1, 8.5}, {5, 8.5}});
        pao_operator.recoverPao({{2, 9}, {5, 9}});
    }

    // wave 10
    SetTime(-55, 10);
    pao_operator.pao({{2, 9}, {5, 9}});
    SetTime(-55 + 110);
    pao_operator.pao({{1, 8}, {5, 8}});
    SetTime(-55 + 373 - 100);
    Card("ytzd", 2, 9);

    // wave 20
    SetTime(-150, 20);
    pao_operator.pao(4, 7);
    SetTime(-95);
    pao_operator.pao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
    SetTime(-95 + 110);
    pao_operator.pao({{1, 8.5}, {5, 8.5}});
    SetTime(600);
    pao_operator.recoverPao({{2, 9}, {5, 9}});
    plant_fixer.stop();
    SetTime(1500);
    ice_filler.stop();
    //铲除多余植物
    for (auto row : {3, 4})
    {
        Shovel({{row, 3}, {row, 3}, {row, 8}, {row, 8}});
    }
#else
    AvZ::popErrorWindowNotInQueue("您的版本号与此脚本不对应！");
#endif

#else
    AvZ::popErrorWindowNotInQueue("您的版本号与此脚本不对应！");
#endif
}
