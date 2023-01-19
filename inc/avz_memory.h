/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-09 10:45:11
 * @Description:
 */
#ifndef __AVZ_MEMORY_H__
#define __AVZ_MEMORY_H__

#include "avz_state_hook.h"

__ANodiscard inline AMainObject* AGetMainObject() { return __aInternalGlobal.mainObject; }

__ANodiscard inline APvzBase* AGetPvzBase() { return __aInternalGlobal.pvzBase; }

// 返回鼠标所在行
__ANodiscard int AMouseRow();

// 返回鼠标所在列
__ANodiscard float AMouseCol();

// 获取指定类型植物的卡槽对象序列 植物类型与图鉴顺序相同，从0开始
// 返回的卡片对象序列范围：[0,9]
// AGetSeedIndex(16)------------获得荷叶的卡槽对象序列
// AGetSeedIndex(16, true)-------获得模仿者荷叶的卡槽对象序列
__ANodiscard int AGetSeedIndex(int type, bool imitator = false);

// 得到指定位置和类型的植物对象序列
// 当参数type为默认值-1时该函数无视南瓜花盆荷叶
// *** 使用示例：
// GetPlantIndex(3, 4)------如果三行四列有除南瓜花盆荷叶之外的植物时，返回该植物的对象序列，否则返回-1
// GetPlantIndex(3, 4, 47)---如果三行四列有春哥，返回其对象序列，如果有其他植物，返回-2，否则返回-1
__ANodiscard int AGetPlantIndex(int row, int col, int type = -1);

// 得到一组指定位置的植物的对象序列
// 参数1：填写一组指定位置
// 参数2：填写指定类型
// 参数3：得到对象序列，此函数按照位置的顺序填写对象序列
// *** 注意：如果没有植物填写-1，如果有植物但是不是指定类型，会填写-2
void AGetPlantIndices(const std::vector<AGrid>& lstIn, int type, std::vector<int>& indexsOut);
__ANodiscard std::vector<int> AGetPlantIndices(const std::vector<AGrid>& lst, int type);

// 检查僵尸是否存在
// *** 使用示例
// AIsZombieExist()-------检查场上是否存在僵尸
// AIsZombieExist(23)-------检查场上是否存在巨人僵尸
// AIsZombieExist(-1,4)-----检查第四行是否有僵尸存在
// AIsZombieExist(23,4)-----检查第四行是否有巨人僵尸存在
__ANodiscard bool AIsZombieExist(int type, int row = -1);

// 卡片精准生效
// *** 注意此函数不允许大幅度修正生效时间，如果修改的时间与真实值超过 2cs，则会报错
// *** 使用示例：
// ASetPlantActiveTime(AICE_SHROOM, 298) --------- 修正寒冰菇生效时间点至当前时刻的 298cs 后
void ASetPlantActiveTime(APlantType plantType, int delayTime);

// 设置冰卡精准生效
// *** 注意巨人的投掷时间为 105 210
// *** 注意此函数不允许大幅度修正生效时间，如果修改的时间与真实值超过 2cs，则会报错
// 之前的 Ice3 由于作者比较脑瘫, 整错了一秒, 这里进行修正
// *** 使用示例：
// AIce3(298) --------- 修正寒冰菇生效时间点至当前时刻的 298cs 后
inline void AIce3(int delayTime)
{
    ASetPlantActiveTime(AICE_SHROOM, delayTime);
}

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
//     ABUCKETHEAD_ZOMBIE,
//     AZOMBONI,
//     AZOMBONI,
// });
// 设置出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
void ASetZombies(const std::vector<int>& zombieType);

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
void ASetWaveZombies(int wave, const std::vector<int>& zombieType);

// *** 使用示例：
// 检查当前出怪中是否有红眼：
// auto zombieTypeList = AGetZombieTypeList();
// bool isHasHongYan = zombieTypeList[HY_32]
// if(isHasHongYan){
//      // 如果有红眼干啥
// }
__ANodiscard bool* AGetZombieTypeList();

class __AGameSpeedManager : public AOrderedStateHook<-1> {
public:
    static void Set(float x);

protected:
    int _oriTickMs = 10;
    virtual void _BeforeScript() override;
    virtual void _ExitFight() override;
};

// 设置游戏倍速
// *** 注意：倍速设置的范围为 [0.05, 10]
// *** 使用示例
// ASetGameSpeed(5) ---- 将游戏速度设置为 5 倍速
// ASetGameSpeed(0.1) --- 将游戏速度设置为 0.1 倍速
inline void ASetGameSpeed(float x)
{
    __AGameSpeedManager::Set(x);
}

// 女仆秘籍
class AMaidCheats : public AOrderedStateHook<-1> {
public:
    // 召唤舞伴
    // 舞王不前进且每帧尝试召唤舞伴
    static void CallPartner()
    {
        *((uint32_t*)(0x52DFC9)) = 0x00F0B890;
    }

    // 跳舞
    // 舞王不前进且不会召唤舞伴
    static void Dancing()
    {
        *((uint32_t*)(0x52DFC9)) = 0x0140B890;
    }

    // 保持前进
    // 舞王一直前进
    static void Move()
    {
        *((uint32_t*)(0x52DFC9)) = 0x00E9B890;
    }

    // 停止女仆秘籍
    // 恢复游戏原样
    static void Stop()
    {
        *((uint32_t*)(0x52DFC9)) = 0x838808B;
    }

protected:
    virtual void _ExitFight() override
    {
        Stop();
    }
};

// 判断游戏是否暂停
bool AGameIsPaused();

// 移除植物函数
void ARemovePlant(int row, int col, APlantType type);

inline __AGameSpeedManager __agsm; // AStateHook
inline AMaidCheats __amc;          // AStateHook
#endif