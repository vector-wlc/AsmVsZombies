<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 15:51:50
 * @Description: 
-->
# 卡片相关


## 卡片操作

首先是选卡函数
```C++
ASelectCards({
    AICE_SHROOM,   // 寒冰菇
    AM_ICE_SHROOM, // 模仿寒冰菇
    ACOFFEE_BEAN,  // 咖啡豆
    ADOOM_SHROOM,  // 毁灭菇
    ALILY_PAD,     // 荷叶
    ASQUASH,       // 倭瓜
    ACHERRY_BOMB,  // 樱桃炸弹
    ABLOVER,       // 三叶草
    APUMPKIN,      // 南瓜头
    APUFF_SHROOM,  // 小喷菇
});
```
emmm， 这个选卡函数没什么好说的，我相信你应该一看就懂

然后是用卡函数
本框架的卡片操作十分简单，共有以下五种调用方式
```C++
// 根据卡片名称用卡

// 将荷叶放在三行四列
ACard(ALILY_PAD, 3, 4); 

// 将荷叶放在三行四列，将倭瓜放在三行四列
ACard({{ALILY_PAD, 3, 4}, {ASQUASH, 3, 4}}); 

// 优先将荷叶放在二行三列，如果不能种，则放在二行四列
ACard(ALILY_PAD, {{3, 4}, {2, 4}}); 

// 根据卡片所在卡槽位置用卡

// 将第一张卡片放在二行三列
ACard(1, 2, 3);

// 优先将第一张卡片放在二行三列，如果不能种，则放在二行四列
ACard(1, {{2, 3}, {2, 4}}); 
```

我们常用的是前三种，即使用卡片名称进行用卡，因为这样使用代码可读性更高，当然，除了使用英文单词外，本框架 还支持中文拼音，例如倭瓜可以写为 `AWG_17`，至于后面的数字是为了避免重名的，当然他还有一个意义是 PvZ 程序中的植物类型代号。

读者可能会疑惑 本框架 中卡片名称的使用标准是什么，如 ALILY_PAD 代表的是荷叶，本页教程的附录会详细列出卡片名称对应表。

这里还需要给大家提的一点是，ACard 函数如果种植植物成功，则会返回其种下的植物对象的指针，如果种植失败，则会返回 nullptr，
那么可能有疑惑的就是植物对象的指针，看下面的源码

```C++
auto plant = ACard(ALILY_PAD, 3, 4);
if(plant != nullptr){ // 首先需要检查返回值是否有效
    plant->Row(); // 得到种下的植物所处行，其值为 2，因为 pvz 是从 0 开始数的 
    plant->Type(); // 得到种下的植物类型，这个肯定是荷叶
    plant->Hp(); // 得到种下的植物血量
}

```

看完了上面的代码，你可能还有疑惑，plant 的成员函数都有什么，你需要查看 `avz_pvz_struct.h` 中的内容。
如果你看不懂上面的代码，没关系，慢慢就会懂了，不用急。

## 修正植物生效函数
由于 PvZ 序号大小的影响，植物的生效倒计时会发生 1cs 的波动，为了解决这个问题，本框架 提供了设定植物生效时间的功能。

**请不要滥用此功能，这可能会破坏游戏规则**

`ASetPlantActiveTime` 函数参数的意义是将指定类型的植物的生效时间设置为调用此函数时刻的 `参数cs` 之后，例如 

```C++
// 修正寒冰菇生效时间点到此刻的 298cs 后
ACard({{AICE_SHROOM, 1, 1}, {ACOFFEE_BEAN, 1, 1}});
ASetPlantActiveTime(AICE_SHROOM, 298);
```

## 铲除
铲除操作由函数 AShovel 实现

```C++
// 铲除4行6列的植物,如果植物有南瓜保护默认优先铲除被保护植物
AShovel(4, 6);

// 铲除4行6列的植物,如果植物有南瓜保护优先铲除南瓜
AShovel(4, 6, true);

// 铲除3行6列，4行6列的植物
AShovel({{3, 6},{4, 6}});
```


[目录](./0catalogue.md)


## 附录

```C++

// 植物类型
enum APlantType {
    APEASHOOTER = 0, // 豌豆射手
    ASUNFLOWER,      // 向日葵
    ACHERRY_BOMB,    // 樱桃炸弹
    AWALL_NUT,       // 坚果
    APOTATO_MINE,    // 土豆地雷
    ASNOW_PEA,       // 寒冰射手
    ACHOMPER,        // 大嘴花
    AREPEATER,       // 双重射手
    APUFF_SHROOM,    // 小喷菇
    ASUN_SHROOM,     // 阳光菇
    AFUME_SHROOM,    // 大喷菇
    AGRAVE_BUSTER,   // 墓碑吞噬者
    AHYPNO_SHROOM,   // 魅惑菇
    ASCAREDY_SHROOM, // 胆小菇
    AICE_SHROOM,     // 寒冰菇
    ADOOM_SHROOM,    // 毁灭菇
    ALILY_PAD,       // 荷叶
    ASQUASH,         // 倭瓜
    ATHREEPEATER,    // 三发射手
    ATANGLE_KELP,    // 缠绕海藻
    AJALAPENO,       // 火爆辣椒
    ASPIKEWEED,      // 地刺
    ATORCHWOOD,      // 火炬树桩
    ATALL_NUT,       // 高坚果
    ASEA_SHROOM,     // 水兵菇
    APLANTERN,       // 路灯花
    ACACTUS,         // 仙人掌
    ABLOVER,         // 三叶草
    ASPLIT_PEA,      // 裂荚射手
    ASTARFRUIT,      // 杨桃
    APUMPKIN,        // 南瓜头
    AMAGNET_SHROOM,  // 磁力菇
    ACABBAGE_PULT,   // 卷心菜投手
    AFLOWER_POT,     // 花盆
    AKERNEL_PULT,    // 玉米投手
    ACOFFEE_BEAN,    // 咖啡豆
    AGARLIC,         // 大蒜
    AUMBRELLA_LEAF,  // 叶子保护伞
    AMARIGOLD,       // 金盏花
    AMELON_PULT,     // 西瓜投手
    AGATLING_PEA,    // 机枪射手
    ATWIN_SUNFLOWER, // 双子向日葵
    AGLOOM_SHROOM,   // 忧郁菇
    ACATTAIL,        // 香蒲
    AWINTER_MELON,   // 冰西瓜投手
    AGOLD_MAGNET,    // 吸金磁
    ASPIKEROCK,      // 地刺王
    ACOB_CANNON,     // 玉米加农炮
    AIMITATOR,       // 模仿者

    // 模仿者命名 + M
    AM_PEASHOOTER,     // 豌豆射手
    AM_SUNFLOWER,      // 向日葵
    AM_CHERRY_BOMB,    // 樱桃炸弹
    AM_WALL_NUT,       // 坚果
    AM_POTATO_MINE,    // 土豆地雷
    AM_SNOW_PEA,       // 寒冰射手
    AM_CHOMPER,        // 大嘴花
    AM_REPEATER,       // 双重射手
    AM_PUFF_SHROOM,    // 小喷菇
    AM_SUN_SHROOM,     // 阳光菇
    AM_FUME_SHROOM,    // 大喷菇
    AM_GRAVE_BUSTER,   // 墓碑吞噬者
    AM_HYPNO_SHROOM,   // 魅惑菇
    AM_SCAREDY_SHROOM, // 胆小菇
    AM_ICE_SHROOM,     // 寒冰菇
    AM_DOOM_SHROOM,    // 毁灭菇
    AM_LILY_PAD,       // 荷叶
    AM_SQUASH,         // 倭瓜
    AM_THREEPEATER,    // 三发射手
    AM_TANGLE_KELP,    // 缠绕海藻
    AM_JALAPENO,       // 火爆辣椒
    AM_SPIKEWEED,      // 地刺
    AM_TORCHWOOD,      // 火炬树桩
    AM_TALL_NUT,       // 高坚果
    AM_SEA_SHROOM,     // 水兵菇
    AM_PLANTERN,       // 路灯花
    AM_CACTUS,         // 仙人掌
    AM_BLOVER,         // 三叶草
    AM_SPLIT_PEA,      // 裂荚射手
    AM_STARFRUIT,      // 杨桃
    AM_PUMPKIN,        // 南瓜头
    AM_MAGNET_SHROOM,  // 磁力菇
    AM_CABBAGE_PULT,   // 卷心菜投手
    AM_FLOWER_POT,     // 花盆
    AM_KERNEL_PULT,    // 玉米投手
    AM_COFFEE_BEAN,    // 咖啡豆
    AM_GARLIC,         // 大蒜
    AM_UMBRELLA_LEAF,  // 叶子保护伞
    AM_MARIGOLD,       // 金盏花
    AM_MELON_PULT,     // 西瓜投手
};

// 兼容旧版的拼音

constexpr APlantType AWDSS_0 = APEASHOOTER;       // 豌豆射手
constexpr APlantType AXRK_1 = ASUNFLOWER;         // 向日葵
constexpr APlantType AYTZD_2 = ACHERRY_BOMB;      // 樱桃炸弹
constexpr APlantType AJG_3 = AWALL_NUT;           // 坚果
constexpr APlantType ATDDL_4 = APOTATO_MINE;      // 土豆地雷
constexpr APlantType AHBSS_5 = ASNOW_PEA;         // 寒冰射手
constexpr APlantType ADZH_6 = ACHOMPER;           // 大嘴花
constexpr APlantType ASCSS_7 = AREPEATER;         // 双重射手
constexpr APlantType AXPG_8 = APUFF_SHROOM;       // 小喷菇
constexpr APlantType AYGG_9 = ASUN_SHROOM;        // 阳光菇
constexpr APlantType ADPG_10 = AFUME_SHROOM;      // 大喷菇
constexpr APlantType AMBTSZ_11 = AGRAVE_BUSTER;   // 墓碑吞噬者
constexpr APlantType AMHG_12 = AHYPNO_SHROOM;     // 魅惑菇
constexpr APlantType ADXG_13 = ASCAREDY_SHROOM;   // 胆小菇
constexpr APlantType AHBG_14 = AICE_SHROOM;       // 寒冰菇
constexpr APlantType AHMG_15 = ADOOM_SHROOM;      // 毁灭菇
constexpr APlantType AHY_16 = ALILY_PAD;          // 荷叶
constexpr APlantType AWG_17 = ASQUASH;            // 倭瓜
constexpr APlantType ASFSS_18 = ATHREEPEATER;     // 三发射手
constexpr APlantType ACRHZ_19 = ATANGLE_KELP;     // 缠绕海藻
constexpr APlantType AHBLJ_20 = AJALAPENO;        // 火爆辣椒
constexpr APlantType ADC_21 = ASPIKEWEED;         // 地刺
constexpr APlantType AHJSZ_22 = ATORCHWOOD;       // 火炬树桩
constexpr APlantType AGJG_23 = ATALL_NUT;         // 高坚果
constexpr APlantType ASBG_24 = ASEA_SHROOM;       // 水兵菇
constexpr APlantType ALDH_25 = APLANTERN;         // 路灯花
constexpr APlantType AXRZ_26 = ACACTUS;           // 仙人掌
constexpr APlantType ASYC_27 = ABLOVER;           // 三叶草
constexpr APlantType ALJSS_28 = ASPLIT_PEA;       // 裂荚射手
constexpr APlantType AYT_29 = ASTARFRUIT;         // 杨桃
constexpr APlantType ANGT_30 = APUMPKIN;          // 南瓜头
constexpr APlantType ACLG_31 = AMAGNET_SHROOM;    // 磁力菇
constexpr APlantType AJXCTS_32 = ACABBAGE_PULT;   // 卷心菜投手
constexpr APlantType AHP_33 = AFLOWER_POT;        // 花盆
constexpr APlantType AYMTS_34 = AKERNEL_PULT;     // 玉米投手
constexpr APlantType AKFD_35 = ACOFFEE_BEAN;      // 咖啡豆
constexpr APlantType ADS_36 = AGARLIC;            // 大蒜
constexpr APlantType AYZBHS_37 = AUMBRELLA_LEAF;  // 叶子保护伞
constexpr APlantType AJZH_38 = AMARIGOLD;         // 金盏花
constexpr APlantType AXGTS_39 = AMELON_PULT;      // 西瓜投手
constexpr APlantType AJQSS_40 = AGATLING_PEA;     // 机枪射手
constexpr APlantType ASZXRK_41 = ATWIN_SUNFLOWER; // 双子向日葵
constexpr APlantType AYYG_42 = AGLOOM_SHROOM;     // 忧郁菇
constexpr APlantType AXP_43 = ACATTAIL;           // 香蒲
constexpr APlantType ABXGTS_44 = AWINTER_MELON;   // 冰西瓜投手
constexpr APlantType AXJC_45 = AGOLD_MAGNET;      // 吸金磁
constexpr APlantType ADCW_46 = ASPIKEROCK;        // 地刺王
constexpr APlantType AYMJNP_47 = ACOB_CANNON;     // 玉米加农炮
constexpr APlantType AMFZ_48 = AIMITATOR;         // 模仿者

// 模仿者植物

constexpr APlantType AM_WDSS_0 = AM_PEASHOOTER;      // 豌豆射手
constexpr APlantType AM_XRK_1 = AM_SUNFLOWER;        // 向日葵
constexpr APlantType AM_YTZD_2 = AM_CHERRY_BOMB;     // 樱桃炸弹
constexpr APlantType AM_JG_3 = AM_WALL_NUT;          // 坚果
constexpr APlantType AM_TDDL_4 = AM_POTATO_MINE;     // 土豆地雷
constexpr APlantType AM_HBSS_5 = AM_SNOW_PEA;        // 寒冰射手
constexpr APlantType AM_DZH_6 = AM_CHOMPER;          // 大嘴花
constexpr APlantType AM_SCSS_7 = AM_REPEATER;        // 双重射手
constexpr APlantType AM_XPG_8 = AM_PUFF_SHROOM;      // 小喷菇
constexpr APlantType AM_YGG_9 = AM_SUN_SHROOM;       // 阳光菇
constexpr APlantType AM_DPG_10 = AM_FUME_SHROOM;     // 大喷菇
constexpr APlantType AM_MBTSZ_11 = AM_GRAVE_BUSTER;  // 墓碑吞噬者
constexpr APlantType AM_MHG_12 = AM_HYPNO_SHROOM;    // 魅惑菇
constexpr APlantType AM_DXG_13 = AM_SCAREDY_SHROOM;  // 胆小菇
constexpr APlantType AM_HBG_14 = AM_ICE_SHROOM;      // 寒冰菇
constexpr APlantType AM_HMG_15 = AM_DOOM_SHROOM;     // 毁灭菇
constexpr APlantType AM_HY_16 = AM_LILY_PAD;         // 荷叶
constexpr APlantType AM_WG_17 = AM_SQUASH;           // 倭瓜
constexpr APlantType AM_SFSS_18 = AM_THREEPEATER;    // 三发射手
constexpr APlantType AM_CRHZ_19 = AM_TANGLE_KELP;    // 缠绕海藻
constexpr APlantType AM_HBLJ_20 = AM_JALAPENO;       // 火爆辣椒
constexpr APlantType AM_DC_21 = AM_SPIKEWEED;        // 地刺
constexpr APlantType AM_HJSZ_22 = AM_TORCHWOOD;      // 火炬树桩
constexpr APlantType AM_GJG_23 = AM_TALL_NUT;        // 高坚果
constexpr APlantType AM_SBG_24 = AM_SEA_SHROOM;      // 水兵菇
constexpr APlantType AM_LDH_25 = AM_PLANTERN;        // 路灯花
constexpr APlantType AM_XRZ_26 = AM_CACTUS;          // 仙人掌
constexpr APlantType AM_SYC_27 = AM_BLOVER;          // 三叶草
constexpr APlantType AM_LJSS_28 = AM_SPLIT_PEA;      // 裂荚射手
constexpr APlantType AM_YT_29 = AM_STARFRUIT;        // 杨桃
constexpr APlantType AM_NGT_30 = AM_PUMPKIN;         // 南瓜头
constexpr APlantType AM_CLG_31 = AM_MAGNET_SHROOM;   // 磁力菇
constexpr APlantType AM_JXCTS_32 = AM_CABBAGE_PULT;  // 卷心菜投手
constexpr APlantType AM_HP_33 = AM_FLOWER_POT;       // 花盆
constexpr APlantType AM_YMTS_34 = AM_KERNEL_PULT;    // 玉米投手
constexpr APlantType AM_KFD_35 = AM_COFFEE_BEAN;     // 咖啡豆
constexpr APlantType AM_DS_36 = AM_GARLIC;           // 大蒜
constexpr APlantType AM_YZBHS_37 = AM_UMBRELLA_LEAF; // 叶子保护伞
constexpr APlantType AM_JZH_38 = AM_MARIGOLD;        // 金盏花
constexpr APlantType AM_XGTS_39 = AM_MELON_PULT;     // 西瓜投手

```
