#include "libavz.h"

std::unordered_set<int> ACobManager::_lockSet;
ATickRunner ACobManager::_tickRunner;

void ACobManager::_BeforeScript() {
    _next = 0;
    _lockSet.clear();
    AutoSetList();
}

void ACobManager::_Skip(int n) {
    _next = (_next + n) % _gridVec.size();
}

void ACobManager::Skip(int n) {
    if (_sequentialMode == PRIORITY) {
        aLogger->Error("Skip 在优先级模式下无效，请使用 MoveToListTop 和 MoveToListBottom 控制炮序");
        return;
    }
    _Skip(n);
}

// 获取屋顶炮飞行时间
int ACobManager::GetRoofFlyTime(int cobCol, float dropCol) {
    int dropX = static_cast<int>(dropCol * 80);
    int minDropX = _flyTimeData[cobCol - 1].minDropX;
    int minFlyTime = _flyTimeData[cobCol - 1].minFlyTime;
    if (dropX >= minDropX)
        return minFlyTime;
    else
        return minFlyTime + 1 - (dropX - (minDropX - 1)) / 32;
}

void ACobManager::_BasicFire(int cobIdx, int dropRow, float dropCol) {
    AAsm::ReleaseMouse();
    int x = 0;
    int y = 0;
    auto plant = AGetMainObject()->PlantArray() + cobIdx;
    if (plant->Type() != ACOB_CANNON || plant->State() != 37) {
        aLogger->Error("ACobManager 内部错误，请联系开发者修复");
        return;
    }
    aLogger->Info("Fire from {} to {}",
        AGrid(plant->Row() + 1, plant->Col() + 1), APosition(dropRow, dropCol));

    AGridToCoordinate(dropRow, dropCol, x, y);
    AAsm::Fire(x, y, cobIdx);
    AAsm::ReleaseMouse();
}

void ACobManager::_DelayFire(int delayTime, int cobIdx, int row, float col) {
    // 将操作动态插入操作队列
    _lockSet.insert(cobIdx);
    AConnect(ANowDelayTime(delayTime), [=] {
        _BasicFire(cobIdx, row, col);
        _lockSet.erase(cobIdx);
    });
}

// 用户自定义炮位置发炮：单发
void ACobManager::RawFire(int cobRow, int cobCol, int dropRow, float dropCol) {
    int index = AGetPlantIndex(cobRow, cobCol, ACOB_CANNON);
    if (index < 0) {
        aLogger->Error("请检查 {} 是否为炮", AGrid(cobRow, cobCol));
        return;
    }

    int recoverTime = AGetCobRecoverTime(index);
    if (recoverTime > 0) {
        aLogger->Error("位于 {} 的炮还有 {}cs 恢复", AGrid(cobRow, cobCol), recoverTime);
        return;
    }
    _BasicFire(index, dropRow, dropCol);
}

// 用户自定义炮位置发炮：多发
void ACobManager::RawFire(const std::vector<FireDrop>& lst) {
    for (const auto& each : lst)
        RawFire(each.cobRow, each.cobCol, each.dropRow, each.dropCol);
}

// 屋顶修正时间发炮，单发
void ACobManager::RawRoofFire(int cobRow, int cobCol, int dropRow, float dropCol) {
    if (!aFieldInfo.isRoof) {
        aLogger->Error("RawRoofFire : RawRoofFire 函数只适用于 RE 与 ME");
        return;
    }
    int index = AGetPlantIndex(cobRow, cobCol, ACOB_CANNON);
    if (index < 0) {
        aLogger->Error("请检查 {} 是否为炮", AGrid(cobRow, cobCol));
        return;
    }

    int recoverTime = AGetCobRecoverTime(index);
    int delayTime = 387 - GetRoofFlyTime(cobCol, dropCol);
    if (recoverTime > delayTime) {
        aLogger->Error("位于 {} 的炮还有 {}cs 恢复", AGrid(cobRow, cobCol), recoverTime - delayTime);
        return;
    }
    _DelayFire(delayTime, index, dropRow, dropCol);
}

// 屋顶修正时间发炮 多发
void ACobManager::RawRoofFire(const std::vector<FireDrop>& lst) {
    for (const auto& each : lst)
        RawRoofFire(each.cobRow, each.cobCol, each.dropRow, each.dropCol);
}

void ACobManager::Plant(int row, int col) {
    _tickRunner.Start([row, col]() {
        int ymjnpSeedIdx = AGetSeedIndex(ACOB_CANNON);
        int ymtseedIdx = AGetSeedIndex(AKERNEL_PULT);
        if (ymjnpSeedIdx == -1 || ymtseedIdx == -1)
            return;
        for (int tCol = col; tCol < col + 2; ++tCol) {
            if (AGetPlantIndex(row, tCol, AKERNEL_PULT) != -1)
                continue;
            auto seedPtr = AGetMainObject()->SeedArray() + ymtseedIdx;
            if (!AIsSeedUsable(seedPtr))
                return;
            ACard(ymtseedIdx + 1, row, tCol);
        }
        auto seedPtr = AGetMainObject()->SeedArray() + ymjnpSeedIdx;
        if (!AIsSeedUsable(seedPtr))
            return;
        ACard(ymjnpSeedIdx + 1, row, col);
        _tickRunner.Stop();
    });
}

// 用户重置炮列表
void ACobManager::SetList(const std::vector<AGrid>& lst) {
    _next = 0;
    // 重置炮列表
    _gridVec = lst;
    AGetPlantIndices(_gridVec, ACOB_CANNON, _indexVec);
    auto idxIter = _indexVec.begin();
    auto gridIter = _gridVec.begin();
    while (idxIter != _indexVec.end()) {
        if ((*idxIter) < 0) {
            aLogger->Error("resetFireList : 请检查 {} 位置是否为炮", *gridIter);
            return;
        }
        ++gridIter;
        ++idxIter;
    }
}

// 自动填充炮列表
// *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
void ACobManager::AutoSetList() {
    _gridVec.clear();
    _next = 0;
    AGrid cobGrid = {0, 0};
    auto PlantArray = AGetMainObject()->PlantArray();
    for (int index = 0; index < AGetMainObject()->PlantCountMax(); ++index) {
        if (!PlantArray[index].IsCrushed() && !PlantArray[index].IsDisappeared()
            && PlantArray[index].Type() == ACOB_CANNON) {
            cobGrid = {PlantArray[index].Row() + 1, PlantArray[index].Col() + 1};
            _gridVec.push_back(cobGrid);
        }
    }
    // 对得到的炮的位置进行排序
    std::sort(_gridVec.begin(), _gridVec.end());
    // 得到对应的炮的序号
    AGetPlantIndices(_gridVec, ACOB_CANNON, _indexVec);
}

void ACobManager::SetNext(int tempNext) {
    if (_sequentialMode == PRIORITY) {
        aLogger->Error("SetNext 在优先级模式下无效，请使用 MoveToListTop 和 MoveToListBottom 控制炮序");
        return;
    }

    if (tempNext > _gridVec.size()) {
        aLogger->Error("SetNext : 本炮列表中一共有 {} 门炮，您设的参数已溢出", _gridVec.size());
        return;
    }
    _next = tempNext - 1;
}

void ACobManager::SetNext(int row, int col) {
    if (_sequentialMode == PRIORITY) {
        aLogger->Error("SetNext 在优先级模式下无效，请使用 MoveToListTop 和 MoveToListBottom 控制炮序");
        return;
    }

    AGrid grid = {row, col};
    auto iter = std::find(_gridVec.begin(), _gridVec.end(), grid);

    if (iter != _gridVec.end()) {
        _next = iter - _gridVec.begin();
    } else {
        aLogger->Error("SetNext : 请检查 {} 是否在本炮列表中", grid);
        return;
    }
}

void ACobManager::EraseFromList(const std::vector<AGrid>& lst) {
    SetList(__AErase(_gridVec, lst));
}

void ACobManager::EraseFromList(int row, int col) {
    EraseFromList({{row, col}});
}

void ACobManager::MoveToListTop(const std::vector<AGrid>& lst) {
    if (_sequentialMode != PRIORITY) {
        aLogger->Error("MoveToListTop 在非优先级模式下无效，请使用 SetNext 和 Skip 控制炮序");
        return;
    }
    SetList(__AMoveToTop(_gridVec, lst));
}

void ACobManager::MoveToListTop(int row, int col) {
    MoveToListTop({{row, col}});
}

void ACobManager::MoveToListBottom(const std::vector<AGrid>& lst) {
    if (_sequentialMode != PRIORITY) {
        aLogger->Error("MoveToListBottom 在非优先级模式下无效，请使用 SetNext 和 Skip 控制炮序");
        return;
    }
    SetList(__AMoveToBottom(_gridVec, lst));
}

void ACobManager::MoveToListBottom(int row, int col) {
    MoveToListBottom({{row, col}});
}

void ACobManager::FixLatest() {
    if (_lastestMsg.vecIndex == -1) {
        aLogger->Error("FixLatest ：您尚未使用炮");
        return;
    }
    _lastestMsg.isWritable = false; // 锁定信息
    int delayTime = _lastestMsg.fireTime + 205 - AGetMainObject()->GameClock();
    if (delayTime < 0)
        delayTime = 0;
    AConnect(ANowDelayTime(delayTime), [this]() {
        _lastestMsg.isWritable = true; // 解锁信息
        ARemovePlant(_gridVec[_lastestMsg.vecIndex].row, _gridVec[_lastestMsg.vecIndex].col, {AYMJNP_47, AYMJNP_47});
        Plant(_gridVec[_lastestMsg.vecIndex].row, _gridVec[_lastestMsg.vecIndex].col);
    });
}

int ACobManager::_GetRecoverTimeVec() {
    auto mainObject = AGetMainObject();
    int cobIdx = _indexVec[_next];
    auto cobPtr = mainObject->PlantArray() + cobIdx;
    if (cobIdx < 0 || cobIdx >= mainObject->PlantCountMax()
        || cobPtr->Type() != ACOB_CANNON
        || cobPtr->IsDisappeared() || cobPtr->IsCrushed()) {
        int index = AGetPlantIndex(_gridVec[_next].row, _gridVec[_next].col, ACOB_CANNON);
        if (index < 0) // 找不到本来位置的炮
            return NO_EXIST_RECOVER_TIME;
        _indexVec[_next] = index;
    }
    return AGetCobRecoverTime(_indexVec[_next]);
}

int ACobManager::_UpdateNextCob(bool isDelayFire, float dropCol, bool isShowError) {
    if (_sequentialMode == PRIORITY)
        _next = 0;
    int minRecoverTime = 0xFFFF, minRecoverOffset = 0;
    int recoverTime;
    // 此变量用于选择最短时间内可用的炮
    int tmpNext = _next;
    // 要迭代的次数
    // 对于时间模式, 需要迭代整个炮列表
    // 对于空间模式, 需要迭代一次
    int iterCnt = _sequentialMode == SPACE ? 1 : _indexVec.size();
    // 开始遍历
    for (int i = 0; i < iterCnt; ++i, _Skip(1)) {
        // 被锁定的炮不允许发射
        if (_lockSet.find(_indexVec[_next]) != _lockSet.end())
            continue;
        recoverTime = _GetRecoverTimeVec();
        // 炮不存在直接跳过
        if (recoverTime == NO_EXIST_RECOVER_TIME)
            continue;
        int roofOffsetTime = dropCol < 0 ? 0 : (387 - GetRoofFlyTime(_gridVec[_next].col, dropCol));
        recoverTime -= roofOffsetTime;
        // 这里返回 roofOffsetTime 目的是直接让 RoofFire 使用, 对于普通的炮, roofOffsetTime 一直为 0
        if (recoverTime <= 0)
            return roofOffsetTime;
        if (minRecoverTime > recoverTime) {
            minRecoverTime = recoverTime;
            minRecoverOffset = roofOffsetTime;
            tmpNext = _next;
        }
    }
    _next = tmpNext;

    // 以上的判断条件已经解决炮是否存在以及炮当前时刻是否能用的问题
    // 如果炮当前时刻不能使用但是为 RecoverFire 时则不会报错，
    // 并返回恢复时间
    if (isDelayFire)
        return minRecoverTime + minRecoverOffset;
    if (isShowError) {
        std::string error_str = (_sequentialMode == SPACE ? "SPACE 模式 : " : "TIME 模式 : 未找到能够发射的炮，");
        error_str += "位于 {} 的第 {} 门炮还有 {}cs 恢复";
        aLogger->Error(std::move(error_str), _gridVec[_next], _next + 1, minRecoverTime);
    }
    return NO_EXIST_RECOVER_TIME;
}

APlant* ACobManager::_BasicGetPtr(bool isRecover, float col) {
    int tmpIdx = _next;
    auto tmpSeqMode = _sequentialMode;
    if (_sequentialMode == SPACE)
        _sequentialMode = TIME;
    auto ret = _UpdateNextCob(isRecover, col, false);
    _next = tmpIdx;
    _sequentialMode = tmpSeqMode;
    return ret == NO_EXIST_RECOVER_TIME ? nullptr : AGetMainObject()->PlantArray() + _indexVec[_next];
}

__ANodiscard std::vector<ACobManager::RecoverInfo> ACobManager::_BasicGetRecoverList(float col) {
    int iterCnt = _indexVec.size();
    std::vector<RecoverInfo> ret(iterCnt);
    int tmpNext = _next;
    _next = 0;
    auto plantArray = AGetMainObject()->PlantArray();
    // 开始遍历
    for (int i = 0; i < iterCnt; ++i, _Skip(1)) {
        // 被锁定的炮不允许发射
        if (_lockSet.find(_indexVec[_next]) != _lockSet.end()) {
            ret[i].ptr = nullptr;
            ret[i].recoverTime = -1;
            continue;
        }
        ret[i].recoverTime = _GetRecoverTimeVec();
        // 炮不存在直接跳过
        if (ret[i].recoverTime == NO_EXIST_RECOVER_TIME) {
            ret[i].ptr = nullptr;
            continue;
        }
        ret[i].ptr = plantArray + _indexVec[i];
        int roofOffsetTime = col < 0 ? 0 : (387 - GetRoofFlyTime(_gridVec[_next].col, col));
        ret[i].recoverTime = std::max(0, ret[i].recoverTime - roofOffsetTime);
    }
    _next = tmpNext;
    return ret;
}

APlant* ACobManager::GetUsablePtr() {
    return _BasicGetPtr(false, -1);
}

APlant* ACobManager::GetRoofUsablePtr(float col) {
    if (col < 0 || col > 10) {
        aLogger->Error("ACobManager::GetNextRoofUsable 参数溢出, 范围为 [0, 10]");
        col = -1;
    }
    return _BasicGetPtr(false, col);
}

__ANodiscard APlant* ACobManager::GetRecoverPtr() {
    return _BasicGetPtr(true, -1);
}

__ANodiscard APlant* ACobManager::GetRoofRecoverPtr(float col) {
    if (col < 0 || col > 10) {
        aLogger->Error("ACobManager::GetNextRoofUsable 参数溢出, 范围为 [0, 10]");
        col = -1;
    }
    return _BasicGetPtr(true, col);
}

__ANodiscard std::vector<ACobManager::RecoverInfo> ACobManager::GetRecoverList() {
    return _BasicGetRecoverList(-1);
}

__ANodiscard std::vector<ACobManager::RecoverInfo> ACobManager::GetRoofRecoverList(float col) {
    if (col < 0 || col > 10) {
        aLogger->Error("ACobManager::GetRoofRecoverList 参数溢出, 范围为 [0, 10]");
        col = -1;
    }
    return _BasicGetRecoverList(col);
}

__ANodiscard std::vector<APlant*> ACobManager::GetUsableList() {
    std::vector<APlant*> ret;
    for (auto&& [ptr, recoverTime] : GetRecoverList())
        if (ptr != nullptr && recoverTime == 0)
            ret.push_back(ptr);
    return ret;
}

__ANodiscard std::vector<APlant*> ACobManager::GetRoofUsableList(float col) {
    std::vector<APlant*> ret;
    for (auto&& [ptr, recoverTime] : GetRoofRecoverList(col))
        if (ptr != nullptr && recoverTime == 0)
            ret.push_back(ptr);
    return ret;
}

// 发炮函数：单发
int ACobManager::Fire(int row, float col) {
    if (_gridVec.size() == 0) {
        aLogger->Error("Fire : 您尚未为此炮列表分配炮");
        return -1;
    }
    if (_UpdateNextCob() == NO_EXIST_RECOVER_TIME)
        return -1;
    _BasicFire(_indexVec[_next], row, col);
    _UpdateLastestMsg(AGetMainObject()->GameClock(), _next);
    auto tmpNext = _next;
    _Skip(1);
    return tmpNext;
}

// 发炮函数：多发
std::vector<int> ACobManager::Fire(const std::vector<APosition>& lst) {
    std::vector<int> vec;
    for (const auto& each : lst)
        vec.push_back(Fire(each.row, each.col));
    return vec;
}

// 恢复发炮
int ACobManager::_RecoverBasicFire(int row, float col, bool isRoof) {
    if (_gridVec.size() == 0) {
        aLogger->Error("RecoverFire : 您尚未为此炮列表分配炮");
        return -1;
    }
    int delayTime = _UpdateNextCob(true, isRoof ? col : -1);
    if (delayTime == NO_EXIST_RECOVER_TIME)
        return -1;
    _DelayFire(delayTime, _indexVec[_next], row, col);
    _UpdateLastestMsg(AGetMainObject()->GameClock() + delayTime, _next);
    auto tmpNext = _next;
    _Skip(1);
    return tmpNext;
}

int ACobManager::RecoverFire(int row, float col) {
    return _RecoverBasicFire(row, col, false);
}

std::vector<int> ACobManager::RecoverFire(const std::vector<APosition>& lst) {
    std::vector<int> vec;
    for (const auto& each : lst)
        vec.push_back(_RecoverBasicFire(each.row, each.col, false));
    return vec;
}

int ACobManager::RoofFire(int row, float col) {
    if (!aFieldInfo.isRoof) {
        aLogger->Error("RoofFire : RoofFire 函数只适用于 RE 与 ME ");
        return -1;
    }
    if (_gridVec.empty()) {
        aLogger->Error("RoofFire : 您尚未为此炮列表分配炮");
        return -1;
    }

    int delayTime = _UpdateNextCob(false, col);
    if (delayTime == NO_EXIST_RECOVER_TIME)
        return -1;

    _DelayFire(delayTime, _indexVec[_next], row, col);
    _UpdateLastestMsg(AGetMainObject()->GameClock() + delayTime, _next);
    auto tmpNext = _next;
    _Skip(1);
    return tmpNext;
}

std::vector<int> ACobManager::RoofFire(const std::vector<APosition>& lst) {
    std::vector<int> vec;
    for (const auto& each : lst)
        vec.push_back(RoofFire(each.row, each.col));
    return vec;
}

int ACobManager::RecoverRoofFire(int row, float col) {
    if (!aFieldInfo.isRoof) {
        aLogger->Error("RecoverRoofFire : RecoverRoofFire 函数只适用于 RE 与 ME");
        return -1;
    }
    return _RecoverBasicFire(row, col, true);
}

std::vector<int> ACobManager::RecoverRoofFire(const std::vector<APosition>& lst) {
    std::vector<int> vec;
    for (const auto& each : lst)
        vec.push_back(RecoverRoofFire(each.row, each.col));
    return vec;
}
