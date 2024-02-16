#pragma once
#ifndef __TICKPLANTERPLUS_H__
#define __TICKPLANTERPLUS_H__

#include "avz.h"

class TickPlanterPlus : public ATickRunnerWithNoStart {
public:
    // 在种植操作队列尾部添加操作，如果这些操作失败则什么也不做，相当于ACard
    // ***使用示例：
    // tick_planter_plus.PlantNotInQuene(ASUNFLOWER, {{2, 8}});
    // 在2行8列种植向日葵，如果种植失败则什么也不做
    void PlantNotInQuene(int type, std::vector<AGrid> grid_list) { _Planter(type, grid_list, ABANDON); }
    void PlantNotInQuene(int type, int row, int col) { PlantNotInQuene(type, {{row, col}}); }

    // 连续种植：在种植操作队列尾部添加操作，如果这些操作失败则重试，直到全部种植操作执行成功
    // ***使用示例：
    // tick_planter_plus.DelayPlantNotInQuene(ASUNFLOWER, {{1, 1}, {2, 1}});
    // 先后在(1,1)、(2,1)种植向日葵，如果种植失败则重试，直到全部种植操作执行成功
    void DelayPlantNotInQuene(int type, std::vector<AGrid> grid_list) { _Planter(type, grid_list, DELAY); }
    void DelayPlantNotInQuene(int type, int row, int col) { DelayPlantNotInQuene(type, {{row, col}}); }

    // 铲除后种植：在种植操作队列尾部添加操作，如果这些操作仅因位置被占用而失败则铲除占位的植物重试，如果仍失败，则继续重试直到全部种植操作执行成功
    // ***使用示例：
    // tick_planter_plus.ShovelPlantNotInQuene(ASQUASH, {{1, 4}});
    // 在(1,4)种植窝瓜，如果仅因(1,4)被其他植物占用而种植失败，则将其铲除并重试，如果仍失败，则继续重试直到全部种植操作执行成功
    void ShovelPlantNotInQuene(int type, std::vector<AGrid> grid_list) { _Planter(type, grid_list, SHOVEL_AND_TRY_AGAIN); }
    void ShovelPlantNotInQuene(int type, int row, int col) { ShovelPlantNotInQuene(type, {{row, col}}); }

    // 补充后种植：在种植操作队列尾部添加操作，如果这些操作仅因需要容器而失败则补种容器重试，如果仍失败，则继续重试直到种植成功
    // ***使用示例：
    // tick_planter_plus.AddPlantNotInQuene(ACHERRY_BOMB, {{2, 8}});
    // 在(2,8)种植樱桃炸弹，如果仅因(2,8)需要花盆或荷叶而种植失败，则补种花盆或荷叶后重试，如果仍失败，则继续重试直到种植成功
    void AddPlantNotInQuene(int type, std::vector<AGrid> grid_list) { _Planter(type, grid_list, ADD_AND_TRY_AGAIN); }
    void AddPlantNotInQuene(int type, int row, int col) { AddPlantNotInQuene(type, {{row, col}}); }

    // 优先种植：在种植操作队列尾部添加操作，如果这些操作失败则暂停其他种植操作并重试此操作，直到全部种植操作执行成功
    // ***使用示例：
    // tick_planter_plus.WaitPlantNotInQuene(AGARLIC, {{1, 4}, {2, 4}});
    // 先后在(1,4)、(2,4)种植大蒜，仅当最靠前的种植操作成功时，才会尝试其他种植操作
    void WaitPlantNotInQuene(int type, std::vector<AGrid> grid_list) { _Planter(type, grid_list, WAIT); }
    void WaitPlantNotInQuene(int type, int row, int col) { WaitPlantNotInQuene(type, {{row, col}}); }

    // 优先铲除种植：在种植操作队列尾部添加操作，如果这些操作仅因位置被占用而失败则暂停其他种植操作并铲除占位的植物重试
    // ***使用示例：
    // tick_planter_plus.WaitShovelPlant(ASNOW_PEA, {{1, 1}, {2, 1}});
    // 先后在(1,1)、(2,1)种植寒冰射手，如果这些操作仅因位置被占用而失败则暂停其他种植操作并铲除占位的植物重试，仅当最靠前的种植操作成功时，才会尝试其他种植操作
    void WaitShovelPlant(int type, std::vector<AGrid> grid_list) { _Planter(type, grid_list, WAIT_SHOVEL); }
    void WaitShovelPlant(int type, int row, int col) { WaitShovelPlant(type, {{row, col}}); }

    // 优先补充种植：在种植操作队列尾部添加操作，如果这些操作仅因需要容器而失败则暂停其他种植操作并补种容器重试
    // ***使用示例：
    // tick_planter_plus.WaitAddPlant(AWALL_NUT, {{1, 1}, {2, 1}});
    // 先后在(1,1)、(2,1)种植坚果，如果这些操作仅因需要容器而失败则暂停其他种植操作并补种容器重试，仅当最靠前的种植操作成功时，才会尝试其他种植操作
    void WaitAddPlant(int type, std::vector<AGrid> grid_list) { _Planter(type, grid_list, WAIT_ADD); }
    void WaitAddPlant(int type, int row, int col) { WaitAddPlant(type, {{row, col}}); }

    // 启用智能种植操作
    // [isClearQueue]选择是否在开始种植前清除之前的种植信息，默认为 是
    void Start(bool isClearQueue = true)
    {
        if (isClearQueue) {
            Clear();
        }
        ATickRunnerWithNoStart::_Start([this] { _Run(); }, ATickRunner::ONLY_FIGHT);
    }

    // 清空种植操作队列
    // ***使用示例：
    // tick_planter_plus.Clear();
    void Clear()
    {
        priorPlantQueue.clear();
        routinePlantQueue.clear();
        plantQueue.clear();
        plantQueue = {priorPlantQueue, routinePlantQueue};
    }

    // 设置tick_planter_plus遍历种植操作队列时间间隔，默认为1厘秒
    // 使用示例：
    // tick_planter_plus.SetInterval(10);
    // 设置tick_planter_plus遍历种植操作队列时间间隔为10厘秒
    void SetInterval(int timeInterval) { _timeInterval = timeInterval; }

protected:
    // 种植失败的处理
    enum IfFailDo {
        // 放弃，不会再执行此操作
        ABANDON,
        // 推迟，下次仍会执行此操作
        DELAY,
        // 如果仅因为位置被占用而失败，则铲除占位植物并重试，如果重试失败，则推迟
        SHOVEL_AND_TRY_AGAIN,
        // 如果仅因为位置需要花盆或荷叶而失败，则补种花盆或荷叶后重试，如果重试失败，则推迟
        ADD_AND_TRY_AGAIN,
        // 阻塞，跳过所有其他操作直到此操作被执行成功
        WAIT,
        // 如果仅因为位置被占用而失败，则铲除占位植物并重试，如果重试失败，则阻塞
        WAIT_SHOVEL,
        // 如果仅因为位置需要花盆或荷叶而失败，则补种花盆或荷叶后重试，如果重试失败，则阻塞
        WAIT_ADD,
    };
    // 种植操作数据
    struct Planting {
        // 种植的种子类型
        int plantType;
        // 种植的位置：{row, col}:[row]行[col]列
        AGrid position;
        // 种植失败时的处理
        IfFailDo IF_FAIL_DO = DELAY;
    };
    // 优先队列
    std::vector<Planting> priorPlantQueue;
    // 常规队列
    std::vector<Planting> routinePlantQueue;
    // 种植操作队列向量
    std::vector<std::vector<Planting>> plantQueue = {priorPlantQueue, routinePlantQueue};
    // 遍历种植操作队列的时间间隔
    int _timeInterval = 1;

    // 在[grid_list]位置种植[plantType]植物，如果失败则按[plantMode]方式处理，直到所有种植操作执行成功
    void _Planter(int type, std::vector<AGrid> gridList, IfFailDo plantMode)
    {
        for (const auto& [row, col] : gridList) {
            plantQueue[ARangeIn(plantMode, {WAIT, WAIT_SHOVEL, WAIT_ADD}) ? 0 : 1].push_back({type, {row, col}, plantMode});
        }
    }

    void _Run()
    {
        if (AGetMainObject()->GameClock() % _timeInterval != 0)
            return;
        auto seed_array = AGetMainObject()->SeedArray();
        for (int i = 0; i < 2; ++i) {
            for (int index = 0; index < plantQueue[i].size(); ++index) {
                auto iter = plantQueue[i][index];
                int plant_type = iter.plantType > AIMITATOR ? iter.plantType - AIMITATOR - 1 : iter.plantType;
                int index_seed = AGetSeedIndex(iter.plantType);
                int plantRejectType = AAsm::GetPlantRejectType(plant_type, iter.position.row - 1, iter.position.col - 1);
                bool isPlantSuccessful = false;
                if (plantRejectType == AAsm::NIL && AIsSeedUsable(&seed_array[index_seed])) {
                    auto plantPtr = ACard(index_seed + 1, iter.position.row, iter.position.col);
                    if (plantPtr != nullptr) {
                        isPlantSuccessful = true;
                        plantQueue[i].erase(plantQueue[i].begin() + index);
                    }
                }

                if (!isPlantSuccessful) {
                    switch (iter.IF_FAIL_DO) {
                    case ABANDON:
                        plantQueue[i].erase(plantQueue[i].begin() + index);
                        break;
                    case SHOVEL_AND_TRY_AGAIN:
                        if (plantRejectType == AAsm::NOT_HERE && AIsSeedUsable(&seed_array[index_seed]) && AGetPlantIndex(iter.position.row, iter.position.col) >= 0) {
                            AShovel(iter.position.row, iter.position.col);
                            index--;
                        }
                        break;
                    case ADD_AND_TRY_AGAIN:
                        if (AIsSeedUsable(&seed_array[index_seed])) {
                            if (plantRejectType == AAsm::NEEDS_POT && AIsSeedUsable(AFLOWER_POT)) {
                                ACard(AFLOWER_POT, iter.position.row, iter.position.col);
                                index--;
                            }
                            if (plantRejectType == AAsm::NOT_ON_WATER && AIsSeedUsable(ALILY_PAD)) {
                                ACard(ALILY_PAD, iter.position.row, iter.position.col);
                                index--;
                            }
                        }
                        break;
                    case WAIT:
                        return;
                        break;
                    case WAIT_SHOVEL:
                        if (plantRejectType == AAsm::NOT_HERE && AIsSeedUsable(&seed_array[index_seed]) && AGetPlantIndex(iter.position.row, iter.position.col) >= 0)
                            AShovel(iter.position.row, iter.position.col);
                        return;
                        break;
                    case WAIT_ADD:
                        if (AIsSeedUsable(&seed_array[index_seed])) {
                            if (plantRejectType == AAsm::NEEDS_POT && AIsSeedUsable(AFLOWER_POT))
                                ACard(AFLOWER_POT, iter.position.row, iter.position.col);
                            if (plantRejectType == AAsm::NOT_ON_WATER && AIsSeedUsable(ALILY_PAD))
                                ACard(ALILY_PAD, iter.position.row, iter.position.col);
                        }
                        return;
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    }
};
inline TickPlanterPlus tickPlanterPlus;

#endif //!__TICKPLANTERPLUS_H__