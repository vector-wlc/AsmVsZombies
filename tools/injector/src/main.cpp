/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-07 21:30:31
 * @Description:
 */
#include "injector.h"

int main()
{
    setlocale(LC_ALL, "");
    Process process;
    if (process.SelectWindow()) {
        process.ManageDLL();
    }
    return 0;
}
