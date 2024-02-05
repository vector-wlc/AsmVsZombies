#ifndef __ADSL_SHORTHAND__
#define __ADSL_SHORTHAND__

#include "main.h"
#include <format>

using namespace ALiterals;

class {
public:
    operator ATime() const
    {
        return ANowTime();
    }
} inline constexpr now;

inline ATimeline Trig(bool force = false)
{
    return [=](ATime time) {
        if (time.wave % 10 == 9 || time.wave == AGetMainObject()->TotalWave())
            return;
        time.time = std::max(time.time + 200, 601);
        force ? ASetWavelength({time}) : AAssumeWavelength({time});
    };
}

class ARoofCobManager : public ACobManager {
protected:
    std::vector<int> _columns;

    void _BeforeScript() override
    {
        RefreshCobList();
        ACobManager::_BeforeScript();
    }

    using ACobManager::AutoGetList;
    using ACobManager::AutoSetList;

public:
    ARoofCobManager(const std::vector<int>& columns)
        : ACobManager(PRIORITY)
        , _columns(columns)
    {
    }
    ARoofCobManager(std::convertible_to<int> auto... column)
        : ACobManager(PRIORITY)
        , _columns {column...}
    {
    }

    void SetColumns(const std::vector<int>& columns)
    {
        _columns = columns;
        RefreshCobList();
    }

    void RefreshCobList()
    {
        std::vector<int> cobs[9];
        for (auto&& plant : aAlivePlantFilter)
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

inline ATimeline P(ACobManager& cm, int row, float col)
{
    std::vector<int> rows;
    for (char r : std::to_string(row)) {
        if (r < '1' || r > '0' + aFieldInfo.nRows) {
            AGetInternalLogger()->Error(std::format("P: 输入的行数 {} 不合法", row));
            return {};
        }
        rows.push_back(r - '0');
    }
    ATimeline ret;
    for (int row : rows) {
        ATimeOffset offset;
        if (aFieldInfo.isRoof)
            offset = -387;
        else
            offset = (aFieldInfo.rowType[row] == ARowType::POOL ? -378 : -373);
        ret += At(offset)[=, &cm]
        {
            aFieldInfo.isRoof ? cm.RoofFire(row, col) : cm.Fire(row, col);
        };
    }
    return ret;
}

inline ATimeline P(ACobManager& cm, int row, float col, ACobManager& cm2, auto&&... args)
{
    return P(cm, row, col) + P(cm2, std::forward<decltype(args)>(args)...);
}

inline ATimeline P(ACobManager& cm, int row, float col, int row2, float col2, auto&&... args)
{
    return P(cm, row, col) + P(cm, row2, col2, std::forward<decltype(args)>(args)...);
}

inline ATimeline P(int row, float col, auto&&... args)
{
    return P(aCobManager, row, col, std::forward<decltype(args)>(args)...);
}

inline ATimeline PP(float col = 9)
{
    return P(aFieldInfo.nRows == 5 ? 24 : 25, col);
}

template <ATimeOffset DELAY = 0>
inline ATimeline D(auto&&... args)
{
    return P(std::forward<decltype(args)>(args)...).Offset(DELAY);
}

template <ATimeOffset DELAY = 0>
inline ATimeline DD(float col)
{
    return P(aFieldInfo.nRows == 5 ? 14 : 15, col).Offset(DELAY);
}

inline APlant* __CardInstant(APlantType seed, int row, float col)
{
    int seed_ = seed % AM_PEASHOOTER;
    if (AAsm::GetPlantRejectType(seed_, row - 1, int(col - 0.5)) == AAsm::NEEDS_POT)
        ACard(AFLOWER_POT, row, col);
    if (AAsm::GetPlantRejectType(seed_, row - 1, int(col - 0.5)) == AAsm::NOT_ON_WATER)
        ACard(ALILY_PAD, row, col);
    return ACard(seed, row, col);
}

inline APlant* __CardInstant(APlantType seed, const std::vector<APosition>& positions)
{
    int seed_ = seed % AM_PEASHOOTER;
    for (auto [row, col] : positions) {
        int rejectType = AAsm::GetPlantRejectType(seed_, row - 1, int(col - 0.5));
        if (!ARangeIn(rejectType, {AAsm::NIL, AAsm::NEEDS_POT, AAsm::NOT_ON_WATER}))
            continue;
        if (rejectType == AAsm::NEEDS_POT)
            ACard(AFLOWER_POT, row, col);
        if (rejectType == AAsm::NOT_ON_WATER)
            ACard(ALILY_PAD, row, col);
        return ACard(seed, row, col);
    }
    return nullptr;
}

inline ATimeline Card(APlantType seed, int row, float col)
{
    return Do { __CardInstant(seed, row, col); };
}

inline ATimeline Card(const std::vector<ACardName>& cards)
{
    return Do
    {
        for (auto [seed, row, col] : cards)
            __CardInstant(seed, row, col);
    };
}

inline ATimeline Card(const std::vector<APlantType>& seeds, int row, float col)
{
    std::vector<ACardName> cards;
    for (auto seed : seeds)
        cards.push_back({seed, row, col});
    return Card(cards);
}

inline ATimeline Card(APlantType seed, const std::vector<APosition>& positions)
{
    return Do { __CardInstant(seed, positions); };
}

inline ATimeline Card(const std::vector<APlantType>& seeds, const std::vector<APosition>& positions)
{
    return Do
    {
        for (auto seed : seeds)
            __CardInstant(seed, positions);
    };
}

inline ATimeline Shovel(int row, int col, int targetType = -1)
{
    return Do { AShovel(row, col, targetType); };
}

inline ATimeline Shovel(int row, int col, bool pumpkin)
{
    return Do { AShovel(row, col, pumpkin); };
}

inline ATimeline Shovel(const std::vector<AShovelPosition>& positions)
{
    return Do { AShovel(positions); };
}

inline ATimeline A(int row, float col)
{
    return At(-100_cs) Card(ACHERRY_BOMB, row, col);
}

inline ATimeline J(int row, float col)
{
    return At(-100_cs) Card(AJALAPENO, row, col);
}

inline ATimeline a(int row, float col)
{
    return At(-182_cs) Card(ASQUASH, row, col);
}

inline ATimeline __UseMushroomDay(APlantType type, int row, float col, bool tryImitator)
{
    APlantType imitatorType = APlantType(type + AM_PEASHOOTER);
    if (!tryImitator)
        return At(-299_cs) Do
        {
            __CardInstant(type, row, col);
            ACard(ACOFFEE_BEAN, row, col);
            ASetPlantActiveTime(type, 299);
        };
    else
        return At(-619_cs) Do
        {
            if (AIsSeedUsable(imitatorType)) {
                __CardInstant(imitatorType, row, col);
                ASetPlantActiveTime(type, 619);
                At(now + 420_cs) Card(ACOFFEE_BEAN, row, col);
            } else
                At(now + 619_cs) __UseMushroomDay(type, row, col, false);
        };
}

inline ATimeline __UseMushroomNight(APlantType type, int row, float col, bool tryImitator)
{
    APlantType imitatorType = APlantType(type + AM_PEASHOOTER);
    if (!tryImitator)
        return At(-100_cs) Card(type, row, col);
    else
        return At(-420_cs) Do
        {
            if (AIsSeedUsable(imitatorType)) {
                __CardInstant(imitatorType, row, col);
                ASetPlantActiveTime(type, 420);
            } else
                At(now + 420_cs) __UseMushroomNight(type, row, col, false);
        };
}

inline ATimeline N(int row, float col, bool tryImitator = false)
{
    if (aFieldInfo.isNight)
        return __UseMushroomNight(ADOOM_SHROOM, row, col, tryImitator);
    else
        return __UseMushroomDay(ADOOM_SHROOM, row, col, tryImitator);
}

inline ATimeline N(const std::vector<APosition>& positions, bool tryImitator = false)
{
    ATimeOffset offset = aFieldInfo.isNight ? -100_cs : -299_cs;
    if (tryImitator)
        offset -= 320_cs;
    return At(offset) Do
    {
        for (auto [row, col] : positions) {
            if (ARangeIn(AAsm::GetPlantRejectType(ADOOM_SHROOM, row - 1, col - 1),
                    {AAsm::NIL, AAsm::NOT_ON_WATER, AAsm::NEEDS_POT})) {
                At(now - offset) N(row, col, tryImitator);
                break;
            }
        }
    };
}

inline ATimeline I(int row, float col, bool tryImitator = true)
{
    if (aFieldInfo.isNight)
        return __UseMushroomNight(AICE_SHROOM, row, col, tryImitator);
    else
        return __UseMushroomDay(AICE_SHROOM, row, col, tryImitator);
}

inline ATimeline I(AIceFiller& if_ = aIceFiller)
{
    if (aFieldInfo.isNight) {
        AGetInternalLogger()->Error("I: 需要指定放置寒冰菇的位置");
        return {};
    }
    return At(-299_cs)[=, &if_]
    {
        if_.Coffee();
        ASetPlantActiveTime(AICE_SHROOM, 299);
    };
}

class AFodder : protected AStateHook {
protected:
    std::vector<APlantType> _seeds;
    std::vector<int> _triggerTypes;

    virtual void _EnterFight() override
    {
        if (!_seeds.empty())
            return;
        std::vector<std::pair<int, int>> candidates;
        for (auto&& seed : ABasicFilter<ASeed>()) {
            int seedType = (seed.Type() == AIMITATOR ? seed.ImitatorType() : seed.Type());
            if (ARangeIn(seedType, {ALILY_PAD, ACOFFEE_BEAN}))
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

    static std::vector<APosition> _ParseRow(const std::vector<APosition>& positions)
    {
        std::vector<APosition> ret;
        for (auto [row, col] : positions)
            for (char r : std::to_string(row)) {
                if (r < '1' || r > '0' + aFieldInfo.nRows) {
                    AGetInternalLogger()->Error(std::format("AFodder: 输入的行数 {} 不合法", row));
                    return {};
                }
                ret.push_back({r - '0', col});
            }
        return ret;
    }

    void _Fodder(int removalDelay, const std::vector<APosition>& positions) const
    {
        std::string log = "AFodder: 垫";
        for (auto [row, col] : positions)
            log += std::format(" {}-{}", row, col);
        AGetInternalLogger()->Info(log);

        auto currentSeed = _seeds.begin();
        std::vector<AShovelPosition> shovelTargets;
        for (auto [row, col] : positions) {
            while (currentSeed != _seeds.end() && !AIsSeedUsable(*currentSeed))
                ++currentSeed;
            if (currentSeed == _seeds.end()) {
                int cd = 9999;
                for (auto&& card : _seeds)
                    if (ASeed* seed = AGetSeedPtr(card))
                        cd = std::min(cd, seed->InitialCd() - seed->Cd() + 1);
                AGetInternalLogger()->Error(std::format("AFodder: 距下一个垫材可用还有 {} cs", cd));
                break;
            }
            ACard(*currentSeed, row, col);
            shovelTargets.push_back({row, col, *currentSeed});
            ++currentSeed;
        }

        if (removalDelay <= 0)
            return;
        At(now + removalDelay) Do
        {
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
    public:
        class WaveConstraint {
        private:
            std::set<int> _waves;

        public:
            WaveConstraint() { }
            WaveConstraint(const std::set<int>& waves)
                : _waves(waves)
            {
            }

            bool operator()(int wave) const
            {
                return _waves.empty() || _waves.contains(wave) || (wave < 0 && _waves.contains(INT_MIN));
            }

            WaveConstraint& operator&=(const WaveConstraint& rhs)
            {
                if (_waves.empty()) {
                    _waves = rhs._waves;
                    return *this;
                }
                for (auto it = _waves.begin(); it != _waves.end();)
                    rhs(*it) ? ++it : (it = _waves.erase(it));
                return *this;
            }

            friend WaveConstraint operator&(WaveConstraint lhs, const WaveConstraint& rhs)
            {
                return lhs &= rhs;
            }
        };

        class AbscConstraint {
        private:
            int _l = INT_MIN;
            int _r = INT_MAX;

        public:
            AbscConstraint() { }
            AbscConstraint(int l, int r)
                : _l(l)
                , _r(r)
            {
            }

            bool operator()(float x) const
            {
                return _l <= int(x) && int(x) <= _r;
            }

            friend AbscConstraint operator&(AbscConstraint lhs, AbscConstraint rhs)
            {
                return {std::max(lhs._l, rhs._l), std::min(lhs._r, rhs._r)};
            }

            AbscConstraint& operator&=(const AbscConstraint& rhs)
            {
                return *this = *this & rhs;
            }
        };

    private:
        AZombieType _type;
        WaveConstraint _wave;
        AbscConstraint _absc;

    public:
        Constraint(AZombieType type_)
            : _type(type_)
        {
        }

        Constraint& operator&=(const WaveConstraint& rhs)
        {
            _wave &= rhs;
            return *this;
        }

        Constraint& operator&=(AbscConstraint rhs)
        {
            _absc &= rhs;
            return *this;
        }

        friend Constraint operator&(Constraint lhs, const WaveConstraint& rhs)
        {
            return lhs &= rhs;
        }

        friend Constraint operator&(Constraint lhs, AbscConstraint rhs)
        {
            return lhs &= rhs;
        }

        bool operator()(AZombie* zombie) const
        {
            return _type == zombie->Type() && _wave(zombie->AtWave() + 1 - ANowWave()) && _absc(zombie->Abscissa());
        }
    };

    class TriggerByProxy {
        friend class AFodder;

    protected:
        const AFodder* _fodder;
        std::vector<Constraint> _constraints;

        TriggerByProxy(const AFodder* fodder, const std::vector<Constraint>& constraint)
            : _fodder(fodder)
            , _constraints(constraint)
        {
        }

        std::set<int> _GetTriggeredRows() const
        {
            std::set<int> triggeredRows;
            for (auto&& zombie : aAliveZombieFilter) {
                if (triggeredRows.contains(zombie.Row() + 1))
                    continue;
                for (auto&& constraint : _constraints)
                    if (constraint(&zombie)) {
                        triggeredRows.insert(zombie.Row() + 1);
                        break;
                    }
            }
            return triggeredRows;
        }

    public:
        ATimeline operator()(int removalDelay, const std::vector<APosition>& positions) const
        {
            return [=, *this, positions = _ParseRow(positions)] {
                std::set<int> triggeredRows = _GetTriggeredRows();
                std::vector<APosition> triggeredPositions;
                for (auto [row, col] : positions)
                    if (triggeredRows.contains(row))
                        triggeredPositions.push_back({row, col});
                _fodder->_Fodder(removalDelay, triggeredPositions);
            };
        }

        ATimeline operator()(int removalDelay, APosition position) const
        {
            return operator()(removalDelay, std::vector<APosition> {position});
        }

        ATimeline operator()(int removalDelay, int row) const
        {
            return operator()(removalDelay, {row, 9});
        }

        ATimeline operator()(int removalDelay) const
        {
            std::vector<APosition> positions;
            for (int row = 1; row <= aFieldInfo.nRows; ++row)
                if (aFieldInfo.rowType[row] == ARowType::LAND)
                    positions.push_back({row, 9});
            return operator()(removalDelay, positions);
        }
    };

    AFodder()
    {
    }
    AFodder(const std::vector<APlantType>& seeds)
        : _seeds(seeds)
    {
    }
    AFodder(std::convertible_to<int> auto... seed)
        : _seeds {seed...}
    {
    }

    void SetCards(const std::vector<APlantType>& seeds)
    {
        _seeds = seeds;
    }

    TriggerByProxy TriggerBy(const std::vector<Constraint>& constraints) const
    {
        return {this, constraints};
    }

    TriggerByProxy TriggerBy(std::convertible_to<Constraint> auto&&... constraints) const
    {
        return TriggerBy(std::vector<Constraint> {constraints...});
    }

    // 返回 maxCd cs 之内可用的垫材数量
    int AvailableCount(int maxCd = 0) const
    {
        int cnt = 0;
        for (auto&& card : _seeds)
            if (ASeed* seed = AGetSeedPtr(card))
                cnt += (seed->IsUsable() || seed->InitialCd() - seed->Cd() + 1 <= maxCd);
        return cnt;
    }

    ATimeline operator()(int removalDelay, const std::vector<APosition>& positions) const
    {
        return [=, this, positions = _ParseRow(positions)] { _Fodder(removalDelay, positions); };
    }

    ATimeline operator()(int removalDelay, APosition position) const
    {
        return operator()(removalDelay, std::vector<APosition> {position});
    }

    ATimeline operator()(int removalDelay, int row) const
    {
        return operator()(removalDelay, {row, 9});
    }

    ATimeline operator()(int removalDelay) const
    {
        std::vector<APosition> positions;
        for (int row = 1; row <= 6; ++row)
            if (aFieldInfo.rowType[row] == ARowType::LAND)
                positions.push_back({row, 9});
        return operator()(removalDelay, positions);
    }
} inline C;

inline const AFodder::Constraint::WaveConstraint CURR_WAVE({0}), PREV_WAVES({INT_MIN});

inline AFodder::Constraint::WaveConstraint WaveIn(const std::vector<int>& waves)
{
    return std::set<int>(waves.begin(), waves.end());
}

inline AFodder::Constraint::WaveConstraint WaveIn(const std::set<int>& waves)
{
    return waves;
}

inline AFodder::Constraint::WaveConstraint WaveIn(std::convertible_to<int> auto... waves)
{
    return std::set<int> {waves...};
}

inline AFodder::Constraint::AbscConstraint AbscIn(int l, int r)
{
    return {l, r};
}

inline AFodder::Constraint operator&(AZombieType lhs, const AFodder::Constraint::WaveConstraint& rhs)
{
    return AFodder::Constraint(lhs) & rhs;
}

inline AFodder::Constraint operator&(AZombieType lhs, AFodder::Constraint::AbscConstraint rhs)
{
    return AFodder::Constraint(lhs) & rhs;
}

#endif
