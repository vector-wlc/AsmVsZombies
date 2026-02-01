<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-19 14:40:06
 * @Description: 
-->
# 实用内存函数

## 女仆秘籍
```C++

// 召唤舞伴
// 舞王不前进且每帧尝试召唤舞伴
AMaidCheats::CallPartner();
// 跳舞
// 舞王不前进且不会召唤舞伴
AMaidCheats::Dancing();

// 保持前进
// 舞王一直前进
AMaidCheats::Move();

// 停止女仆秘籍
// 恢复游戏原样
AMaidCheats::Stop()
 
```

## 设定植物精准生效函数

```C++
// 卡片精准生效
// *** 注意此函数不允许大幅度修正生效时间，如果修改的时间与真实值超过 2cs，则会报错
// *** 使用示例：
// ASetPlantActiveTime(AICE_SHROOM, 298) --------- 修正寒冰菇生效时间点至当前时刻的 298cs 后
void ASetPlantActiveTime(APlantType plantType, int delayTime, const ARunOrder& runOrder = ARunOrder(0));

// 设置冰卡精准生效
// *** 注意巨人的投掷时间为 106 211
// *** 注意此函数不允许大幅度修正生效时间，如果修改的时间与真实值超过 2cs，则会报错
// 本框架1 中的 Ice3 由于作者比较脑瘫, 整错了一秒, 这里进行修正
// *** 使用示例：
// AIce3(298) --------- 修正寒冰菇生效时间点至当前时刻的 298cs 后
// 此函数用的较多，所以就特地设了一个接口，实际上其内部调用的 ASetPlantActiveTime
inline void AIce3(int delayTime, const ARunOrder& runOrder = ARunOrder(0))
{
    ASetPlantActiveTime(AICE_SHROOM, delayTime, runOrder);
}
```

## 设定游戏速度函数

```C++
// 设置游戏倍速
// *** 注意：倍速设置的范围为 [0.05, 10]
// *** 使用示例
// ASetGameSpeed(5) ---- 将游戏速度设置为 5 倍速
// ASetGameSpeed(0.1) --- 将游戏速度设置为 0.1 倍速
void ASetGameSpeed(float x);
```

## 设定僵尸出怪类型函数

```C++
// 设置出怪
// 参数命名规则：与英文原版图鉴名称一致
// *** 使用示例：
// ASetZombies({
//     APOLE_VAULTING_ZOMBIE,   // 撑杆
//     ABUCKETHEAD_ZOMBIE,      // 铁桶
//     AZOMBONI,                // 冰车
//     AJACK_IN_THE_BOX_ZOMBIE, // 小丑
//     ABALLOON_ZOMBIE,         // 气球
//     ALADDER_ZOMBIE,          // 梯子
//     ACATAPULT_ZOMBIE,        // 投篮
//     AGARGANTUAR,             // 巨人
//     AGIGA_GARGANTUAR,        // 红眼巨人
//     APOGO_ZOMBIE,            // 跳跳
// });
// 设置出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
//
// ASetZombies({
//     APOLE_VAULTING_ZOMBIE,   // 撑杆
//     ABUCKETHEAD_ZOMBIE,      // 铁桶
//     AZOMBONI,                // 冰车
//     AJACK_IN_THE_BOX_ZOMBIE, // 小丑
//     ABALLOON_ZOMBIE,         // 气球
//     ALADDER_ZOMBIE,          // 梯子
//     ACATAPULT_ZOMBIE,        // 投篮
//     AGARGANTUAR,             // 巨人
//     AGIGA_GARGANTUAR,        // 红眼巨人
//     APOGO_ZOMBIE,            // 跳跳
// }, ASetZombieMode::INTERNAL);
// 设置出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
// 并设置为自然出怪
//
// ASetZombies({
//     ABUCKETHEAD_ZOMBIE,
//     AZOMBONI,
//     AZOMBONI,
// });
// 设置出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
void ASetZombies(const std::vector<int>& zombie_type);

// 参数命名规则：与英文原版图鉴名称一致
// *** 使用示例：
// ASetWaveZombies(1, {
//     APOLE_VAULTING_ZOMBIE,   // 撑杆
//     ABUCKETHEAD_ZOMBIE,      // 铁桶
//     AZOMBONI,                // 冰车
//     AJACK_IN_THE_BOX_ZOMBIE, // 小丑
//     ABALLOON_ZOMBIE,         // 气球
//     ALADDER_ZOMBIE,          // 梯子
//     ACATAPULT_ZOMBIE,        // 投篮
//     AGARGANTUAR,             // 巨人
//     AGIGA_GARGANTUAR,        // 红眼巨人
//     APOGO_ZOMBIE,            // 跳跳
// });
// 设置第一波出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
//
// ASetWaveZombies(1, {
//     ABUCKETHEAD_ZOMBIE,
//     AZOMBONI,
//     AZOMBONI,
// });
// 设置第一波出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
void ASetWaveZombies(int wave, const std::vector<int>& zombie_type);
```

## 波长相关函数

```C++
// 设定特定波的波长
// 波长的设定范围为 601 - 2510
// *** 使用示例：
// ASetWavelength({ATime(1, 601), ATime(4, 1000)}) ----- 将第一波的波长设置为 601，将第四波的波长设置为 1000
void ASetWavelength(const std::vector<ATime>& lst, const ARunOrder& runOrder = ARunOrder(0));

// 假定特定波的波长
// *** 注意： wave 9 19 20 无法假定波长
// 波长的假定范围为 [601, 2510]
// 本函数与 ASetWavelength 区别在于, 本函数不会对内存进行修改
// 只是可以让时间点的书写范围小于 -200, 如果真实的波长与假定的波长不一致, 则会报错
// *** 使用示例：
// AAssumeWavelength({ATime(1, 601), ATime(4, 1000)}) ----- 将第一波的波长假定为 601，将第四波的波长假定为 1000
void AAssumeWavelength(const std::vector<ATime>& lst, const ARunOrder& runOrder = ARunOrder(0));
```

[目录](./00_catalogue.md)

## 附录

```C++

enum AZombieType {
    AZOMBIE = 0,             // 普僵
    AFLAG_ZOMBIE,            // 旗帜
    ACONEHEAD_ZOMBIE,        // 路障
    APOLE_VAULTING_ZOMBIE,   // 撑杆
    ABUCKETHEAD_ZOMBIE,      // 铁桶
    ANEWSPAPER_ZOMBIE,       // 读报
    ASCREEN_DOOR_ZOMBIE,     // 铁门
    AFOOTBALL_ZOMBIE,        // 橄榄
    ADANCING_ZOMBIE,         // 舞王
    ABACKUP_DANCER,          // 伴舞
    ADUCKY_TUBE_ZOMBIE,      // 鸭子
    ASNORKEL_ZOMBIE,         // 潜水
    AZOMBONI,                // 冰车
    AZOMBIE_BOBSLED_TEAM,    // 雪橇
    ADOLPHIN_RIDER_ZOMBIE,   // 海豚
    AJACK_IN_THE_BOX_ZOMBIE, // 小丑
    ABALLOON_ZOMBIE,         // 气球
    ADIGGER_ZOMBIE,          // 矿工
    APOGO_ZOMBIE,            // 跳跳
    AZOMBIE_YETI,            // 雪人
    ABUNGEE_ZOMBIE,          // 蹦极
    ALADDER_ZOMBIE,          // 扶梯
    ACATAPULT_ZOMBIE,        // 投篮
    AGARGANTUAR,             // 白眼
    AIMP,                    // 小鬼
    ADR_ZOMBOSS,             // 僵博
    AGIGA_GARGANTUAR = 32    // 红眼
};

constexpr AZombieType APJ_0 = AZOMBIE;                  // 普僵
constexpr AZombieType AQZ_1 = AFLAG_ZOMBIE;             // 旗帜
constexpr AZombieType ALZ_2 = ACONEHEAD_ZOMBIE;         // 路障
constexpr AZombieType ACG_3 = APOLE_VAULTING_ZOMBIE;    // 撑杆
constexpr AZombieType ATT_4 = ABUCKETHEAD_ZOMBIE;       // 铁桶
constexpr AZombieType ADB_5 = ANEWSPAPER_ZOMBIE;        // 读报
constexpr AZombieType ATM_6 = ASCREEN_DOOR_ZOMBIE;      // 铁门
constexpr AZombieType AGL_7 = AFOOTBALL_ZOMBIE;         // 橄榄
constexpr AZombieType AWW_8 = ADANCING_ZOMBIE;          // 舞王
constexpr AZombieType ABW_9 = ABACKUP_DANCER;           // 伴舞
constexpr AZombieType AYZ_10 = ADUCKY_TUBE_ZOMBIE;      // 鸭子
constexpr AZombieType AQS_11 = ASNORKEL_ZOMBIE;         // 潜水
constexpr AZombieType ABC_12 = AZOMBONI;                // 冰车
constexpr AZombieType AXQ_13 = AZOMBIE_BOBSLED_TEAM;    // 雪橇
constexpr AZombieType AHT_14 = ADOLPHIN_RIDER_ZOMBIE;   // 海豚
constexpr AZombieType AXC_15 = AJACK_IN_THE_BOX_ZOMBIE; // 小丑
constexpr AZombieType AQQ_16 = ABALLOON_ZOMBIE;         // 气球
constexpr AZombieType AKG_17 = ADIGGER_ZOMBIE;          // 矿工
constexpr AZombieType ATT_18 = APOGO_ZOMBIE;            // 跳跳
constexpr AZombieType AXR_19 = AZOMBIE_YETI;            // 雪人
constexpr AZombieType ABJ_20 = ABUNGEE_ZOMBIE;          // 蹦极
constexpr AZombieType AFT_21 = ALADDER_ZOMBIE;          // 扶梯
constexpr AZombieType ATL_22 = ACATAPULT_ZOMBIE;        // 投篮
constexpr AZombieType ABY_23 = AGARGANTUAR;             // 白眼
constexpr AZombieType AXG_24 = AIMP;                    // 小鬼
constexpr AZombieType AJB_25 = ADR_ZOMBOSS;             // 僵博
constexpr AZombieType AHY_32 = AGIGA_GARGANTUAR;        // 红眼
```