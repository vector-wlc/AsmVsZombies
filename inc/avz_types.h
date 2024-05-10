/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-06 15:34:36
 * @Description:
 */

#ifndef __AVZ_TYPES_H__
#define __AVZ_TYPES_H__
#include <cstdint>
#include <functional>
#include <set>
#include <stack>
#include <string>
#include <vector>

using ARetVoidFuncPtr = void (*)();
using ARetBoolFunc = std::function<bool()>;
using ARetVoidFunc = std::function<void()>;
using APredication = ARetBoolFunc;
using AOperation = ARetVoidFunc;
using AKey = uint32_t;

template <typename Functor, typename Ret>
concept __ACheckRet = std::is_same_v<std::invoke_result_t<Functor>, Ret>;

template <typename T>
concept __AIsOperation = std::is_convertible_v<T, AOperation> && __ACheckRet<T, void>;

template <typename T>
concept __AIsPredication = std::is_convertible_v<T, APredication> && __ACheckRet<T, bool>;

#define __ADeprecated(...) [[deprecated("此功能已弃用, 请尽量不要使用此功能. " __VA_ARGS__)]]

#define __ANodiscard [[nodiscard("不要丢弃此函数的返回值")]]

#define __ADeleteCopyAndMove(ClassName)              \
public:                                              \
    ClassName(ClassName&&) = delete;                 \
    ClassName(const ClassName&) = delete;            \
    ClassName& operator=(ClassName&&) = delete;      \
    ClassName& operator=(const ClassName&) = delete; \
                                                     \
protected:

struct AGrid {
    int row;
    int col;

    auto operator<=>(const AGrid&) const = default;
};

struct APosition {
    int row;
    float col;
};

struct ATime {
    int time;
    int wave;

    ATime() = default;

    explicit ATime(int wave, int time)
        : time(time)
        , wave(wave)
    {
    }
};

struct APixel {
    int x;
    int y;
};

enum class APos { // 控制文本矩形框出现在顶点的哪个方位
    RIGHT_TOP,
    RIGHT_BUTTOM,
    LEFT_TOP,
    LEFT_BOTTOM,
};

struct ARect {
    int x;
    int y;
    int width;
    int height;
    APos pos;

    ARect() = default;

    explicit ARect(int x, int y, int width, int height, APos pos = APos::RIGHT_BUTTOM)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , pos(pos)
    {
    }
};

struct AText {
    std::string text;
    int x;
    int y;
    APos pos;
    bool isHasBkg;

    explicit AText(const std::string& text, int x, int y,
        APos pos = APos::RIGHT_BUTTOM, bool isHasBkg = true)
        : text(text)
        , x(x)
        , y(y)
        , pos(pos)
        , isHasBkg(true)
    {
    }

    explicit AText(std::string&& text, int x, int y,
        APos pos = APos::RIGHT_BUTTOM, bool isHasBkg = true)
        : text(std::move(text))
        , x(x)
        , y(y)
        , pos(pos)
        , isHasBkg(true)
    {
    }
};

struct ACursor {
    int x;
    int y;
    int type; // 0 为指针，1 为手形
    int pressType;
    ACursor() = default;
    explicit ACursor(int x, int y, int type = 0, int pressType = 0)
        : x(x)
        , y(y)
        , type(type)
        , pressType(pressType)
    {
    }
};

enum class AReloadMode {
    NONE,
    MAIN_UI,
    MAIN_UI_OR_FIGHT_UI,
};

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
    APEASHOOTER_ZOMBIE,      // 豌豆僵尸
    AWALL_NUT_ZOMBIE,        // 坚果僵尸
    AJALAPENO_ZOMBIE,        // 辣椒僵尸
    AGATLING_PEA_ZOMBIE,     // 机枪僵尸
    ASQUASH_ZOMBIE,          // 倭瓜僵尸
    ATALL_NUT_ZOMBIE,        // 高坚果僵尸
    AGIGA_GARGANTUAR,        // 红眼
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

#endif