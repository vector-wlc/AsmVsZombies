#include "avz_card.h"
#include "avz_asm.h"
#include "avz_click.h"
#include "avz_logger.h"
#include "avz_memory.h"
#include "avz_script.h"
#include <unordered_set>

std::vector<int> __ACardManager::_selectCardVec;
std::unordered_map<int, int> __ACardManager::_seedNameToIndexMap;
AMainObject* __ACardManager::_mainObject;
APvzBase* __ACardManager::_pvzBase;
int __ACardManager::_selectInterval = 17;

void __ACardManager::_EnterFight()
{
    _seedNameToIndexMap.clear();
    _mainObject = __aInternalGlobal.mainObject;
    _pvzBase = __aInternalGlobal.pvzBase;
    auto seed = _mainObject->SeedArray();
    if (_pvzBase->GameUi() == 3) {
        int seedCount = seed->Count();
        int seedType;
        std::pair<int, int> seedInfo;
        for (int i = 0; i < seedCount; ++i, ++seed) {
            seedType = seed->Type();
            // 如果是模仿者卡片
            if (seedType == 48) {
                seedType = seed->ImitatorType();
                seedInfo.first = seedType + 49;
                seedInfo.second = i;
            } else { // if(seed_info != 48)
                seedInfo.first = seedType;
                seedInfo.second = i;
            }
            _seedNameToIndexMap.insert(seedInfo);
        }
    }
    _selectCardVec.clear();
}

void __ACardManager::ChooseSingleCard()
{
    if (_selectCardVec.empty()) {
        return;
    }
    static auto iter = _selectCardVec.begin();
    _mainObject = __aInternalGlobal.mainObject;
    _pvzBase = __aInternalGlobal.pvzBase;
    if (_mainObject->Words()->DisappearCountdown() || //
        _mainObject->SelectCardUi_m()->OrizontalScreenOffset() != 4250) {
        iter = _selectCardVec.begin();
        return;
    }

    if (_mainObject->GlobalClock() % _selectInterval != 0) {
        return; // 选卡间隔为 _selectInterval
    }

    if (iter != _selectCardVec.end()) {
        if (*iter >= 49) {
            AAsm::ChooseImitatorCard(*iter - 49);
        } else {
            AAsm::ChooseCard(*iter);
        }
        ++iter;
        return;
    }

    if (_pvzBase->SelectCardUi_p()->LetsRockBtn()->IsUnusable()) {
        static int cnt = 0;
        ++cnt;
        AAsm::Click(_pvzBase->MouseWindow(), 100, 50, 1);
        if (cnt == 10) {
            cnt = 0;
            iter = _selectCardVec.begin();
        }

        return;
    }

    // 等待最后一个卡片进入卡槽
    int index = *(iter - 1);
    if (index > 48) {
        index = 48;
    }

    if (_pvzBase->SelectCardUi_p()->CardMoveState(index) == 1) {
        AAsm::Rock();
    }
}

void __ACardManager::SelectCards(const std::vector<int>& lst, int selectInterval)
{
    if (selectInterval <= 0) {
        __aInternalGlobal.loggerPtr->Error("ASelectCards 不允许选卡间隔小于 1cs");
        return;
    }
    _selectInterval = selectInterval;

    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();

    _selectCardVec.clear();
    std::unordered_set<int> repetitiveTypeSet;
    bool isImitatorSelected = false;
    for (const auto& cardType : lst) {
        if (cardType > 87) {
            __aInternalGlobal.loggerPtr->Error("您选择的代号为 " + pattern + " 的卡片在 PvZ 中不存在",
                cardType);
            return;
        }

        if (repetitiveTypeSet.find(cardType) == repetitiveTypeSet.end()) { // 没有被选择的卡片
            repetitiveTypeSet.insert(cardType);
        } else {
            __aInternalGlobal.loggerPtr->Error("您重复选择了代号为 " + pattern + " 的卡片", cardType);
            return;
        }

        if (!isImitatorSelected) {
            isImitatorSelected = (cardType > AIMITATOR);
        } else if (cardType > AIMITATOR) {
            __aInternalGlobal.loggerPtr->Error("您重复选择了模仿者卡片");
            return;
        }
    }

    _selectCardVec = lst;
    AWaitForFight();
}

bool __ACardManager::_Check(int seedIndex)
{
    auto seedCount = _mainObject->SeedArray()->Count();
    if (seedIndex > seedCount || seedIndex < 1) {
        __aInternalGlobal.loggerPtr->Error(
            "Card : 您填写的参数 " + std::to_string(seedIndex) + " 已溢出");
        return false;
    }

    AAsm::ReleaseMouse();
    auto seed = _mainObject->SeedArray() + seedIndex - 1;
    if (!AIsSeedUsable(seed)) {
        __aInternalGlobal.loggerPtr->Error(
            "Card : 第 " + std::to_string(seedIndex) + " 张卡片还有 "
            + std::to_string(seed->InitialCd() - seed->Cd() + 1) // PvZ计算问题导致+1
            + " cs 才能使用或者阳光不足");
        return false;
    }
    return true;
}

APlant* __ACardManager::_BasicCard(int seedIndex, int row, float col)
{
    if (!_Check(seedIndex)) {
        return nullptr;
    }
    __aInternalGlobal.loggerPtr->Info("Plant Card (" + std::to_string(seedIndex) + ") to ("
            + std::to_string(row) + ", " + AGetInternalLogger()->GetPattern() + ")",
        col);
    int x;
    int y;
    col = int(col + 0.5);
    AGridToCoordinate(row, col, x, y);
    auto mainObject = __aInternalGlobal.mainObject;
    auto currentIdx = mainObject->PlantNext();
    AAsm::PlantCard(x, y, seedIndex - 1);
    AAsm::ReleaseMouse();
    return currentIdx == mainObject->PlantNext() ? nullptr : mainObject->PlantArray() + currentIdx;
}

APlant* __ACardManager::_BasicCard(int seedIndex, const std::vector<APosition>& lst)
{
    if (!_Check(seedIndex)) {
        return nullptr;
    }

    int x = 0;
    int y = 0;
    int col = 0;
    auto mainObject = __aInternalGlobal.mainObject;
    auto currentIdx = mainObject->PlantNext();
    for (const auto& crood : lst) {
        col = int(crood.col + 0.5);
        AGridToCoordinate(crood.row, col, x, y);
        AGetInternalLogger()->Info(
            "Try Plant Card (" + std::to_string(seedIndex) + ") to ("
                + std::to_string(crood.row) + ", " + AGetInternalLogger()->GetPattern() + ")",
            crood.col);
        AAsm::PlantCard(x, y, seedIndex - 1);
        if (currentIdx != mainObject->PlantNext()) {
            return mainObject->PlantArray() + currentIdx;
        }
    }
    AAsm::ReleaseMouse();
    return nullptr;
}

int __ACardManager::GetCardIndex(APlantType plantType)
{
    auto&& pattern = __aInternalGlobal.loggerPtr->GetPattern();
    auto it = _seedNameToIndexMap.find(plantType);
    if (it == _seedNameToIndexMap.end()) {
        __aInternalGlobal.loggerPtr->Error("你没有选择卡片代号为 " + pattern + " 的植物", plantType);
        return -1;
    }
    return it->second;
}

__ANodiscard ASeed* AGetCardPtr(APlantType plantType)
{
    auto index = __ACardManager::GetCardIndex(plantType);
    return index < 0 ? nullptr : AGetMainObject()->SeedArray() + index;
}

APlant* __ACardManager::Card(int seedIndex, int row, float col)
{
    return _BasicCard(seedIndex, row, col);
}

APlant* __ACardManager::Card(APlantType plantType, int row, float col)
{
    int seedIndex = GetCardIndex(plantType);
    if (seedIndex == -1) {
        return nullptr;
    }

    return _BasicCard(seedIndex + 1, row, col);
}

std::vector<APlant*> __ACardManager::Card(const std::vector<ACardName>& lst)
{
    std::vector<APlant*> vec;
    for (const auto& each : lst) {
        vec.push_back(Card(each.plantType, each.row, each.col));
    }
    return vec;
}

APlant* __ACardManager::Card(int seedIndex, const std::vector<APosition>& lst)
{
    return _BasicCard(seedIndex, lst);
}

APlant* __ACardManager::Card(APlantType plantType, const std::vector<APosition>& lst)
{
    int seedIndex = GetCardIndex(plantType);
    if (seedIndex == -1) {
        return nullptr;
    }
    return _BasicCard(seedIndex + 1, lst);
}