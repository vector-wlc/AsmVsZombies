/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvzfunc
 */

#include "pvzfunc.h"

void click_scene(MainObject *level, int x, int y, int key)
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushl %0;"
        "pushl %1;"
        "pushl %2;"
        "movl %3, %%ecx;"
        "movl $0x411f20, %%eax;"
        "calll %%eax;"
        :
        : "g"(key), "g"(y), "g"(x), "g"(level)
        : "eax", "ecx");
#else
    __asm {
        push key
        push y
        push x
        mov ecx, level
        mov eax, 0x411f20
        call eax
    }
#endif
}

void click(MouseWindow *mw, int x, int y, int key)
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushl %0;"
        "movl %1, %%ebx;"
        "movl %2, %%eax;"
        "movl %3, %%ecx;"
        "movl $0x539390, %%edx;"
        "calll %%edx;"
        :
        : "g"(x), "g"(key), "g"(y), "g"(mw)
        : "eax", "ecx", "ebx", "edx");
#else
    __asm {
		push ebx;
		push x;
		mov ebx, key;
		mov eax, y;
		mov ecx, mw;
		mov edx, 0x539390;
		call edx;
		pop ebx;
    }
    ;
#endif
}
