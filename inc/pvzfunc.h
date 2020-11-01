/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvz func click_scene
 */
#pragma once

#include <Windows.h>

#include <initializer_list>

#include "pvzstruct.h"

// 使用汇编代码调用 PvZ 游戏内部的函数
class Asm
{
public:
    // key 1: left click
    // key -1: right click
    // key 3: middle click
    static void ClickScene(MainObject *level, int x, int y, int key);

    static void click(MouseWindow *mw, int x, int y, int key);

    static void saveData();

    static void loadData();

    // 进入战斗页面，开始游戏
    static void rock();

    static void mouseClick(int x, int y, int key);

    static void paoShoot(int x, int y, int rank);
};
