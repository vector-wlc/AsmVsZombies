# 4 脚本示例：\[FE\] 经典四炮

```cpp
// 布阵码：LI43NMQIfe3BVVEc+sHIXUAPX7dvSCBUt1TS1OE8dn9XgraGkdQ1ZlSKiEUaVw==
// 节奏：邻C6u I-PP|I-PP|N|PP 1976|1976|750|749

#include <avz.h>
#include <dsl/shorthand.h>

ALogger<AConsole> logger;

void AScript()
{
    // 对这一部分不熟悉的话建议回顾 AvZ 本体教程
    ASetInternalLogger(logger);
    ASetReloadMode(AReloadMode::MAIN_UI);
    ASetZombies({AZOMBIE, APOLE_VAULTING_ZOMBIE, ADANCING_ZOMBIE, AZOMBONI, ADOLPHIN_RIDER_ZOMBIE, AJACK_IN_THE_BOX_ZOMBIE, ABALLOON_ZOMBIE, ADIGGER_ZOMBIE, ACATAPULT_ZOMBIE, ABUNGEE_ZOMBIE, AGIGA_GARGANTUAR});
    ASelectCards({AICE_SHROOM, AM_ICE_SHROOM, ADOOM_SHROOM, ALILY_PAD, ASQUASH, ACHERRY_BOMB, APUMPKIN, APUFF_SHROOM, ASUN_SHROOM, AFLOWER_POT});
    aPlantFixer.Start(APUMPKIN, {}, 4000 / 3);

    // 定义用冰和用核操作（随用随定义也可以，但这么写在需要修改冰位核位时只需要改一处）
    ATimeline ice = I(3, 5), doom = N({{3, 8}, {3, 9}, {4, 9}});

    // 冰波：I-PP 1976
    ATimeline i = { At(11_cs) ice, TrigAt(1776_cs) PP(8.75) };
    // 加速波 1：N 750
    ATimeline n = { TrigAt(550_cs) doom };
    // 加速波 2：PP 749
    ATimeline p = { TrigAt(549_cs) PP(8.75) };

    // 主循环：第 1 波首代，第 2 波开始循环
    // 注意 1_1 % 4 不能写成 1 % 4，后者会被当作算术运算
    OnWave(1_1 % 4, 2_9 % 2, 11_19 % 1) i | i | n | p;

    // 第 10 波 PPAa 消延迟
    OnWave(10) {
        TrigAt(341_cs) PP(),
        At(401_cs) A(2, 9) & a(5, 9),
    };

    // 第 9、19 波额外补一波操作
    OnWave(9, 19) At(1976_cs) i;

    OnWave(20) {
        At(341_cs) PP(),
        At(395_cs) ice,
        At(2000_cs) PP(),
    };
}
```

[目录](../0catalogue.md)

