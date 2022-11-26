#ifndef __AVZ_PVZ_STRUCT_H__
#define __AVZ_PVZ_STRUCT_H__

#include "avz_types.h"
#include <Windows.h>
#include <cstdint>
#include <type_traits>

struct APvzBase;         // 游戏主体
struct AMainObject;      // 主要对象
struct APlant;           // 植物
struct AZombie;          // 僵尸
struct ASeed;            // 种子（卡片）
struct AItem;            // 收集物
struct APlaceItem;       // 场地物品
struct AMouseWindow;     // 鼠标窗口
struct ATopMouseWindow;  // 顶层鼠标窗口
struct ALetsRockBtn;     // lets_rock 按钮
struct ASelectCardUi_m;  // 选卡界面在 main_object
struct ASelectCardUi_p;  // 选卡界面在 pvz_base
struct AMouse;           // 鼠标
struct AMouseExtra;      // 鼠标额外属性
struct AWords;           // 文字属性
struct AAnimationMain;   // 动画主要对象
struct AAnimationOffset; // 动画地址偏移
struct AAnimation;       // 动画
struct ACardSlot;        // 卡槽

class APvzStruct {
    __ADeleteCopyAndMove(APvzStruct);

public:
    template <typename T>
    __ANodiscard T& MRef(uintptr_t addr) noexcept
    {
        return (T&)((uint8_t*)this)[addr];
    }

    template <typename T>
    __ANodiscard T* MPtr(uintptr_t addr) noexcept
    {
        return *(T**)((uint8_t*)this + addr);
    }

    template <typename T>
    __ANodiscard T MVal(uintptr_t addr) noexcept
    {
        return (T)((uint8_t*)this + addr);
    }
};

// 游戏基址
struct APvzBase : public APvzStruct {
    __ADeleteCopyAndMove(APvzBase);

public:
    // 当前游戏信息和对象
    __ANodiscard AMainObject* MainObject() noexcept
    {
        return MPtr<AMainObject>(0x768);
    }

    // 鼠标和窗口
    __ANodiscard AMouseWindow* MouseWindow() noexcept
    {
        return MPtr<AMouseWindow>(0x320);
    }

    // 选卡界面属性
    __ANodiscard ASelectCardUi_p* SelectCardUi_p() noexcept
    {
        return MPtr<ASelectCardUi_p>(0x774);
    }

    // 动画信息
    __ANodiscard AAnimationMain* AnimationMain() noexcept
    {
        return MPtr<AAnimationMain>(0x820);
    }

    // 游戏界面
    // 1 - 主界面
    // 2 - 选卡界面
    // 3 - 战斗界面
    __ANodiscard int& GameUi() noexcept
    {
        return MRef<int>(0x7fc);
    }

    // 每帧的时长
    __ANodiscard int& TickMs() noexcept
    {
        return MRef<int>(0x454);
    }

    // MJ 时钟
    __ANodiscard int& MjClock() noexcept
    {
        return MRef<int>(0x838);
    }
};

// 当前游戏信息和对象
struct AMainObject : public APvzStruct {
    __ADeleteCopyAndMove(AMainObject);
    uint8_t _data[0x57b0];

public:
    // 僵尸内存数组
    __ANodiscard AZombie* ZombieArray() noexcept
    {
        return MPtr<AZombie>(0x90);
    }

    // 僵尸数组大小
    __ANodiscard int& ZombieCountMax() noexcept
    {
        return MRef<int>(0x94);
    }

    // 僵尸数组大小
    __ANodiscard int& ZombieTotal() noexcept
    {
        return MRef<int>(0x94);
    }

    // 僵尸数量上限
    __ANodiscard int& ZombieLimit() noexcept
    {
        return MRef<int>(0x98);
    }

    // 下一个僵尸的编号
    __ANodiscard int& ZombieNext() noexcept
    {
        return MRef<int>(0x9c);
    }

    // 当前僵尸的数量
    __ANodiscard int& ZombieCount() noexcept
    {
        return MRef<int>(0xa0);
    }

    // 最后一个僵尸的编号
    __ANodiscard int& ZombieLast() noexcept
    {
        return MRef<int>(0xa4);
    }

    // 僵尸刷新血量
    __ANodiscard int& ZombieRefreshHp() noexcept
    {
        return MRef<int>(0x5594);
    }

    // 植物内存数组
    __ANodiscard APlant* PlantArray() noexcept
    {
        return MPtr<APlant>(0xac);
    }

    // 植物数组大小
    __ANodiscard int& PlantCountMax() noexcept
    {
        return MRef<int>(0xb0);
    }

    // 植物数组大小
    __ANodiscard int& PlantTotal() noexcept
    {
        return MRef<int>(0xb0);
    }

    // 植物数量上限
    __ANodiscard int& PlantLimit() noexcept
    {
        return MRef<int>(0xb4);
    }

    // 下一个植物的编号
    __ANodiscard int& PlantNext() noexcept
    {
        return MRef<int>(0xb8);
    }

    // 当前植物的数量
    __ANodiscard int& PlantCount() noexcept
    {
        return MRef<int>(0xbc);
    }

    // 最后一个植物的编号
    __ANodiscard int& PlantLast() noexcept
    {
        return MRef<int>(0xc0);
    }

    // 种子内存数组
    __ANodiscard ASeed* SeedArray() noexcept
    {
        return MPtr<ASeed>(0x144);
    }

    // 收集物内存数组
    __ANodiscard AItem* ItemArray() noexcept
    {
        return MPtr<AItem>(0xe4);
    }

    // 收集物内存数组大小
    __ANodiscard int& ItemCountMax() noexcept
    {
        return MRef<int>(0xe8);
    }

    // 收集物内存数组大小
    __ANodiscard int& ItemTotal() noexcept
    {
        return MRef<int>(0xe8);
    }

    // 收集物内存数组
    APlaceItem* PlaceItemArray() noexcept
    {
        return MPtr<APlaceItem>(0x11c);
    }

    // 收集物内存数组大小
    __ANodiscard int& PlaceItemCountMax() noexcept
    {
        return MRef<int>(0x120);
    }

    // 收集物内存数组大小
    __ANodiscard int& PlaceItemTotal() noexcept
    {
        return MRef<int>(0x120);
    }

    // 游戏是否暂停
    __ANodiscard bool& GamePaused() noexcept
    {
        return MRef<bool>(0x164);
    }

    // 游戏场景
    // 0 - 白天
    // 1 - 黑夜
    // 2 - 泳池
    // 3 - 雾夜
    // 4 - 天台
    __ANodiscard int& Scene() noexcept
    {
        return MRef<int>(0x554c);
    }

    // 当前阳光数量
    __ANodiscard int& Sun() noexcept
    {
        return MRef<int>(0x5560);
    }

    // 战斗界面游戏时钟
    // 选卡界面暂停计时
    __ANodiscard int& GameClock() noexcept
    {
        return MRef<int>(0x5568);
    }

    // 全局时钟
    // 战斗界面和选卡界面都计时
    __ANodiscard int& GlobalClock() noexcept
    {
        return MRef<int>(0x556c);
    }

    // 僵尸刷新倒计时
    __ANodiscard int& RefreshCountdown() noexcept
    {
        return MRef<int>(0x559c);
    }

    // 大波僵尸刷新倒计时
    __ANodiscard int& HugeWaveCountdown() noexcept
    {
        return MRef<int>(0x55a4);
    }

    // 总波数
    __ANodiscard int& TotalWave() noexcept
    {
        return MRef<int>(0x5564);
    }

    // 当前波数
    __ANodiscard int& Wave() noexcept
    {
        return MRef<int>(0x557c);
    }

    // 僵尸初始刷新倒计时
    __ANodiscard int& InitialCountdown() noexcept
    {
        return MRef<int>(0x55a0);
    }

    // 点炮倒计时
    // 游戏 30cs 防误触机制
    __ANodiscard int& ClickPaoCountdown() noexcept
    {
        return MRef<int>(0x5754);
    }

    // 鼠标属性
    __ANodiscard AMouse* MouseAttribution() noexcept
    {
        return MPtr<AMouse>(0x138);
    }

    // 鼠标额外属性
    __ANodiscard AMouseExtra* MouseExtraAttribution() noexcept
    {
        return MPtr<AMouseExtra>(0x13c);
    }

    // 选卡界面属性
    __ANodiscard ASelectCardUi_m* SelectCardUi_m() noexcept
    {
        return MPtr<ASelectCardUi_m>(0x15c);
    }

    // 场地格子类型列表
    __ANodiscard uint32_t* GridTypeList() noexcept
    {
        return MVal<uint32_t*>(0x168);
    }

    // 出怪列表
    __ANodiscard uint32_t* ZombieList() noexcept
    {
        return MVal<uint32_t*>(0x6b4);
    }

    // 出怪种类列表
    __ANodiscard bool* ZombieTypeList() noexcept
    {
        return MVal<bool*>(0x54d4);
    }

    // 文字信息
    __ANodiscard AWords* Words() noexcept
    {
        return MPtr<AWords>(0x140);
    }

    // 载入存档的状态
    __ANodiscard int& LoadDataState() noexcept
    {
        return MRef<int>(0x5604);
    }
};

// 植物内存属性
struct APlant : public APvzStruct {
    __ADeleteCopyAndMove(APlant);

protected:
    uint8_t _data[0x14c];

public:
    // 横坐标
    __ANodiscard int& Xi() noexcept
    {
        return MRef<int>(0x8);
    }

    // 纵坐标
    __ANodiscard int& Yi() noexcept
    {
        return MRef<int>(0xc);
    }

    // 横坐标
    __ANodiscard int& Abscissa() noexcept
    {
        return MRef<int>(0x8);
    }

    // 纵坐标
    __ANodiscard int& Ordinate() noexcept
    {
        return MRef<int>(0xc);
    }

    // 植物是否可见
    __ANodiscard bool& Visible() noexcept
    {
        return MRef<bool>(0x18);
    }

    // 植物所在行
    // 范围 : [0, 5]
    __ANodiscard int& Row() noexcept
    {
        return MRef<int>(0x1c);
    }

    // 植物类型
    // 范围 : [0, 47]
    // 注意模仿者植物种下去后的类型与原卡数字代号一样
    // 只是外表不一样，例如 南瓜头 和 模仿南瓜头 的植物类型都是 30
    __ANodiscard int& Type() noexcept
    {
        return MRef<int>(0x24);
    }

    // 植物所在列
    // 范围 : [0, 8]
    __ANodiscard int& Col() noexcept
    {
        return MRef<int>(0x28);
    }

    // 返回植物的状态
    // 35：空炮
    // 36：正在装填
    // 37：准备就绪
    // 38：正在发射
    __ANodiscard int& State() noexcept
    {
        return MRef<int>(0x3c);
    }

    // 植物状态倒计时
    __ANodiscard int& StateCountdown() noexcept
    {
        return MRef<int>(0x54);
    }

    // 植物当前血量
    __ANodiscard int& Hp() noexcept
    {
        return MRef<int>(0x40);
    }

    // 植物最大血量
    __ANodiscard int& HpMax() noexcept
    {
        return MRef<int>(0x44);
    }

    // 为1则可以攻击
    __ANodiscard int& CanShoot() noexcept
    {
        return MRef<int>(0x48);
    }

    // 三叶草消失倒计时
    __ANodiscard int& BloverCountdown() noexcept
    {
        return MRef<int>(0x4c);
    }

    // 灰烬冰核三叶草生效倒计时
    __ANodiscard int& ExplodeCountdown() noexcept
    {
        return MRef<int>(0x50);
    }

    // 子弹发射倒计时
    __ANodiscard int& ShootCountdown() noexcept
    {
        return MRef<int>(0x90);
    }

    // 蘑菇倒计时
    __ANodiscard int& MushroomCountdown() noexcept
    {
        return MRef<int>(0x130);
    }

    // 受伤判定宽度
    __ANodiscard int& HurtWidth() noexcept
    {
        return MRef<int>(0x10);
    }

    // 蘑菇倒计时
    __ANodiscard int& HurtHeight() noexcept
    {
        return MRef<int>(0x14);
    }

    // 植物是否消失
    __ANodiscard const bool& IsDisappeared() noexcept
    {
        return MRef<bool>(0x141);
    }

    // 植物是否被压扁
    __ANodiscard bool& IsCrushed() noexcept
    {
        return MRef<bool>(0x142);
    }

    // 植物是否在睡觉
    __ANodiscard bool& IsSleeping() noexcept
    {
        return MRef<bool>(0x143);
    }

    // 动作编号
    __ANodiscard uint16_t& AnimationCode() noexcept
    {
        return MRef<uint16_t>(0x94);
    }
};

struct AZombie : public APvzStruct {
    __ADeleteCopyAndMove(AZombie);

protected:
    uint8_t _data[0x15c];

public:
    // 僵尸是否存在
    // 只读
    __ANodiscard bool IsExist()
    {
        return !MRef<bool>(0xEC);
    }

    // 僵尸所在行
    // 范围 : [0, 5]
    __ANodiscard int& Row() noexcept
    {
        return MRef<int>(0x1c);
    }

    // 僵尸横坐标
    __ANodiscard float& Abscissa() noexcept
    {
        return MRef<float>(0x2c);
    }

    // 僵尸纵坐标
    __ANodiscard float& Ordinate() noexcept
    {
        return MRef<float>(0x30);
    }

    // 僵尸类型
    __ANodiscard int& Type() noexcept
    {
        return MRef<int>(0x24);
    }

    // 僵尸目前本体血量
    __ANodiscard int& Hp() noexcept
    {
        return MRef<int>(0xC8);
    }

    // 僵尸一类饰品目前血量
    __ANodiscard int& OneHp() noexcept
    {
        return MRef<int>(0xD0);
    }

    // 僵尸二类饰品目前血量
    __ANodiscard int& TwoHp() noexcept
    {
        return MRef<int>(0xDC);
    }

    // 僵尸是否在啃食
    __ANodiscard bool& IsEat() noexcept
    {
        return MRef<bool>(0x51);
    }

    // 僵尸状态
    // 1 - 倒地
    // 2 - 灰烬
    // 3 - 小推车
    // 70 - 巨人举锤
    __ANodiscard int& State() noexcept
    {
        return MRef<int>(0x28);
    }

    // 返回选卡界面僵尸站立状态
    // 为-2/-3时静止,-4时向上(对于选卡界面的僵尸)

    __ANodiscard int& StandState() noexcept
    {
        return MRef<int>(0x6c);
    }

    // 僵尸所在波数
    __ANodiscard int& AtWave() noexcept
    {
        return MRef<int>(0x6c);
    }

    // 僵尸是否死亡
    // 只读
    __ANodiscard bool IsDead()
    {
        return State() == 1 || State() == 2 || State() == 3;
    }

    // 僵尸是否举锤
    // 只读
    __ANodiscard bool IsHammering()
    {
        return State() == 70;
    }

    // 僵尸是否隐形
    __ANodiscard bool& IsStealth() noexcept
    {
        return MRef<bool>(0x18);
    }

    // 僵尸横向相对速度
    __ANodiscard float& Speed() noexcept
    {
        return MRef<float>(0x34);
    }

    // 僵尸存在时间
    __ANodiscard int& ExistTime() noexcept
    {
        return MRef<int>(0x60);
    }

    // 僵尸状态倒计时
    __ANodiscard int& StateCountdown() noexcept
    {
        return MRef<int>(0x68);
    }

    // 僵尸是否消失
    __ANodiscard const bool& IsDisappeared() noexcept
    {
        return MRef<bool>(0xEC);
    }

    // 中弹判定的横坐标
    __ANodiscard int& BulletAbscissa() noexcept
    {
        return MRef<int>(0x8C);
    }

    // 中弹判定的纵坐标
    __ANodiscard int& BulletOrdinate() noexcept
    {
        return MRef<int>(0x90);
    }

    // 攻击判定的横坐标
    __ANodiscard int& AttackAbscissa() noexcept
    {
        return MRef<int>(0x9C);
    }

    // 攻击判定的纵坐标
    __ANodiscard int& AttackOrdinate() noexcept
    {
        return MRef<int>(0xA0);
    }

    // 僵尸减速倒计时
    __ANodiscard int& SlowCountdown() noexcept
    {
        return MRef<int>(0xAC);
    }

    // 僵尸黄油固定倒计时
    __ANodiscard int& FixationCountdown() noexcept
    {
        return MRef<int>(0xB0);
    }

    // 僵尸冻结倒计时
    __ANodiscard int& FreezeCountdown() noexcept
    {
        return MRef<int>(0xB4);
    }

    // 受伤判定宽度
    __ANodiscard int& HurtWidth() noexcept
    {
        return MRef<int>(0x94);
    }

    // 蘑菇倒计时
    __ANodiscard int& HurtHeight() noexcept
    {
        return MRef<int>(0x98);
    }
};

// 种子 / 卡牌 属性
struct ASeed : public APvzStruct {
    __ADeleteCopyAndMove(ASeed);

protected:
    uint8_t _data[0x50];

public:
    // 返回卡槽中的卡牌个数
    // 注意：此函数不能迭代使用！！！
    // 只能这样使用：auto seed_count = GetMainObject()->seedArray()->count();
    __ANodiscard int& Count() noexcept
    {
        return MRef<int>(0x24);
    }

    // 种子是否可用
    __ANodiscard bool& IsUsable() noexcept
    {
        return MRef<bool>(0x48 + 0x28);
    }

    // 种子冷却
    __ANodiscard int& Cd() noexcept
    {
        return MRef<int>(0x24 + 0x28);
    }

    // 种子初始冷却
    __ANodiscard int& InitialCd() noexcept
    {
        return MRef<int>(0x28 + 0x28);
    }

    // 模仿者类型
    __ANodiscard int& ImitatorType() noexcept
    {
        return MRef<int>(0x38 + 0x28);
    }

    // 种子类型
    __ANodiscard int& Type() noexcept
    {
        return MRef<int>(0x34 + 0x28);
    }

    // 种子横坐标
    __ANodiscard int& Abscissa() noexcept
    {
        return MRef<int>(0x8 + 0x28);
    }

    // 种子纵坐标
    __ANodiscard int& Ordinate() noexcept
    {
        return MRef<int>(0xc + 0x28);
    }

    // 卡牌判定高度
    __ANodiscard int& Height() noexcept
    {
        return MRef<int>(0x14 + 0x28);
    }

    // 卡牌判定宽度
    __ANodiscard int& Width() noexcept
    {
        return MRef<int>(0x10 + 0x28);
    }

    // 卡牌 x 偏移量
    // 在老虎机中的位置
    __ANodiscard int& XOffset() noexcept
    {
        return MRef<int>(0x30 + 0x28);
    }
};

// 收集物品属性
struct AItem : public APvzStruct {
    __ADeleteCopyAndMove(AItem);

protected:
    uint8_t _data[0xd8];

public:
    // 物品是否消失
    __ANodiscard const bool& IsDisappeared() noexcept
    {
        return MRef<bool>(0x38);
    }

    // 物品是否被收集
    __ANodiscard bool& IsCollected() noexcept
    {
        return MRef<bool>(0x50);
    }

    // 物品所在横坐标
    __ANodiscard float& Abscissa() noexcept
    {
        return MRef<float>(0x24);
    }

    // 物品所在纵坐标
    __ANodiscard float& Ordinate() noexcept
    {
        return MRef<float>(0x28);
    }

    // 物品类型
    __ANodiscard int& Type() noexcept
    {
        return MRef<int>(0x58);
    }
};

// 场地物品属性
struct APlaceItem : public APvzStruct {
    __ADeleteCopyAndMove(APlaceItem);

protected:
    uint8_t _data[0xec];

public:
    // 物品是否消失
    __ANodiscard const bool& IsDisappeared() noexcept
    {
        return MRef<bool>(0x20);
    }

    // 物品所在行
    __ANodiscard int& Row() noexcept
    {
        return MRef<int>(0x14);
    }

    // 物品所在列
    __ANodiscard int& Col() noexcept
    {
        return MRef<int>(0x10);
    }

    // 物品类型
    __ANodiscard int& Type() noexcept
    {
        return MRef<int>(0x8);
    }

    // 墓碑冒出的量,弹坑消失倒计时,脑子血量,钉钯消失倒计时
    __ANodiscard int& Value() noexcept
    {
        return MRef<int>(0x18);
    }
};

// 动画主要属性
struct AAnimationMain : public APvzStruct {
    __ADeleteCopyAndMove(AAnimationMain);

public:
    // 动画偏移
    __ANodiscard AAnimationOffset* AnimationOffset() noexcept
    {
        return MPtr<AAnimationOffset>(0x8);
    }
};

// 动画偏移属性
struct AAnimationOffset : public APvzStruct {
    __ADeleteCopyAndMove(AAnimationOffset);

public:
    // 动画内存数组
    __ANodiscard AAnimation* AnimationArray() noexcept
    {
        return MPtr<AAnimation>(0x0);
    }
};

// 动画属性
struct AAnimation : public APvzStruct {
    __ADeleteCopyAndMove(AAnimation);

protected:
    uint8_t _data[0xa0];

public:
    // 动画循环率
    __ANodiscard float& CirculationRate() noexcept
    {
        return MRef<float>(0x4);
    }
};

struct ATopMouseWindow : public APvzStruct {
    __ADeleteCopyAndMove(ATopMouseWindow);

public:
    // 窗口类型(1图鉴,2暂停,3是否,4商店等,6用户管理,8菜单)
    __ANodiscard int& Type() noexcept
    {
        return MRef<int>(0xc);
    }

    __ANodiscard bool& IsDisplay() noexcept
    {
        return MRef<bool>(0x54);
    }
};

struct AMouseWindow : public APvzStruct {
    __ADeleteCopyAndMove(AMouseWindow);

public:
    __ANodiscard ATopMouseWindow* TopWindow() noexcept
    {
        return MPtr<ATopMouseWindow>(0x94);
    }

    __ANodiscard bool& IsInWindow() noexcept
    {
        return MRef<bool>(0xdc);
    }

    __ANodiscard int& MouseAbscissa() noexcept
    {
        return MRef<int>(0xe0);
    }

    __ANodiscard int& MouseOrdinate() noexcept
    {
        return MRef<int>(0xe4);
    }
};

struct ASelectCardUi_m : public APvzStruct {
    __ADeleteCopyAndMove(ASelectCardUi_m);

public:
    __ANodiscard int& OrizontalScreenOffset() noexcept
    {
        return MRef<int>(0x8);
    }

    __ANodiscard bool& IsCreatZombie() noexcept
    {
        return MRef<bool>(0x35);
    }
};

struct ASelectCardUi_p : public APvzStruct {
    __ADeleteCopyAndMove(ASelectCardUi_p);

public:
    __ANodiscard ALetsRockBtn* LetsRockBtn() noexcept
    {
        return MPtr<ALetsRockBtn>(0x88);
    }

    // 卡片移动状态
    // 参数：植物类型代号
    // 0移上卡槽
    // 1在卡槽里
    // 2移下卡槽
    // 3在选卡界面里
    __ANodiscard int& CardMoveState(int index) noexcept
    {
        return MRef<int>(index * 0x3c + 0xc8);
    }
};

struct ALetsRockBtn : public APvzStruct {
    __ADeleteCopyAndMove(ALetsRockBtn);

public:
    __ANodiscard bool& IsUnusable() noexcept
    {
        return MRef<bool>(0x1a);
    }
};

struct AMouse : public APvzStruct {
    __ADeleteCopyAndMove(AMouse);

public:
    // 鼠标上物品的类型
    __ANodiscard int& Type() noexcept
    {
        return MRef<int>(0x30);
    }

    __ANodiscard int& Abscissa() noexcept
    {
        return MRef<int>(0x8);
    }

    __ANodiscard uintptr_t& CannonAddress() noexcept
    {
        return MRef<uintptr_t>(0x40);
    }
};

struct AMouseExtra : public APvzStruct {
    __ADeleteCopyAndMove(AMouseExtra);

public:
    __ANodiscard int& Row() noexcept
    {
        return MRef<int>(0x28);
    }
};

struct AWords : public APvzStruct {
    __ADeleteCopyAndMove(AWords);

public:
    __ANodiscard int& DisappearCountdown() noexcept
    {
        return MRef<int>(0x88);
    }
};

#endif