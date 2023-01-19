/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvz func click_scene
 */
#ifndef __AVZ_ASM_H__
#define __AVZ_ASM_H__

#include "avz_pvz_struct.h"
#include "avz_types.h"

// 使用汇编代码调用 PvZ 游戏内部的函数
class AAsm {

public:
    // key 1: left click
    // key -1: right click
    // key 3: middle click
    static void ClickScene(AMainObject* level, int x, int y, int key);

    static void Click(AMouseWindow* mw, int x, int y, int key);

    static void SaveData();

    static void LoadData();

    // 进入战斗页面，开始游戏
    static void Rock();

    static void MouseClick(int x, int y, int key);

    // 发射炮
    static void Fire(int x, int y, int rank);

    // 种卡函数
    static void PlantCard(int x, int y, int index);

    // 铲除函数
    static void ShovelPlant(int x, int y);

    // 选择卡片
    static void ChooseCard(int card_type);

    // 选择模仿者卡片
    static void ChooseImitatorCard(int card_type);

    enum PlantReject {
        NIL, // 没有植物, 即可以种植
        NOT_HERE,
        ONLY_ON_GRAVES,
        ONLY_IN_POOL,
        ONLY_ON_GROUND,
        NEEDS_POT,
        NOT_ON_ART,
        NOT_PASSED_LINE,
        NEEDS_UPGRADE,
        NOT_ON_GRAVE,
        NOT_ON_CRATER,
        NOT_ON_WATER,
        NEEDS_GROUND,
        NEEDS_SLEEPING
    };

    // 是否可种卡片
    // 返回值意义查看 `enum PlantReject`
    // *** 使用示例:
    // if(AAsm::GetPlantRejectType(XRK_1, 1, 1) == AAsm::NIL) { // NIL 表示可以种植物， 检查一下 2 行 2 列是否可以种植物
    //
    // }
    __ANodiscard static int GetPlantRejectType(int card_type, int row, int col);

    // 战斗界面刷新函数
    static void GameFightLoop();

    // 游戏整体刷新函数
    static void GameTotalLoop();

    // 游戏最完整刷新函数，带帧率控制
    static void GameSleepLoop();

    static void ClearObjectMemory();

    static void GameExit();

    static void KillZombiesPreview();

    static void SetImprovePerformance(bool is_improve_performance);

    static void ReleaseMouse();

    __ANodiscard static int GridToAbscissa(int row, int col);

    __ANodiscard static int GridToOrdinate(int row, int col);

    // 放置一个植物
    // PutPlant(1, 1, ANGT_30) ----- 在2行2列放置一个南瓜头
    static APlant* PutPlant(int row, int col, APlantType type);

    // 放置一个僵尸
    // PutZombie(1, 1, APJ_0) ----- 在2行2列放置一个普通僵尸
    static AZombie* PutZombie(int row, int col, AZombieType type);

    static void RemovePlant(APlant* plant);

    static void RemoveZombie(AZombie* zombie);

protected:
    static void _ClickScene();

    static void _Click();

    static void _MouseClick();

    static void _ShootPao();

    static void _PlantCard();

    static void _ShovelPlant();

    static void _ChooseCard();

    static void _ChooseImitatorCard();

    static void _GetPlantRejectType();

    static void _RemovePlant();
};

#endif
