<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 15:51:50
 * @Description: 
-->
# 卡片操作

AvZ 的卡片操作十分简单，共有以下五种调用方式
```C++
// 根据卡片名称用卡

// 将荷叶放在三行四列
Card(LILY_PAD, 3, 4); 

// 将荷叶放在三行四列，将倭瓜放在三行四列
Card({{LILY_PAD, 3, 4}, {SQUASH, 3, 4}}); 

// 优先将荷叶放在二行三列，如果不能种，则放在二行四列
Card(LILY_PAD, {{3, 4}, {2, 4}}); 

// 根据卡片所在卡槽位置用卡

// 将第一张卡片放在二行三列
Card(1, 2, 3);

// 优先将第一张卡片放在二行三列，如果不能种，则放在二行四列
Card(1, {{2, 3}, {2, 4}}); 
```

我们常用的是前三种，即使用卡片名称进行用卡，因为这样使用代码可读性更高，当然，除了使用英文单词外，AvZ 还支持中文拼音，例如倭瓜可以写为 `WG_17`，至于后面的数字是为了避免重名的，当然他还有一个意义是 PvZ 程序中的植物类型代号。

读者可能会疑惑 AvZ 中卡片名称的使用标准是什么，如 LILY_PAD 代表的是荷叶，下面是卡片名称对应表

```C++

// 植物类型
enum PlantType {
    PEASHOOTER = 0, // 豌豆射手
    SUNFLOWER,      // 向日葵
    CHERRY_BOMB,    // 樱桃炸弹
    WALL_NUT,       // 坚果
    POTATO_MINE,    // 土豆地雷
    SNOW_PEA,       // 寒冰射手
    CHOMPER,        // 大嘴花
    REPEATER,       // 双重射手
    PUFF_SHROOM,    // 小喷菇
    SUN_SHROOM,     // 阳光菇
    FUME_SHROOM,    // 大喷菇
    GRAVE_BUSTER,   // 墓碑吞噬者
    HYPNO_SHROOM,   // 魅惑菇
    SCAREDY_SHROOM, // 胆小菇
    ICE_SHROOM,     // 寒冰菇
    DOOM_SHROOM,    // 毁灭菇
    LILY_PAD,       // 荷叶
    SQUASH,         // 倭瓜
    THREEPEATER,    // 三发射手
    TANGLE_KELP,    // 缠绕海藻
    JALAPENO,       // 火爆辣椒
    SPIKEWEED,      // 地刺
    TORCHWOOD,      // 火炬树桩
    TALL_NUT,       // 高坚果
    SEA_SHROOM,     // 水兵菇
    PLANTERN,       // 路灯花
    CACTUS,         // 仙人掌
    BLOVER,         // 三叶草
    SPLIT_PEA,      // 裂荚射手
    STARFRUIT,      // 杨桃
    PUMPKIN,        // 南瓜头
    MAGNET_SHROOM,  // 磁力菇
    CABBAGE_PULT,   // 卷心菜投手
    FLOWER_POT,     // 花盆
    KERNEL_PULT,    // 玉米投手
    COFFEE_BEAN,    // 咖啡豆
    GARLIC,         // 大蒜
    UMBRELLA_LEAF,  // 叶子保护伞
    MARIGOLD,       // 金盏花
    MELON_PULT,     // 西瓜投手
    GATLING_PEA,    // 机枪射手
    TWIN_SUNFLOWER, // 双子向日葵
    GLOOM_SHROOM,   // 忧郁菇
    CATTAIL,        // 香蒲
    WINTER_MELON,   // 冰西瓜投手
    GOLD_MAGNET,    // 吸金磁
    SPIKEROCK,      // 地刺王
    COB_CANNON,     // 玉米加农炮

    // 模仿者命名 + M
    M_PEASHOOTER,     // 豌豆射手
    M_SUNFLOWER,      // 向日葵
    M_CHERRY_BOMB,    // 樱桃炸弹
    M_WALL_NUT,       // 坚果
    M_POTATO_MINE,    // 土豆地雷
    M_SNOW_PEA,       // 寒冰射手
    M_CHOMPER,        // 大嘴花
    M_REPEATER,       // 双重射手
    M_PUFF_SHROOM,    // 小喷菇
    M_SUN_SHROOM,     // 阳光菇
    M_FUME_SHROOM,    // 大喷菇
    M_GRAVE_BUSTER,   // 墓碑吞噬者
    M_HYPNO_SHROOM,   // 魅惑菇
    M_SCAREDY_SHROOM, // 胆小菇
    M_ICE_SHROOM,     // 寒冰菇
    M_DOOM_SHROOM,    // 毁灭菇
    M_LILY_PAD,       // 荷叶
    M_SQUASH,         // 倭瓜
    M_THREEPEATER,    // 三发射手
    M_TANGLE_KELP,    // 缠绕海藻
    M_JALAPENO,       // 火爆辣椒
    M_SPIKEWEED,      // 地刺
    M_TORCHWOOD,      // 火炬树桩
    M_TALL_NUT,       // 高坚果
    M_SEA_SHROOM,     // 水兵菇
    M_PLANTERN,       // 路灯花
    M_CACTUS,         // 仙人掌
    M_BLOVER,         // 三叶草
    M_SPLIT_PEA,      // 裂荚射手
    M_STARFRUIT,      // 杨桃
    M_PUMPKIN,        // 南瓜头
    M_MAGNET_SHROOM,  // 磁力菇
    M_CABBAGE_PULT,   // 卷心菜投手
    M_FLOWER_POT,     // 花盆
    M_KERNEL_PULT,    // 玉米投手
    M_COFFEE_BEAN,    // 咖啡豆
    M_GARLIC,         // 大蒜
    M_UMBRELLA_LEAF,  // 叶子保护伞
    M_MARIGOLD,       // 金盏花
    M_MELON_PULT,     // 西瓜投手
};

// 兼容汉语拼音

#define WDSS_0 PEASHOOTER       // 豌豆射手
#define XRK_1 SUNFLOWER         // 向日葵
#define YTZD_2 CHERRY_BOMB      // 樱桃炸弹
#define JG_3 WALL_NUT           // 坚果
#define TDDL_4 POTATO_MINE      // 土豆地雷
#define HBSS_5 SNOW_PEA         // 寒冰射手
#define DZH_6 CHOMPER           // 大嘴花
#define SCSS_7 REPEATER         // 双重射手
#define XPG_8 PUFF_SHROOM       // 小喷菇
#define YGG_9 SUN_SHROOM        // 阳光菇
#define DPG_10 FUME_SHROOM      // 大喷菇
#define MBTSZ_11 GRAVE_BUSTER   // 墓碑吞噬者
#define MHG_12 HYPNO_SHROOM     // 魅惑菇
#define DXG_13 SCAREDY_SHROOM   // 胆小菇
#define HBG_14 ICE_SHROOM       // 寒冰菇
#define HMG_15 DOOM_SHROOM      // 毁灭菇
#define HY_16 LILY_PAD          // 荷叶
#define WG_17 SQUASH            // 倭瓜
#define SFSS_18 THREEPEATER     // 三发射手
#define CRHZ_19 TANGLE_KELP     // 缠绕海藻
#define HBLJ_20 JALAPENO        // 火爆辣椒
#define DC_21 SPIKEWEED         // 地刺
#define HJSZ_22 TORCHWOOD       // 火炬树桩
#define GJG_23 TALL_NUT         // 高坚果
#define SBG_24 SEA_SHROOM       // 水兵菇
#define LDH_25 PLANTERN         // 路灯花
#define XRZ_26 CACTUS           // 仙人掌
#define SYC_27 BLOVER           // 三叶草
#define LJSS_28 SPLIT_PEA       // 裂荚射手
#define YT_29 STARFRUIT         // 杨桃
#define NGT_30 PUMPKIN          // 南瓜头
#define CLG_31 MAGNET_SHROOM    // 磁力菇
#define JXCTS_32 CABBAGE_PULT   // 卷心菜投手
#define HP_33 FLOWER_POT        // 花盆
#define YMTS_34 KERNEL_PULT     // 玉米投手
#define KFD_35 COFFEE_BEAN      // 咖啡豆
#define DS_36 GARLIC            // 大蒜
#define YZBHS_37 UMBRELLA_LEAF  // 叶子保护伞
#define JZH_38 MARIGOLD         // 金盏花
#define XGTS_39 MELON_PULT      // 西瓜投手
#define JQSS_40 GATLING_PEA     // 机枪射手
#define SZXRK_41 TWIN_SUNFLOWER // 双子向日葵
#define YYG_42 GLOOM_SHROOM     // 忧郁菇
#define XP_43 CATTAIL           // 香蒲
#define BXGTS_44 WINTER_MELON   // 冰西瓜投手
#define XJC_45 GOLD_MAGNET      // 吸金磁
#define DCW_46 SPIKEROCK        // 地刺王
#define YMJNP_47 COB_CANNON     // 玉米加农炮

// 模仿者植物

#define M_WDSS_0 M_PEASHOOTER      // 豌豆射手
#define M_XRK_1 M_SUNFLOWER        // 向日葵
#define M_YTZD_2 M_CHERRY_BOMB     // 樱桃炸弹
#define M_JG_3 M_WALL_NUT          // 坚果
#define M_TDDL_4 M_POTATO_MINE     // 土豆地雷
#define M_HBSS_5 M_SNOW_PEA        // 寒冰射手
#define M_DZH_6 M_CHOMPER          // 大嘴花
#define M_SCSS_7 M_REPEATER        // 双重射手
#define M_XPG_8 M_PUFF_SHROOM      // 小喷菇
#define M_YGG_9 M_SUN_SHROOM       // 阳光菇
#define M_DPG_10 M_FUME_SHROOM     // 大喷菇
#define M_MBTSZ_11 M_GRAVE_BUSTER  // 墓碑吞噬者
#define M_MHG_12 M_HYPNO_SHROOM    // 魅惑菇
#define M_DXG_13 M_SCAREDY_SHROOM  // 胆小菇
#define M_HBG_14 M_ICE_SHROOM      // 寒冰菇
#define M_HMG_15 M_DOOM_SHROOM     // 毁灭菇
#define M_HY_16 M_LILY_PAD         // 荷叶
#define M_WG_17 M_SQUASH           // 倭瓜
#define M_SFSS_18 M_THREEPEATER    // 三发射手
#define M_CRHZ_19 M_TANGLE_KELP    // 缠绕海藻
#define M_HBLJ_20 M_JALAPENO       // 火爆辣椒
#define M_DC_21 M_SPIKEWEED        // 地刺
#define M_HJSZ_22 M_TORCHWOOD      // 火炬树桩
#define M_GJG_23 M_TALL_NUT        // 高坚果
#define M_SBG_24 M_SEA_SHROOM      // 水兵菇
#define M_LDH_25 M_PLANTERN        // 路灯花
#define M_XRZ_26 M_CACTUS          // 仙人掌
#define M_SYC_27 M_BLOVER          // 三叶草
#define M_LJSS_28 M_SPLIT_PEA      // 裂荚射手
#define M_YT_29 M_STARFRUIT        // 杨桃
#define M_NGT_30 M_PUMPKIN         // 南瓜头
#define M_CLG_31 M_MAGNET_SHROOM   // 磁力菇
#define M_JXCTS_32 M_CABBAGE_PULT  // 卷心菜投手
#define M_HP_33 M_FLOWER_POT       // 花盆
#define M_YMTS_34 M_KERNEL_PULT    // 玉米投手
#define M_KFD_35 M_COFFEE_BEAN     // 咖啡豆
#define M_DS_36 M_GARLIC           // 大蒜
#define M_YZBHS_37 M_UMBRELLA_LEAF // 叶子保护伞
#define M_JZH_38 M_MARIGOLD        // 金盏花
#define M_XGTS_39 M_MELON_PULT     // 西瓜投手

```


[上一篇 多次生效](./multiple_effective.md)

[目录](../catalogue.md)

[下一篇 时间设定](./time_rule.md)