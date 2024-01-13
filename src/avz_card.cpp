#include "avz_card.h"
#include "avz_asm.h"
#include "avz_click.h"
#include "avz_logger.h"
#include "avz_memory.h"
#include "avz_script.h"
#include "avz_tick_runner.h"
#include <unordered_set>

void __ACardManager::_BeforeScript()
{
    _isSelectCards = false;
    _tickRunner.Start([this] {
        _selectInterval = std::max(1, _selectInterval);
        if (__aig.mainObject->GlobalClock() % _selectInterval != 0) {
            return; // 选卡间隔为 _selectInterval
        }
        __aCardManager._ChooseSingleCard();
    },
        ATickRunner::GLOBAL);
}

void __ACardManager::_EnterFight()
{
    _tickRunner.Stop();
    _seedNameToIndexMap.clear();
    _selectCardVec.clear();

    if (__aig.pvzBase->GameUi() != 3) {
        return;
    }

    auto seed = __aig.mainObject->SeedArray();
    int seedCount = seed->Count();
    int seedType;
    std::pair<int, int> seedInfo;
    for (int i = 0; i < seedCount; ++i, ++seed) {
        seedType = seed->Type();
        // 如果是模仿者卡片
        if (seedType == AIMITATOR) {
            seedType = seed->ImitatorType();
            seedInfo.first = seedType + AM_PEASHOOTER;
            seedInfo.second = i;
        } else { // if(seed_info != 48)
            seedInfo.first = seedType;
            seedInfo.second = i;
        }
        _seedNameToIndexMap.insert(seedInfo);
    }
}

void __ACardManager::_ChooseSingleCard()
{
    static auto iter = _selectCardVec.begin();
    if (__aig.mainObject->Words()->DisappearCountdown() || //
        __aig.mainObject->SelectCardUi_m()->OrizontalScreenOffset() != 4250) {
        iter = _selectCardVec.begin();
        return;
    }

    if (!_isSelectCards) {
        return;
    }

    if (iter != _selectCardVec.end()) {
        if (*iter >= AM_PEASHOOTER) {
            AAsm::ChooseImitatorCard(*iter - AM_PEASHOOTER);
        } else {
            AAsm::ChooseCard(*iter);
        }
        ++iter;
        return;
    }

    int okCnt = 0;
    for (auto&& type : _selectCardVec) {
        int idx = std::min(48, type);
        int state = __aig.pvzBase->SelectCardUi_p()->CardMoveState(idx);
        if (state == 1 || state == 0) {
            ++okCnt;
        }
    }

    if (okCnt != _selectCardVec.size()) {
        static int cnt = 0;
        ++cnt;
        AAsm::Click(__aig.pvzBase->MouseWindow(), 100, 50, 1);
        if (cnt == 10) {
            cnt = 0;
            iter = _selectCardVec.begin();
        }
        return;
    }
    AAsm::PickRandomSeeds();
}

void __ACardManager::SelectCards(const std::vector<int>& lst, int selectInterval)
{
    if (selectInterval < 0) {
        __aig.loggerPtr->Error("ASelectCards 不允许选卡间隔小于 0cs");
        return;
    }
    if (lst.size() > __aig.mainObject->SeedArray()->Count()) {
        __aig.loggerPtr->Error("ASelectCards 不允许选卡的数量大于卡片的数量");
        return;
    }
    _selectInterval = std::max(1, selectInterval);

    auto&& pattern = __aig.loggerPtr->GetPattern();

    _selectCardVec.clear();
    std::unordered_set<int> repetitiveTypeSet;
    bool isImitatorSelected = false;
    for (const auto& cardType : lst) {
        if (cardType > AM_MELON_PULT) {
            __aig.loggerPtr->Error("您选择的代号为 " + pattern + " 的卡片在 PvZ 中不存在",
                cardType);
            return;
        }

        if (repetitiveTypeSet.find(cardType) == repetitiveTypeSet.end()) { // 没有被选择的卡片
            repetitiveTypeSet.insert(cardType);
        } else {
            __aig.loggerPtr->Error("您重复选择了代号为 " + pattern + " 的卡片", cardType);
            return;
        }

        if (!isImitatorSelected) {
            isImitatorSelected = (cardType > AIMITATOR);
        } else if (cardType > AIMITATOR) {
            __aig.loggerPtr->Error("您重复选择了模仿者卡片");
            return;
        }
    }

    _selectCardVec = lst;
    _isSelectCards = true;
    AWaitForFight(selectInterval == 0);
}

bool __ACardManager::_Check(int seedIndex)
{
    auto seedCount = __aig.mainObject->SeedArray()->Count();
    if (seedIndex > seedCount || seedIndex < 1) {
        __aig.loggerPtr->Error(
            "Card : 您填写的参数 " + std::to_string(seedIndex) + " 已溢出");
        return false;
    }

    AAsm::ReleaseMouse();
    auto seed = __aig.mainObject->SeedArray() + seedIndex - 1;
    if (!AIsSeedUsable(seed)) {
        __aig.loggerPtr->Error(
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
    __aig.loggerPtr->Info("Plant Card (" + std::to_string(seedIndex) + ") to ("
            + std::to_string(row) + ", " + AGetInternalLogger()->GetPattern() + ")",
        col);
    int x;
    int y;
    col = int(col + 0.5);
    AGridToCoordinate(row, col, x, y);
    auto mainObject = __aig.mainObject;
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
    auto mainObject = __aig.mainObject;
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
    auto it = _seedNameToIndexMap.find(plantType);
    return it == _seedNameToIndexMap.end() ? -1 : it->second;
}

__ANodiscard ASeed* AGetCardPtr(APlantType plantType)
{
    auto index = __aCardManager.GetCardIndex(plantType);
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
        __aig.loggerPtr->Error("你没有选择卡片代号为 " + std::to_string(plantType) + " 的植物");
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
        __aig.loggerPtr->Error("你没有选择卡片代号为 " + std::to_string(plantType) + " 的植物");
        return nullptr;
    }
    return _BasicCard(seedIndex + 1, lst);
}

std::vector<APlant*> ACard(const std::vector<APlantType>& plantTypeVec, int row, float col)
{
    std::vector<APlant*> ret;
    for (auto&& plantType : plantTypeVec) {
        ret.push_back(__aCardManager.Card(plantType, row, col));
    }
    return ret;
}

std::vector<APlant*> ACard(const std::vector<APlantType>& plantTypeVec, const std::vector<APosition>& lst)
{
    std::vector<APlant*> ret;
    for (auto&& plantType : plantTypeVec) {
        for (auto&& pos : lst) {
            auto ptr = __aCardManager.Card(plantType, pos.row, pos.col);
            if (ptr != nullptr) {
                ret.push_back(ptr);
                break;
            }
        }
    }
    return ret;
}