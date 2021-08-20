/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-07-04 16:05:27
 * @Description: PE 经典十二 新手入门首选阵型
 */

#include "avz.h"

using namespace AvZ;

void Script()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 210820

    SetZombies({CG_3, TT_4, BC_12, XC_15, QQ_16, FT_21, TL_22, BY_23, HY_32, TT_18});

    SelectCards({
        ICE_SHROOM,   // 寒冰菇
        M_ICE_SHROOM, // 模仿寒冰菇
        COFFEE_BEAN,  // 咖啡豆
        DOOM_SHROOM,  // 毁灭菇
        LILY_PAD,     // 荷叶
        SQUASH,       // 倭瓜
        CHERRY_BOMB,  // 樱桃炸弹
        BLOVER,       // 三叶草
        PUMPKIN,      // 南瓜头
        PUFF_SHROOM,  // 小喷菇
    });

    SetTime(-600, 1);
    pao_operator.resetPaoList({
        {1, 5},
        {2, 5},
        {3, 1},
        {3, 3},
        {3, 5},
        {3, 7},
        {4, 1},
        {4, 3},
        {4, 5},
        {4, 7},
        {5, 5},
        {6, 5},
    });

    // P6
    // 主体节奏
    for (auto wave : {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19}) {
        SetTime(-95, wave);
        pao_operator.pao({{2, 9}, {5, 9}});
    }

    // wave 9 19 20的附加操作
    // 收尾发四门炮
    for (auto wave : {9, 19, 20}) {
        SetTime(300, wave);
        pao_operator.recoverPao({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
    }

    // wave 10 的附加操作
    // 樱桃消延迟
    // 解决僵尸出生点靠右的问题
    SetTime(-55 + 373 - 100, 10);
    Card(CHERRY_BOMB, 2, 9);
    SetTime(-55);
    pao_operator.pao({{2, 9}, {5, 9}});

    // wave 20 的附加操作
    // 咆哮珊瑚
    // 解决僵尸出生点靠右的问题
    SetTime(-150, 20);
    pao_operator.pao(4, 7);
    SetTime(-55);
    pao_operator.pao({{2, 9}, {5, 9}});
#else
#error 您的版本号与此脚本不对应！
#endif

#else
#error 您的版本号与此脚本不对应！
#endif
}