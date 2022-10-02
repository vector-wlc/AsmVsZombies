#pragma once

#include <Windows.h>
#include <cstdint>
#include <type_traits>

struct PvZ;             // 游戏主体
struct MainObject;      // 主要对象
struct Plant;           // 植物
struct Zombie;          // 僵尸
struct Seed;            // 种子（卡片）
struct Item;            // 收集物
struct PlaceItem;       // 场地物品
struct MouseWindow;     // 鼠标窗口
struct TopMouseWindow;  // 顶层鼠标窗口
struct LetsRockBtn;     // lets_rock 按钮
struct SelectCardUi_m;  // 选卡界面在 main_object
struct SelectCardUi_p;  // 选卡界面在 pvz_base
struct Mouse;           // 鼠标
struct MouseExtra;      // 鼠标额外属性
struct Text;            // 文字属性
struct AnimationMain;   // 动画主要对象
struct AnimationOffset; // 动画地址偏移
struct Animation;       // 动画
struct CardSlot;        // 卡槽

class PvzStruct {
public:
    PvzStruct(PvzStruct&& __) = delete;
    PvzStruct(const PvzStruct& __) = delete;
    PvzStruct& operator=(PvzStruct&& __) = delete;
    PvzStruct& operator=(const PvzStruct& __) = delete;

    template <typename T>
    T& mRef(uintptr_t addr)
    {
        return (T&)((uint8_t*)this)[addr];
    }

    template <typename T>
    T* mPtr(uintptr_t addr)
    {
        return *(T**)((uint8_t*)this + addr);
    }

    template <typename T>
    T mVal(uintptr_t addr)
    {
        return (T)((uint8_t*)this + addr);
    }
};

// 游戏基址
struct PvZ : public PvzStruct {

public:
    // 当前游戏信息和对象
    MainObject* mainObject()
    {
        return mPtr<MainObject>(0x768);
    }

    // 鼠标和窗口
    MouseWindow* mouseWindow()
    {
        return mPtr<MouseWindow>(0x320);
    }

    // 选卡界面属性
    SelectCardUi_p* selectCardUi_p()
    {
        return mPtr<SelectCardUi_p>(0x774);
    }

    // 动画信息
    AnimationMain* animationMain()
    {
        return mPtr<AnimationMain>(0x820);
    }

    // 游戏界面
    // 1 - 主界面
    // 2 - 选卡界面
    // 3 - 战斗界面
    int& gameUi()
    {
        return mRef<int>(0x7fc);
    }

    // 每帧的时长
    int& tickMs()
    {
        return mRef<int>(0x454);
    }

    // MJ 时钟
    int& mjClock()
    {
        return mRef<int>(0x838);
    }
};

// 当前游戏信息和对象
struct MainObject : public PvzStruct {

    uint8_t data[0x57b0];

public:
    // 僵尸内存数组
    Zombie* zombieArray()
    {
        return mPtr<Zombie>(0x90);
    }

    // 僵尸数组大小
    int& zombieCountMax()
    {
        return mRef<int>(0x94);
    }

    // 僵尸数组大小
    int& zombieTotal()
    {
        return mRef<int>(0x94);
    }

    // 僵尸数量上限
    int& zombieLimit()
    {
        return mRef<int>(0x98);
    }

    // 下一个僵尸的编号
    int& zombieNext()
    {
        return mRef<int>(0x9c);
    }

    // 当前僵尸的数量
    int& zombieCount()
    {
        return mRef<int>(0xa0);
    }

    // 最后一个僵尸的编号
    int& zombieLast()
    {
        return mRef<int>(0xa4);
    }

    // 僵尸刷新血量
    int& zombieRefreshHp()
    {
        return mRef<int>(0x5594);
    }

    // 植物内存数组
    Plant* plantArray()
    {
        return mPtr<Plant>(0xac);
    }

    // 植物数组大小
    int& plantCountMax()
    {
        return mRef<int>(0xb0);
    }

    // 植物数组大小
    int& plantTotal()
    {
        return mRef<int>(0xb0);
    }

    // 植物数量上限
    int& plantLimit()
    {
        return mRef<int>(0xb4);
    }

    // 下一个植物的编号
    int& plantNext()
    {
        return mRef<int>(0xb8);
    }

    // 当前植物的数量
    int& plantCount()
    {
        return mRef<int>(0xbc);
    }

    // 最后一个植物的编号
    int& plantLast()
    {
        return mRef<int>(0xc0);
    }

    // 种子内存数组
    Seed* seedArray()
    {
        return mPtr<Seed>(0x144);
    }

    // 收集物内存数组
    Item* itemArray()
    {
        return mPtr<Item>(0xe4);
    }

    // 收集物内存数组大小
    int& itemCountMax()
    {
        return mRef<int>(0xe8);
    }

    // 收集物内存数组大小
    int& itemTotal()
    {
        return mRef<int>(0xe8);
    }

    // 收集物内存数组
    PlaceItem* placeItemArray()
    {
        return mPtr<PlaceItem>(0x11c);
    }

    // 收集物内存数组大小
    int& placeItemCountMax()
    {
        return mRef<int>(0x120);
    }

    // 收集物内存数组大小
    int& placeItemTotal()
    {
        return mRef<int>(0x120);
    }

    // 游戏是否暂停
    bool& gamePaused()
    {
        return mRef<bool>(0x164);
    }

    // 游戏场景
    // 0 - 白天
    // 1 - 黑夜
    // 2 - 泳池
    // 3 - 雾夜
    // 4 - 天台
    int& scene()
    {
        return mRef<int>(0x554c);
    }

    // 当前阳光数量
    int& sun()
    {
        return mRef<int>(0x5560);
    }

    // 战斗界面游戏时钟
    // 选卡界面暂停计时
    int& gameClock()
    {
        return mRef<int>(0x5568);
    }

    // 全局时钟
    // 战斗界面和选卡界面都计时
    int& globalClock()
    {
        return mRef<int>(0x556c);
    }

    // 僵尸刷新倒计时
    int& refreshCountdown()
    {
        return mRef<int>(0x559c);
    }

    // 大波僵尸刷新倒计时
    int& hugeWaveCountdown()
    {
        return mRef<int>(0x55a4);
    }

    // 总波数
    int& totalWave()
    {
        return mRef<int>(0x5564);
    }

    // 当前波数
    int& wave()
    {
        return mRef<int>(0x557c);
    }

    // 僵尸初始刷新倒计时
    int& initialCountdown()
    {
        return mRef<int>(0x55a0);
    }

    // 点炮倒计时
    // 游戏 30cs 防误触机制
    int& clickPaoCountdown()
    {
        return mRef<int>(0x5754);
    }

    // 鼠标属性
    Mouse* mouseAttribution()
    {
        return mPtr<Mouse>(0x138);
    }

    // 鼠标额外属性
    MouseExtra* mouseExtraAttribution()
    {
        return mPtr<MouseExtra>(0x13c);
    }

    // 选卡界面属性
    SelectCardUi_m* selectCardUi_m()
    {
        return mPtr<SelectCardUi_m>(0x15c);
    }

    // 场地格子类型列表
    uint32_t* gridTypeList()
    {
        return mVal<uint32_t*>(0x168);
    }

    // 出怪列表
    uint32_t* zombieList()
    {
        return mVal<uint32_t*>(0x6b4);
    }

    // 出怪种类列表
    uint8_t* zombieTypeList()
    {
        return mVal<uint8_t*>(0x54d4);
    }

    // 文字信息
    Text* text()
    {
        return mPtr<Text>(0x140);
    }

    // 载入存档的状态
    int& loadDataState()
    {
        return mRef<int>(0x5604);
    }
};

// 植物内存属性
struct Plant : public PvzStruct {
private:
    uint8_t data[0x14c];

public:
    // 横坐标
    int& xi()
    {
        return mRef<int>(0x8);
    }

    // 纵坐标
    int& yi()
    {
        return mRef<int>(0xc);
    }

    // 横坐标
    int& abscissa()
    {
        return mRef<int>(0x8);
    }

    // 纵坐标
    int& ordinate()
    {
        return mRef<int>(0xc);
    }

    // 植物是否可见
    bool& visible()
    {
        return mRef<bool>(0x18);
    }

    // 植物所在行
    // 范围 : [0, 5]
    int& row()
    {
        return mRef<int>(0x1c);
    }

    // 植物类型
    // 范围 : [0, 47]
    // 注意模仿者植物种下去后的类型与原卡数字代号一样
    // 只是外表不一样，例如 南瓜头 和 模仿南瓜头 的植物类型都是 30
    int& type()
    {
        return mRef<int>(0x24);
    }

    // 植物所在列
    // 范围 : [0, 8]
    int& col()
    {
        return mRef<int>(0x28);
    }

    // 返回植物的状态
    // 35：空炮
    // 36：正在装填
    // 37：准备就绪
    // 38：正在发射
    int& state()
    {
        return mRef<int>(0x3c);
    }

    // 植物状态倒计时
    int& stateCountdown()
    {
        return mRef<int>(0x54);
    }

    // 植物当前血量
    int& hp()
    {
        return mRef<int>(0x40);
    }

    // 植物最大血量
    int& hpMax()
    {
        return mRef<int>(0x44);
    }

    // 为1则可以攻击
    int& canShoot()
    {
        return mRef<int>(0x48);
    }

    // 三叶草消失倒计时
    int& bloverCountdown()
    {
        return mRef<int>(0x4c);
    }

    // 灰烬冰核三叶草生效倒计时
    int& explodeCountdown()
    {
        return mRef<int>(0x50);
    }

    // 子弹发射倒计时
    int& shootCountdown()
    {
        return mRef<int>(0x90);
    }

    // 蘑菇倒计时
    int& mushroomCountdown()
    {
        return mRef<int>(0x130);
    }

    // 受伤判定宽度
    int& hurtWidth()
    {
        return mRef<int>(0x10);
    }

    // 蘑菇倒计时
    int& hurtHeight()
    {
        return mRef<int>(0x14);
    }

    // 植物是否消失
    const bool& isDisappeared()
    {
        return mRef<bool>(0x141);
    }

    // 植物是否被压扁
    bool& isCrushed()
    {
        return mRef<bool>(0x142);
    }

    // 植物是否在睡觉
    bool& sleeping()
    {
        return mRef<bool>(0x143);
    }

    // 动作编号
    uint16_t& animationCode()
    {
        return mRef<uint16_t>(0x94);
    }
};

struct Zombie : public PvzStruct {
private:
    uint8_t data[0x15c];

public:
    // 僵尸是否存在
    // 只读
    bool isExist()
    {
        return !mRef<bool>(0xEC);
    }

    // 僵尸所在行
    // 范围 : [0, 5]
    int& row()
    {
        return mRef<int>(0x1c);
    }

    // 僵尸横坐标
    float& abscissa()
    {
        return mRef<float>(0x2c);
    }

    // 僵尸纵坐标
    float& ordinate()
    {
        return mRef<float>(0x30);
    }

    // 僵尸类型
    int& type()
    {
        return mRef<int>(0x24);
    }

    // 僵尸目前本体血量
    int& hp()
    {
        return mRef<int>(0xC8);
    }

    // 僵尸一类饰品目前血量
    int& oneHp()
    {
        return mRef<int>(0xD0);
    }

    // 僵尸二类饰品目前血量
    int& twoHp()
    {
        return mRef<int>(0xDC);
    }

    // 僵尸是否在啃食
    bool& isEat()
    {
        return mRef<bool>(0x51);
    }

    // 僵尸状态
    // 1 - 倒地
    // 2 - 灰烬
    // 3 - 小推车
    // 70 - 巨人举锤
    int& state()
    {
        return mRef<int>(0x28);
    }

    // 返回选卡界面僵尸站立状态
    // 为-2/-3时静止,-4时向上(对于选卡界面的僵尸)
    // 僵尸所在波数 ?????
    // https://space.bilibili.com/1321409404
    int& standState()
    {
        return mRef<int>(0x6c);
    }

    // 僵尸是否死亡
    // 只读
    bool isDead()
    {
        return state() == 1 || state() == 2 || state() == 3;
    }

    // 僵尸是否举锤
    // 只读
    bool isHammering()
    {
        return state() == 70;
    }

    // 僵尸是否隐形
    bool& isStealth()
    {
        return mRef<bool>(0x18);
    }

    // 僵尸横向相对速度
    float& speed()
    {
        return mRef<float>(0x34);
    }

    // 僵尸存在时间
    int& existTime()
    {
        return mRef<int>(0x60);
    }

    // 僵尸状态倒计时
    int& stateCountdown()
    {
        return mRef<int>(0x68);
    }

    // 僵尸是否消失
    const bool& isDisappeared()
    {
        return mRef<bool>(0xEC);
    }

    // 中弹判定的横坐标
    int& bulletAbscissa()
    {
        return mRef<int>(0x8C);
    }

    // 中弹判定的纵坐标
    int& bulletOrdinate()
    {
        return mRef<int>(0x90);
    }

    // 攻击判定的横坐标
    int& attackAbscissa()
    {
        return mRef<int>(0x9C);
    }

    // 攻击判定的纵坐标
    int& attackOrdinate()
    {
        return mRef<int>(0xA0);
    }

    // 僵尸减速倒计时
    int& slowCountdown()
    {
        return mRef<int>(0xAC);
    }

    // 僵尸黄油固定倒计时
    int& fixationCountdown()
    {
        return mRef<int>(0xB0);
    }

    // 僵尸冻结倒计时
    int& freezeCountdown()
    {
        return mRef<int>(0xB4);
    }

    // 受伤判定宽度
    int& hurtWidth()
    {
        return mRef<int>(0x94);
    }

    // 蘑菇倒计时
    int& hurtHeight()
    {
        return mRef<int>(0x98);
    }
};

// 种子 / 卡牌 属性
struct Seed : public PvzStruct {
private:
    uint8_t data[0x50];

public:
    // 返回卡槽中的卡牌个数
    // 注意：此函数不能迭代使用！！！
    // 只能这样使用：auto seed_count = GetMainObject()->seedArray()->count();
    int& count()
    {
        return mRef<int>(0x24);
    }

    // 种子是否可用
    bool& isUsable()
    {
        return mRef<bool>(0x48 + 0x28);
    }

    // 种子冷却
    int& cd()
    {
        return mRef<int>(0x24 + 0x28);
    }

    // 种子初始冷却
    int& initialCd()
    {
        return mRef<int>(0x28 + 0x28);
    }

    // 模仿者类型
    int& imitatorType()
    {
        return mRef<int>(0x38 + 0x28);
    }

    // 种子类型
    int& type()
    {
        return mRef<int>(0x34 + 0x28);
    }

    // 种子横坐标
    int& abscissa()
    {
        return mRef<int>(0x8 + 0x28);
    }

    // 种子纵坐标
    int& ordinate()
    {
        return mRef<int>(0xc + 0x28);
    }

    // 卡牌判定高度
    int& height()
    {
        return mRef<int>(0x14 + 0x28);
    }

    // 卡牌判定宽度
    int& width()
    {
        return mRef<int>(0x10 + 0x28);
    }

    // 卡牌 x 偏移量
    // 在老虎机中的位置
    int xOffset()
    {
        return mRef<int>(0x30 + 0x28);
    }
};

// 收集物品属性
struct Item : public PvzStruct {
private:
    uint8_t data[0xd8];

public:
    // 物品是否消失
    const bool& isDisappeared()
    {
        return mRef<bool>(0x38);
    }

    // 物品是否被收集
    bool& isCollected()
    {
        return mRef<bool>(0x50);
    }

    // 物品所在横坐标
    float& abscissa()
    {
        return mRef<float>(0x24);
    }

    // 物品所在纵坐标
    float& ordinate()
    {
        return mRef<float>(0x28);
    }

    // 物品类型
    int& type()
    {
        return mRef<int>(0x58);
    }
};

// 场地物品属性
struct PlaceItem : public PvzStruct {
private:
    uint8_t data[0xec];

public:
    // 物品是否消失
    const bool& isDisappeared()
    {
        return mRef<bool>(0x20);
    }

    // 物品所在行
    int& row()
    {
        return mRef<int>(0x14);
    }

    // 物品所在列
    int& col()
    {
        return mRef<int>(0x10);
    }

    // 物品类型
    int& type()
    {
        return mRef<int>(0x8);
    }

    // 墓碑冒出的量,弹坑消失倒计时,脑子血量,钉钯消失倒计时
    int& value()
    {
        return mRef<int>(0x18);
    }
};

// 动画主要属性
struct AnimationMain : public PvzStruct {
public:
    // 动画偏移
    AnimationOffset* animationOffset()
    {
        return mPtr<AnimationOffset>(0x8);
    }
};

// 动画偏移属性
struct AnimationOffset : public PvzStruct {
public:
    // 动画内存数组
    Animation* animationArray()
    {
        return mPtr<Animation>(0x0);
    }
};

// 动画属性
struct Animation : public PvzStruct {
private:
    uint8_t data[0xa0];

public:
    // 动画循环率
    float& circulationRate()
    {
        return mRef<float>(0x4);
    }
};

struct TopMouseWindow : public PvzStruct {
public:
    // 窗口类型(1图鉴,2暂停,3是否,4商店等,6用户管理,8菜单)
    int& type()
    {
        return mRef<int>(0xc);
    }
};

struct MouseWindow : public PvzStruct {
public:
    TopMouseWindow* topWindow()
    {
        return mPtr<TopMouseWindow>(0x94);
    }

    bool& isInWindow()
    {
        return mRef<bool>(0xdc);
    }

    int& mouseAbscissa()
    {
        return mRef<int>(0xe0);
    }

    int& mouseOrdinate()
    {
        return mRef<int>(0xe4);
    }
};

struct SelectCardUi_m : public PvzStruct {
public:
    int& orizontalScreenOffset()
    {
        return mRef<int>(0x8);
    }

    bool& isCreatZombie()
    {
        return mRef<bool>(0x35);
    }
};

struct SelectCardUi_p : public PvzStruct {
public:
    LetsRockBtn* letsRockBtn()
    {
        return mPtr<LetsRockBtn>(0x88);
    }

    // 卡片移动状态
    // 参数：植物类型代号
    // 0移上卡槽
    // 1在卡槽里
    // 2移下卡槽
    // 3在选卡界面里
    int& cardMoveState(int index)
    {
        return mRef<int>(index * 0x3c + 0xc8);
    }
};

struct LetsRockBtn : public PvzStruct {
public:
    bool& isUnusable()
    {
        return mRef<bool>(0x1a);
    }
};

struct Mouse : public PvzStruct {
public:
    // 鼠标上物品的类型
    int& type()
    {
        return mRef<int>(0x30);
    }

    int& abscissa()
    {
        return mRef<int>(0x8);
    }

    uintptr_t& cannonAddress()
    {
        return mRef<uintptr_t>(0x40);
    }
};

struct MouseExtra : public PvzStruct {
public:
    int& row()
    {
        return mRef<int>(0x28);
    }
};

struct Text : public PvzStruct {
public:
    int& disappearCountdown()
    {
        return mRef<int>(0x88);
    }
};

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
    IMITATOR,       // 模仿者

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

// 兼容旧版的拼音

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
#define MFZ_48 IMITATOR         // 模仿者

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