/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-07 21:30:31
 * @Description: 
 */
#include <iostream>
#include "_process.h"
#include <fstream>

using namespace std;

int main()
{
    Process process;
    if (process.OpenByWindow(L"MainWindow", L"Plants vs. Zombies"))
    {
        process.ManageDLL();
    }
    return 0;
}
