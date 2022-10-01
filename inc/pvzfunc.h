/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvz func click_scene
 */
#pragma once

#include "avz_global.h"
#include "pvzstruct.h"
#include <Windows.h>
#include <initializer_list>

// 使用汇编代码调用 PvZ 游戏内部的函数
class Asm {

public:
    // key 1: left click
    // key -1: right click
    // key 3: middle click
    static void clickScene(MainObject* level, int x, int y, int key);

    static void click(MouseWindow* mw, int x, int y, int key);

    static void saveData();

    static void loadData();

    // 进入战斗页面，开始游戏
    static void rock();

    static void mouseClick(int x, int y, int key);

    // 发射炮
    static void shootPao(int x, int y, int rank);

    // 种卡函数
    static void plantCard(int x, int y, int index);

    // 铲除函数
    static void shovelPlant(int x, int y);

    // 选择卡片
    static void chooseCard(int card_type);

    // 选择模仿者卡片
    static void chooseImitatorCard(int card_type);

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
    // if(getPlantRejectType(XRK_1, 1, 1) == Asm::NIL) { // NIL 表示可以种植物， 检查一下 2 行 2 列是否可以种植物
    //
    // }
    static int getPlantRejectType(int card_type, int row, int col);

    // 战斗界面刷新函数
    static void gameFightLoop();

    // 游戏整体刷新函数
    static void gameTotalLoop();

    // 游戏最完整刷新函数，带帧率控制
    static void gameSleepLoop();

    static void clearObjectMemory();

    static void gameExit();

    static void killZombiesPreview();

    static void setImprovePerformance(bool is_improve_performance);

    static void releaseMouse();

    static int gridToAbscissa(int row, int col);

    static int gridToOrdinate(int row, int col);

private:
    static void __clickScene();

    static void __click();

    static void __mouseClick();

    static void __shootPao();

    static void __plantCard();

    static void __shovelPlant();

    static void __chooseCard();

    static void __chooseImitatorCard();

    static void __getPlantRejectType();
};
