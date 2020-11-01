#pragma once

#include <Windows.h>
#include <type_traits>
#include <cstdint>

struct PvZ;             // 游戏主体
struct MainObject;      // 主要对象
struct Plant;           // 植物
struct Zombie;          // 僵尸
struct Seed;            // 种子（卡片）
struct Item;            // 收集物
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

// 安全指针，防止访问野指针
template <class T>
class SafePtr
{
private:
    T *t;
    static T __t;

    int &game_ui()
    {
        return (int &)((uint8_t *)((*(void **)(0x6a9ec0))))[0x7fc];
    }

    template <class D>
    SafePtr<typename std::enable_if<std::is_base_of<T, D>::value, D>::type> safeptr_to_safeptr()
    {
        return SafePtr<D>((D *)t);
    }

    template <class D>
    typename std::enable_if<std::is_base_of<T, D>::value, D>::type *safeptr_to_unsafeptr()
    {
        return (D *)t;
    }

public:
    SafePtr(T *_t)
    {
        t = _t;
    }

    SafePtr()
    {
    }

    operator void *()
    {
        return t;
    }

    T *toUnsafe()
    {
        return t;
    }

    template <class D>
    operator SafePtr<D>()
    {
        return safeptr_to_safeptr<D>();
    }

    template <class D>
    operator D *()
    {
        return safeptr_to_unsafeptr<D>();
    }

    T *operator->()
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return t;
    }

    T &operator[](int index)
    {
        if (game_ui() == 1)
        {
            return __t;
        }
        return t[index];
    }

    SafePtr<T> operator+(int index)
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return t + index;
    }

    SafePtr<T> operator-(int index)
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return t - index;
    }

    SafePtr<T> operator++()
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return ++t;
    }

    SafePtr<T> operator++(int)
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return t++;
    }
    SafePtr<T> operator--()
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return --t;
    }

    SafePtr<T> operator--(int)
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return t--;
    }

    SafePtr<T> operator+=(int index)
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return t += index;
    }

    SafePtr<T> operator-=(int index)
    {
        if (game_ui() == 1)
        {
            return &__t;
        }
        return t -= index;
    }
};

template <typename T>
T SafePtr<T>::__t;

struct PvZ
{
private:
    void operator=(PvZ) {}

public:
    SafePtr<MainObject> mainObject()
    {
        return *(MainObject **)((uint8_t *)this + 0x768);
    }

    MouseWindow *mouseWindow()
    {
        return *(MouseWindow **)((uint8_t *)this + 0x320);
    }

    SelectCardUi_p *selectCardUi_p()
    {
        return *(SelectCardUi_p **)((uint8_t *)this + 0x774);
    }

    AnimationMain *animationMain()
    {
        return *(AnimationMain **)((uint8_t *)this + 0x820);
    }

    int &gameUi()
    {
        return (int &)((uint8_t *)this)[0x7fc];
    }

    int &tickMs()
    {
        return (int &)((uint8_t *)this)[0x454];
    }
};

struct MainObject
{
private:
    void operator=(MainObject) {}
    uint8_t data[0x57b0];

public:
    SafePtr<Zombie> zombieArray()
    {
        return *(Zombie **)((uint8_t *)this + 0x90);
    }
    int &zombieCountMax()
    {
        return (int &)((uint8_t *)this)[0x94];
    }
    int &zombieTotal()
    {
        return (int &)((uint8_t *)this)[0x94];
    }
    int &zombieLimit()
    {
        return (int &)((uint8_t *)this)[0x98];
    }
    int &zombieNext()
    {
        return (int &)((uint8_t *)this)[0x9c];
    }
    int &zombieCount()
    {
        return (int &)((uint8_t *)this)[0xa0];
    }
    int &zombieLast()
    {
        return (int &)((uint8_t *)this)[0xa4];
    }
    int &zombieRefreshHp()
    {
        return (int &)((uint8_t *)this)[0x5594];
    }
    SafePtr<Plant> plantArray()
    {
        return *(Plant **)((uint8_t *)this + 0xac);
    }
    int &plantCountMax()
    {
        return (int &)((uint8_t *)this)[0xb0];
    }
    int &plantTotal()
    {
        return (int &)((uint8_t *)this)[0xb0];
    }
    int &plantLimit()
    {
        return (int &)((uint8_t *)this)[0xb4];
    }
    int &plantNext()
    {
        return (int &)((uint8_t *)this)[0xb8];
    }
    int &plantCount()
    {
        return (int &)((uint8_t *)this)[0xbc];
    }
    int &plantLast()
    {
        return (int &)((uint8_t *)this)[0xc0];
    }

    SafePtr<Seed> seedArray()
    {
        return *(Seed **)((uint8_t *)this + 0x144);
    }

    SafePtr<Item> itemArray()
    {
        return *(Item **)((uint8_t *)this + 0xe4);
    }
    int &itemCountMax()
    {
        return (int &)((uint8_t *)this)[0xe8];
    }
    int &itemTotal()
    {
        return (int &)((uint8_t *)this)[0xe8];
    }
    bool &gamePaused()
    {
        return (bool &)((uint8_t *)this)[0x164];
    }
    int &scene()
    {
        return (int &)((uint8_t *)this)[0x554c];
    }

    int &sun()
    {
        return (int &)((uint8_t *)this)[0x5560];
    }
    int &gameClock()
    {
        return (int &)((uint8_t *)this)[0x5568];
    }
    int &globalClock()
    {
        return (int &)((uint8_t *)this)[0x556c];
    }
    int &refreshCountdown()
    {
        return (int &)((uint8_t *)this)[0x559c];
    }
    int &hugeWaveCountdown()
    {
        return (int &)((uint8_t *)this)[0x55a4];
    }
    int &wave()
    {
        return (int &)((uint8_t *)this)[0x557c];
    }
    int &initialCountdown()
    {
        return (int &)((uint8_t *)this)[0x55a0];
    }

    int &clickPaoCountdown()
    {
        return (int &)((uint8_t *)this)[0x5754];
    }

    Mouse *mouseAttribution()
    {
        return *(Mouse **)((uint8_t *)this + 0x138);
    }

    MouseExtra *mouseExtraAttribution()
    {
        return *(MouseExtra **)((uint8_t *)this + 0x13c);
    }

    SelectCardUi_m *selectCardUi_m()
    {
        return *(SelectCardUi_m **)((uint8_t *)this + 0x15c);
    }

    uint32_t *zombieList()
    {
        return (uint32_t *)((uint8_t *)this + 0x6b4);
    }

    Text *text()
    {
        return *(Text **)((uint8_t *)this + 0x140);
    }
};

struct Plant
{
private:
    void operator=(Plant) {}

public:
    uint8_t data[0x14c];

    int &xi()
    {
        return (int &)((uint8_t *)this)[0x8];
    }
    int &yi()
    {
        return (int &)((uint8_t *)this)[0xc];
    }

    int &visible()
    {
        return (int &)((uint8_t *)this)[0x18];
    }
    int &row()
    {
        return (int &)((uint8_t *)this)[0x1c];
    }
    int &type()
    {
        return (int &)((uint8_t *)this)[0x24];
    }
    int &col()
    {
        return (int &)((uint8_t *)this)[0x28];
    }

    //返回植物的状态
    //35：空炮
    //36：正在装填
    //37：准备就绪
    //38：正在发射
    int &state()
    {
        return (int &)((uint8_t *)this)[0x3c];
    }
    int &stateCountdown()
    {
        return (int &)((uint8_t *)this)[0x54];
    }
    int &hp()
    {
        return (int &)((uint8_t *)this)[0x40];
    }
    int &hpMax()
    {
        return (int &)((uint8_t *)this)[0x44];
    }
    int &canShoot()
    {
        return (int &)((uint8_t *)this)[0x48];
    }
    int &bloverCountdown()
    {
        return (int &)((uint8_t *)this)[0x4c];
    }
    int &explodeCountdown()
    {
        return (int &)((uint8_t *)this)[0x50];
    }
    int &shootCountdown()
    {
        return (int &)((uint8_t *)this)[0x90];
    }
    int &mushroomCountdown()
    {
        return (int &)((uint8_t *)this)[0x130];
    }
    bool &isDisappeared()
    {
        return (bool &)((uint8_t *)this)[0x141];
    }
    bool &isCrushed()
    {
        return (bool &)((uint8_t *)this)[0x142];
    }
    bool &sleeping()
    {
        return (bool &)((uint8_t *)this)[0x143];
    }
    // 动作编号
    uint16_t &animationCode()
    {
        return (uint16_t &)((uint8_t *)this)[0x94];
    }
};

struct Zombie
{
private:
    void operator=(Zombie) {}

public:
    uint8_t data[0x15c];

    bool isExist()
    {
        return (short)((uint8_t *)this)[0x15a];
    }
    int &row()
    {
        return (int &)((uint8_t *)this)[0x1c];
    }

    float &abscissa()
    {
        return (float &)((uint8_t *)this)[0x2c];
    }

    float &ordinate()
    {
        return (float &)((uint8_t *)this)[0x30];
    }

    int &type()
    {
        return (int &)((uint8_t *)this)[0x24];
    }

    int &hp()
    {
        return (int &)((uint8_t *)this)[0xC8];
    }

    int &oneHp()
    {
        return (int &)((uint8_t *)this)[0xD0];
    }

    int &twoHp()
    {
        return (int &)((uint8_t *)this)[0xDC];
    }

    bool &isEat()
    {
        return (bool &)((uint8_t *)this)[0x51];
    }

    int &state()
    {
        return (int &)((uint8_t *)this)[0x28];
    }

    // 返回选卡界面僵尸站立状态
    // 为-2/-3时静止,-4时向上(对于选卡界面的僵尸)
    int &standState()
    {
        return (int &)((uint8_t *)this)[0x6c];
    }

    bool isDead()
    {
        return state() == 1;
    }

    bool isHammering()
    {
        return state() == 70;
    }
    bool &isStealth()
    {
        return (bool &)((uint8_t *)this)[0x18];
    }

    float &speed()
    {
        return (float &)((uint8_t *)this)[0x34];
    }
    int &existTime()
    {
        return (int &)((uint8_t *)this)[0x60];
    }

    int &stateCountdown()
    {
        return (int &)((uint8_t *)this)[0x68];
    }

    bool &isDisappeared()
    {
        return (bool &)((uint8_t *)this)[0xEC];
    }

    int &bulletAbscissa()
    {
        return (int &)((uint8_t *)this)[0x8C];
    }
    int &bulletOrdinate()
    {
        return (int &)((uint8_t *)this)[0x90];
    }

    int &attackAbscissa()
    {
        return (int &)((uint8_t *)this)[0x9C];
    }

    int &attackOrdinate()
    {
        return (int &)((uint8_t *)this)[0xA0];
    }

    int &slowCountdown()
    {
        return (int &)((uint8_t *)this)[0xAC];
    }

    int &fixationCountdown()
    {
        return (int &)((uint8_t *)this)[0xB0];
    }

    int &freezeCountdown()
    {
        return (int &)((uint8_t *)this)[0xB4];
    }
};

struct Seed
{
private:
    void operator=(Seed) {}

public:
    uint8_t data[0x50];

    int &count()
    {
        return (int &)((uint8_t *)this)[0x24];
    }

    bool &isUsable()
    {
        return (bool &)((uint8_t *)this)[0x70];
    }

    int &cd()
    {
        return (int &)((uint8_t *)this)[0x4c];
    }

    int &initialCd()
    {
        return (int &)((uint8_t *)this)[0x50];
    }

    int &imitatorType()
    {
        return (int &)((uint8_t *)this)[0x60];
    }

    int &type()
    {
        return (int &)((uint8_t *)this)[0x5c];
    }
};

struct Item
{
private:
    void operator=(Item) {}

public:
    uint8_t data[0xd8];

    bool &isDisappeared()
    {
        return (bool &)((uint8_t *)this)[0x38];
    }

    bool &is_collected()
    {
        return (bool &)((uint8_t *)this)[0x50];
    }

    float &abscissa()
    {
        return (float &)((uint8_t *)this)[0x24];
    }

    float &ordinate()
    {
        return (float &)((uint8_t *)this)[0x28];
    }
};

struct AnimationMain
{
private:
    void operator=(AnimationMain) {}

public:
    AnimationOffset *animationOffset()
    {
        return *(AnimationOffset **)((uint8_t *)this + 0x8);
    }
};

struct AnimationOffset
{
private:
    void operator=(AnimationOffset) {}

public:
    Animation *animationArray()
    {
        return *(Animation **)((uint8_t *)this + 0x0);
    }
};

struct Animation
{
private:
    void operator=(Animation) {}

public:
    uint8_t data[0xa0];

    // 动画循环率
    float &circulationRate()
    {
        return (float &)((uint8_t *)this)[0x4];
    }
};

struct TopMouseWindow
{
private:
    void operator=(TopMouseWindow) {}

public:
    // 窗口类型(1图鉴,2暂停,3是否,4商店等,6用户管理,8菜单)
    int &type()
    {
        return (int &)((uint8_t *)this)[0xc];
    }
};

struct MouseWindow
{
private:
    void operator=(MouseWindow) {}

public:
    TopMouseWindow *topWindow()
    {
        return *(TopMouseWindow **)((uint8_t *)this + 0x94);
    }

    bool &isInWindow()
    {
        return (bool &)((uint8_t *)this)[0xdc];
    }
};

struct SelectCardUi_m
{
private:
    void operator=(SelectCardUi_m) {}

public:
    int &orizontalScreenOffset()
    {
        return (int &)((uint8_t *)this)[0x8];
    }

    bool &isCreatZombie()
    {
        return (bool &)((uint8_t *)this)[0x35];
    }
};

struct SelectCardUi_p
{
private:
    void operator=(SelectCardUi_p) {}

public:
    LetsRockBtn *letsRockBtn()
    {
        return *(LetsRockBtn **)((uint8_t *)this + 0x88);
    }

    // 卡片移动状态
    // 参数：植物类型代号
    // 0移上卡槽
    // 1在卡槽里
    // 2移下卡槽
    // 3在选卡界面里
    int &cardMoveState(int index)
    {
        return (int &)((uint8_t *)this)[index * 0x3c + 0xc8];
    }
};

struct LetsRockBtn
{
private:
    void operator=(LetsRockBtn) {}

public:
    bool &isUnusable()
    {
        return (bool &)((uint8_t *)this)[0x1a];
    }
};

struct Mouse
{
private:
    void operator=(Mouse) {}

public:
    // 鼠标上物品的类型
    int &type()
    {
        return (int &)((uint8_t *)this)[0x30];
    }

    int &abscissa()
    {
        return (int &)((uint8_t *)this)[0x8];
    }

    uintptr_t &cannonAddress()
    {
        return (uintptr_t &)((uint8_t *)this)[0x40];
    }
};

struct MouseExtra
{
private:
    void operator=(MouseExtra) {}

public:
    int &row()
    {
        return (int &)((uint8_t *)this)[0x28];
    }
};

struct Text
{
private:
    void operator=(Text) {}

public:
    int &disappearCountdown()
    {
        return (int &)((uint8_t *)this)[0x88];
    }
};

// 植物类型
enum PlantType
{
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

enum ZombieType
{
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