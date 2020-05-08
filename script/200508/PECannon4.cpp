/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-17 14:38:10
 * @Description: 经典四炮 C7i 
 */

#include "avz.h"

void PECannon4()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 200508 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

    OpenMultipleEffective();
    SetZombies({PJ_0, TT_4, WW_8, QS_11, BC_12, XC_15, KG_17, HT_14, FT_21, BY_23, HY_32});
    SelectCards({"hbg", "Mhbg", "kfd", "hmg", "hy", "wg", "ytzd", "syc", "ngt", "xpg"});
    Prejudge(-600, 1);
    pao_operator.resetPaoList({{3, 1}, {4, 1}, {3, 3}, {4, 3}});
    ice_filler.start({{3, 5}, {1, 4}, {6, 4}, {1, 5}, {6, 5}});

    //自动补南瓜
    plant_fixer.start(NGT_30, {{3, 5}, {3, 6}, {4, 5}, {4, 6}, {1, 4}, {6, 4}});
    plant_fixer.resetFixHp(4000 / 3 * 2);

    for (int wave = 1; wave < 21; ++wave)
    {
        if (RangeIn(wave, {1, 5, 9, 14, 18}))
        {
            Prejudge(-95, wave);
            pao_operator.pao({{2, 9}, {5, 9}});

            if (wave == 9)
            {
                pao_operator.recoverPao({{2, 8.5}, {5, 8.5}});
            }
            else
            {
                //僵尸在 601 刷新，白天冰的生效时间为 298，所以当参数填为 601 - 298 时
                //就是僵尸刚一刷新冰就会生效，但是所有的僵尸并不是在这一时刻全部刷新
                //所以可以延迟 50 来达到冰冻全部僵尸的效果，这里 50 是随便取的，
                //你可以取得更多或更少，根据实际操作需求来定
                Until(601 - 298 + 50);
                ice_filler.coffee();
            }
        }
        else if (RangeIn(wave, {2, 6, 11, 15, 19}))
        {
            Prejudge(1800 - 373 - 200, wave);
            pao_operator.pao({{2, 8.5}, {5, 8.5}});
            if (wave == 19)
            {
                pao_operator.recoverPao({{2, 8.5}, {5, 8.5}});
                Delay(1000);
                //停止自动存冰线程
                ice_filler.pause();
            }
            else
            {
                // Ice3
                Until(1800 - 298 - 200 + 210);
                ice_filler.coffee();
                Ice3(298);
            }
        }
        else if (RangeIn(wave, {3, 7, 12, 16}))
        {
            Prejudge(1800 - 373 - 200, wave);
            pao_operator.pao({{2, 8.5}, {5, 8.5}});
            Until(1800 - 298 - 200 + 210);
            ice_filler.coffee();
            Ice3(298);
        }
        else if (RangeIn(wave, {4, 8, 13, 17}))
        {
            //使用核
            Prejudge(1200 - 200 - 298, wave);
            if (RangeIn(wave, {4, 17}))
                Card({{"hy", 3, 8}, {"hmg", 3, 8}, {"kfd", 3, 8}});
            else if (wave == 8)
                Card({{"hy", 3, 9}, {"hmg", 3, 9}, {"kfd", 3, 9}, {"ngt", 3, 9}});
            else
                Card({{"hy", 4, 9}, {"hmg", 4, 9}, {"kfd", 4, 9}, {"ngt", 4, 9}});
        }
        else if (wave == 10)
        {
            Prejudge(-55, wave);
            pao_operator.pao({{2, 9}, {5, 9}});
            //种植樱桃消除延迟
            Until(-55 + 373 - 100);
            Card("ytzd", 2, 9);
            Until(601 - 298 + 50);
            ice_filler.coffee();
        }
        else //if(wave == 20)
        {
            Prejudge(150, wave);
            ice_filler.coffee(); //冰杀小偷
            Delay(298 + 500);
            pao_operator.recoverPao({{2, 9}, {5, 9}, {2, 8.5}, {5, 8.5}});
        }
    }

#else
    AvZ::popErrorWindowNotInQueue("您的版本号与此脚本不对应！");
#endif

#else
    AvZ::popErrorWindowNotInQueue("您的版本号与此脚本不对应！");
#endif
}