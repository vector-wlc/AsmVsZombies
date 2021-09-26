<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 19:21:57
 * @Description: 
-->

# 出怪设置与女仆秘籍

## 出怪设置
AvZ 中设置出怪的函数有两个，分别是 `SetZombies` 和 `SetWaveZombies`，前者设置全局的出怪类型，后者设置特定波的出怪类型。


```C++
// 设置出怪为 橄榄、红眼、白眼、气球
SetZombies({GL_7, HY_32, BY_23, QQ_16});
```

其中参数的填写可以在 pvzstruct.h 中找到相应的命名

```C++
enum ZombieType {
    ZOMBIE = 0,             // 普僵
    FLAG_ZOMBIE,            // 旗帜
    CONEHEAD_ZOMBIE,        // 路障
    POLE_VAULTING_ZOMBIE,   // 撑杆
    BUCKETHEAD_ZOMBIE,      // 铁桶
    NEWSPAPER_ZOMBIE,       // 读报
    SCREEN_DOOR_ZOMBIE,     // 铁门
    FOOTBALL_ZOMBIE,        // 橄榄
    DANCING_ZOMBIE,         // 舞王
    BACKUP_DANCER,          // 伴舞
    DUCKY_TUBE_ZOMBIE,      // 鸭子
    SNORKEL_ZOMBIE,         // 潜水
    ZOMBONI,                // 冰车
    ZOMBIE_BOBSLED_TEAM,    // 雪橇
    DOLPHIN_RIDER_ZOMBIE,   // 海豚
    JACK_IN_THE_BOX_ZOMBIE, // 小丑
    BALLOON_ZOMBIE,         // 气球
    DIGGER_ZOMBIE,          // 矿工
    POGO_ZOMBIE,            // 跳跳
    ZOMBIE_YETI,            // 雪人
    BUNGEE_ZOMBIE,          // 蹦极
    LADDER_ZOMBIE,          // 扶梯
    CATAPULT_ZOMBIE,        // 投篮
    GARGANTUAR,             // 白眼
    IMP,                    // 小鬼
    DR_ZOMBOSS,             // 僵博
    GIGA_GARGANTUAR = 32    // 红眼
};

#define PJ_0 ZOMBIE                  // 普僵
#define QZ_1 FLAG_ZOMBIE             // 旗帜
#define LZ_2 CONEHEAD_ZOMBIE         // 路障
#define CG_3 POLE_VAULTING_ZOMBIE    // 撑杆
#define TT_4 BUCKETHEAD_ZOMBIE       // 铁桶
#define DB_5 NEWSPAPER_ZOMBIE        // 读报
#define TM_6 SCREEN_DOOR_ZOMBIE      // 铁门
#define GL_7 FOOTBALL_ZOMBIE         // 橄榄
#define WW_8 DANCING_ZOMBIE          // 舞王
#define BW_9 BACKUP_DANCER           // 伴舞
#define YZ_10 DUCKY_TUBE_ZOMBIE      // 鸭子
#define QS_11 SNORKEL_ZOMBIE         // 潜水
#define BC_12 ZOMBONI                // 冰车
#define XQ_13 ZOMBIE_BOBSLED_TEAM    // 雪橇
#define HT_14 DOLPHIN_RIDER_ZOMBIE   // 海豚
#define XC_15 JACK_IN_THE_BOX_ZOMBIE // 小丑
#define QQ_16 BALLOON_ZOMBIE         // 气球
#define KG_17 DIGGER_ZOMBIE          // 矿工
#define TT_18 POGO_ZOMBIE            // 跳跳
#define XR_19 ZOMBIE_YETI            // 雪人
#define BJ_20 BUNGEE_ZOMBIE          // 蹦极
#define FT_21 LADDER_ZOMBIE          // 扶梯
#define TL_22 CATAPULT_ZOMBIE        // 投篮
#define BY_23 GARGANTUAR             // 白眼
#define XG_24 IMP                    // 小鬼
#define JB_25 DR_ZOMBOSS             // 僵博
#define HY_32 GIGA_GARGANTUAR        // 红眼
```

```C++
// 设置第二波出怪为 橄榄、红眼、白眼、气球
SetWaveZombies(2, {GL_7, HY_32, BY_23, QQ_16});
```
除需要额外填写一个波数参数外，这两个函数其他方面没有什么区别。需要注意的是，不论出怪类型里面有没有写蹦极，这两个函数都会在大波（10， 20）刷出蹦极僵尸，除此之外这两个函数还有下列使用方法

```C++
// 这里面写了两次橄榄，意思就是刷 橄榄、白眼、气球 这三种僵尸，而且比例为 2：1：1，SetWaveZombies 同理。
SetZombies({GL_7, GL_7, BY_23, QQ_16});
```

## 女仆秘籍

AvZ 中的女仆秘籍的功能由女仆秘籍类来提供。

```C++
// 舞王不前进，而且每帧都尝试召唤舞伴。
AvZ::MaidCheats::callPartner();

// 舞王不前进，而且永远不会尝试召唤舞伴。
AvZ::MaidCheats::dancing();

// 舞王将会一直前进。
AvZ::MaidCheats::move();

// 女仆秘籍将会被关闭，舞王行为恢复正常。
AvZ::MaidCheats::stop();
```
本教程的基础部分到此结束，如果只是想使用此框架编写炮阵脚本，并且不涉及"自动智能"操作，恭喜您毕业了！
后面的进阶部分，我们将深入探讨 AvZ 键控逻辑的核心——操作队列，并且介绍如何使用 AvZ 读取内存，编写无炮脚本，带大家一起玩转 AvZ! 


[上一篇 按键绑定](./key_connect.md)

[目录](../catalogue.md)

[下一篇 操作队列](../advance/time_operate.md)