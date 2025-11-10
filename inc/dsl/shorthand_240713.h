#ifndef __ADSL_SHORTHAND__
#define __ADSL_SHORTHAND__

#define __DSL_SHORTHAND_VERSION__ 240713

#include "main.h"

using namespace ALiterals;

class {
public:
    operator ATime() const {
        return ANowTime(true);
    }
} inline constexpr now;

/*
OnWave(1) At(341_cs) Trig(); // 把第 1 波的波长设为 601cs
OnWave(2) At(1538_cs) Trig(true); // 强制第 2 波 在 1538cs 时激活，相当于波长 1738cs
第 9、19、20 波的 Trig() 会被忽略
*/
inline ATimeline Trig(bool force = false) {
    return [=](ATime time) {
        if (time.wave % 10 == 9 || time.wave == AGetMainObject()->TotalWave())
            return;
        time.time = std::max(time.time + 200, 601);
        force ? ASetWavelength({time}) : AAssumeWavelength({time});
    };
}

#define TrigAt(...) (__VA_ARGS__) <<= Trig() & (__ADSLCastHelper)

// Repeat(op, times, interval) // 重复执行 op，间隔 interval，执行 times 次
// Repeat(PP(), 3, 1000) 等效于 {At(0) PP(), At(1000) PP(), At(2000) PP()}
inline ATimeline Repeat(ATimeline op, int times, ATimeOffset interval) {
    ATimeline result;
    for (int i = 0; i < times; ++i)
        result &= op + i * interval;
    return result;
}

class ARoofCobManager : public ACobManager {
protected:
    std::vector<int> _columns;

    void _BeforeScript() override {
        ACobManager::_BeforeScript();
        RefreshCobList();
    }

    using ACobManager::AutoGetList;
    using ACobManager::AutoSetList;

public:
    ARoofCobManager(const std::vector<int>& columns)
        : ACobManager(PRIORITY), _columns(columns) {}

    ARoofCobManager(std::convertible_to<int> auto... column)
        requires(sizeof...(column) > 0)
        : ACobManager(PRIORITY), _columns {column...} {}

    void SetColumns(const std::vector<int>& columns) {
        _columns = columns;
        RefreshCobList();
    }

    void RefreshCobList() {
        std::vector<int> cobs[9];
        for (auto& plant : aAlivePlantFilter)
            if (plant.Type() == ACOB_CANNON)
                cobs[plant.Col()].push_back(plant.Row());
        for (int i = 0; i < 9; ++i)
            std::ranges::sort(cobs[i]);
        std::vector<AGrid> cobList;
        for (int col : _columns)
            for (int row : cobs[col - 1])
                cobList.push_back({row + 1, col});
        SetList(cobList);
    }
} inline slope(1, 2, 3, 4, 5, 6, 7, 8), flat(8, 7, 6, 5, 4, 3, 2, 1);

/*
RP(1, 1, 3, 9) // 发射炮尾位于 1-1 的炮，落点为 3-9
*/
inline ATimeline RP(int cobRow, int cobCol, int row, float col) {
    if (aFieldInfo.isRoof)
        return At(-387_cs) Do {
            ACobManager::RawRoofFire(cobRow, cobCol, row, col);
        };
    else if (aFieldInfo.rowType[row] == ARowType::POOL)
        return At(-378_cs) Do {
            ACobManager::RawFire(cobRow, cobCol, row, col);
        };
    else
        return At(-373_cs) Do {
            ACobManager::RawFire(cobRow, cobCol, row, col);
        };
}

inline std::vector<APosition> __ParseRow(const std::vector<APosition>& positions) {
    std::vector<APosition> ret;
    for (auto [row, col] : positions) {
        for (char r : std::to_string(row)) {
            if (r < '1' || r > '0' + aFieldInfo.nRows) {
                aLogger->Error(std::format("ParseRow: 输入的行数 {} 不合法", row));
                return {};
            }
            ret.push_back({r - '0', col});
        }
    }
    return ret;
}

inline std::vector<APosition> __ParseRow(int row, float col) {
    return __ParseRow({{row, col}});
}

enum AFirePolicy {
    INSTANT_FIRE,
    RECOVER_FIRE
};

/*
P(1, 8.75) // 炮炸 1-8.75
P(1256, 9) // 炮炸 1-9、2-9、5-9、6-9
P<RECOVER_FIRE>(1256, 9) // 等待炮恢复后炸 1-9、2-9、5-9、6-9
PP(15, 9, 26, 8) // 炮炸 1-9、5-9、2-8、6-8
P(slope, 2, 9) + P(flat, 4, 9) // 风炮炸 2-9，平炮炸 4-9
P(slope, 2, 9, flat, 4, 9) // 同上
*/
template <AFirePolicy policy = INSTANT_FIRE>
inline ATimeline P(ACobManager& cm, int row, float col) {
    ATimeline ret;
    for (auto pos : __ParseRow(row, col)) {
        int r = pos.row;
        float c = pos.col;
        ATimeOffset offset;
        if (aFieldInfo.isRoof)
            offset = -387;
        else
            offset = (aFieldInfo.rowType[r] == ARowType::POOL ? -378 : -373);
        ret &= At(offset)[=, &cm] {
            if constexpr (policy == INSTANT_FIRE)
                aFieldInfo.isRoof ? cm.RoofFire(r, c) : cm.Fire(r, c);
            else
                aFieldInfo.isRoof ? cm.RecoverRoofFire(r, c) : cm.RecoverFire(r, c);
        };
    }
    return ret;
}

template <AFirePolicy policy = INSTANT_FIRE>
inline ATimeline P(ACobManager& cm, int row, float col, ACobManager& cm2, auto&&... args) {
    return P<policy>(cm, row, col) & P<policy>(cm2, std::forward<decltype(args)>(args)...);
}

template <AFirePolicy policy = INSTANT_FIRE>
inline ATimeline P(ACobManager& cm, int row, float col, int row2, float col2, auto&&... args) {
    return P<policy>(cm, row, col) & P<policy>(cm, row2, col2, std::forward<decltype(args)>(args)...);
}

template <AFirePolicy policy = INSTANT_FIRE>
inline ATimeline P(int row, float col, auto&&... args) {
    return P<policy>(aCobManager, row, col, std::forward<decltype(args)>(args)...);
}

/*
PP(8) // 在五行场地炸 2-8、4-8；在六行场地炸 2-8、5-8
PP() // 等效于 PP(9)
PP<RECOVER_FIRE>() // 等效于 P<RECOVER_FIRE>(24 或 25, 9)
*/
template <AFirePolicy policy = INSTANT_FIRE>
inline ATimeline PP(float col = 9) {
    return P<policy>(aFieldInfo.nRows == 5 ? 24 : 25, col);
}

/*
D<110>(1, 8.75) // 等效于 At(110) P(1, 8.75)（注意 <> 里的数值必须是编译时常量，不能传一个变量的值）
D 支持的参数和 P 相同，但不包含 RECOVER_FIRE（指定延迟和自动发炮本身就是冲突的）
*/
template <ATimeOffset delay = 0>
inline ATimeline D(auto&&... args) {
    return P(std::forward<decltype(args)>(args)...) + delay;
}

/*
DD<110>(8.75) // 在六行场地炸 1、5 路；在五行场地炸 1、4 路
*/
template <ATimeOffset delay = 0>
inline ATimeline DD(float col) {
    return P(aFieldInfo.nRows == 5 ? 14 : 15, col) + delay;
}

inline APlant* __CardInstant(APlantType seed, const std::vector<APosition>& positions, int delay = 0) {
    for (auto pos : positions) {
        int row = pos.row;
        float col = pos.col;
        int rejectType = AAsm::GetPlantRejectType(seed % AM_PEASHOOTER, row - 1, int(col - 0.5));
        if (!ARangeIn(rejectType, {AAsm::NIL, AAsm::NEEDS_POT, AAsm::NOT_ON_WATER}))
            continue;
        APlant* container = nullptr;
        if (rejectType == AAsm::NEEDS_POT)
            container = ACard(AFLOWER_POT, row, col);
        else if (rejectType == AAsm::NOT_ON_WATER)
            container = ACard(ALILY_PAD, row, col);
        APlant* plant = ACard(seed, row, col);
        if (delay > 0 && (container || plant))
            At(now + delay) Do {
                if (plant)
                    AShovel(row, col, seed);
                if (container)
                    AShovel(row, col, container->Type());
            };
        if (plant)
            return plant;
    }
    return nullptr;
}

/*
Card(ASPIKEWEED, 1, 9) // 在 1-9 种地刺
Card 与 ACard 用法相同，但 Card 会自动补种荷叶和花盆
*/
template <ATimeOffset delay = 0>
inline ATimeline Card(APlantType seed, int row, float col) {
    return Do {
        __CardInstant(seed, __ParseRow(row, col), delay.time);
    };
}

template <ATimeOffset delay = 0>
inline ATimeline Card(const std::vector<ACardName>& cards) {
    return Do {
        for (auto [seed, row, col] : cards)
            __CardInstant(seed, __ParseRow(row, col), delay.time);
    };
}

template <ATimeOffset delay = 0>
inline ATimeline Card(const std::vector<APlantType>& seeds, int row, float col) {
    std::vector<ACardName> cards;
    for (auto seed : seeds)
        cards.push_back({seed, row, col});
    return Card<delay>(cards);
}

template <ATimeOffset delay = 0>
inline ATimeline Card(APlantType seed, const std::vector<APosition>& positions) {
    return Do {
        __CardInstant(seed, __ParseRow(positions), delay.time);
    };
}

template <ATimeOffset delay = 0>
inline ATimeline Card(const std::vector<APlantType>& seeds, const std::vector<APosition>& positions) {
    return Do {
        for (auto seed : seeds)
            __CardInstant(seed, __ParseRow(positions), delay.time);
    };
}

/*
Shovel(1, 9) // 铲 1-9 的普通植物
Shovel(1, 9, APUMPKIN) // 铲 1-9 的南瓜（没有则不铲除）
*/
inline ATimeline Shovel(int row, int col, int targetType = -1) {
    return Do {
        for (auto [r, c] : __ParseRow(row, col))
            AShovel(r, c, targetType);
    };
}

inline ATimeline Shovel(int row, int col, bool pumpkin) {
    return Do {
        for (auto [r, c] : __ParseRow(row, col))
            AShovel(r, c, pumpkin);
    };
}

inline ATimeline Shovel(const std::vector<AShovelPosition>& positions) {
    return Do {
        for (auto [row, col, type] : positions)
            for (auto [r, c] : __ParseRow(row, col))
                AShovel(r, c, type);
    };
}

/*
A(2, 9) // 在 2-9 使用樱桃（与 Card 相比附加了 -100cs 的偏移，相当于以生效时间为基准；下同）
*/
inline ATimeline A(int row, float col) {
    return At(-100_cs) Card(ACHERRY_BOMB, row, col);
}

/*
J(2, 9) // 使用辣椒
*/
inline ATimeline J(int row, float col) {
    return At(-100_cs) Card(AJALAPENO, row, col);
}

/*
W(2, 9) // 使用窝瓜
*/
inline ATimeline W(int row, float col) {
    return At(-182_cs) Card(ASQUASH, row, col);
}

inline ATimeline __UseMushroomDay(APlantType type, int row, float col, bool tryImitator) {
    APlantType imitatorType = APlantType(type + AM_PEASHOOTER);
    if (!tryImitator)
        return At(-299_cs) Do {
            if (!AGetPlantPtr(row, col, type))
                __CardInstant(type, {{row, col}});
            ACard(ACOFFEE_BEAN, row, col);
            ASetPlantActiveTime(type, 299);
        };
    else
        return At(-619_cs) Do {
            if (AIsSeedUsable(imitatorType)) {
                __CardInstant(imitatorType, {{row, col}});
                ASetPlantActiveTime(type, 619);
                At(now + 320_cs) Card(ACOFFEE_BEAN, row, col);
            } else
                At(now + 619_cs) __UseMushroomDay(type, row, col, false);
        };
}

inline ATimeline __UseMushroomNight(APlantType type, int row, float col, bool tryImitator) {
    APlantType imitatorType = APlantType(type + AM_PEASHOOTER);
    if (!tryImitator)
        return At(-100_cs) Card(type, row, col);
    else
        return At(-420_cs) Do {
            if (AIsSeedUsable(imitatorType)) {
                __CardInstant(imitatorType, {{row, col}});
                ASetPlantActiveTime(type, 420);
            } else
                At(now + 420_cs) __UseMushroomNight(type, row, col, false);
        };
}

/*
N(2, 9) // 使用毁灭菇（自动使用咖啡豆，自动校正生效时间；下同）
N(3, 9, true) // 优先使用模仿者卡片，其次使用原版卡片
N({{3, 8}, {3, 9}, {4, 9}}) // 从位置列表中挑选第一个可用位置使用
*/
inline ATimeline N(int row, float col, bool tryImitator = false) {
    if (aFieldInfo.isNight)
        return __UseMushroomNight(ADOOM_SHROOM, row, col, tryImitator);
    else
        return __UseMushroomDay(ADOOM_SHROOM, row, col, tryImitator);
}

inline ATimeline N(const std::vector<APosition>& positions, bool tryImitator = false) {
    ATimeOffset offset = aFieldInfo.isNight ? -100_cs : -299_cs;
    if (tryImitator)
        offset -= 320_cs;
    return At(offset) Do {
        for (auto [row, col] : positions) {
            if (ARangeIn(AAsm::GetPlantRejectType(ADOOM_SHROOM, row - 1, col - 1),
                    {AAsm::NIL, AAsm::NOT_ON_WATER, AAsm::NEEDS_POT})) {
                At(now - offset) N(row, col, tryImitator);
                break;
            }
        }
    };
}

/*
I(1, 1) // 优先使用模仿者卡片，其次使用原版卡片
I(1, 1, false) // 只尝试使用原版卡片
I() // 仅限白天：使用 aIceFiller 中的存冰
*/
inline ATimeline I(int row, float col, bool tryImitator = true) {
    if (aFieldInfo.isNight)
        return __UseMushroomNight(AICE_SHROOM, row, col, tryImitator);
    else
        return __UseMushroomDay(AICE_SHROOM, row, col, tryImitator);
}

inline ATimeline I(AIceFiller& if_ = aIceFiller) {
    if (aFieldInfo.isNight) {
        aLogger->Error("I: 需要指定放置寒冰菇的位置");
        return {};
    }
    return At(-299_cs)[=, &if_] {
        if_.Coffee();
        ASetPlantActiveTime(AICE_SHROOM, 299);
    };
}

/*
C.SetCards({APUFF_SHROOM, AM_PUFF_SHROOM}); // 设置使用的垫材；不设置时默认为所有 751cs 冷却的植物按阳光从低到高排序（这个函数是即时生效的）
C(134) // 在所有陆地行 9 列放置垫材，134cs 后铲除
C(-1, 56) // 在 5、6 行 9 列放置垫材，不主动铲除
C(1, {1256, 8}) // 在 1、2、5、6 行 8 列放置垫材，秒铲
C(266, {{1, 9}, {256, 8}}) // 在 1-9、2-8、5-8、6-8 放置垫材，266cs 后铲除
C.TriggerBy(ADANCING_ZOMBIE & CURR_WAVE, AGIGA_GARGANTUAR & PREV_WAVES)(40) // 只在有本波舞王或非本波红眼的行放置垫材
C.TriggerBy(ALADDER_ZOMBIE, AJACK_IN_THE_BOX_ZOMBIE & XIn(600, 700))(266) // 只在有梯子或横坐标位于 600~700 之间的小丑的行放置垫材
*/
class AFodder : protected AStateHook {
protected:
    std::vector<APlantType> _seeds;
    bool _manuallyInitialized = false;

    virtual void _EnterFight() override {
        if (!_manuallyInitialized)
            AutoSetCards();
    }

    static std::vector<APosition> _ParseRow(const std::vector<APosition>& positions) {
        std::vector<APosition> ret;
        for (auto [row, col] : positions)
            for (char r : std::to_string(row)) {
                if (r < '1' || r > '0' + aFieldInfo.nRows) {
                    aLogger->Error(std::format("AFodder: 输入的行数 {} 不合法", row));
                    return {};
                }
                ret.push_back({r - '0', col});
            }
        return ret;
    }

    void _Fodder(int removalDelay, const std::vector<APosition>& positions) const {
        std::string log = "AFodder: 垫";
        for (auto [row, col] : positions)
            log += std::format(" {}-{}", row, col);
        aLogger->Info(log);

        auto currentSeed = _seeds.begin();
        std::vector<AShovelPosition> shovelTargets;
        for (auto [row, col] : positions) {
            while (currentSeed != _seeds.end() && !AIsSeedUsable(*currentSeed))
                ++currentSeed;
            if (currentSeed == _seeds.end()) {
                int cd = 9999;
                for (auto& seedType : _seeds)
                    if (ASeed* seed = AGetSeedPtr(seedType))
                        cd = std::min(cd, seed->InitialCd() - seed->Cd() + 1);
                aLogger->Error(std::format("AFodder: 距下一个垫材可用还有 {} cs", cd));
                break;
            }
            ACard(*currentSeed, row, col);
            shovelTargets.push_back({row, col, *currentSeed});
            ++currentSeed;
        }

        if (removalDelay <= 0)
            return;
        At(now + removalDelay) Do {
            for (auto [row, col, type] : shovelTargets) {
                if (!AGetPlantPtr(row, col, type % AM_PEASHOOTER) && !AGetPlantPtr(row, col, std::min(type, AIMITATOR + 0)))
                    continue;
                AShovel(row, col, type);
                for (auto type2 : {ALILY_PAD, APUMPKIN, AFLOWER_POT})
                    AShovel(row, col, type2);
            }
        };
    }

public:
    class Constraint {
    private:
        std::vector<std::function<bool(AZombie*)>> _preds;

    public:
        template <typename Pred>
            requires(std::is_invocable_r_v<bool, Pred, AZombie*> && !std::is_same_v<std::remove_cvref_t<Pred>, Constraint>)
        Constraint(Pred&& pred)
            : _preds {std::forward<Pred>(pred)} {}

        Constraint(AZombieType type)
            : _preds {[=](AZombie* zombie) { return zombie->Type() == type; }} {
        }

        Constraint& operator&=(const Constraint& rhs) {
            _preds.insert(_preds.end(), rhs._preds.begin(), rhs._preds.end());
            return *this;
        }

        friend Constraint operator&(Constraint lhs, const Constraint& rhs) {
            return lhs &= rhs;
        }

        bool operator()(AZombie* zombie) const {
            for (auto& pred : _preds)
                if (!pred(zombie))
                    return false;
            return true;
        }
    };

    class [[nodiscard]] TriggerByProxy {
        friend class AFodder;

    protected:
        const AFodder* _fodder;
        std::vector<Constraint> _constraints;

        TriggerByProxy(const AFodder* fodder, const std::vector<Constraint>& constraints)
            : _fodder(fodder), _constraints(constraints) {
        }

        std::set<int> _GetTriggeredRows() const {
            std::set<int> triggeredRows;
            for (auto& zombie : aAliveZombieFilter) {
                if (triggeredRows.contains(zombie.Row() + 1))
                    continue;
                for (auto& constraint : _constraints)
                    if (constraint(&zombie)) {
                        triggeredRows.insert(zombie.Row() + 1);
                        break;
                    }
            }
            return triggeredRows;
        }

    public:
        ATimeline operator()(int removalDelay, const std::vector<APosition>& positions) const {
            return [=, *this, positions = _ParseRow(positions)] {
                std::set<int> triggeredRows = _GetTriggeredRows();
                std::vector<APosition> triggeredPositions;
                for (auto [row, col] : positions)
                    if (triggeredRows.contains(row))
                        triggeredPositions.push_back({row, col});
                _fodder->_Fodder(removalDelay, triggeredPositions);
            };
        }

        ATimeline operator()(int removalDelay, APosition position) const {
            return operator()(removalDelay, std::vector<APosition> {position});
        }

        ATimeline operator()(int removalDelay, int row) const {
            return operator()(removalDelay, {row, 9});
        }

        ATimeline operator()(int removalDelay) const {
            std::vector<APosition> positions;
            for (int row = 1; row <= aFieldInfo.nRows; ++row)
                if (aFieldInfo.rowType[row] == ARowType::LAND)
                    positions.push_back({row, 9});
            return operator()(removalDelay, positions);
        }
    };

    AFodder() = default;

    AFodder(const std::vector<APlantType>& seeds)
        : _seeds(seeds), _manuallyInitialized(true) {}

    AFodder(std::convertible_to<int> auto... seed)
        requires(sizeof...(seed) > 0)
        : _seeds {seed...}, _manuallyInitialized(true) {}

    void SetCards(const std::vector<APlantType>& seeds) {
        _seeds = seeds;
        _manuallyInitialized = true;
    }

    void AutoSetCards() {
        _seeds.clear();
        std::vector<std::pair<int, int>> candidates;
        for (auto& seed : ABasicFilter<ASeed>()) {
            int seedType = (seed.Type() == AIMITATOR ? seed.ImitatorType() : seed.Type());
            if (ARangeIn(seedType, {AGRAVE_BUSTER, ALILY_PAD, ACOFFEE_BEAN}))
                continue;
            int cd = AMRef<int>(0x69f2c4 + 0x24 * seedType);
            int cost = AMRef<int>(0x69f2c0 + 0x24 * seedType);
            if (cd == 750) {
                int type = seedType + (seed.Type() == AIMITATOR ? AM_PEASHOOTER : 0);
                candidates.push_back({cost, type});
            }
        }
        std::ranges::sort(candidates);
        for (auto [cost, seed_type] : candidates)
            _seeds.push_back(APlantType(seed_type));
    }

    void EraseFromList(const std::vector<APlantType>& seeds) {
        _seeds = __AErase(_seeds, seeds);
    }

    void EraseFromList(APlantType seed) {
        EraseFromList(std::vector<APlantType> {seed});
    }

    void MoveToListTop(const std::vector<APlantType>& seeds) {
        _seeds = __AMoveToTop(_seeds, seeds);
    }

    void MoveToListTop(APlantType seed) {
        MoveToListTop(std::vector<APlantType> {seed});
    }

    void MoveToListBottom(const std::vector<APlantType>& seeds) {
        _seeds = __AMoveToBottom(_seeds, seeds);
    }

    void MoveToListBottom(APlantType seed) {
        MoveToListBottom(std::vector<APlantType> {seed});
    }

    TriggerByProxy TriggerBy(const std::vector<Constraint>& constraints) const {
        return {this, constraints};
    }

    TriggerByProxy TriggerBy(std::convertible_to<Constraint> auto&&... constraints) const
        requires(sizeof...(constraints) > 0)
    {
        return TriggerBy(std::vector<Constraint> {constraints...});
    }

    // 返回 maxCd cs 之内可用的垫材数量
    int AvailableCount(int maxCd = 0) const {
        int cnt = 0;
        for (auto& seedType : _seeds)
            if (ASeed* seed = AGetSeedPtr(seedType))
                cnt += (seed->IsUsable() || seed->InitialCd() - seed->Cd() + 1 <= maxCd);
        return cnt;
    }

    ATimeline operator()(int removalDelay, const std::vector<APosition>& positions) const {
        return [=, this, positions = _ParseRow(positions)] { _Fodder(removalDelay, positions); };
    }

    ATimeline operator()(int removalDelay, APosition position) const {
        return operator()(removalDelay, std::vector<APosition> {position});
    }

    ATimeline operator()(int removalDelay, int row) const {
        return operator()(removalDelay, {row, 9});
    }

    ATimeline operator()(int removalDelay) const {
        std::vector<APosition> positions;
        for (int row = 1; row <= aFieldInfo.nRows; ++row)
            if (aFieldInfo.rowType[row] == ARowType::LAND)
                positions.push_back({row, 9});
        return operator()(removalDelay, positions);
    }
} inline C;

inline const AFodder::Constraint CURR_WAVE = [](AZombie* zombie) {
    return zombie->AtWave() + 1 == ANowWave();
};

inline const AFodder::Constraint PREV_WAVES = [](AZombie* zombie) {
    return zombie->AtWave() + 1 < ANowWave();
};

inline AFodder::Constraint WaveIn(const std::set<int>& waves) {
    return [=](AZombie* zombie) {
        return waves.contains(zombie->AtWave() + 1 - ANowWave());
    };
}

inline AFodder::Constraint WaveIn(const std::vector<int>& waves) {
    return WaveIn(std::set<int>(waves.begin(), waves.end()));
}

inline AFodder::Constraint WaveIn(std::convertible_to<int> auto... waves)
    requires(sizeof...(waves) > 0)
{
    return WaveIn(std::set<int> {waves...});
}

inline AFodder::Constraint AbscIn(int l, int r) {
    return [=](AZombie* zombie) {
        return l <= int(zombie->Abscissa()) && int(zombie->Abscissa()) <= r;
    };
}

inline std::vector<int> __GetPossibleRows(int type) {
    std::vector<int> lands, pools, all;
    for (int i = 0; i < aFieldInfo.nRows; ++i) {
        all.push_back(i);
        if (aFieldInfo.rowType[i + 1] == ARowType::POOL)
            pools.push_back(i);
        else
            lands.push_back(i);
    }
    if (pools.empty())
        pools.push_back(5); // 对于无水路的场地，第 6 行视作水路
    if (ARangeIn(type, {ASNORKEL_ZOMBIE, ADOLPHIN_RIDER_ZOMBIE}))
        // 水路僵尸只能出现在泳池
        return pools;
    else if (type == ADANCING_ZOMBIE) {
        if (aFieldInfo.hasPool && aFieldInfo.rowHeight == 85)
            // 对于 PE/FE，舞王可以出现在所有陆地行
            return lands;
        else {
            // 对于其他场地，舞王可以出现在非边路的陆地行
            std::vector<int> ret;
            for (int row : lands)
                if (row != 0 && row != aFieldInfo.nRows - 1)
                    ret.push_back(row);
            return ret;
        }
    } else if (aFieldInfo.hasPool && ANowWave(false) > 5 &&
        ARangeIn(type, {AZOMBIE, ACONEHEAD_ZOMBIE, ABUCKETHEAD_ZOMBIE, ABALLOON_ZOMBIE}))
        // 对于泳池场地，普僵三人组和气球僵尸在第 6 波后可以出现在所有行
        return {0, 1, 2, 3, 4, 5};
    else
        return lands;
}

inline void __MoveZombie(AZombie* zombie, int newRow) {
    int row = zombie->Row();
    zombie->Row() = newRow;
    zombie->Ordinate() += aFieldInfo.rowHeight * (newRow - row);
    zombie->MRef<int>(0x20) += 10000 * (newRow - row);
}

/*
确保指定类型的僵尸存在于指定行
对蹦极僵尸无效；对普僵三人组使用可能导致救生圈僵尸出现在陆地行，或水路的普僵三人组无救生圈
用法：
// 确保 w9 和 w19 的 1 路和 2 路各出至少一只白眼和一只红眼
OnWave(9, 19) EnsureExist({AGARANTUAR, AGIGA_GARGANTUAR}, {1, 2});
*/
inline ATimeline EnsureExist(const std::set<int>& types, const std::vector<int>& rows) {
    return Do {
        int cnt[33] = {};
        for (auto& z : AObjSelector(&AZombie::ExistTime, 0)) {
            int type = z.Type();
            if (type == ABUNGEE_ZOMBIE || !types.contains(type))
                continue;
            if (cnt[type] >= rows.size())
                continue;
            __MoveZombie(&z, rows[cnt[type]++] - 1);
        }
        for (int type : types)
            if (cnt[type] < rows.size()) {
                aLogger->Warning("EnsureExist: 僵尸类型 {} 数量不足", type);
                break;
            }
    };
}

inline ATimeline EnsureExist(int type, const std::vector<int>& rows) {
    return EnsureExist(std::set<int>{type}, rows);
}

/*
确保指定类型的僵尸不存在于指定行
对蹦极僵尸无效；对普僵三人组使用可能导致救生圈僵尸出现在陆地行，或水路的普僵三人组无救生圈
用法：
// 确保 w9 和 w19 的 1 路和 2 路不出白眼和红眼
OnWave(9, 19) EnsureAbsent({AGARANTUAR, AGIGA_GARGANTUAR}, {1, 2});
*/
inline ATimeline EnsureAbsent(const std::set<int>& types, const std::vector<int>& rows) {
    return Do {
        std::set<int> absentRows(rows.begin(), rows.end());
        std::map<int, std::vector<int>> allowedRows;
        for (int type : types) {
            allowedRows[type] = {};
            for (int row : __GetPossibleRows(type))
                if (!absentRows.contains(row + 1))
                    allowedRows[type].push_back(row);
            if (allowedRows[type].empty()) {
                aLogger->Error("EnsureAbsent: 僵尸类型 {} 没有可选的行", type);
                return;
            }
        }
        for (auto& z : AObjSelector(&AZombie::ExistTime, 0)) {
            int type = z.Type();
            if (type == ABUNGEE_ZOMBIE || !types.contains(type))
                continue;
            if (absentRows.contains(z.Row() + 1))
                __MoveZombie(&z, aRandom.Choice(allowedRows[type]));
        }
    };
}

inline ATimeline EnsureAbsent(int type, const std::vector<int>& rows) {
    return EnsureAbsent(std::set<int>{type}, rows);
}

/*
代码来自 crescendo/avz-more
平均分配指定类型的僵尸到各行；默认对所有类型的僵尸生效
这个函数是即时生效的
*/
inline void AAverageSpawn(const std::set<int>& types = {}) {
    std::vector<int> waves;
    for (int wave = 1; wave <= AGetMainObject()->TotalWave(); ++wave)
        waves.push_back(wave);
    OnWave(waves) Do {
        std::vector<int> rows[33];
        int cnt[33];
        for (int type = 0; type < 33; ++type) {
            rows[type] = __GetPossibleRows(type);
            std::ranges::shuffle(rows[type], aRandom.GetEngine());
            cnt[type] = 0;
        }
        for (auto& z : AObjSelector(&AZombie::ExistTime, 0)) {
            int type = z.Type();
            if (type == ABUNGEE_ZOMBIE)
                continue;
            if (!types.empty() && !types.contains(type))
                continue;
            __MoveZombie(&z, rows[type][cnt[type]++ % rows[type].size()]);
        }
    };
}

inline void ASelectCards(const std::string& str, const std::vector<int>& lst, int selectInterval = 17) {
    static const std::unordered_set<char32_t> separators {' ', ',', ';', U'　', U'，', U'；'};
    static const std::unordered_map<char32_t, int> cardAbbr {
        {'A', ACHERRY_BOMB},
        {'B', ABLOVER},
        {'F', AFLOWER_POT},
        {'G', AGRAVE_BUSTER},
        {'I', AICE_SHROOM},
        {'J', AJALAPENO},
        {'K', ACOFFEE_BEAN},
        {'L', ALILY_PAD},
        {'M', APOTATO_MINE},
        {'N', ADOOM_SHROOM},
        {'P', APUMPKIN},
        {'T', ATALL_NUT},
        {'U', AUMBRELLA_LEAF},
        {'W', ASQUASH},
        {'_', ASPIKEWEED},
    }; // DEHOQRSVXYZ
    static const std::array fodders {APUFF_SHROOM, AFLOWER_POT, ASCAREDY_SHROOM, ASUN_SHROOM, ASUNFLOWER};

    std::vector<int> newList;
    std::unordered_set<int> existedCards;
    int fodderCount = 0;
    for (auto ch : AStrToU32str(str)) {
        if (separators.contains(ch))
            continue;
        else if (cardAbbr.contains(ch)) {
            int card = cardAbbr.at(ch);
            if (existedCards.contains(card))
                card += 49;
            newList.push_back(card);
            existedCards.insert(card);
        } else if (ch == 'C' || ch == 'c') {
            while (fodderCount < fodders.size() && existedCards.contains(fodders[fodderCount]))
                ++fodderCount;
            if (fodderCount == fodders.size()) {
                aLogger->Error("ASelectCards: 自动选择的垫材数量最大为 {}", fodders.size());
                break;
            }
            newList.push_back(fodders[fodderCount]);
            existedCards.insert(fodders[fodderCount]);
        } else
            aLogger->Error("ASelectCards: 未知的卡片缩写 {}", ch);
    }
    newList.insert(newList.end(), lst.begin(), lst.end());
    ASelectCards(newList, selectInterval);
}

inline void ASelectCards(const std::string& str, std::initializer_list<int> lst, int selectInterval = 17) {
    ASelectCards(str, std::vector<int>(lst), selectInterval);
}

inline void ASelectCards(const std::string& str, int selectInterval = 17) {
    ASelectCards(str, {}, selectInterval);
}

#endif
