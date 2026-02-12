#include "libavz.h"

////////////////////////////////////////
//  ItemCollector
////////////////////////////////////////

AItemCollector::AItemCollector() {
    _types.fill(true);
}

void AItemCollector::_EnterFight() {
    AItemCollector::Start();
}

void AItemCollector::SetInterval(int timeInterval) {
    if (timeInterval < 1) {
        aLogger->Error("自动收集类收集间隔应大于 0");
        return;
    }
    this->_timeInterval = timeInterval;
}

void AItemCollector::SetTypeList(const std::vector<int>& types) {
    _types.fill(false);
    for (auto type : types) {
        if (type < 1 && type >= _TYPE_SIZE) {
            aLogger->Warning("AItemCollector::SetTypeList : 您设置的收集物类型 {} 不存在", type);
            continue;
        }
        _types[type] = true;
    }
}

void AItemCollector::Start() {
    ATickRunnerWithNoStart::_Start([this] { _Run(); }, ONLY_FIGHT);
}

void AItemCollector::_Run() {
    if (AGetMainObject()->GameClock() % _timeInterval != 0 || //
        AGetMainObject()->MouseAttribution()->Type() != 0)
        return;

    AItem* collectItem = nullptr;
    for (auto& item : aAliveItemFilter) {
        if (!_types[item.Type()])
            continue;
        collectItem = &item;
        if (ARangeIn(item.Type(), {4, 5, 6})) // 优先采集阳光
            break;
    }
    if (!collectItem) // 没有要收集的物品
        return;

    float itemX = collectItem->Abscissa();
    float itemY = collectItem->Ordinate();
    if (itemX >= 0.0 && itemY >= 70) {
        AAsm::ReleaseMouse();
        int x = static_cast<int>(itemX + 30);
        int y = static_cast<int>(itemY + 30);
        ALeftClick(x, y);
        AAsm::ReleaseMouse();
    }
}

////////////////////////////////////////
//  IceFiller
////////////////////////////////////////

void AIceFiller::_EnterFight() {
    _tempIceGridVec.clear();
    _iceSeedIdxVec.clear();
    if (int iceSeedIdx = AGetSeedIndex(AICE_SHROOM); iceSeedIdx != -1)
        _iceSeedIdxVec.push_back(iceSeedIdx);
    if (int iceSeedIdx = AGetSeedIndex(AM_ICE_SHROOM); iceSeedIdx != -1)
        _iceSeedIdxVec.push_back(iceSeedIdx);
    _seedType = AICE_SHROOM;
    _coffeeSeedIdx = AGetSeedIndex(ACOFFEE_BEAN);
}

void AIceFiller::SetIceSeedList(const std::vector<int>& lst) {
    if (lst.empty()) {
        aLogger->Error("SetIceSeedList : 卡片列表不能为空");
        return;
    }
    _iceSeedIdxVec.clear();
    _seedType = lst[0] % AM_PEASHOOTER;
    for (auto seed : lst) {
        if (seed % AM_PEASHOOTER != _seedType) {
            aLogger->Error("SetIceSeedList : 请确保您选择的卡片类型相同");
            return;
        }
    }
    for (auto seedType : lst) {
        int iceIdx = AGetSeedIndex(seedType);
        if (iceIdx == -1) {
            aLogger->Error("SetIceSeedList : 您没有选择对应的卡片");
            continue;
        }
        _iceSeedIdxVec.push_back(iceIdx);
    }
}

void AIceFiller::EraseFromList(const std::vector<AGrid>& lst) {
    _fillIceGridVec = __AErase(_fillIceGridVec, lst);
}

void AIceFiller::EraseFromList(int row, int col) {
    EraseFromList({{row, col}});
}

void AIceFiller::MoveToListTop(const std::vector<AGrid>& lst) {
    _fillIceGridVec = __AMoveToTop(_fillIceGridVec, lst);
}

void AIceFiller::MoveToListTop(int row, int col) {
    MoveToListTop({{row, col}});
}

void AIceFiller::MoveToListBottom(const std::vector<AGrid>& lst) {
    _fillIceGridVec = __AMoveToBottom(_fillIceGridVec, lst);
}

void AIceFiller::MoveToListBottom(int row, int col) {
    MoveToListBottom({{row, col}});
}

void AIceFiller::Start(const std::vector<AGrid>& lst) {
    if (aFieldInfo.isNight) {
        aLogger->Error("AIceFiller 无法在夜间场地使用");
        return;
    }
    if (_iceSeedIdxVec.empty()) {
        aLogger->Error("AIceFiller 未找到寒冰菇卡片");
        return;
    }
    if (_coffeeSeedIdx == -1) {
        aLogger->Error("AIceFiller 未找到咖啡豆卡片");
        return;
    }
    _fillIceGridVec = lst;
    ATickRunnerWithNoStart::_Start([this]() { _Run(); }, ONLY_FIGHT);
}

void AIceFiller::_Run() {
    std::vector<AGrid> grids = _tempIceGridVec;
    grids.insert(grids.end(), _fillIceGridVec.begin(), _fillIceGridVec.end());
    auto seedIt = _iceSeedIdxVec.begin();
    for (auto grid : grids) {
        if (AAsm::GetPlantRejectType(AICE_SHROOM, grid.row - 1, grid.col - 1) != AAsm::NIL)
            continue;
        while (seedIt != _iceSeedIdxVec.end() && !AIsSeedUsable(AGetMainObject()->SeedArray() + *seedIt))
            ++seedIt;
        if (seedIt == _iceSeedIdxVec.end())
            break;
        ACard(*seedIt + 1, grid.row, grid.col);
        ++seedIt;
    }
}

void AIceFiller::Coffee(int row, int col) {
    if (_tempIceGridVec.empty() && _fillIceGridVec.empty()) {
        aLogger->Error("AIceFiller : 存冰列表还未设置");
        return;
    }

    // 尝试使用临时存冰
    APlant* ice = AGetPlantPtr(row, col, _seedType);
    auto icePtrs = AGetPlantPtrs(_tempIceGridVec, _seedType);
    for (int i = _tempIceGridVec.size() - 1; i >= 0; --i) {
        if (ice != nullptr)
            break;
        if (icePtrs[i] != nullptr) {
            ice = icePtrs[i];
            _tempIceGridVec.erase(_tempIceGridVec.begin() + i);
        }
    }

    // 尝试使用存冰列表
    if (ice == nullptr) {
        icePtrs = AGetPlantPtrs(_fillIceGridVec, _seedType);
        for (int i = _fillIceGridVec.size() - 1; i >= 0; --i) {
            if (icePtrs[i] == nullptr)
                continue;
            if (_priorityMode == GRID) {
                ice = icePtrs[i];
                break;
            }
            if (_priorityMode == HP && (ice == nullptr || icePtrs[i]->Hp() < ice->Hp()))
                ice = icePtrs[i];
        }
    }

    if (ice == nullptr) {
        aLogger->Error("Coffee : 未找到可用的存冰");
        return;
    }
    AAsm::ReleaseMouse();
    ACard(_coffeeSeedIdx + 1, ice->Row() + 1, ice->Col() + 1);
    AAsm::ReleaseMouse();
}

void AIceFiller::Coffee() {
    Coffee(0, 0);
}

/////////////////////////////////////////////////
//    PlantFixer
/////////////////////////////////////////////////

void APlantFixer::EraseFromList(const std::vector<AGrid>& lst) {
    _gridLst = __AErase(_gridLst, lst);
}

void APlantFixer::EraseFromList(int row, int col) {
    EraseFromList({{row, col}});
}

void APlantFixer::MoveToListTop(const std::vector<AGrid>& lst) {
    _gridLst = __AMoveToTop(_gridLst, lst);
}

void APlantFixer::MoveToListTop(int row, int col) {
    MoveToListTop({{row, col}});
}

void APlantFixer::MoveToListBottom(const std::vector<AGrid>& lst) {
    _gridLst = __AMoveToBottom(_gridLst, lst);
}

void APlantFixer::MoveToListBottom(int row, int col) {
    MoveToListBottom({{row, col}});
}

void APlantFixer::AutoSetList() {
    _gridLst.clear();
    for (auto &plant : AObjSelector(&APlant::Type, _plantType))
        _gridLst.push_back({plant.Row() + 1, plant.Col() + 1});
}

void APlantFixer::_UseSeed(int seed_index, int row, float col,
    bool isNeedShovel) {
    if (isNeedShovel) {
        // {_plantType, _plantType} 会转成 std::vector
        // 但是 {_plantType} 会转成 int
        ARemovePlant(row, col, {_plantType, _plantType});
    }
    ACard(seed_index + 1, row, col);
    if (_isUseCoffee)
        ACard(_coffeeSeedIdx + 1, row, col);
}

void APlantFixer::_GetSeedList() {
    _seedIdxVec.clear();
    int seedIndex;
    seedIndex = AGetSeedIndex(_plantType);
    if (-1 != seedIndex)
        _seedIdxVec.push_back(seedIndex);
    seedIndex = AGetSeedIndex(_plantType, true);

    if (-1 != seedIndex)
        _seedIdxVec.push_back(seedIndex);
    if (_seedIdxVec.size() == 0)
        aLogger->Error("您没有选择修补该植物的卡片！");
    _coffeeSeedIdx = AGetSeedIndex(ACOFFEE_BEAN);
}

void APlantFixer::Start(int plantType, const std::vector<AGrid>& lst,
    int fixHp) {
    if (plantType == ACOFFEE_BEAN) {
        aLogger->Error("PlantFixer 不支持修补咖啡豆");
        return;
    }

    if (plantType >= AGATLING_PEA) {
        aLogger->Error("修补植物类仅支持绿卡");
        return;
    }

    _plantType = plantType;
    _fixHp = fixHp;
    _GetSeedList();
    // 如果没有给出列表信息
    if (lst.size() == 0)
        AutoSetList();
    else
        _gridLst = lst;
    ATickRunnerWithNoStart::_Start([this]() { _Run(); }, ONLY_FIGHT);
}

void APlantFixer::_Run() {
    auto usableSeedIndexIter = _seedIdxVec.begin();
    if (usableSeedIndexIter == _seedIdxVec.end())
        return;

    if (_isUseCoffee) {
        if (_coffeeSeedIdx == -1)
            return;
        auto coffeeSeed = AGetMainObject()->SeedArray() + _coffeeSeedIdx;
        if (!AIsSeedUsable(coffeeSeed))
            return;
    }

    do {
        auto seedMemory = AGetMainObject()->SeedArray();
        seedMemory += *usableSeedIndexIter;
        if (AIsSeedUsable(seedMemory))
            break;
        ++usableSeedIndexIter;
    } while (usableSeedIndexIter != _seedIdxVec.end());

    // 没找到可用的卡片
    if (usableSeedIndexIter == _seedIdxVec.end())
        return;
    auto plantIdxVec = AGetPlantIndices(_gridLst, _plantType);
    AGrid needPlantGrid = {0, 0};
    int minHp = _fixHp;
    auto gridIter = _gridLst.begin();
    auto plantIdxIter = plantIdxVec.begin();
    for (; gridIter != _gridLst.end(); ++gridIter, ++plantIdxIter) {
        // 如果此处存在除植物类植物的植物
        if (*plantIdxIter == -2)
            continue;

        if (*plantIdxIter == -1) {
            if (AAsm::GetPlantRejectType(_plantType, gridIter->row - 1, gridIter->col - 1) != AAsm::NIL)
                continue;
            _UseSeed((*usableSeedIndexIter), gridIter->row, gridIter->col,
                false);
            return;
        }
        auto plant = AGetMainObject()->PlantArray();
        plant += *plantIdxIter;
        int plantHp = plant->Hp();
        // 如果当前生命值低于最小生命值，记录下来此植物的信息
        if (plantHp < minHp) {
            minHp = plantHp;
            needPlantGrid.row = gridIter->row;
            needPlantGrid.col = gridIter->col;
        }
    }

    // 如果有需要修补的植物且植物卡片能用则进行种植
    if (needPlantGrid.row) {
        // 种植植物
        _UseSeed((*usableSeedIndexIter), needPlantGrid.row,
            needPlantGrid.col, true);
    }
}
