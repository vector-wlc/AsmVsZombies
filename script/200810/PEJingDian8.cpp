/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-07-03 17:07:14
 * @Description: 
 */

#include "avz.h"

void Script()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 200810 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

    OpenMultipleEffective();
    SetZombies({PJ_0, TT_4, WW_8, QS_11, BC_12, XC_15, KG_17, HT_14, FT_21, BY_23, HY_32});
    SelectCards({"hbg", "Mhbg", "kfd", "hmg", "hy", "wg", "ytzd", "syc", "ngt", "xpg"});
    SetTime(-600, 1);
    pao_operator.resetPaoList({{1, 5}, {2, 5}, {3, 1}, {3, 3}, {4, 1}, {4, 3}, {5, 5}, {6, 5}});
    StartMaidCheats();

    KeyConnect('Q', [=]() {
        pao_operator.pao(MouseRow(), MouseCol());
    });

    // 1 - 19 波
    for (int wave = 1; wave < 21; ++wave)
    {
        SetTime(900 - 373 - 200, wave);
        pao_operator.pao({{2, 8.5}, {5, 8.5}});
    }

    // 9 19 收尾
    for (auto wave : {9, 19, 20}) // 这是本框架独特支持的语法，使用此语法可使代码更简洁，当然你也可以在上一个循环体中使用 RangeIn
    {
        SetTime(1000 - 373 - 200, wave);
        pao_operator.recoverPao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
        Delay(100);
    }

#else
    AvZ::showErrorNotInQueue("您的版本号与此脚本不对应！");
#endif

#else
    AvZ::showErrorNotInQueue("您的版本号与此脚本不对应！");
#endif
}
