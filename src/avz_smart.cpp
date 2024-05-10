/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description:
 */
#include "avz_smart.h"
#include "avz_asm.h"
#include "avz_card.h"
#include "avz_click.h"
#include "avz_connector.h"
#include "avz_memory.h"
#include "avz_time_queue.h"
#include <algorithm>
#include <unordered_set>

////////////////////////////////////////
//  ACobManager
////////////////////////////////////////

std::unordered_set<int> ACobManager::_lockSet; // 锁定的炮
ATickRunner ACobManager::_tickRunner;
ACobManager::RoofFlyTime ACobManager::_flyTimeData[8] = {
    {515, 359},
    {499, 362},
    {515, 364},
    {499, 367},
    {515, 369},
    {499, 372},
    {511, 373},
    {511, 373},
};

void ACobManager::_BeforeScript()
{
    _next = 0;
    _lockSet.clear();
    AutoSetList();
}

void ACobManager::_Skip(int n)
{
    _next = (_next + n) % _gridVec.size();
}

void ACobManager::Skip(int n)
{
    if (_sequentialMode == PRIORITY) {
        __aig.loggerPtr->Error("Skip 在优先级模式下无效，请使用 MoveToListTop 和 MoveToListBottom 控制炮序");
        return;
    }
    _Skip(n);
}

// 获取屋顶炮飞行时间
int ACobManager::GetRoofFlyTime(int cobCol, float dropCol)
{
    // 得到落点对应的横坐标
    int dropX = static_cast<int>(dropCol * 80);
    // 得到该列炮最小飞行时间对应的最小的横坐标
    int minDropX = _flyTimeData[cobCol - 1].minDropX;
    // 得到最小的飞行时间
    int minFlyTime = _flyTimeData[cobCol - 1].minFlyTime;
    // 返回飞行时间
    return (dropX >= minDropX
            ? minFlyTime
            : (minFlyTime + 1 - (dropX - (minDropX - 1)) / 32));
}

void ACobManager::_BasicFire(int cobIdx, int dropRow, float dropCol)
{
    AAsm::ReleaseMouse();
    int x = 0;
    int y = 0;
    auto plant = AGetMainObject()->PlantArray() + cobIdx;
    if (plant->Type() != ACOB_CANNON || plant->State() != 37) {
        __aig.loggerPtr->Error("ACobManager 内部错误，请联系开发者修复");
        return;
    }
    auto&& pattern = __aig.loggerPtr->GetPattern();
    __aig.loggerPtr->Info(
        "Fire from (" + pattern + ", " + pattern + ") to (" + pattern + ", " + pattern + ")",
        plant->Row() + 1, plant->Col() + 1, dropRow, dropCol);

    AGridToCoordinate(dropRow, dropCol, x, y);
    AAsm::Fire(x, y, cobIdx);
    AAsm::ReleaseMouse();
}

void ACobManager::_DelayFire(int delayTime, int cobIdx, int row, float col)
{
    // 将操作动态插入操作队列
    _lockSet.insert(cobIdx);
    AConnect(ANowDelayTime(delayTime), [=] {
        _BasicFire(cobIdx, row, col);
        _lockSet.erase(cobIdx);
    });
}

// 用户自定义炮位置发炮：单发
void ACobManager::RawFire(int cobRow, int cobCol, int dropRow, float dropCol)
{
    auto&& pattern = __aig.loggerPtr->GetPattern();
    int index = AGetPlantIndex(cobRow, cobCol, ACOB_CANNON);
    if (index < 0) {
        __aig.loggerPtr->Error("请检查 (" + pattern + ", " + pattern + ") 是否为炮", cobRow, cobCol);
        return;
    }

    int recoverTime = AGetCobRecoverTime(index);
    if (recoverTime > 0) {
        __aig.loggerPtr->Error("位于 (" + pattern + ", " + pattern + ") 的炮还有 " + pattern + " cs 恢复",
            cobRow,
            cobCol,
            recoverTime);
        return;
    }
    _BasicFire(index, dropRow, dropCol);
}

// 用户自定义炮位置发炮：多发
void ACobManager::RawFire(const std::vector<FireDrop>& lst)
{
    for (const auto& each : lst) {
        RawFire(each.cobRow, each.cobCol, each.dropRow, each.dropCol);
    }
}

// 屋顶修正时间发炮，单发
void ACobManager::RawRoofFire(int cobRow, int cobCol, int dropRow, float dropCol)
{
    auto&& pattern = __aig.loggerPtr->GetPattern();
    if (!aFieldInfo.isRoof) {
        __aig.loggerPtr->Error("RawRoofFire : RawRoofFire 函数只适用于 RE 与 ME");
        return;
    }
    int index = AGetPlantIndex(cobRow, cobCol, ACOB_CANNON);
    if (index < 0) {
        __aig.loggerPtr->Error("请检查 (" + pattern + ", " + pattern + ") 是否为炮", cobRow, cobCol);
        return;
    }

    int recoverTime = AGetCobRecoverTime(index);
    int delayTime = 387 - GetRoofFlyTime(cobCol, dropCol);
    if (recoverTime > delayTime) {
        __aig.loggerPtr->Error("位于 (" + pattern + ", " + pattern + ") 的炮还有 " + pattern + " cs 恢复",
            cobRow,
            cobCol,
            recoverTime - delayTime);
        return;
    }
    _DelayFire(delayTime, index, dropRow, dropCol);
}

// 屋顶修正时间发炮 多发
void ACobManager::RawRoofFire(const std::vector<FireDrop>& lst)
{
    for (const auto& each : lst) {
        RawRoofFire(each.cobRow, each.cobCol, each.dropRow, each.dropCol);
    }
}

void ACobManager::Plant(int row, int col)
{
    _tickRunner.Start([row, col]() {
        int ymjnpSeedIdx = AGetSeedIndex(ACOB_CANNON);
        int ymtseedIdx = AGetSeedIndex(AKERNEL_PULT);
        if (ymjnpSeedIdx == -1 || ymtseedIdx == -1) {
            return;
        }
        for (int tCol = col; tCol < col + 2; ++tCol) {
            if (AGetPlantIndex(row, tCol, AKERNEL_PULT) != -1) {
                continue;
            }
            auto seedPtr = AGetMainObject()->SeedArray() + ymtseedIdx;
            if (!AIsSeedUsable(seedPtr)) {
                return;
            }
            ACard(ymtseedIdx + 1, row, tCol);
        }
        auto seedPtr = AGetMainObject()->SeedArray() + ymjnpSeedIdx;
        if (!AIsSeedUsable(seedPtr)) {
            return;
        }
        ACard(ymjnpSeedIdx + 1, row, col);
        _tickRunner.Stop();
    });
}

// 用户重置炮列表
void ACobManager::SetList(const std::vector<AGrid>& lst)
{
    _next = 0;
    // 重置炮列表
    _gridVec = lst;
    AGetPlantIndices(_gridVec, ACOB_CANNON, _indexVec);
    auto idxIter = _indexVec.begin();
    auto gridIter = _gridVec.begin();
    while (idxIter != _indexVec.end()) {
        if ((*idxIter) < 0) {
            __aig.loggerPtr->Error(
                "resetFireList : 请检查 (" + std::to_string(gridIter->row)
                + ", " + std::to_string(gridIter->col) + ") 位置是否为炮");
            return;
        }

        ++gridIter;
        ++idxIter;
    }
}

// 自动填充炮列表
// *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
void ACobManager::AutoSetList()
{
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

void ACobManager::SetNext(int tempNext)
{
    if (_sequentialMode == PRIORITY) {
        __aig.loggerPtr->Error("SetNext 在优先级模式下无效，请使用 MoveToListTop 和 MoveToListBottom 控制炮序");
        return;
    }

    auto&& pattern = __aig.loggerPtr->GetPattern();
    if (tempNext > _gridVec.size()) {
        __aig.loggerPtr->Error(
            "SetNext : 本炮列表中一共有 " + pattern + " 门炮，您设的参数已溢出",
            _gridVec.size());
        return;
    }
    _next = tempNext - 1;
}

void ACobManager::SetNext(int row, int col)
{
    if (_sequentialMode == PRIORITY) {
        __aig.loggerPtr->Error("SetNext 在优先级模式下无效，请使用 MoveToListTop 和 MoveToListBottom 控制炮序");
        return;
    }

    AGrid grid = {row, col};
    auto iter = std::find(_gridVec.begin(), _gridVec.end(), grid);

    if (iter != _gridVec.end()) {
        _next = iter - _gridVec.begin();
    } else {
        auto&& pattern = __aig.loggerPtr->GetPattern();
        __aig.loggerPtr->Error("SetNext : 请检查 (" + pattern + ", " + pattern + ") 是否在本炮列表中", row, col);
        return;
    }
}

void ACobManager::MoveToListTop(const std::vector<AGrid>& lst)
{
    if (_sequentialMode != PRIORITY) {
        __aig.loggerPtr->Error("MoveToListTop 在非优先级模式下无效，请使用 SetNext 和 Skip 控制炮序");
        return;
    } else {
        std::set<AGrid> tmpGridVec(_gridVec.begin(), _gridVec.end());
        for (auto&& grid : lst) {
            if (!tmpGridVec.contains(grid)) {
                auto&& pattern = __aig.loggerPtr->GetPattern();
                __aig.loggerPtr->Error("MoveToListTop : 请检查 (" + pattern + ", " + pattern + ") 是否在本炮列表中", grid.row, grid.col);
                return;
            }
        }
    }

    std::set<AGrid> tmpLst(lst.begin(), lst.end());
    std::vector<AGrid> tmpGridVec;
    tmpGridVec.reserve(_gridVec.size());
    tmpGridVec.insert(tmpGridVec.end(), lst.begin(), lst.end());
    for (auto&& grid : _gridVec) {
        if (!tmpLst.contains(grid)) {
            tmpGridVec.emplace_back(grid);
        }
    }
    SetList(tmpGridVec);
}

void ACobManager::MoveToListTop(int row, int col)
{
    MoveToListTop({{row, col}});
}

void ACobManager::MoveToListBottom(const std::vector<AGrid>& lst)
{
    if (_sequentialMode != PRIORITY) {
        __aig.loggerPtr->Error("MoveToListBottom 在非优先级模式下无效，请使用 SetNext 和 Skip 控制炮序");
        return;
    } else {
        std::set<AGrid> tmpGridVec(_gridVec.begin(), _gridVec.end());
        for (auto&& grid : lst) {
            if (!tmpGridVec.contains(grid)) {
                auto&& pattern = __aig.loggerPtr->GetPattern();
                __aig.loggerPtr->Error("MoveToListTop : 请检查 (" + pattern + ", " + pattern + ") 是否在本炮列表中", grid.row, grid.col);
                return;
            }
        }
    }

    std::set<AGrid> tmpLst(lst.begin(), lst.end());
    std::vector<AGrid> tmpGridVec;
    tmpGridVec.reserve(_gridVec.size());
    for (auto&& grid : _gridVec) {
        if (!tmpLst.contains(grid)) {
            tmpGridVec.emplace_back(grid);
        }
    }
    tmpGridVec.insert(tmpGridVec.end(), lst.begin(), lst.end());
    SetList(tmpGridVec);
}

void ACobManager::MoveToListBottom(int row, int col)
{
    MoveToListBottom({{row, col}});
}

void ACobManager::FixLatest()
{
    if (_lastestMsg.vecIndex == -1) {
        __aig.loggerPtr->Error("FixLatest ：您尚未使用炮");
        return;
    }
    _lastestMsg.isWritable = false; // 锁定信息
    int delayTime = _lastestMsg.fireTime + 205 - AGetMainObject()->GameClock();
    if (delayTime < 0) {
        delayTime = 0;
    }
    AConnect(ANowDelayTime(delayTime), [this]() {
        _lastestMsg.isWritable = true; // 解锁信息
        ARemovePlant(_gridVec[_lastestMsg.vecIndex].row, _gridVec[_lastestMsg.vecIndex].col, {AYMJNP_47, AYMJNP_47});
        Plant(_gridVec[_lastestMsg.vecIndex].row, _gridVec[_lastestMsg.vecIndex].col);
    });
}

int ACobManager::_GetRecoverTimeVec()
{
    auto mainObject = AGetMainObject();
    int cobIdx = _indexVec[_next];
    auto cobPtr = mainObject->PlantArray() + cobIdx;
    if (cobIdx < 0 || cobIdx >= mainObject->PlantCountMax()
        || cobPtr->Type() != ACOB_CANNON
        || cobPtr->IsDisappeared() || cobPtr->IsCrushed()) {
        int index = AGetPlantIndex(_gridVec[_next].row, _gridVec[_next].col, ACOB_CANNON);
        if (index < 0) { // 找不到本来位置的炮
            return NO_EXIST_RECOVER_TIME;
        }
        _indexVec[_next] = index;
    }
    return AGetCobRecoverTime(_indexVec[_next]);
}

int ACobManager::_UpdateNextCob(bool isDelayFire, float dropCol, bool isShowError)
{
    if (_sequentialMode == PRIORITY) {
        _next = 0;
    }
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
        if (_lockSet.find(_indexVec[_next]) != _lockSet.end()) {
            continue;
        }
        recoverTime = _GetRecoverTimeVec();
        // 炮不存在直接跳过
        if (recoverTime == NO_EXIST_RECOVER_TIME) {
            continue;
        }
        int roofOffsetTime = dropCol < 0 ? 0 : (387 - GetRoofFlyTime(_gridVec[_next].col, dropCol));
        recoverTime -= roofOffsetTime;
        // 这里返回 roofOffsetTime 目的是直接让 RoofFire 使用, 对于普通的炮, roofOffsetTime 一直为 0
        if (recoverTime <= 0) {
            return roofOffsetTime;
        }
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
    if (isDelayFire) {
        return minRecoverTime + minRecoverOffset;
    }
    if (isShowError) {
        std::string error_str = (_sequentialMode == SPACE ? "SPACE 模式 : " : "TIME 模式 : 未找到能够发射的炮，");
        error_str += "位于 (" + std::to_string(_gridVec[_next].row) + ", " + std::to_string(_gridVec[_next].col) + ") 的第 "
            + std::to_string(_next + 1) + " 门炮还有 " + std::to_string(minRecoverTime) + "cs 恢复";
        __aig.loggerPtr->Error(std::move(error_str));
    }
    return NO_EXIST_RECOVER_TIME;
}

APlant* ACobManager::_BasicGetPtr(bool isRecover, float col)
{
    int tmpIdx = _next;
    auto tmpSeqMode = _sequentialMode;
    if (_sequentialMode == SPACE) {
        _sequentialMode = TIME;
    }
    auto ret = _UpdateNextCob(isRecover, col, false);
    _next = tmpIdx;
    _sequentialMode = tmpSeqMode;
    return ret == NO_EXIST_RECOVER_TIME ? nullptr : AGetMainObject()->PlantArray() + _indexVec[_next];
}

__ANodiscard std::vector<ACobManager::RecoverInfo> ACobManager::_BasicGetRecoverList(float col)
{
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

APlant* ACobManager::GetUsablePtr()
{
    return _BasicGetPtr(false, -1);
}

APlant* ACobManager::GetRoofUsablePtr(float col)
{
    if (col < 0 || col > 10) {
        __aig.loggerPtr->Error("ACobManager::GetNextRoofUsable 参数溢出, 范围为 [0, 10]");
        col = -1;
    }
    return _BasicGetPtr(false, col);
}

__ANodiscard APlant* ACobManager::GetRecoverPtr()
{
    return _BasicGetPtr(true, -1);
}

__ANodiscard APlant* ACobManager::GetRoofRecoverPtr(float col)
{
    if (col < 0 || col > 10) {
        __aig.loggerPtr->Error("ACobManager::GetNextRoofUsable 参数溢出, 范围为 [0, 10]");
        col = -1;
    }
    return _BasicGetPtr(true, col);
}

__ANodiscard std::vector<ACobManager::RecoverInfo> ACobManager::GetRecoverList()
{
    return _BasicGetRecoverList(-1);
}

__ANodiscard std::vector<ACobManager::RecoverInfo> ACobManager::GetRoofRecoverList(float col)
{
    if (col < 0 || col > 10) {
        __aig.loggerPtr->Error("ACobManager::GetRoofRecoverList 参数溢出, 范围为 [0, 10]");
        col = -1;
    }
    return _BasicGetRecoverList(col);
}

__ANodiscard std::vector<APlant*> ACobManager::GetUsableList()
{
    std::vector<APlant*> ret;
    for (auto&& [ptr, recoverTime] : GetRecoverList()) {
        if (ptr != nullptr && recoverTime == 0) {
            ret.push_back(ptr);
        }
    }
    return ret;
}

__ANodiscard std::vector<APlant*> ACobManager::GetRoofUsableList(float col)
{
    std::vector<APlant*> ret;
    for (auto&& [ptr, recoverTime] : GetRoofRecoverList(col)) {
        if (ptr != nullptr && recoverTime == 0) {
            ret.push_back(ptr);
        }
    }
    return ret;
}

// 发炮函数：单发
int ACobManager::Fire(int row, float col)
{
    if (_gridVec.size() == 0) {
        __aig.loggerPtr->Error("Fire : 您尚未为此炮列表分配炮");
        return -1;
    }
    if (_UpdateNextCob() == NO_EXIST_RECOVER_TIME) {
        return -1;
    }
    _BasicFire(_indexVec[_next], row, col);
    _UpdateLastestMsg(AGetMainObject()->GameClock(), _next);
    auto tmpNext = _next;
    _Skip(1);
    return tmpNext;
}

// 发炮函数：多发
std::vector<int> ACobManager::Fire(const std::vector<APosition>& lst)
{
    std::vector<int> vec;
    for (const auto& each : lst) {
        vec.push_back(Fire(each.row, each.col));
    }
    return vec;
}

// 恢复发炮
int ACobManager::_RecoverBasicFire(int row, float col, bool isRoof)
{
    if (_gridVec.size() == 0) {
        __aig.loggerPtr->Error("RecoverFire : 您尚未为此炮列表分配炮");
        return -1;
    }
    int delayTime = _UpdateNextCob(true, isRoof ? col : -1);
    if (delayTime == NO_EXIST_RECOVER_TIME) {
        return -1;
    }
    _DelayFire(delayTime, _indexVec[_next], row, col);
    _UpdateLastestMsg(AGetMainObject()->GameClock() + delayTime, _next);
    auto tmpNext = _next;
    _Skip(1);
    return tmpNext;
}

int ACobManager::RecoverFire(int row, float col)
{
    return _RecoverBasicFire(row, col, false);
}

std::vector<int> ACobManager::RecoverFire(const std::vector<APosition>& lst)
{
    std::vector<int> vec;
    for (const auto& each : lst) {
        vec.push_back(_RecoverBasicFire(each.row, each.col, false));
    }
    return vec;
}

int ACobManager::RoofFire(int row, float col)
{
    if (!aFieldInfo.isRoof) {
        __aig.loggerPtr->Error("RoofFire : RoofFire 函数只适用于 RE 与 ME ");
        return -1;
    }
    if (_gridVec.empty()) {
        __aig.loggerPtr->Error("RoofFire : 您尚未为此炮列表分配炮");
        return -1;
    }

    int delayTime = _UpdateNextCob(false, col);
    if (delayTime == NO_EXIST_RECOVER_TIME) {
        return -1;
    }

    _DelayFire(delayTime, _indexVec[_next], row, col);
    _UpdateLastestMsg(AGetMainObject()->GameClock() + delayTime, _next);
    auto tmpNext = _next;
    _Skip(1);
    return tmpNext;
}

std::vector<int> ACobManager::RoofFire(const std::vector<APosition>& lst)
{
    std::vector<int> vec;
    for (const auto& each : lst) {
        vec.push_back(RoofFire(each.row, each.col));
    }
    return vec;
}

int ACobManager::RecoverRoofFire(int row, float col)
{
    if (!aFieldInfo.isRoof) {
        __aig.loggerPtr->Error("RecoverRoofFire : RecoverRoofFire 函数只适用于 RE 与 ME");
        return -1;
    }
    return _RecoverBasicFire(row, col, true);
}

std::vector<int> ACobManager::RecoverRoofFire(const std::vector<APosition>& lst)
{
    std::vector<int> vec;
    for (const auto& each : lst) {
        vec.push_back(RecoverRoofFire(each.row, each.col));
    }
    return vec;
}

////////////////////////////////////////
//  ItemCollector
////////////////////////////////////////

AItemCollector::AItemCollector()
{
    _types.fill(true);
}

void AItemCollector::_EnterFight()
{
    AItemCollector::Start();
}

void AItemCollector::SetInterval(int timeInterval)
{
    if (timeInterval < 1) {
        auto&& pattern = __aig.loggerPtr->GetPattern();
        __aig.loggerPtr->Error("自动收集类时间间隔范围为:[1, 正无穷], 你现在设定的参数为 " + pattern, timeInterval);
        return;
    }
    this->_timeInterval = timeInterval;
}

void AItemCollector::SetTypeList(const std::vector<int>& types)
{
    _types.fill(false);
    for (auto type : types) {
        if (type < 1 && type >= _TYPE_SIZE) {
            AGetInternalLogger()->Warning("AItemCollector::SetTypeList : 您设置的收集物类型: {} 不存在", type);
            continue;
        }
        _types[type] = true;
    }
}

void AItemCollector::Start()
{
    ATickRunnerWithNoStart::_Start([this] { _Run(); }, ONLY_FIGHT);
}

void AItemCollector::_Run()
{
    if (AGetMainObject()->GameClock() % _timeInterval != 0 || //
        AGetMainObject()->MouseAttribution()->Type() != 0) {
        return;
    }

    auto item = AGetMainObject()->ItemArray();
    int total = AGetMainObject()->ItemTotal();
    int collectIdx = -1;
    for (int index = 0; index < total; ++index, ++item) {
        int type = item->Type();
        if (item->IsCollected() || item->IsDisappeared() || !_types[item->Type()]) {
            continue;
        }
        collectIdx = index;
        if (ARangeIn(type, {4, 5, 6})) { // 优先采集阳光
            break;
        }
    }
    if (collectIdx == -1) { // 没有要收集的物品
        return;
    }

    item = AGetMainObject()->ItemArray() + collectIdx;
    float itemX = item->Abscissa();
    float itemY = item->Ordinate();
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

void AIceFiller::SetIceSeedList(const std::vector<int>& lst)
{
    // 未运行即进行检查是否为冰卡
    for (const auto& seedType : lst) {
        if (seedType != AICE_SHROOM && seedType != AM_ICE_SHROOM) {
            auto&& pattern = __aig.loggerPtr->GetPattern();
            __aig.loggerPtr->Error(
                "resetIceSeedList : 您填写的参数为 " + pattern + //
                    " , 然而此函数只接受植物类型为寒冰菇或模仿寒冰菇的参数",
                seedType);
            return;
        }
    }

    _iceSeedIdxVec.clear();
    int iceIdx = 0;
    for (const auto& seedType : lst) {
        iceIdx = AGetSeedIndex(AICE_SHROOM, seedType / AM_PEASHOOTER);
        if (iceIdx == -1) {
            __aig.loggerPtr->Error("resetIceSeedList : 您没有选择对应的冰卡");
            continue;
        }
        _iceSeedIdxVec.push_back(iceIdx);
    }
}

void AIceFiller::Start(const std::vector<AGrid>& lst)
{
    _iceSeedIdxVec.clear();
    int iceSeedIdx = AGetSeedIndex(AICE_SHROOM);
    if (iceSeedIdx != -1) {
        _iceSeedIdxVec.push_back(iceSeedIdx);
    }
    iceSeedIdx = AGetSeedIndex(AICE_SHROOM, true);
    if (iceSeedIdx != -1) {
        _iceSeedIdxVec.push_back(iceSeedIdx);
    }
    _coffeeSeedIdx = AGetSeedIndex(ACOFFEE_BEAN);
    _fillIceGridVec = lst;
    ATickRunnerWithNoStart::_Start([this]() { _Run(); }, ONLY_FIGHT);
}

void AIceFiller::_Run()
{
    ASeed* seed = nullptr;
    auto iceSeedIdxIter = _iceSeedIdxVec.begin();
    for (; iceSeedIdxIter != _iceSeedIdxVec.end(); ++iceSeedIdxIter) {
        auto tmpSeed = AGetMainObject()->SeedArray() + *iceSeedIdxIter;
        if (AIsSeedUsable(tmpSeed)) {
            seed = tmpSeed;
            break;
        }
    }
    if (seed == nullptr) {
        return;
    }

    for (auto fillIceGridIter = _fillIceGridVec.begin();
         fillIceGridIter != _fillIceGridVec.end();
         ++fillIceGridIter) {
        if (AAsm::GetPlantRejectType(AICE_SHROOM, fillIceGridIter->row - 1, fillIceGridIter->col - 1) != AAsm::NIL) {
            continue;
        }
        ACard(*iceSeedIdxIter + 1, fillIceGridIter->row,
            fillIceGridIter->col);
        ++fillIceGridIter;
        break;
    }
}

void AIceFiller::Coffee()
{
    if (_coffeeSeedIdx == -1) {
        __aig.loggerPtr->Error("你没有选择咖啡豆卡片!");
        return;
    }
    if (_fillIceGridVec.empty()) {
        __aig.loggerPtr->Error("你还未为自动存冰对象初始化存冰列表");
        return;
    }
    auto icePlantIdxVec = AGetPlantIndices(_fillIceGridVec, AICE_SHROOM);
    for (int i = _fillIceGridVec.size() - 1; i >= 0; --i) {
        if (icePlantIdxVec[i] < 0) {
            continue;
        }
        AAsm::ReleaseMouse();
        ACard(_coffeeSeedIdx + 1, _fillIceGridVec[i].row, _fillIceGridVec[i].col);
        AAsm::ReleaseMouse();
        return;
    }
    __aig.loggerPtr->Error("Coffee : 未找到可用的存冰");
}

/////////////////////////////////////////////////
//    PlantFixer
/////////////////////////////////////////////////

void APlantFixer::AutoSetList()
{
    _gridLst.clear();
    auto plant = AGetMainObject()->PlantArray();
    int plantCntMax = AGetMainObject()->PlantCountMax();
    AGrid grid;
    for (int index = 0; index < plantCntMax; ++index, ++plant) {
        if (!plant->IsCrushed() && !plant->IsDisappeared() && plant->Type() == _plantType) {
            grid.col = plant->Col() + 1;
            grid.row = plant->Row() + 1;
            _gridLst.push_back(grid);
        }
    }
}

void APlantFixer::_UseSeed(int seed_index, int row, float col,
    bool isNeedShovel)
{
    if (isNeedShovel) {
        // {_plantType, _plantType} 会转成 std::vector
        // 但是 {_plantType} 会转成 int
        ARemovePlant(row, col, {_plantType, _plantType});
    }
    ACard(seed_index + 1, row, col);
    if (_isUseCoffee) {
        ACard(_coffeeSeedIdx + 1, row, col);
    }
}

void APlantFixer::_GetSeedList()
{
    _seedIdxVec.clear();
    int seedIndex;
    seedIndex = AGetSeedIndex(_plantType);
    if (-1 != seedIndex) {
        _seedIdxVec.push_back(seedIndex);
    }
    seedIndex = AGetSeedIndex(_plantType, true);

    if (-1 != seedIndex) {
        _seedIdxVec.push_back(seedIndex);
    }
    if (_seedIdxVec.size() == 0) {
        __aig.loggerPtr->Error("您没有选择修补该植物的卡片！");
    }
    _coffeeSeedIdx = AGetSeedIndex(ACOFFEE_BEAN);
}

void APlantFixer::Start(int plantType, const std::vector<AGrid>& lst,
    int fixHp)
{
    if (plantType == ACOFFEE_BEAN) {
        __aig.loggerPtr->Error("PlantFixer 不支持修补咖啡豆");
        return;
    }

    if (plantType >= AGATLING_PEA) {
        __aig.loggerPtr->Error("修补植物类仅支持绿卡");
        return;
    }

    _plantType = plantType;
    _fixHp = fixHp;
    _GetSeedList();
    // 如果没有给出列表信息
    if (lst.size() == 0) {
        AutoSetList();
    } else {
        _gridLst = lst;
    }
    ATickRunnerWithNoStart::_Start([this]() { _Run(); }, ONLY_FIGHT);
}

void APlantFixer::_Run()
{
    auto usableSeedIndexIter = _seedIdxVec.begin();
    if (usableSeedIndexIter == _seedIdxVec.end()) {
        return;
    }

    if (_isUseCoffee) {
        if (_coffeeSeedIdx == -1) {
            return;
        }
        auto coffeeSeed = AGetMainObject()->SeedArray() + _coffeeSeedIdx;
        if (!AIsSeedUsable(coffeeSeed)) {
            return;
        }
    }

    do {
        auto seedMemory = AGetMainObject()->SeedArray();
        seedMemory += *usableSeedIndexIter;
        if (AIsSeedUsable(seedMemory)) {
            break;
        }
        ++usableSeedIndexIter;
    } while (usableSeedIndexIter != _seedIdxVec.end());

    // 没找到可用的卡片
    if (usableSeedIndexIter == _seedIdxVec.end()) {
        return;
    }
    auto plantIdxVec = AGetPlantIndices(_gridLst, _plantType);
    AGrid needPlantGrid = {0, 0};
    int minHp = _fixHp;
    auto gridIter = _gridLst.begin();
    auto plantIdxIter = plantIdxVec.begin();
    for (; gridIter != _gridLst.end(); ++gridIter, ++plantIdxIter) {
        // 如果此处存在除植物类植物的植物
        if (*plantIdxIter == -2) {
            continue;
        }

        if (*plantIdxIter == -1) {
            if (AAsm::GetPlantRejectType(_plantType, gridIter->row - 1, gridIter->col - 1) != AAsm::NIL) {
                continue;
            }
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