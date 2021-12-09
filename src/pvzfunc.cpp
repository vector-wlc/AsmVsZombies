/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvzfunc
 */

#include "pvzfunc.h"

#pragma GCC push_options
#pragma GCC optimize("O0")

void Asm::clickScene(MainObject* level, int x, int y, int key)
{
    if ((*(PvZ**)0x6a9ec0)->gameUi() != 3) {
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

void Asm::click(MouseWindow* mw, int x, int y, int key)
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
        : [x] "g"(x), [y] "g"(y), [key] "g"(key)
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

void Asm::shootPao(int x, int y, int rank)
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
        : [x] "g"(x), [y] "g"(y), [rank] "g"(rank)
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

void Asm::plantCard(int x, int y, int index)
{
#ifdef __MINGW32__
    __asm__ __volatile__(

        "pushal;"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%edi;"
        "pushl %[y];"
        "pushl %[x];"
        "pushl %%edi;"
        "movl 0x144(%%edi), %%eax;"
        "movl $0x50, %%ecx;"
        "imull %[index], %%ecx;"
        "addl %%ecx, %%eax;"
        "addl $0x28, %%eax;"
        "pushl %%eax;"
        "movl $0x488590, %%ecx;"
        "calll %%ecx;"
        "movl $0x1, %%ecx;"
        "movl $0x40fd30, %%edx;"
        "calll %%edx;"
        "popal;"
        :
        : [x] "g"(x), [y] "g"(y), [index] "g"(index)
        :);
#else
    __asm {
        pushad
        mov eax,[6A9EC0]
        mov eax,[eax+768]
        push y
        push x
        push eax
        mov eax,[eax+144]
        mov ecx,rank
        imul ecx,50
        add eax,28
        add eax,ecx
        push eax
        mov ecx,488590
        call ecx
        mov ecx,1
        mov eax,40FD30
        call eax
        popad

    }
#endif
}

void Asm::shovelPlant(int x, int y)
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "pushl $0x6;"
        "pushl $0x1;"
        "movl %[y], %%ecx;"
        "movl %[x], %%edx;"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%eax;"
        "movl $0x411060, %%ebx;"
        "calll %%ebx;"
        "popal;"
        :
        : [x] "g"(x), [y] "g"(y)
        :);
#else
    __asm {
        pushad
        push 6
        push 1
        mov ecx y
        mov edx x
        mov eax,[6A9EC0]
        mov eax,[eax+768]
        mov ebx,411060
        call ebx
        popad

    }
#endif
}

// 选择卡片
void Asm::chooseCard(int card_type)
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl $0x6A9EC0, %%eax;"
        "movl (%%eax), %%eax;"
        "movl 0x774(%%eax), %%eax;"
        "movl %[card_type], %%edx;"
        "shll $0x4, %%edx;"
        "subl %[card_type], %%edx;"
        "shll $0x2, %%edx;"
        "addl $0xa4, %%edx;"
        "addl %%eax, %%edx;"
        "pushl %%edx;"
        "movl $0x486030, %%ecx;"
        "calll %%ecx;"
        "popal;"
        :
        : [card_type] "g"(card_type)
        :);
#else
    __asm {
        pushad
        mov eax,0x6A9EC0
        mov eax,[eax]
        mov eax,[eax+0x774]
        mov edx,cardType
        shl edx,0x4
        sub edx,cardType
        shl edx,0x2
        add edx,0xA4
        add edx,eax
        push edx
        mov ecx,0x486030
        call ecx
        popad
    }
#endif
}

// 选择模仿者卡片
void Asm::chooseImitatorCard(int card_type)
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl $0x6A9EC0, %%eax;"
        "movl (%%eax), %%eax;"
        "movl 0x774(%%eax), %%eax;"

        "movl $0x3, 0x0C08(%%eax);"
        "movl $0x0, 0x0C09(%%eax);"
        "movl $0x0, 0x0C0A(%%eax);"
        "movl $0x0, 0x0C0B(%%eax);"
        "movl %[card_type], %%edx;"
        "movl %%edx, 0x0C18(%%eax);"
        "movl $0x0, 0x0C19(%%eax);"
        "movl $0x0, 0x0C1A(%%eax);"
        "movl $0x0, 0x0C1B(%%eax);"

        "leal 0xbe4(%%eax), %%ecx;"
        "movl 0xa0(%%eax), %%edx;"
        "movl 0x8(%%edx), %%ebx;"
        "movl %%ebx, (%%ecx);"
        "movl 0xc(%%edx), %%ebx;"
        "movl %%ebx, 0x4(%%ecx);"
        "pushl %%eax;"
        "pushl %%ecx;"
        "movl $0x486030, %%edx;"
        "calll %%edx;"
        "movl $0x4866E0, %%edx;"
        "calll %%edx;"
        "popal;"
        :
        : [card_type] "g"(card_type)
        :);
#else
    __asm {
        pushad
        mov eax, 0x6A9EC0
        mov eax, [eax]
        mov eax, [eax + 0x774]
        mov dword ptr[eax + 0x0C08], 0x3
        mov dword ptr[eax + 0x0C18], cardType
        lea ecx, [eax + 0xBE4]
        mov edx, [eax + 0xA0]
        mov ebx, [edx + 0x8]
        mov[ecx], ebx
        mov ebx, [edx + 0xC]
        mov[ecx + 0x4], ebx
        push eax
        push ecx
        mov edx, 0x486030
        call edx
        mov edx, 0x4866E0
        call edx
        popad
    }
#endif
}

int Asm::GetPlantRejectType(int card_type, int row, int col)
{
    int is_plantable = 0;
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl %[row], %%eax;"
        "pushl %[card_type];"
        "pushl %[col];"
        "movl $0x6A9EC0, %%ebx;"
        "movl (%%ebx), %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "pushl %%ebx;"
        "movl $0x40E020, %%edx;"
        "calll %%edx;"
        "movl %%eax, %[is_plantable];"
        "popal;"
        :
        : [card_type] "g"(card_type), [row] "g"(row), [col] "g"(col), [is_plantable] "g"(is_plantable)
        :);
#else
    __asm {
        pushad
        esi row
        push cardtype
        push col
        push 6A9EC0+768
        call 40E020
        mov ans, eax
        popad
    }
#endif
    return is_plantable;
}

#pragma GCC pop_options