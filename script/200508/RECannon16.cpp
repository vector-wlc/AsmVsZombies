/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-03-15 11:28:35
 * @Description: RE 艹栋十六炮 
 */

#include "avz.h"

//设定两个炮列表，以第四列作为分界线
//设定成全局变量以方便程序编写
AvZ::PaoOperator pao_col_in_5_and_7;
AvZ::PaoOperator pao_col_in_1_and_3;

#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 200508
//分别使用一门 1 3 列炮和一门 5 7 列炮
//inline 是内联，对于短小的函数效率更高
inline void MyRoofPao(float col_1_3, float col_5_7)
{
    pao_col_in_1_and_3.roofPao(2, col_1_3);
    pao_col_in_5_and_7.roofPao(4, col_5_7);
}
#endif
#endif

void RECannon16()
{
#ifdef __AVZ_VERSION__
#if __AVZ_VERSION__ == 200508 // 这里使用预编译指令是来识别版本号的，使用者不需要声明这些！！！

    OpenMultipleEffective();
    SetZombies({CG_3, TT_4, BC_12, XC_15, QQ_16, BJ_20, FT_21, TL_22, BY_23, HY_32, TT_18});
    SelectCards({"kfd", "hbg", "Mhbg", "hmg", "ytzd", "hblj", "hp", "ymts", "ymjnp", "gjg"});
    // 按下 Q 发射一枚炮
    KeyConnect('Q', [&]() {
        pao_col_in_5_and_7.roofPao(MouseRow(), MouseCol());
    });

    // 按下 W 种高坚果
    KeyConnect('W', [&]() {
        auto mouse_row = MouseRow();
        auto mouse_col = MouseCol();
        Card({{"hp", mouse_row, mouse_col}, {"gjg", mouse_row, mouse_col}});
    });

    // 调整炮列表
    pao_col_in_1_and_3.resetPaoList({{1, 1}, {1, 3}, {2, 1}, {3, 1}, {3, 3}, {4, 1}, {5, 1}, {5, 3}});
    pao_col_in_5_and_7.resetPaoList({{1, 5}, {1, 7}, {2, 5}, {3, 5}, {3, 7}, {4, 5}, {5, 5}, {5, 7}});

    Card("hp", 2, 9);

    // 启用存冰线程
    ice_filler.start({{2, 3}, {4, 3}, {2, 8}, {4, 8}});

    // ############# wave 1 ##################
    SetTime(223 - 387, 1);
    MyRoofPao(9, 9);
    SetTime(330 - 387);
    MyRoofPao(8.8, 7.7);
    SetTime(223 - 100);
    Card("ytzd", 2, 9);
    SetTime(361 - 100);
    Card("hblj", 2, 9);

    // ############# wave 2 ##################
    SetTime(250 - 387, 2);
    MyRoofPao(9, 9);
    SetTime(250 - 298);
    Card({{"hmg", 2, 9}, {"kfd", 2, 9}});
    SetTime(361 - 387);
    MyRoofPao(8.8, 8.8);
    SetTime(0);
    Card("hp", 5, 9);
    Delay(2);
    Shovel(5, 9);

    // ############# wave 3 ##################
    SetTime(385 - 387, 3);
    MyRoofPao(9, 9);
    SetTime(513 - 387);
    MyRoofPao(8.75, 8.75);
    SetTime(601 + 42 - 387);
    MyRoofPao(8.75, 8.75);
    SetTime(601 + 19 - 298);
    ice_filler.coffee();

    // ############# wave 4 ##################
    SetTime(1145 - 387, 4);
    MyRoofPao(8.8, 8.8);
    Delay(387 + 210 - 298);
    ice_filler.coffee();
    Ice3(298);

    // ############# wave 5 ##################
    SetTime(570 - 387, 5);
    MyRoofPao(9, 8.8);
    SetTime(570 + 220 - 387);
    MyRoofPao(8.4, 8.4);
    SetTime(1200 - 387);
    MyRoofPao(8.8, 8.8);
    Delay(387 + 210 - 298);
    ice_filler.coffee();
    Ice3(298);

    // ############# wave 6 ##################
    SetTime(410 - 387, 6);
    MyRoofPao(9, 8.4);
    SetTime(410 + 220 - 387);
    MyRoofPao(8.45, 8.45);
    SetTime(1200 - 387);
    MyRoofPao(8.9, 8.8);
    Delay(387 + 210 - 298);
    ice_filler.coffee();
    Ice3(298);

    // ############# wave 7 ##################
    SetTime(410 - 387, 7);
    MyRoofPao(8.8, 8.8);
    pao_col_in_5_and_7.fixLatestPao();
    SetTime(410 + 220 - 387);
    pao_col_in_1_and_3.roofPao(2, 8.45);
    SetTime(410 + 220 - 100);
    Card("ytzd", 4, 8);
    SetTime(1190 - 298);
    Card({{"hp", 3, 9}, {"hmg", 3, 9}, {"kfd", 3, 9}});
    SetTime(1190 + 220 - 387);
    MyRoofPao(7.7, 7.7);

    // ############# wave 8 ##################
    SetTime(400 - 387, 8);
    pao_col_in_5_and_7.roofPao({{4, 9}, {2, 9}});
    SetTime(495 - 387);
    MyRoofPao(8.9, 8.9);
    SetTime(601 + 36 - 387);
    MyRoofPao(8.75, 8.75);
    SetTime(601 + 150 - 298);
    ice_filler.coffee();

    // ############# wave 9 ##################
    SetTime(900 - 387, 9);
    MyRoofPao(9, 9);
    SetTime(1070 - 387);
    MyRoofPao(9, 9);
    SetTime(1290 - 387);
    MyRoofPao(8.8, 8.8);
    SetTime(2376 - 387);
    pao_col_in_1_and_3.roofPao({{2, 9}, {4, 9}});

    // ############# wave 10 ##################
    SetTime(223 - 387, 10);
    MyRoofPao(9, 9);
    pao_col_in_5_and_7.fixLatestPao();
    SetTime(375 - 110 - 387);
    pao_col_in_5_and_7.roofPao({{2, 9}, {4, 9}});
    SetTime(375 - 387);
    pao_col_in_1_and_3.roofPao(2, 8.8);
    SetTime(375 - 100);
    Card({{"hp", 4, 9}, {"ytzd", 4, 9}});
    Delay(130);
    Shovel(4, 9);

    // ############# wave 11 ##################
    SetTime(385 - 387, 11);
    pao_col_in_5_and_7.roofPao({{2, 9}, {4, 9}});
    SetTime(513 - 387);

    pao_col_in_1_and_3.roofPao({{4, 8.75}, {2, 8.75}});
    SetTime(601 + 42 - 387);
    MyRoofPao(8.75, 8.75);
    SetTime(601 + 19 - 298);
    ice_filler.coffee();

    // ############# wave 12 ##################
    SetTime(1145 - 387, 12);
    MyRoofPao(9, 8.8);
    Delay(387 + 210 - 298);
    ice_filler.coffee();
    Ice3(298);

    // ############# wave 13 ##################
    SetTime(570 - 387, 13);
    MyRoofPao(9, 8.8);
    SetTime(570 + 220 - 387);
    MyRoofPao(8.4, 8.4);
    SetTime(1215 - 387);
    MyRoofPao(8.8, 8.4);
    Delay(387 + 210 - 298);
    ice_filler.coffee();
    Ice3(298);

    // ############# wave 14 ##################
    SetTime(420 - 373, 14);
    MyRoofPao(9, 8.8);
    SetTime(420 + 230 - 387);
    MyRoofPao(8.45, 8.45);
    SetTime(1215 - 387);
    MyRoofPao(8.9, 8.9);
    Delay(387 + 210 - 298);
    ice_filler.coffee();
    Ice3(298);

    // ############# wave 15 ##################
    SetTime(410 - 387, 15);
    MyRoofPao(9, 8.8);
    SetTime(410 + 220 - 387);
    MyRoofPao(8.45, 8.45);
    SetTime(1215 - 387);
    MyRoofPao(8.9, 8.9);
    Delay(387 + 210 - 298);
    ice_filler.coffee();
    Ice3(298);

    // ############# wave 16 ##################
    SetTime(410 - 387, 16);
    MyRoofPao(9, 9);
    SetTime(410 + 220 - 387);
    MyRoofPao(8.45, 8.45);
    SetTime(1200 - 387);
    MyRoofPao(8.9, 8.8);
    Delay(387 + 210 - 298);
    ice_filler.coffee();
    Ice3(298);

    // ############# wave 17 ##################
    SetTime(410 - 387, 17);
    MyRoofPao(8.8, 8.8);
    pao_col_in_1_and_3.fixLatestPao();
    SetTime(410 + 220 - 387);
    pao_col_in_1_and_3.roofPao(2, 8.45);
    SetTime(410 + 220 - 100);
    Card("ytzd", 4, 8);
    SetTime(1200 - 298);
    Card({{"hp", 4, 9}, {"hmg", 4, 9}, {"kfd", 4, 9}});
    SetTime(1200 + 220 - 387);
    MyRoofPao(7.7, 7.7);

    // ############# wave 18 ##################
    SetTime(400 - 387, 18);
    MyRoofPao(9, 9);
    SetTime(400 - 100 + 110);
    Card({{"hp", 3, 9}, {"hblj", 3, 9}});
    SetTime(400 - 100 + 110 + 130);
    Shovel(3, 9);
    SetTime(495 - 387);
    MyRoofPao(8.9, 8.9);
    SetTime(601 + 36 - 387);
    MyRoofPao(8.75, 8.75);
    SetTime(601 + 150 - 298);
    ice_filler.coffee();

    // ############# wave 19 ##################
    SetTime(900 - 387, 19);
    MyRoofPao(9, 9);
    SetTime(1070 - 387);
    MyRoofPao(9, 9);
    SetTime(1290 - 387);
    MyRoofPao(9, 9);
    SetTime(2376 - 387);
    MyRoofPao(9, 9);

    // ############# wave 20 ##################
    SetTime(0, 20);
    MyRoofPao(9, 9);
    MyRoofPao(9, 9);
    Delay(100);
    MyRoofPao(9, 9);
    MyRoofPao(9, 9);
#else
    AvZ::popErrorWindowNotInQueue("您的版本号与此脚本不对应！");
#endif

#else
    AvZ::popErrorWindowNotInQueue("您的版本号与此脚本不对应！");
#endif
}
