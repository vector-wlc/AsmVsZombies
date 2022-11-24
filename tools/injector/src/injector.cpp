/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-07 21:30:31
 * @Description:
 */
#include "_process.h"
#include <fstream>
#include <iostream>

int main()
{
    Process process;
    if (process.OpenByWindow(L"MainWindow", L"Plants vs. Zombies")) {
        process.ManageDLL();
    }
    return 0;
}
