#pragma once
#ifndef __PLANTOPERATOR_H__
#define __PLANTOPERATOR_H__

#include "avz.h"

class PlantOperator : public ATickRunnerWithNoStart {
protected:
    // 待种植的植物种类
    APlantType _plantType;
    // 待种植植物的格子位置列表
    std::vector<AGrid> _gridList;
    // 种子序号列表
    std::vector<int> _seedIndexList = {-1, -1, -1};
    // 是否同时使用模仿种子执行种植
    bool _isUseImitatorSeed = true;
    // 种植阳光阈值
    int _plantSunThreshold = 0;
    // 定时执行的种植判断与操作
    void _Run();

    // 返回[plantType]植物是否为升级植物
    bool _GetIsUpdateType(int plantType);

    // 返回[plantType]种类植物的模仿种类，[plantType]为一个属于[0, 47]的整数
    int _GetImitatorType(int plantType);

    // 如果[plantType]植物为升级植物，返回其前置植物种类，否则返回[plantType]
    APlantType _GetPrePlantType(APlantType plantType);

    // 返回[plantType]的[seed_index]号种子是否可用，升级植物的阳光花费考虑生存模式的递增
    bool _GetIsSeedUsable(int plantType, int seed_index);

    // 返回某卡的冷却倒计时
    int _GetCardCD(int plantType);

public:
    // 阳光不低于[plantThreshold]时，在[grids]位置种植[plantType]植物，自动补种花盆或睡莲叶，默认使用模仿种子种植
    // ------------参数------------
    // [plantType]为植物种类，填入一个属于[0, 47]的整数或APlantType（模仿植物除外）
    // [grids]为待种植的格子位置列表
    // [plantSunThreshold]为种植阳光阈值，阳光不低于此值时触发种植操作，不填时为 [plantType]植物的阳光值
    // [isUseImitatorSeed]为是否同时使用模仿种子种植，不填时为 是
    // ------------示例------------
    // Start(ACOB_CANNON, {{1, 1}, {2, 1}}, 1200, false); // 种植{1，1}，{2，1}的玉米加农炮，种植阳光阈值为1200，不使用模仿种子
    void Start(APlantType plantType, std::vector<AGrid> grids, int plantSunThreshold = 0, bool isUseImitatorSeed = true);

    // 设置种植阳光阈值，阳光数量小于此值时不种植，默认为 0
    void SetPlantSunThreshold(int plantSunThreshold);

    // 重置植物种植位置
    void SetPlantList(std::vector<AGrid> grids);
};

inline bool PlantOperator::_GetIsUpdateType(int plantType)
{
    return plantType >= AGATLING_PEA && plantType <= ACOB_CANNON;
}
inline int PlantOperator::_GetImitatorType(int plantType)
{
    return plantType + (AIMITATOR + 1);
}
inline APlantType PlantOperator::_GetPrePlantType(APlantType plantType)
{
    std::map<APlantType, APlantType> pre_plant_map = {{AGATLING_PEA, AREPEATER}, {ATWIN_SUNFLOWER, ASUNFLOWER}, {AGLOOM_SHROOM, AFUME_SHROOM}, {ACATTAIL, ALILY_PAD}, {AWINTER_MELON, AMELON_PULT}, {AGOLD_MAGNET, AMAGNET_SHROOM}, {ASPIKEROCK, ASPIKEWEED}, {ACOB_CANNON, AKERNEL_PULT}};
    return _GetIsUpdateType(plantType) ? pre_plant_map[plantType] : plantType;
}
inline int PlantOperator::_GetCardCD(int plantType)
{
    int index_seed = AGetSeedIndex(plantType);
    if (index_seed == -1)
        return -1;

    auto seeds = AGetMainObject()->SeedArray();
    if (seeds[index_seed].IsUsable())
        return 0;

    return seeds[index_seed].InitialCd() - seeds[index_seed].Cd() + 1;
}
inline bool PlantOperator::_GetIsSeedUsable(int plantType, int seed_index)
{
    auto seeds = AGetMainObject()->SeedArray();
    bool isUsable = seeds[seed_index].IsUsable();
    if (plantType == ACOB_CANNON) {
        if (AGetSeedIndex(AKERNEL_PULT) >= 0 && AGetSeedIndex(AM_KERNEL_PULT) >= 0)
            isUsable = _GetCardCD(ACOB_CANNON) == 0;
        else
            isUsable = _GetCardCD(ACOB_CANNON) < 751;
    }
    int sunCost = AGetSeedSunVal(APlantType(plantType));
    return isUsable && sunCost <= AGetMainObject()->Sun();
}
inline void PlantOperator::_Run()
{
    int sunCost = AGetSeedSunVal(_plantType);
    if (_GetIsUpdateType(_plantType)) {
        int pre_sun_cost = AGetSeedSunVal(_GetPrePlantType(_plantType));
        sunCost += _plantType == ACOB_CANNON ? 2 * pre_sun_cost : pre_sun_cost;
    }
    // 阳光数量小于阈值时或种植所需阳光时不种植
    if (AGetMainObject()->Sun() < std::max(_plantSunThreshold, sunCost))
        return;

    // 升级植物不可用时不种植
    if (_GetIsUpdateType(_plantType) && !_GetIsSeedUsable(_plantType, _seedIndexList[2]))
        return;

    std::vector<AGrid> plantGrids;
    plantGrids.clear();
    for (const auto& [row, col] : _gridList) {
        if (AGetPlantIndex(row, col, _plantType) < 0) {
            plantGrids.push_back({row, col});
            if (_plantType == ACOB_CANNON) {
                plantGrids.push_back({row, col + 1});
            }
            break;
        }
    }
    if (plantGrids.empty())
        return;

    auto seed_array = AGetMainObject()->SeedArray();
    int seedType, plantType;
    for (const auto& [row, col] : plantGrids) {
        for (const auto& eachSeedIndex : _seedIndexList) {
            if (eachSeedIndex < 0)
                continue;

            seedType = seed_array[eachSeedIndex].Type();
            plantType = seedType == AIMITATOR ? _GetPrePlantType(_plantType) : seedType;
            if (AGetPlantIndex(row, col, plantType) >= 0)
                continue;

            if (!_GetIsSeedUsable(plantType, eachSeedIndex))
                continue;

            auto plant_reject_type = AAsm::GetPlantRejectType(plantType, row - 1, col - 1);
            if (plant_reject_type == AAsm::NOT_HERE && AGetPlantIndex(row, col) >= 0) {
                AShovel(row, col, _plantType == ACOB_CANNON ? true : false);
                return;
            }
            if (plant_reject_type == AAsm::NEEDS_POT && AIsSeedUsable(AFLOWER_POT))
                ACard(AFLOWER_POT, row, col);
            if (plant_reject_type == AAsm::NOT_ON_WATER && AIsSeedUsable(ALILY_PAD))
                ACard(ALILY_PAD, row, col);
            if (plant_reject_type == AAsm::NIL)
                ACard(eachSeedIndex + 1, row, col);
        }
    }
}

inline void PlantOperator::Start(APlantType plantType, std::vector<AGrid> grids, int plantSunThreshold, bool isUseImitatorSeed)
{
    // 植物种类合法性检查
    if (!(plantType >= APEASHOOTER && plantType <= ACOB_CANNON)) {
        AGetInternalLogger()->Error("PlantOperator:\n  非法的植物种类 # ，请填入一个属于[0, 47]的整数", plantType);
        return;
    }
    _plantType = plantType;
    // 获取种子序号数组
    // 种子序号或其前置植物种子的序号
    _seedIndexList[0] = AGetSeedIndex(_GetPrePlantType(_plantType));
    // 模仿种子序号或其前置植物模仿种子的序号
    _seedIndexList[1] = _isUseImitatorSeed ? AGetSeedIndex(_GetImitatorType(_GetPrePlantType(_plantType))) : -1;
    // 升级植物的种子序号
    _seedIndexList[2] = _GetIsUpdateType(_plantType) ? AGetSeedIndex(_plantType) : -1;
    // 卡槽可用性检查
    if (_seedIndexList[0] == -1 && _seedIndexList[1] == -1 && _seedIndexList[2] == -1) {
        AGetInternalLogger()->Error("PlantOperator:\n    由于您未携带种类代号为 # 的植物的相关种子，无法执行种植操作。", _plantType);
        return;
    }
    SetPlantList(grids);
    SetPlantSunThreshold(plantSunThreshold);
    _isUseImitatorSeed = isUseImitatorSeed;
    ATickRunnerWithNoStart::_Start([this] { _Run(); }, ATickRunner::ONLY_FIGHT);
}
inline void PlantOperator::SetPlantSunThreshold(int plantSunThreshold)
{
    _plantSunThreshold = plantSunThreshold;
}
inline void PlantOperator::SetPlantList(std::vector<AGrid> grids)
{
    if (grids.empty()) {
        AGetInternalLogger()->Error("PlantOperator:\n  种植位置列表不可为空！");
        return;
    }
    // 格子位置合法性检查
    for (const auto& [row, col] : grids) {
        // 泳池或雾夜为六行场地
        if (AGetMainObject()->Scene() == 2 || AGetMainObject()->Scene() == 3) {
            if (!(row >= 1 && row <= 6)) {
                AGetInternalLogger()->Error("PlantOperator:\n  非法的格子位置行数 # ，请填入一个属于[1, 6]的整数", row);
                return;
            }
        } else {
            if (!(row >= 1 && row <= 5)) {
                AGetInternalLogger()->Error("PlantOperator:\n  非法的格子位置行数 # ，请填入一个属于[1, 5]的整数", row);
                return;
            }
        }
        if (_plantType == ACOB_CANNON) {
            if (!(col >= 1 && col <= 8)) {
                AGetInternalLogger()->Error("PlantOperator:\n  非法的格子位置列数 # ，请填入一个属于[1, 8]的整数，注意玉米加农炮的位置判定为后轮（远离眼睛的一侧）所在的格子", row);
                return;
            }

        } else {
            if (!(col >= 1 && col <= 9)) {
                AGetInternalLogger()->Error("PlantOperator:\n  非法的格子位置列数 # ，请填入一个属于[1, 9]的整数", row);
                return;
            }
        }
    }
    _gridList = grids;
}

#endif //!__PLANTOPERATOR_H__