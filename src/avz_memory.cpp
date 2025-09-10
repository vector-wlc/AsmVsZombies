#include "libavz.h"

// 返回鼠标所在行
int AMouseRow() {
    static int lastValue = -1;
    auto memoryValue = AGetMainObject()->MouseExtraAttribution()->Row();
    if (memoryValue >= 0)
        lastValue = memoryValue + 1;
    return lastValue;
}

// 返回鼠标所在列
float AMouseCol() {
    static float lastValue = -1;
    auto memoryValue = AGetMainObject()->MouseAttribution()->Abscissa();
    if (memoryValue >= 0)
        lastValue = float(memoryValue + 25) / 80;
    return lastValue;
}

int AGetSeedIndex(int type, bool imitator) {
    if (type >= AM_PEASHOOTER) {
        type -= AM_PEASHOOTER;
        imitator = true;
    }
    for (auto& seed : ABasicFilter<ASeed>()) {
        if (imitator && seed.Type() != AIMITATOR)
            continue;
        if ((imitator ? seed.ImitatorType() : seed.Type()) == type)
            return &seed - AGetMainObject()->SeedArray();
    }
    return -1;
}

ASeed* AGetSeedPtr(int type, bool imitator) {
    auto idx = AGetSeedIndex(type, imitator);
    return idx < 0 ? nullptr : AGetMainObject()->SeedArray() + idx;
}

int AGetPlantIndex(int row, int col, int type) {
    for (auto& plant : AObjSelector(&APlant::Row, row - 1, &APlant::Col, col - 1)) {
        if (plant.Type() == ASQUASH && plant.State() >= 5)
            continue;
        int plantType = plant.Type();
        if (type == -1) {
            // 如果植物存在	且不为南瓜花盆荷叶咖啡豆
            if ((plantType != APUMPKIN) && (plantType != AFLOWER_POT) && (plantType != ALILY_PAD) && (plantType != ACOFFEE_BEAN))
                return plant.Index(); // 返回植物的对象序列
        } else {
            if (plantType == type)
                return plant.Index();
            else if (type != APUMPKIN && type != AFLOWER_POT && type != ALILY_PAD && type != ACOFFEE_BEAN && plantType != APUMPKIN && plantType != AFLOWER_POT && plantType != ALILY_PAD && plantType != ACOFFEE_BEAN)
                return -2;
        }
    }
    return -1; // 没有符合要求的植物返回 -1
}

APlant* AGetPlantPtr(int row, int col, int type) {
    auto idx = AGetPlantIndex(row, col, type);
    return idx < 0 ? nullptr : AGetMainObject()->PlantArray() + idx;
}

void AGetPlantIndices(const std::vector<AGrid>& lstIn, int type, std::vector<int>& indicesOut) {
    std::map<AGrid, std::vector<int>> gridToIndices;
    for (size_t i = 0; i < lstIn.size(); ++i)
        gridToIndices[lstIn[i]].push_back(i);

    indicesOut.assign(lstIn.size(), -1);
    for (auto& plant : aAlivePlantFilter) {
        if (plant.Type() == ASQUASH && plant.State() >= 5)
            continue;
        AGrid grid{plant.Row() + 1, plant.Col() + 1};
        if (!gridToIndices.contains(grid))
            continue;
        int plantType = plant.Type();
        if (plantType == type)
            for (auto idx : gridToIndices[grid])
                indicesOut[idx] = plant.Index();
        else if (type != APUMPKIN && type != AFLOWER_POT && type != ALILY_PAD && type != ACOFFEE_BEAN && plantType != APUMPKIN && plantType != AFLOWER_POT && plantType != ALILY_PAD && plantType != ACOFFEE_BEAN)
            for (auto idx : gridToIndices[grid])
                indicesOut[idx] = -2;
    }
}

std::vector<int> AGetPlantIndices(const std::vector<AGrid>& lst, int type) {
    std::vector<int> indices;
    AGetPlantIndices(lst, type, indices);
    return indices;
}

void AGetPlantPtrs(const std::vector<AGrid>& lstIn, int type, std::vector<APlant*>& ptrsOut) {
    auto plantArray = AGetMainObject()->PlantArray();
    auto indices = AGetPlantIndices(lstIn, type);
    ptrsOut.clear();
    for (auto&& index : indices)
        ptrsOut.push_back(index < 0 ? nullptr : plantArray + index);
}

std::vector<APlant*> AGetPlantPtrs(const std::vector<AGrid>& lst, int type) {
    std::vector<APlant*> ptrs;
    AGetPlantPtrs(lst, type, ptrs);
    return ptrs;
}

bool AIsZombieExist(int type, int row) {
    for (auto& zombie : aAliveZombieFilter)
        if ((type < 0 || zombie.Type() == type) && (row <= 0 || zombie.Row() == row - 1))
            return true;
    return false;
}

std::vector<AGrid> AGetGraves() {
    std::vector<AGrid> graves;
    for (auto& item : AObjSelector(&APlaceItem::Type, APlaceItemType::GRAVESTONE))
        graves.push_back({item.Row() + 1, item.Col() + 1});
    return graves;
}

void ASetPlantActiveTime(APlantType plantType, int delayTime) {
    AConnect(ANowDelayTime(delayTime - 10), [=]() {
        for (auto& plant : AObjSelector(&APlant::Type, plantType, &APlant::State, 2)) {
            if (std::abs(plant.ExplodeCountdown() - 10) <= 3)
                plant.ExplodeCountdown() = 10;
            else
                aLogger->Error("ASetPlantActiveTime 不允许修改的生效时间超过 3cs");
            return;
        }
    });
}

void AUpdateZombiesPreview() {
    // 去掉当前画面上的僵尸
    AAsm::KillZombiesPreview();
    // 重新生成僵尸
    AGetMainObject()->SelectCardUi_m()->IsCreatZombie() = false;
}

namespace {
std::vector<int> AParseZombieTypeString(std::string_view str) {
    static const std::unordered_set<char32_t> separators {' ', ',', ';', U'　', U'，', U'；'};
    static const std::unordered_map<char32_t, int> zombieAbbr {
        {U'普', AZOMBIE},
        {U'旗', AFLAG_ZOMBIE},
        {U'障', ACONEHEAD_ZOMBIE},
        {U'杆', APOLE_VAULTING_ZOMBIE},
        {U'桶', ABUCKETHEAD_ZOMBIE},
        {U'报', ANEWSPAPER_ZOMBIE},
        {U'门', ASCREEN_DOOR_ZOMBIE},
        {U'橄', AFOOTBALL_ZOMBIE},
        {U'舞', ADANCING_ZOMBIE},
        {U'潜', ASNORKEL_ZOMBIE},
        {U'车', AZOMBONI},
        {U'橇', AZOMBIE_BOBSLED_TEAM},
        {U'豚', ADOLPHIN_RIDER_ZOMBIE},
        {U'丑', AJACK_IN_THE_BOX_ZOMBIE},
        {U'气', ABALLOON_ZOMBIE},
        {U'矿', ADIGGER_ZOMBIE},
        {U'跳', APOGO_ZOMBIE},
        {U'雪', AZOMBIE_YETI},
        {U'偷', ABUNGEE_ZOMBIE},
        {U'梯', ALADDER_ZOMBIE},
        {U'篮', ACATAPULT_ZOMBIE},
        {U'白', AGARGANTUAR},
        {U'博', ADR_ZOMBOSS},
        {U'豌', APEASHOOTER_ZOMBIE},
        {U'坚', AWALL_NUT_ZOMBIE},
        {U'辣', AJALAPENO_ZOMBIE},
        {U'枪', AGATLING_PEA_ZOMBIE},
        {U'窝', ASQUASH_ZOMBIE},
        {U'高', ATALL_NUT_ZOMBIE},
        {U'红', AGIGA_GARGANTUAR},
    };

    std::vector<int> lst;
    for (auto ch : AStrToU32str(std::string(str))) {
        if (separators.contains(ch))
            continue;
        else if (zombieAbbr.contains(ch))
            lst.push_back(zombieAbbr.at(ch));
        else
            aLogger->Error("ASetZombies: 未知的僵尸缩写 {}", ch);
    }
    return lst;
}
}

void ASetZombies(const std::vector<int>& zombieType, ASetZombieMode mode) {
    if (zombieType.empty()) {
        aLogger->Error("ASetZombies: 出怪类型列表不能为空");
        return;
    }

    if (AGetPvzBase()->GameUi() == 3)
        aLogger->Warning("正在战斗模式下重设出怪；ASetZombies 应该在选卡前调用");

    std::vector<int> zombieTypeVec;
    bool isHasBungee = false;

    // 设置出怪类型列表
    auto typeList = AGetMainObject()->ZombieTypeList();
    std::fill_n(typeList, AHY_32 + 1, 0);

    for (const auto& type : zombieType) {
        typeList[type] = true;
        if (type == ABJ_20)
            isHasBungee = true;
        // 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
        if (!ARangeIn(type, {AQZ_1, ABW_9, AXQ_13, AXR_19, ABJ_20, AXG_24}))
            zombieTypeVec.push_back(type);
    }

    if (mode == ASetZombieMode::INTERNAL) {
        if (typeList[AZOMBIE] == false) {
            aLogger->Warning("自然出怪模式下出怪类型必须有普僵，已自动添加");
            typeList[AZOMBIE] = 1;
        }
        AAsm::PickZombieWaves();
    } else if (mode == ASetZombieMode::AVERAGE) {
        auto zombieList = AGetMainObject()->ZombieList();
        int totaNum = AGetMainObject()->TotalWave() * 50;
        for (int idx = 0; idx < totaNum; ++idx)
            zombieList[idx] = zombieTypeVec[idx % zombieTypeVec.size()];
        // 生成旗帜
        for (int idx = 9 * 50; idx < totaNum; idx += 10 * 50)
            zombieList[idx] = AQZ_1;

        if (isHasBungee) {
            // 生成蹦极
            for (int idx = 9 * 50; idx < totaNum; idx += 10 * 50)
                for (auto index : {idx + 1, idx + 2, idx + 3, idx + 4})
                    zombieList[index] = ABJ_20;
        }
    }

    if (AGetPvzBase()->GameUi() == 2)
        AUpdateZombiesPreview();
}

void ASetZombies(std::string_view str, ASetZombieMode mode) {
    ASetZombies(AParseZombieTypeString(str), mode);
}

void ASetWaveZombies(int wave, const std::vector<int>& zombieType) {
    if (wave < 1 || wave > AGetMainObject()->TotalWave()) {
        aLogger->Error("ASetWaveZombies: 波次应在 1~{} 之间", AGetMainObject()->TotalWave());
        return;
    }
    if (zombieType.empty()) {
        aLogger->Error("ASetWaveZombies: 出怪类型列表不能为空");
        return;
    }

    auto zombieList = AGetMainObject()->ZombieList() + (wave - 1) * 50;
    int offset = 0;

    // 生成旗帜
    if (wave % 10 == 0)
        zombieList[offset++] = AQZ_1;

    for (int idx = 0; idx + offset < 50; ++idx)
        zombieList[idx + offset] = zombieType[idx % zombieType.size()];
}

void ASetWaveZombies(int wave, std::string_view str) {
    ASetWaveZombies(wave, AParseZombieTypeString(str));
}

std::vector<int> ACreateRandomTypeList(const std::vector<int>& required, const std::vector<int>& banned) {
    static constexpr std::array<int, 22> CANDIDATES {0, 2, 3, 4, 5, 6, 7, 8, 11, 12, 14, 15, 16, 17, 18, 20, 21, 22, 23, 32, -1, -2};
    std::unordered_map<int, bool> requirements {{AZOMBIE, true}};
    if (aFieldInfo.hasGrave)
        requirements[AZOMBONI] = false;
    if (!aFieldInfo.hasPool)
        requirements[ASNORKEL_ZOMBIE] = requirements[ADOLPHIN_RIDER_ZOMBIE] = false;
    if (aFieldInfo.isRoof)
        requirements[ADANCING_ZOMBIE] = requirements[ADIGGER_ZOMBIE] = false;
    for (int type : required) {
        if (requirements.contains(type) && !requirements[type]) {
            std::string msg = std::format("无法满足出怪类型中包含 {} 的要求", type);
            aLogger->Error(msg);
            throw AException(msg);
        } else {
            requirements[type] = true;
        }
    }
    for (int type : banned) {
        if (requirements.contains(type) && requirements[type]) {
            std::string msg = std::format("无法满足出怪类型中不包含 {} 的要求", type);
            aLogger->Error(msg);
            throw AException(msg);
        } else {
            requirements[type] = false;
        }
    }
    if (!requirements.contains(ACONEHEAD_ZOMBIE) && !requirements.contains(ANEWSPAPER_ZOMBIE)) {
        requirements[aRandom(5) ? ACONEHEAD_ZOMBIE : ANEWSPAPER_ZOMBIE] = true;
    } else {
        bool banConehead = requirements.contains(ACONEHEAD_ZOMBIE) && !requirements[ACONEHEAD_ZOMBIE];
        bool banNewspaper = requirements.contains(ANEWSPAPER_ZOMBIE) && !requirements[ANEWSPAPER_ZOMBIE];
        if (banConehead && banNewspaper) {
            std::string msg = "自然出怪中路障僵尸和读报僵尸至少出现其一";
            aLogger->Error(msg);
            throw AException(msg);
        }
        if (banConehead)
            requirements[ANEWSPAPER_ZOMBIE] = true;
        else if (banNewspaper)
            requirements[ACONEHEAD_ZOMBIE] = true;
    }

    std::vector<int> candidates;
    for (int type : CANDIDATES)
        if (!requirements.contains(type))
            candidates.push_back(type);
    std::vector<int> typeList;
    for (auto [type, required] : requirements)
        if (required)
            typeList.push_back(type);
    if (typeList.size() > 11) {
        std::string msg = std::format("已指定 {} 种必选出怪，而自然出怪的上限为 11 种", typeList.size());
        aLogger->Error(msg);
        throw AException(msg);
    }
    for (int type : aRandom.Sample(candidates, 11 - typeList.size()))
        if (type >= 0)
            typeList.push_back(type);
    return typeList;
}

bool* AGetZombieTypeList() {
    return AGetMainObject()->ZombieTypeList();
}

void __AGameSpeedManager::_BeforeScript() {
    _oriTickMs = AGetPvzBase()->TickMs();
}

void __AGameSpeedManager::_ExitFight() {
    AGetPvzBase()->TickMs() = _oriTickMs;
}

void __AGameSpeedManager::Set(float x) {
    if (x < 0.05 || x > 10) {
        aLogger->Error("SetGameSpeed : 倍速设置失败，倍速设置的合法范围为 [0.05, 10]");
        return;
    }
    int ms = int(10 / x + 0.5);
    AGetPvzBase()->TickMs() = ms;
}

bool AGameIsPaused() {
    if (!AGetPvzBase()->MainObject())
        return false;
    return AGetMainObject()->GamePaused() || AGetPvzBase()->MouseWindow()->TopWindow() != nullptr;
}

ARemovePlantPos::ARemovePlantPos(int row, float col, int type)
    : row(row), col(col) {
    if (type == -1)
        types = {-1, APUMPKIN, AFLOWER_POT, ALILY_PAD, ACOFFEE_BEAN};
    else
        types = {type, -1, APUMPKIN, AFLOWER_POT, ALILY_PAD, ACOFFEE_BEAN};
}

ARemovePlantPos::ARemovePlantPos(int row, float col, const std::vector<int>& types)
    : row(row), col(col), types(types) {}

void ARemovePlant(int row, float col, const std::vector<int>& priority) {
    std::vector<APlant*> plantVec;
    for (auto& plant : AObjSelector(&APlant::Row, row - 1, &APlant::Col, int(col + 0.5) - 1))
        if (plant.Type() != ASQUASH || plant.State() <= 4)
            plantVec.push_back(&plant);

    for (auto priType : priority) {
        for (auto plant : plantVec) {
            int plantType = plant->Type();
            if (priType == plantType || (priType == -1 && plantType != APUMPKIN && plantType != AFLOWER_POT && plantType != ALILY_PAD && plantType != ACOFFEE_BEAN)) {
                AAsm::RemovePlant(plant);
                return;
            }
        }
    }
}

void ARemovePlant(int row, float col, int type) {
    if (type == -1)
        ARemovePlant(row, col, {-1, APUMPKIN, AFLOWER_POT, ALILY_PAD, ACOFFEE_BEAN});
    else
        ARemovePlant(row, col, {type, -1, APUMPKIN, AFLOWER_POT, ALILY_PAD, ACOFFEE_BEAN});
}

void ARemovePlant(const std::vector<ARemovePlantPos>& lst) {
    for (auto&& [row, col, types] : lst)
        ARemovePlant(row, col, types);
}

__ANodiscard int AGetSeedSunVal(APlantType type) {
    int intType = int(type) % AM_PEASHOOTER;
    int intIType = int(type) - AM_PEASHOOTER;
    return AAsm::GetSeedSunVal(intType, intIType);
}

__ANodiscard int AGetSeedSunVal(ASeed* seed) {
    return seed == nullptr ? -1 : AAsm::GetSeedSunVal(seed->Type(), seed->ImitatorType());
}

// 检查卡片是否能用
__ANodiscard bool AIsSeedUsable(APlantType type) {
    auto idx = AGetCardIndex(type);
    if (idx == -1)
        return false;
    return AAsm::IsSeedUsable(AGetMainObject()->SeedArray() + idx);
}

// 检查卡片是否能用
__ANodiscard bool AIsSeedUsable(ASeed* seed) {
    if (seed == nullptr)
        return false;
    return AAsm::IsSeedUsable(seed);
}

// 得到炮的恢复时间
// index 为玉米加农炮的内存索引
__ANodiscard int AGetCobRecoverTime(int index) {
    if (index < 0 || index >= AGetMainObject()->PlantCountMax()) {
        aLogger->Error("AGetCobRecoverTime 参数值不合法");
        return INT_MIN;
    }
    return AGetCobRecoverTime(AGetMainObject()->PlantArray() + index);
}

__ANodiscard int AGetCobRecoverTime(APlant* cob) {
    if (cob == nullptr || cob->IsDisappeared() || cob->Type() != ACOB_CANNON) {
        aLogger->Error("AGetCobRecoverTime 参数值不合法");
        return INT_MIN;
    }
    auto animation = cob->AnimationPtr();

    switch (cob->State()) {
    case 35:
        return 125 + cob->StateCountdown();
    case 36:
        return int(125 * (1 - animation->CirculationRate()) + 0.5) + 1;
    case 37:
        return 0;
    case 38:
        return 3125 + int(350 * (1 - animation->CirculationRate()) + 0.5);
    default:
        return INT_MIN;
    }
}

void AFieldInfo::_BeforeScript() {
    rowHeight = AAsm::GridToOrdinate(1, 0) - AAsm::GridToOrdinate(0, 0);
    rowType[0] = ARowType::NONE;
    for (int i = 1; i <= 6; i++) {
        rowType[i] = ARowType(AGetMainObject()->MRef<int>(0x5d8 + (i - 1) * 4));
        if (rowType[i] == ARowType::NONE && AAsm::CanSpawnZombies(i - 1))
            rowType[i] = ARowType::UNSODDED;
    }
    if (rowHeight == 100 && rowType[6] != ARowType::NONE) // AQE
        rowType[6] = ARowType::UNSODDED;
    isNight = AAsm::IsNight();
    isRoof = AAsm::IsRoof();
    hasGrave = AAsm::HasGrave();
    hasPool = AAsm::HasPool();
    nRows = 6 - std::count(rowType + 1, rowType + 7, ARowType::NONE);
}
