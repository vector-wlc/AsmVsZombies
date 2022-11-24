/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: CLASS FireOperator
 */
#include "avz_smart.h"
#include "avz_asm.h"
#include "avz_card.h"
#include "avz_click.h"
#include "avz_memory.h"
#include <algorithm>

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

void ACobManager::BeforeScript()
{
    _sequentialMode = TIME;
    _next = 0;
    _lockSet.clear();
}

void ACobManager::EnterFight()
{
    AutoGetList();
}

// 得到炮的恢复时间
int ACobManager::_GetRecoverTime(int index)
{
    auto cob = __aInternalGlobal.mainObject->PlantArray() + index;
    if (cob->IsDisappeared() || cob->Type() != ACOB_CANNON) {
        return NO_EXIST_RECOVER_TIME;
    }
    auto animationMemory = __aInternalGlobal.pvzBase->AnimationMain()->AnimationOffset()->AnimationArray() + cob->AnimationCode();

    switch (cob->State()) {
    case 35:
        return 125 + cob->StateCountdown();
    case 36:
        return int(125 * (1 - animationMemory->CirculationRate()) + 0.5) + 1;
    case 37:
        return 0;
    case 38:
        return 3125 + int(350 * (1 - animationMemory->CirculationRate()) + 0.5);
    default:
        return NO_EXIST_RECOVER_TIME;
    }
}

// 获取屋顶炮飞行时间
int ACobManager::_GetRoofFlyTime(int cobCol, float dropCol)
{
    // 得到落点对应的横坐标
    int drop_x = static_cast<int>(dropCol * 80);
    // 得到该列炮最小飞行时间对应的最小的横坐标
    int min_drop_x = _flyTimeData[cobCol - 1].minDropX;
    // 得到最小的飞行时间
    int min_fly_time = _flyTimeData[cobCol - 1].minFlyTime;
    // 返回飞行时间
    return (drop_x >= min_drop_x
            ? min_fly_time
            : (min_fly_time + 1 - (drop_x - (min_drop_x - 1)) / 32));
}

void ACobManager::_BasicFire(int cobIdx, int dropRow, float dropCol)
{
    AAsm::ReleaseMouse();
    int x = 0;
    int y = 0;
    auto plant = __aInternalGlobal.mainObject->PlantArray() + cobIdx;
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    __aInternalGlobal.loggerPtr->Info(
        "Fire from (" + pattern + ", " + pattern + ") to (" + pattern + ", " + pattern + ")",
        plant->Row() + 1, plant->Col() + 1, dropRow, dropCol);

    AGridToCoordinate(dropRow, dropCol, x, y);
    AAsm::Fire(x, y, cobIdx);
    AAsm::ReleaseMouse();
}

void ACobManager::_DelayFire(int delayTime, int cobIdx,
    int row, float col)
{
    // 将操作动态插入操作队列
    _lockSet.insert(cobIdx);
    AConnect(
        ANowDelayTime(delayTime), [=]() {
            _BasicFire(cobIdx, row, col);
            _lockSet.erase(cobIdx);
        });
}

// 用户自定义炮位置发炮：单发
void ACobManager::RawFire(int cobRow, int cobCol, int dropRow, float dropCol)
{
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    int index = AGetPlantIndex(cobRow, cobCol, ACOB_CANNON);
    if (index < 0) {
        __aInternalGlobal.loggerPtr->Error("请检查 (" + pattern + ", " + pattern + ") 是否为炮", cobRow, cobCol);
        return;
    }

    int recoverTime = _GetRecoverTime(index);
    if (recoverTime > 0) {
        __aInternalGlobal.loggerPtr->Error("位于 (" + pattern + ", " + pattern + ") 的炮还有 " + pattern + " cs 恢复",
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
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    if (__aInternalGlobal.mainObject->Scene() != 4 && __aInternalGlobal.mainObject->Scene() != 5) {
        __aInternalGlobal.loggerPtr->Error("RawRoofFire : RawRoofFire函数只适用于RE与ME");
        return;
    }
    int index = AGetPlantIndex(cobRow, cobCol, ACOB_CANNON);
    if (index < 0) {
        __aInternalGlobal.loggerPtr->Error("请检查 (" + pattern + ", " + pattern + ") 是否为炮", cobRow, cobCol);
        return;
    }

    int recoverTime = _GetRecoverTime(index);
    int delayTime = 387 - _GetRoofFlyTime(cobCol, dropCol);
    if (recoverTime > delayTime) {
        __aInternalGlobal.loggerPtr->Error("位于 (" + pattern + ", " + pattern + ") 的炮还有 " + pattern + " cs 恢复",
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
            auto seed_memory = __aInternalGlobal.mainObject->SeedArray() + ymtseedIdx;
            if (!seed_memory->IsUsable()) {
                return;
            }
            ACard(ymtseedIdx + 1, row, tCol);
        }
        auto seed_memory = __aInternalGlobal.mainObject->SeedArray() + ymjnpSeedIdx;
        if (!seed_memory->IsUsable()) {
            return;
        }
        ACard(ymjnpSeedIdx + 1, row, col);
        _tickRunner.Stop();
    });
}

// 用户重置炮列表
void ACobManager::SetList(const std::vector<AGrid>& lst)
{
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    _next = 0;
    // 重置炮列表
    _gridVec = lst;
    AGetPlantIndices(_gridVec, ACOB_CANNON, _indexVec);
    auto idxIter = _indexVec.begin();
    auto gridIter = _gridVec.begin();
    while (idxIter != _indexVec.end()) {
        if ((*idxIter) < 0) {
            __aInternalGlobal.loggerPtr->Error(
                "resetFireList : 请检查 (" + pattern + ", " + pattern + ") 位置是否为炮",
                gridIter->row,
                gridIter->col);
            return;
        }

        ++gridIter;
        ++idxIter;
    }
}

// 自动填充炮列表
// *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
void ACobManager::AutoGetList()
{
    _gridVec.clear();
    _next = 0;
    AGrid cobGrid = {0, 0};
    auto PlantArray = __aInternalGlobal.mainObject->PlantArray();
    for (int index = 0; index < __aInternalGlobal.mainObject->PlantCountMax(); ++index) {
        if (!PlantArray[index].IsCrushed() && !PlantArray[index].IsDisappeared() && PlantArray[index].Type() == ACOB_CANNON) {
            cobGrid = {PlantArray[index].Row() + 1,
                PlantArray[index].Col() + 1};
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
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    if (tempNext > _gridVec.size()) {
        __aInternalGlobal.loggerPtr->Error(
            "SetNext : 本炮列表中一共有 " + pattern + " 门炮，您设的参数已溢出",
            _gridVec.size());
        return;
    }
    _next = tempNext - 1;
}

void ACobManager::SetNext(int row, int col)
{
    AGrid grid = {row, col};
    auto iter = std::find(_gridVec.begin(), _gridVec.end(), grid);

    if (iter != _gridVec.end()) {
        _next = iter - _gridVec.begin();
    } else {
        auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
        __aInternalGlobal.loggerPtr->Error(
            "SetNext : 请检查(" + pattern + ", " + pattern + ")是否在本炮列表中", row, col);
        return;
    }
}

void ACobManager::FixLatest()
{
    if (_lastestMsg.vecIndex == -1) {
        __aInternalGlobal.loggerPtr->Error("FixLatest ：您尚未使用炮");
        return;
    }
    _lastestMsg.isWritable = false; // 锁定信息
    int delayTime = _lastestMsg.fireTime + 205 - __aInternalGlobal.mainObject->GameClock();
    if (delayTime < 0) {
        delayTime = 0;
    }
    AConnect(
        ANowDelayTime(delayTime), [this]() {
            _lastestMsg.isWritable = true; // 解锁信息
            AShovel(_gridVec[_lastestMsg.vecIndex].row,
                _gridVec[_lastestMsg.vecIndex].col);
        });
    Plant(_gridVec[_lastestMsg.vecIndex].row,
        _gridVec[_lastestMsg.vecIndex].col);
}

int ACobManager::_GetRecoverTimeVec()
{
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    int time = _GetRecoverTime(_indexVec[_next]);
    if (time == NO_EXIST_RECOVER_TIME) {
        int index = AGetPlantIndex(
            _gridVec[_next].row, _gridVec[_next].col, ACOB_CANNON);
        if (index < 0) // 找不到本来位置的炮
        {
            __aInternalGlobal.loggerPtr->Error("请检查位于 (" + pattern + ", " + pattern + ") 的第 " + pattern + " 门炮是否存在",
                _gridVec[_next].row,
                _gridVec[_next].col,
                _next + 1);
            return NO_EXIST_RECOVER_TIME;
        }
        _indexVec[_next] = index;
        time = _GetRecoverTime(_indexVec[_next]);
    }
    return time;
}

int ACobManager::_UpdateNextCob(bool isDelayFire, float dropCol)
{
    int recoverTime = 0xFFFF;
    if (_sequentialMode == TIME) {
        int time;
        int _tmpNext = _next;
        // 遍历整个炮列表
        for (int i = 0; i < _indexVec.size(); ++i, Skip(1)) {
            // 被锁定的炮不允许发射
            if (_lockSet.find(_indexVec[_next]) != _lockSet.end()) {
                continue;
            }

            time = _GetRecoverTimeVec();
            if (time == NO_EXIST_RECOVER_TIME) {
                return NO_EXIST_RECOVER_TIME;
            }
            int roof_offset_time = dropCol < 0 ? 0 : (387 - _GetRoofFlyTime(_gridVec[_next].col, dropCol));
            time -= roof_offset_time;
            if (time <= 0) { // 这里返回 roof_offset_time 目的是直接让 RoofFire 使用, 对于普通的炮, roof_offset_time 一直为 0
                return roof_offset_time;
            }
            if (recoverTime > time) {
                recoverTime = time;
                _tmpNext = _next;
            }
        }

        _next = _tmpNext;
    } else { // SPACE
        recoverTime = _GetRecoverTimeVec();
        if (recoverTime <= 0) {
            return recoverTime;
        }
    }

    // 以上的判断条件已经解决炮是否存在以及炮当前时刻是否能用的问题
    // 如果炮当前时刻不能使用但是为 RecoverFire 时则不会报错，
    // 并返回恢复时间
    if (isDelayFire) {
        return recoverTime;
    }
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    std::string error_str = (_sequentialMode == TIME ? "TIME 模式 : 未找到能够发射的炮，"
                                                     : "SPACE 模式 : ");
    error_str += "位于 (" + pattern + ", " + pattern + ") 的第 " + pattern + " 门炮还有 " + pattern + "cs 恢复";
    __aInternalGlobal.loggerPtr->Error(std::move(error_str),
        _gridVec[_next].row,
        _gridVec[_next].col,
        _next + 1,
        recoverTime);
    return NO_EXIST_RECOVER_TIME;
}

// 发炮函数：单发
int ACobManager::Fire(int row, float col)
{
    if (_gridVec.size() == 0) {
        __aInternalGlobal.loggerPtr->Error("Fire : 您尚未为此炮列表分配炮");
        return -1;
    }
    if (_UpdateNextCob() == NO_EXIST_RECOVER_TIME) {
        return -1;
    }
    _BasicFire(_indexVec[_next], row, col);
    _UpdateLastestMsg(__aInternalGlobal.mainObject->GameClock(), _next);
    auto tmpNext = _next;
    Skip(1);
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

int ACobManager::RecoverFire(int row, float col)
{
    if (_gridVec.size() == 0) {
        __aInternalGlobal.loggerPtr->Error("RecoverFire : 您尚未为此炮列表分配炮");
        return -1;
    }
    int delayTime = _UpdateNextCob(true);
    if (delayTime == NO_EXIST_RECOVER_TIME) {
        return -1;
    }
    _DelayFire(delayTime, _indexVec[_next], row, col);
    _UpdateLastestMsg(__aInternalGlobal.mainObject->GameClock() + delayTime,
        _next);
    auto tmpNext = _next;
    Skip(1);
    return tmpNext;
}

std::vector<int> ACobManager::RecoverFire(const std::vector<APosition>& lst)
{
    std::vector<int> vec;
    for (const auto& each : lst) {
        vec.push_back(RecoverFire(each.row, each.col));
    }
    return vec;
}

int ACobManager::RoofFire(int row, float col)
{
    int Scene = __aInternalGlobal.mainObject->Scene();
    if (Scene != 4 && Scene != 5) {
        __aInternalGlobal.loggerPtr->Error("RoofFire : RoofFire函数只适用于 RE 与 ME ");
        return -1;
    }
    if (_gridVec.empty()) {
        __aInternalGlobal.loggerPtr->Error("RoofFire : 您尚未为此炮列表分配炮");
        return -1;
    }

    int delayTime = _UpdateNextCob(false, col);
    if (delayTime == NO_EXIST_RECOVER_TIME) {
        return -1;
    }

    _DelayFire(delayTime, _indexVec[_next], row, col);
    _UpdateLastestMsg(__aInternalGlobal.mainObject->GameClock() + delayTime,
        _next);
    auto tmpNext = _next;
    Skip(1);
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

////////////////////////////////////////
//  ItemCollector
////////////////////////////////////////

void AItemCollector::EnterFight()
{
    AItemCollector::Start();
}

void AItemCollector::SetInterval(int timeInterval)
{
    if (timeInterval < 1) {
        auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
        __aInternalGlobal.loggerPtr->Error("自动收集类时间间隔范围为:[1, 正无穷], 你现在设定的参数为 " + pattern, timeInterval);
        return;
    }
    this->_timeInterval = timeInterval;
}

void AItemCollector::Start()
{
    _tickManager.Start([this] {
        _Run();
    },
        false);
}

void AItemCollector::_Run()
{
    if (__aInternalGlobal.mainObject->GameClock() % _timeInterval != 0 || //
        __aInternalGlobal.mainObject->MouseAttribution()->Type() != 0) {
        return;
    }

    auto item = __aInternalGlobal.mainObject->ItemArray();
    int total = __aInternalGlobal.mainObject->ItemTotal();
    int collectIdx = -1;
    for (int index = 0; index < total; ++index, ++item) {
        if (item->IsCollected() || item->IsDisappeared()) {
            continue;
        }
        collectIdx = index;
        if (ARangeIn(item->Type(), {4, 5, 6})) { // 优先采集阳光
            break;
        }
    }
    if (collectIdx == -1) { // 没有要收集的物品
        return;
    }

    item = __aInternalGlobal.mainObject->ItemArray() + collectIdx;
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
            auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
            __aInternalGlobal.loggerPtr->Error(
                "resetIceSeedList : 您填写的参数为 " + pattern + //
                    " ,然而此函数只接受植物类型为寒冰菇或模仿寒冰菇的参数",
                seedType);
            return;
        }
    }

    _iceSeedIdxVec.clear();
    int iceIdx = 0;
    for (const auto& seedType : lst) {
        iceIdx = AGetSeedIndex(AICE_SHROOM, seedType / 49);
        if (iceIdx == -1) {
            __aInternalGlobal.loggerPtr->Error(
                "resetIceSeedList : 您貌似没有选择对应的冰卡");
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
    _tickManager.Start([this]() { _Run(); }, false);
}

void AIceFiller::_Run()
{
    static auto seed = __aInternalGlobal.mainObject->SeedArray();
    static std::vector<int> icePlantIdxVec;
    static decltype(icePlantIdxVec.begin()) icePlantIdxIter;
    static decltype(_iceSeedIdxVec.begin()) iceSeedIdxIter;
    static decltype(_fillIceGridVec.begin()) fillIceGridIter;
    static bool isGetIdxs = false;

    isGetIdxs = false;
    fillIceGridIter = _fillIceGridVec.begin();

    for (iceSeedIdxIter = _iceSeedIdxVec.begin();
         iceSeedIdxIter != _iceSeedIdxVec.end(); ++iceSeedIdxIter) {
        seed = __aInternalGlobal.mainObject->SeedArray() + *iceSeedIdxIter;
        if (!seed->IsUsable()) {
            continue;
        }
        if (!isGetIdxs) {
            AGetPlantIndices(_fillIceGridVec, AICE_SHROOM, icePlantIdxVec);
            icePlantIdxIter = icePlantIdxVec.begin();
            isGetIdxs = true;
        }

        for (; icePlantIdxIter != icePlantIdxVec.end();
             ++fillIceGridIter, ++icePlantIdxIter) {
            if ((*icePlantIdxIter) == -1) {
                if (AAsm::GetPlantRejectType(AICE_SHROOM, fillIceGridIter->row - 1, fillIceGridIter->col - 1) != AAsm::NIL) {
                    continue;
                }
                ACard(*iceSeedIdxIter + 1, fillIceGridIter->row,
                    fillIceGridIter->col);
                ++fillIceGridIter;
                ++icePlantIdxIter;
                break;
            }
        }
    }
}

void AIceFiller::Coffee()
{

    if (_coffeeSeedIdx == -1) {
        __aInternalGlobal.loggerPtr->Error("你没有选择咖啡豆卡片!");
        return;
    }

    if (_fillIceGridVec.empty()) {
        __aInternalGlobal.loggerPtr->Error("你还未为自动存冰对象初始化存冰列表");
        return;
    }
    std::vector<int> icePlantIdxVec;
    AGetPlantIndices(_fillIceGridVec, AICE_SHROOM, icePlantIdxVec);

    auto fillGridIter = _fillIceGridVec.end();
    do {
        --fillGridIter;
        if (icePlantIdxVec[fillGridIter - _fillIceGridVec.begin()] > -1) {
            AAsm::ReleaseMouse();
            ACard(_coffeeSeedIdx + 1, fillGridIter->row,
                fillGridIter->col);
            AAsm::ReleaseMouse();
            return;
        }
    } while (fillGridIter != _fillIceGridVec.begin());

    __aInternalGlobal.loggerPtr->Error("coffee : 未找到可用的存冰");
}

/////////////////////////////////////////////////
//    PlantFixer
/////////////////////////////////////////////////

void APlantFixer::AutoGetList()
{
    _gridLst.clear();
    auto plant = __aInternalGlobal.mainObject->PlantArray();
    int plantCntMax = __aInternalGlobal.mainObject->PlantCountMax();
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
        AShovel(row, col, _plantType == APUMPKIN);
    }
    ACard(seed_index + 1, row, col);
    if (_isUseCoffee) {
        ACard(_coffeeSeedIdx + 1, row, col);
    }
}

void APlantFixer::_getSeedList()
{
    _seedIdxVec.clear();
    int seed_index;
    seed_index = AGetSeedIndex(_plantType);
    if (-1 != seed_index) {
        _seedIdxVec.push_back(seed_index);
    }
    seed_index = AGetSeedIndex(_plantType, true);

    if (-1 != seed_index) {
        _seedIdxVec.push_back(seed_index);
    }
    if (_seedIdxVec.size() == 0) {
        __aInternalGlobal.loggerPtr->Error("您没有选择修补该植物的卡片！");
    }
    _coffeeSeedIdx = AGetSeedIndex(ACOFFEE_BEAN);
}

void APlantFixer::Start(int plantType, const std::vector<AGrid>& lst,
    int fixHp)
{
    if (plantType == ACOFFEE_BEAN) {
        __aInternalGlobal.loggerPtr->Error("PlantFixer 不支持修补咖啡豆");
        return;
    }

    if (plantType >= AGATLING_PEA) {
        __aInternalGlobal.loggerPtr->Error("修补植物类仅支持绿卡");
        return;
    }

    _plantType = plantType;
    _fixHp = fixHp;
    _getSeedList();
    // 如果没有给出列表信息
    if (lst.size() == 0) {
        AutoGetList();
    } else {
        _gridLst = lst;
    }
    _tickManager.Start([this]() { _Run(); }, false);
}

void APlantFixer::_Run()
{
    static std::vector<int> plantIdxVec;
    static AGrid needPlantGrid; // 记录要使用植物的格子
    static int minHp;           // 记录要使用植物的格子
    static bool isSeedUsed;     // 种子是否被使用
    static decltype(_seedIdxVec.begin()) usableSeedIndexIter;
    static decltype(plantIdxVec.begin()) plantIdxIter;
    static decltype(_gridLst.begin()) gridIter;

    usableSeedIndexIter = _seedIdxVec.begin();

    if (usableSeedIndexIter == _seedIdxVec.end()) {
        return;
    }

    if (_isUseCoffee) {
        if (_coffeeSeedIdx == -1) {
            return;
        }
        auto coffee_seed = __aInternalGlobal.mainObject->SeedArray() + _coffeeSeedIdx;
        if (!coffee_seed->IsUsable()) {
            return;
        }
    }

    do {
        auto seedMemory = __aInternalGlobal.mainObject->SeedArray();
        seedMemory += *usableSeedIndexIter;
        if (seedMemory->IsUsable()) {
            break;
        }
        ++usableSeedIndexIter;
    } while (usableSeedIndexIter != _seedIdxVec.end());

    // 没找到可用的卡片
    if (usableSeedIndexIter == _seedIdxVec.end()) {
        return;
    }
    AGetPlantIndices(_gridLst, _plantType, plantIdxVec);

    isSeedUsed = false;
    needPlantGrid.row = needPlantGrid.col = 0; // 格子信息置零
    minHp = _fixHp;                            // 最小生命值重置

    for (gridIter = _gridLst.begin(), plantIdxIter = plantIdxVec.begin();
         gridIter != _gridLst.end(); ++gridIter, ++plantIdxIter) {
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
            isSeedUsed = true;
            break;
        } else {
            auto plant = __aInternalGlobal.mainObject->PlantArray();
            plant += *plantIdxIter;
            int plantHp = plant->Hp();
            // 如果当前生命值低于最小生命值，记录下来此植物的信息
            if (plantHp < minHp) {
                minHp = plantHp;
                needPlantGrid.row = gridIter->row;
                needPlantGrid.col = gridIter->col;
            }
        }
    }

    // 如果有需要修补的植物且植物卡片能用则进行种植
    if (needPlantGrid.row && !isSeedUsed) {
        // 种植植物
        _UseSeed((*usableSeedIndexIter), needPlantGrid.row,
            needPlantGrid.col, true);
    }
}