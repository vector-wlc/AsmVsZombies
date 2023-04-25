/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-14 11:25:07
 * @Description:
 */
#include "avz_memory.h"
#include "avz_asm.h"
#include "avz_card.h"
#include "avz_connector.h"
#include "avz_smart.h"

// 返回鼠标所在行
int AMouseRow()
{
    static int lastValue = -1;
    auto memoryValue = __aInternalGlobal.mainObject->MouseExtraAttribution()->Row();
    if (memoryValue >= 0) {
        lastValue = memoryValue + 1;
    }
    return lastValue;
}

// 返回鼠标所在列
float AMouseCol()
{
    static float lastValue = -1;
    auto memoryValue = __aInternalGlobal.mainObject->MouseAttribution()->Abscissa();
    if (memoryValue >= 0) {
        lastValue = float(memoryValue + 25) / 80;
    }
    return lastValue;
}

int AGetSeedIndex(int type, bool imitator)
{
    auto seed = __aInternalGlobal.mainObject->SeedArray();
    int cnt = seed->Count();
    for (int index = 0; index < cnt; ++index, ++seed) {
        if (imitator) {
            if (seed->Type() == 48 && seed->ImitatorType() == type) {
                return index;
            }
        } else if (seed->Type() == type) {
            return index;
        }
    }

    return -1;
}

ASeed* AGetSeedPtr(int type, bool imitator)
{
    auto idx = AGetSeedIndex(type, imitator);
    return idx < 0 ? nullptr : AGetMainObject()->SeedArray() + idx;
}

int AGetPlantIndex(int row, int col, int type)
{
    auto plant = __aInternalGlobal.mainObject->PlantArray();
    int plantCntMax = __aInternalGlobal.mainObject->PlantCountMax();
    for (int i = 0; i < plantCntMax; ++i, ++plant) {
        if ((!plant->IsDisappeared()) && (!plant->IsCrushed()) && (plant->Row() + 1 == row) && (plant->Col() + 1 == col)) {
            int plantType = plant->Type();
            if (type == -1) {
                // 如果植物存在	且不为南瓜花盆荷叶
                if ((plantType != 16) && (plantType != 30) && (plantType != 33))
                    return i; // 返回植物的对象序列
            } else {
                if (plantType == type) {
                    return i;
                } else if (type != 16 && type != 30 && type != 33 && plantType != 16 && plantType != 30 && plantType != 33) {
                    return -2;
                }
            }
        }
    }
    return -1; // 没有符合要求的植物返回-1
}

APlant* AGetPlantPtr(int row, int col, int type)
{
    auto idx = AGetPlantIndex(row, col, type);
    return idx < 0 ? nullptr : AGetMainObject()->PlantArray() + idx;
}

void AGetPlantIndices(const std::vector<AGrid>& lstIn, int type,
    std::vector<int>& indexsOut)
{
    auto plant = __aInternalGlobal.mainObject->PlantArray();
    indexsOut.assign(lstIn.size(), -1);
    AGrid grid;

    for (int index = 0; index < __aInternalGlobal.mainObject->PlantCountMax();
         ++index, ++plant) {
        if (plant->IsCrushed() || plant->IsDisappeared()) {
            continue;
        }
        grid.row = plant->Row() + 1;
        grid.col = plant->Col() + 1;

        auto itVec = AFindSameEle<AGrid>(lstIn, grid);
        if (itVec.empty()) {
            continue;
        }
        int plantType = plant->Type();
        if (plantType == type) {
            for (const auto& ele : itVec) {
                indexsOut[ele - lstIn.begin()] = index;
            }
        } else if (type != 16 && type != 30 && type != 33 && plantType != 16 && plantType != 30 && plantType != 33) {
            for (const auto& ele : itVec) {
                indexsOut[ele - lstIn.begin()] = -2;
            }
        }
    }
}

std::vector<int> AGetPlantIndices(const std::vector<AGrid>& lst, int type)
{
    std::vector<int> indexs;
    AGetPlantIndices(lst, type, indexs);
    return indexs;
}

void AGetPlantPtrs(const std::vector<AGrid>& lstIn, int type, std::vector<APlant*>& ptrsOut)
{
    auto plantArray = AGetMainObject()->PlantArray();
    auto indexs = AGetPlantIndices(lstIn, type);
    ptrsOut.clear();
    for (auto&& index : indexs) {
        ptrsOut.push_back(index < 0 ? nullptr : plantArray + index);
    }
}

std::vector<APlant*> AGetPlantPtrs(const std::vector<AGrid>& lst, int type)
{
    std::vector<APlant*> ptrs;
    AGetPlantPtrs(lst, type, ptrs);
    return ptrs;
}

bool AIsZombieExist(int type, int row)
{
    auto zombie = __aInternalGlobal.mainObject->ZombieArray();
    int zombieCntMax = __aInternalGlobal.mainObject->ZombieTotal();
    for (int i = 0; i < zombieCntMax; ++i, ++zombie) {
        if (zombie->IsExist() && !zombie->IsDead()) {
            if (type < 0 && row < 0) {
                return true;
            } else if (type >= 0 && row >= 0) {
                if (zombie->Row() == row - 1 && zombie->Type() == type) {
                    return true;
                }
            } else if (type < 0 && row >= 0) {
                if (zombie->Row() == row - 1) {
                    return true;
                }
            } else { // if (type >= 0 && row < 0)
                if (zombie->Type() == type) {
                    return true;
                }
            }
        }
    }

    return false;
}

void ASetPlantActiveTime(APlantType plantType, int delayTime)
{
    auto activeTime = ANowTime();
    activeTime.time += delayTime - 10;
    auto tmp = [=]() {
        // 这里不做植物类型检测
        auto plant = __aInternalGlobal.mainObject->PlantArray();
        for (int index = 0; index < __aInternalGlobal.mainObject->PlantCountMax();
             ++index, ++plant) {
            if (!plant->IsDisappeared() && !plant->IsCrushed() && plant->Type() == plantType && plant->State() == 2) {
                if (std::abs(plant->ExplodeCountdown() - 10) < 10) {
                    plant->ExplodeCountdown() = 10;
                } else {
                    __aInternalGlobal.loggerPtr->Error("ASetPlantActiveTime 不允许修改的生效时间超过 3cs");
                }
                return;
            }
        }
    };
    AConnect(activeTime, std::move(tmp));
}

void AUpdateZombiesPreview()
{
    // 去掉当前画面上的僵尸
    AAsm::KillZombiesPreview();
    // 重新生成僵尸
    __aInternalGlobal.mainObject->SelectCardUi_m()->IsCreatZombie() = false;
}

void ASetZombies(const std::vector<int>& zombieType)
{
    std::vector<int> zombieTypeVec;
    bool isHasBungee = false;

    // 设置出怪类型列表
    auto typeList = AGetMainObject()->ZombieTypeList();
    std::fill_n(typeList, AHY_32 + 1, 0);

    for (const auto& type : zombieType) {
        typeList[type] = true;
        if (type == ABJ_20) {
            isHasBungee = true;
        }
        // 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
        if (!ARangeIn(type, {AQZ_1, ABW_9, AXQ_13, AXR_19, ABJ_20, AXG_24})) {
            zombieTypeVec.push_back(type);
        }
    }
    auto zombieList = __aInternalGlobal.mainObject->ZombieList();
    for (int index = 0; index < 1000; ++index, ++zombieList) {
        (*zombieList) = zombieTypeVec[index % zombieTypeVec.size()];
    }

    // 生成旗帜
    for (auto index : {450, 950}) {
        (*(__aInternalGlobal.mainObject->ZombieList() + index)) = AQZ_1;
    }

    if (isHasBungee) {
        // 生成蹦极
        for (auto index : {451, 452, 453, 454, 951, 952, 953, 954}) {
            (*(__aInternalGlobal.mainObject->ZombieList() + index)) = ABJ_20;
        }
    }

    if (__aInternalGlobal.pvzBase->GameUi() == 2) {
        AUpdateZombiesPreview();
    }
}

void ASetWaveZombies(int wave, const std::vector<int>& zombieType)
{
    std::vector<int> zombieTypeVec;
    bool isHasBungee = false;
    for (const auto& type : zombieType) {
        if (type == ABJ_20) {
            isHasBungee = wave % 10 == 0; // 大波才能出蹦极
        }
        // 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
        if (!ARangeIn(type, {AQZ_1, ABW_9, AXQ_13, AXR_19, ABJ_20, AXG_24})) {
            zombieTypeVec.push_back(type);
        }
    }
    auto zombieList = __aInternalGlobal.mainObject->ZombieList() + (wave - 1) * 50;
    for (int index = 0; index < 50; ++index, ++zombieList) {
        (*zombieList) = zombieTypeVec[index % zombieTypeVec.size()];
    }

    // 生成旗帜
    for (auto index : {450, 950}) {
        (*(__aInternalGlobal.mainObject->ZombieList() + index)) = AQZ_1;
    }

    if (isHasBungee) {
        // 生成蹦极
        for (auto index : {451, 452, 453, 454, 951, 952, 953, 954}) {
            (*(__aInternalGlobal.mainObject->ZombieList() + index)) = ABJ_20;
        }
    }
}

bool* AGetZombieTypeList()
{
    return __aInternalGlobal.mainObject->ZombieTypeList();
}

void __AGameSpeedManager::_BeforeScript()
{
    _oriTickMs = __aInternalGlobal.pvzBase->TickMs();
}

void __AGameSpeedManager::_ExitFight()
{
    __aInternalGlobal.pvzBase->TickMs() = _oriTickMs;
}

void __AGameSpeedManager::Set(float x)
{
    if (x < 0.05 || x > 10) {
        __aInternalGlobal.loggerPtr->Error(
            "SetGameSpeed : 倍速设置失败，倍速设置的合法范围为 [0.05, 10]");
        return;
    }
    int ms = int(10 / x + 0.5);
    __aInternalGlobal.pvzBase->TickMs() = ms;
}

bool AGameIsPaused()
{
    if (!__aInternalGlobal.pvzBase->MainObject()) {
        return false;
    }
    return __aInternalGlobal.mainObject->GamePaused();
}

// 移除植物函数
void ARemovePlant(int row, int col, APlantType type)
{
    int idx = AGetPlantIndex(row, col, type);
    if (idx < 0) {
        return;
    }
    AAsm::RemovePlant(idx + AGetMainObject()->PlantArray());
}

__ANodiscard int AGetSeedSunVal(APlantType type)
{
    int intType = int(type);
    if (type >= AM_PEASHOOTER) {
        intType -= AM_PEASHOOTER;
    }
    return MRef<int>(0x69F2C0 + 0x24 * intType);
}

// 检查卡片是否能用
__ANodiscard bool AIsSeedUsable(APlantType type)
{
    auto idx = AGetCardIndex(type);
    if (idx == -1) {
        return false;
    }
    auto seed = AGetMainObject()->SeedArray() + idx;
    if (!seed->IsUsable()) {
        return false;
    }
    auto sunVal = AGetSeedSunVal(type);
    return AGetMainObject()->Sun() >= sunVal;
}

// 检查卡片是否能用
__ANodiscard bool AIsSeedUsable(ASeed* seed)
{
    if (!seed->IsUsable()) {
        return false;
    }
    int type = seed->Type();
    if (type == AIMITATOR) {
        type = seed->ImitatorType();
    }
    auto sunVal = AGetSeedSunVal(APlantType(type));
    return AGetMainObject()->Sun() >= sunVal;
}

// 得到炮的恢复时间
// index 为玉米加农炮的内存索引
__ANodiscard int AGetCobRecoverTime(int index)
{
    if (index < 0 || index >= AGetMainObject()->PlantCountMax()) {
        __aInternalGlobal.loggerPtr->Error("AGetCobRecoverTime(int) 参数值为:" + std::to_string(index) + ", 不合法");
        return ACobManager::NO_EXIST_RECOVER_TIME;
    }
    return AGetCobRecoverTime(AGetMainObject()->PlantArray() + index);
}

__ANodiscard int AGetCobRecoverTime(APlant* cob)
{
    if (cob == nullptr || cob->IsDisappeared() || cob->Type() != ACOB_CANNON) {
        __aInternalGlobal.loggerPtr->Error("AGetCobRecoverTime(APlant*) 参数值不合法");
        return ACobManager::NO_EXIST_RECOVER_TIME;
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
        return ACobManager::NO_EXIST_RECOVER_TIME;
    }
}
