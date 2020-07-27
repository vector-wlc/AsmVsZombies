/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvz func click_scene
 */
#pragma once

#include "pvzstruct.h"
#include <Windows.h>
#include <initializer_list>

// key 1: left click
// key -1: right click
// key 3: middle click
void click_scene(MainObject *level, int x, int y, int key);

void click(MouseWindow *mw, int x, int y, int key);

void show_str(const char *str);
