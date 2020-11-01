/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvzfunc
 */

#include "pvzfunc.h"

void Asm::ClickScene(MainObject *level, int x, int y, int key)
{
    if ((*(PvZ **)0x6a9ec0)->gameUi() != 3)
    {
        return;
    }
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

void Asm::click(MouseWindow *mw, int x, int y, int key)
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
void Asm::saveData()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "pushl %%eax;"
        "movl $0x408c30, %%eax;"
        "calll %%eax;"
        "popal;"
        :
        :
        :);
#else
    __asm {
        pushad
        mov eax,[0x6a9ec0]
        mov eax,[eax+0x768]
        push eax
        mov eax,0x408c30
        call eax
        popad
    }
#endif
}

void Asm::loadData()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%eax;"
        "pushl %%eax;"
        "movl $0x44f7a0, %%eax;"
        "calll %%eax;"
        "popal;"
        :
        :
        :);
#else
    __asm {
        pushad
        mov eax,[0x6a9ec0]
        push eax
        mov eax,0x44f7a0
        call eax
        popad
    }
#endif
}

void Asm::rock()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x774(%%ebx), %%ebx;"
        "movl $0x486d20, %%eax;"
        "movl 0x6a9ec0, %%esi;"
        "movl $0x1, %%edi;"
        "movl $0x1, %%ebp;"
        "calll %%eax;"
        "popal;"
        :
        :
        :);
#else
    __asm {
        pushad
        mov ebx,[0x6a9ec0]
        mov ebx,[ebx+0x774]
        mov eax,0x486d20
        mov esi,[0x6a9ec0]
        mov edi,0x1
        mov ebp,0x1
        call eax
        popad
    }
#endif
}

void Asm::mouseClick(int x, int y, int key)
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "pushl %[x];"
        "movl %[y], %%eax;"
        "movl %[key], %%ebx;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x320(%%ecx), %%ecx;"
        "movl $0x539390, %%edx;"
        "calll %%edx;"
        "pushl %[key];"
        "pushl %[x];"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x320(%%eax), %%eax;"
        "movl %[y], %%ebx;"
        "movl $0x5392e0, %%edx;"
        "calll %%edx;"
        "popal;"
        :
        : [ x ] "g"(x), [ y ] "g"(y), [ key ] "g"(key)
        :);
#else
    __asm {
        pushad
        push x
        mov eax,y
        mov ebx,act
        mov ecx,[0x6a9ec0]
        mov ecx,[ecx+0x320]
        mov edx,0x539390
        call edx
        push act
        push x
        mov eax,[0x6a9ec0]
        mov eax,[eax+0x320]
        mov ebx,y
        mov edx,0x5392e0
        call edx
        popad
    }
#endif
}

void Asm::paoShoot(int x, int y, int rank)
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%edi;"
        "movl 0xac(%%edi), %%eax;"
        "movl $0x14c, %%ecx;"
        "imull %[rank], %%ecx;"
        "addl %%ecx, %%eax;"
        "pushl %[y];"
        "pushl %[x];"
        "movl $0x466d50, %%edx;"
        "calll %%edx;"
        "popal;"
        :
        : [ x ] "g"(x), [ y ] "g"(y), [ rank ] "g"(rank)
        :);
#else
    __asm {
        pushad
        mov eax,[0x6a9ec0]
        mov edi,[eax+0x768]
        mov eax,[edi+0xac]
        mov ecx,0x14c
        imul ecx,%[rank]
        add eax,ecx
        push %[x]
        push %[y]
        mov edx,0x466d50
        call edx
        popad
    }
#endif
}