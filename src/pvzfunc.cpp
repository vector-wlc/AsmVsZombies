/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvzfunc
 */

#include "libavz.h"
#include <mutex>

#pragma GCC push_options
#pragma GCC optimize("O0")

static MainObject* __level;
static MouseWindow* __mw;
static int __x;
static int __y;
static int __key;
static int __rank;
static int __index;
static int __card_type;
static int __row;
static int __col;
static int __reject_type;

void Asm::clickScene(MainObject* level, int x, int y, int key)
{
    __level = level;
    __x = x;
    __y = y;
    __key = key;
    __clickScene();
}

void Asm::click(MouseWindow* mw, int x, int y, int key)
{
    __mw = mw;
    __x = x;
    __y = y;
    __key = key;
    __click();
}
void Asm::mouseClick(int x, int y, int key)
{
    __x = x;
    __y = y;
    __key = key;
    __mouseClick();
}

void Asm::shootPao(int x, int y, int rank)
{
    __x = x;
    __y = y;
    __rank = rank;
    __shootPao();
}

void Asm::plantCard(int x, int y, int index)
{
    __x = x;
    __y = y;
    __index = index;
    __plantCard();
}

void Asm::shovelPlant(int x, int y)
{
    __x = x;
    __y = y;
    __shovelPlant();
}

// 选择卡片
void Asm::chooseCard(int card_type)
{
    __card_type = card_type;
    __chooseCard();
}

// 选择模仿者卡片
void Asm::chooseImitatorCard(int card_type)
{
    __card_type = card_type;
    __chooseImitatorCard();
}

int Asm::getPlantRejectType(int card_type, int row, int col)
{
    __card_type = card_type;
    __row = row;
    __col = col;
    __getPlantRejectType();
    return __reject_type;
}

void Asm::gameFightLoop()
{

#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x415d40, %%eax;"
        "calll *%%eax;"
        "popal;"
        :
        :
        :);
#else
    __asm
    {
        popad
        movl ecx, [0x6a9ec0]
        movl ecx, [ecx+0x768]
        mov eax, 0x415d40 
        call eax

        popal
    }
#endif
}

void Asm::gameTotalLoop()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x452650, %%eax;"
        "calll *%%eax;"
        "popal;"
        :
        :
        :);
#else
    __asm
    {
        popad
        movl ecx, [0x6a9ec0]
        mov eax, 0x452650 
        call eax

        popal
    }
#endif
}

void Asm::gameSleepLoop()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x453a50, %%eax;"
        "calll *%%eax;"
        "popal;"
        :
        :
        :);
#else
    __asm
    {
        mov ecx 6A9EC0
        call 0x453a50
    }
#endif
}

void Asm::clearObjectMemory()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%esi;"
        "movl $0x41bad0, %%eax;"
        "calll *%%eax;"

        "movl 0x6a9ec0, %%ebx;"
        "movl 0x820(%%ebx), %%esi;"
        "pushl %%esi;"
        "movl $0x445680, %%eax;"
        "calll *%%eax;"
        "popal;"
        :
        :
        :);
#else
    __asm
    {
        mov esi 6A9EC0+768
        call 41BAD0
        push 6A9EC0+820
        call 445680
    }
#endif
}

void Asm::killZombiesPreview()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl $0x40df70, %%eax;"
        "calll *%%eax;"
        "popal;"
        :
        :
        :);
#else
    __asm
    {
        mov ebx 6A9EC0+768
        call 0x40df70
    }
#endif
}

void Asm::setImprovePerformance(bool is_improve_performance)
{
    *(bool*)(0x6a66f4) = !is_improve_performance;
}

void Asm::gameExit()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%eax;"
        "movl $0x4524f0, %%ecx;"
        "calll *%%ecx;"
        "popal;"
        :
        :
        :);
#else
    __asm
    {
        mov eax 6A9EC0
        call 0x4524f0
    }
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
        "calll *%%eax;"
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
        "calll *%%eax;"
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
        "calll *%%eax;"
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

void Asm::__clickScene()
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
        "calll *%%eax;"
        :
        : "m"(__key), "m"(__y), "m"(__x), "m"(__level)
        : "eax", "ecx");
#else
    __asm {
        push __key
        push __y
        push __x
        mov ecx, __level
        mov eax, 0x411f20
        call eax
    }
#endif
}

void Asm::__click()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushl %0;"
        "movl %1, %%ebx;"
        "movl %2, %%eax;"
        "movl %3, %%ecx;"
        "movl $0x539390, %%edx;"
        "calll *%%edx;"
        :
        : "m"(__x), "m"(__key), "m"(__y), "m"(__mw)
        : "eax", "ecx", "ebx", "edx");
#else
    __asm {
		push ebx;
		push __x;
		mov ebx, __key;
		mov eax, __y;
		mov ecx, __mw;
		mov edx, 0x539390;
		call edx;
		pop ebx;
    }
#endif
}

void Asm::__mouseClick()
{

#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "pushl %[__x];"
        "movl %[__y], %%eax;"
        "movl %[__key], %%ebx;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x320(%%ecx), %%ecx;"
        "movl $0x539390, %%edx;"
        "calll *%%edx;"
        "pushl %[__key];"
        "pushl %[__x];"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x320(%%eax), %%eax;"
        "movl %[__y], %%ebx;"
        "movl $0x5392e0, %%edx;"
        "calll *%%edx;"
        "popal;"
        :
        : [__x] "m"(__x), [__y] "m"(__y), [__key] "m"(__key)
        :);
#else
    __asm {
        pushad
        push __x
        mov eax,__y
        mov ebx,__key
        mov ecx,[0x6a9ec0]
        mov ecx,[ecx+0x320]
        mov edx,0x539390
        call edx
        push __key
        push __x
        mov eax,[0x6a9ec0]
        mov eax,[eax+0x320]
        mov ebx,__y
        mov edx,0x5392e0
        call edx
        popad
    }
#endif
}
void Asm::__shootPao()
{

#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%edi;"
        "movl 0xac(%%edi), %%eax;"
        "movl $0x14c, %%ecx;"
        "imull %[__rank], %%ecx;"
        "addl %%ecx, %%eax;"
        "pushl %[__y];"
        "pushl %[__x];"
        "movl $0x466d50, %%edx;"
        "calll *%%edx;"
        "popal;"
        :
        : [__x] "m"(__x), [__y] "m"(__y), [__rank] "m"(__rank)
        :);
#else
    __asm {
        pushad
        mov eax,[0x6a9ec0]
        mov edi,[eax+0x768]
        mov eax,[edi+0xac]
        mov ecx,0x14c
        imul ecx,%[__rank]
        add eax,ecx
        push %[__x]
        push %[__y]
        mov edx,0x466d50
        call edx
        popad
    }
#endif
}
void Asm::__plantCard()
{

#ifdef __MINGW32__
    __asm__ __volatile__(

        "pushal;"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%edi;"
        "pushl %[__y];"
        "pushl %[__x];"
        "pushl %%edi;"
        "movl 0x144(%%edi), %%eax;"
        "movl $0x50, %%ecx;"
        "imull %[__index], %%ecx;"
        "addl %%ecx, %%eax;"
        "addl $0x28, %%eax;"
        "pushl %%eax;"
        "movl $0x488590, %%ecx;"
        "calll *%%ecx;"
        "movl $0x1, %%ecx;"
        "movl $0x40fd30, %%edx;"
        "calll *%%edx;"
        "popal;"
        :
        : [__x] "m"(__x), [__y] "m"(__y), [__index] "m"(__index)
        :);
#else
    __asm {
        pushad
        mov eax,[6A9EC0]
        mov eax,[eax+768]
        push __y
        push __x
        push eax
        mov eax,[eax+144]
        mov ecx,__rank
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
void Asm::__shovelPlant()
{

#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "pushl $0x6;"
        "pushl $0x1;"
        "movl %[__y], %%ecx;"
        "movl %[__x], %%edx;"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%eax;"
        "movl $0x411060, %%ebx;"
        "calll *%%ebx;"
        "popal;"
        :
        : [__x] "m"(__x), [__y] "m"(__y)
        :);
#else
    __asm {
        pushad
        push 6
        push 1
        mov ecx __y
        mov edx __x
        mov eax,[6A9EC0]
        mov eax,[eax+768]
        mov ebx,411060
        call ebx
        popad

    }
#endif
}
void Asm::__chooseCard()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl $0x6A9EC0, %%eax;"
        "movl (%%eax), %%eax;"
        "movl 0x774(%%eax), %%eax;"
        "movl %[__card_type], %%edx;"
        "shll $0x4, %%edx;"
        "subl %[__card_type], %%edx;"
        "shll $0x2, %%edx;"
        "addl $0xa4, %%edx;"
        "addl %%eax, %%edx;"
        "pushl %%edx;"
        "movl $0x486030, %%ecx;"
        "calll *%%ecx;"
        "popal;"
        :
        : [__card_type] "m"(__card_type)
        :);
#else
    __asm {
        pushad
        mov eax,0x6A9EC0
        mov eax,[eax]
        mov eax,[eax+0x774]
        mov edx, __card_type
        shl edx,0x4
        sub edx, __card_type
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
void Asm::__chooseImitatorCard()
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
        "movl %[__card_type], %%edx;"
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
        "calll *%%edx;"
        "movl $0x4866E0, %%edx;"
        "calll *%%edx;"
        "popal;"
        :
        : [__card_type] "m"(__card_type)
        :);
#else
    __asm {
        pushad
        mov eax, 0x6A9EC0
        mov eax, [eax]
        mov eax, [eax + 0x774]
        mov dword ptr[eax + 0x0C08], 0x3
        mov dword ptr[eax + 0x0C18], __cardType
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

void Asm::__getPlantRejectType()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl %[__row], %%eax;"
        "pushl %[__card_type];"
        "pushl %[__col];"
        "movl $0x6A9EC0, %%ebx;"
        "movl (%%ebx), %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "pushl %%ebx;"
        "movl $0x40E020, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__reject_type];"
        "popal;"
        :
        : [__card_type] "m"(__card_type), [__row] "m"(__row), [__col] "m"(__col), [__reject_type] "m"(__reject_type)
        :);
#else
    __asm {
        pushad
        esi row
        push __card_type
        push col
        push 6A9EC0+768
        call 40E020
        mov __reject_type, eax
        popad
    }
#endif
}

void Asm::releaseMouse()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6A9EC0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x40CD80, %%edx;"
        "calll *%%edx;"
        "popal;"
        :
        :
        :);
#else
    __asm {
        pushad
        eax 6A9EC0+768
        call 40CD80
        popad
    }
#endif
}

int Asm::gridToAbscissa(int row, int col)
{
    __row = row;
    __col = col;
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6A9EC0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl %[__col], %%eax;"
        "movl %[__row], %%esi;"
        "movl $0x41C680, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__x];"
        "popal;"
        :
        : [__col] "m"(__col), [__row] "m"(__row), [__x] "m"(__x)
        :);
#else
    __asm {
        ecx 6A9EC0+768
        eax col(int)
        esi row(int)
        call 41C680
        x eax(int)
    }

#endif
    return __x;
}

int Asm::gridToOrdinate(int row, int col)
{
    __row = row;
    __col = col;
#ifdef __MINGW32__
    __asm__ __volatile__(
        "pushal;"
        "movl 0x6A9EC0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl %[__col], %%ecx;"
        "movl %[__row], %%eax;"
        "movl $0x41C740, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__y];"
        "popal;"
        :
        : [__col] "m"(__col), [__row] "m"(__row), [__y] "m"(__y)
        :);
#else
    __asm {
        ebx 6A9EC0+768
        ecx col(int)
        eax row(int)
        call 41C740
        y eax(int)
    }

#endif
    return __y;
}

#pragma GCC pop_options