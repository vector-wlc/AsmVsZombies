/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvzfunc
 */

#include "avz_asm.h"
#include "avz_memory.h"

static AMainObject* __level;
static AMouseWindow* __mw;
static int __x;
static int __y;
static int __key;
static int __rank;
static int __index;
static int __cardType;
static int __row;
static int __col;
static int __reject_type;
static APlant* __plant;
static AZombie* __zombie;
static int __type;
static int __imitatorType;
static void* __p;
static DWORD __board;
static DWORD __levelprop;

void AAsm::ClickScene(AMainObject* level, int x, int y, int key)
{
    __level = level;
    __x = x;
    __y = y;
    __key = key;
    _ClickScene();
}

void AAsm::Click(AMouseWindow* mw, int x, int y, int key)
{
    __mw = mw;
    __x = x;
    __y = y;
    __key = key;
    _Click();
}
void AAsm::MouseClick(int x, int y, int key)
{
    __x = x;
    __y = y;
    __key = key;
    _MouseClick();
}

void AAsm::Fire(int x, int y, int rank)
{
    __x = x;
    __y = y;
    __rank = rank;
    _ShootPao();
}

void AAsm::PlantCard(int x, int y, int index)
{
    __x = x;
    __y = y;
    __index = index;
    _PlantCard();
}

void AAsm::ShovelPlant(int x, int y)
{
    __x = x;
    __y = y;
    _ShovelPlant();
}

// 选择卡片
void AAsm::ChooseCard(int cardType)
{
    __cardType = cardType;
    _ChooseCard();
}

// 选择模仿者卡片
void AAsm::ChooseImitatorCard(int cardType)
{
    __cardType = cardType;
    _ChooseImitatorCard();
}

int AAsm::GetPlantRejectType(int cardType, int row, int col)
{
    __cardType = cardType;
    __row = row;
    __col = col;
    _GetPlantRejectType();
    return __reject_type;
}

void AAsm::GameFightLoop()
{

#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x415d40, %%eax;"
        "calll *%%eax;"
        :
        :
        : "esp", "ebp", "eax", "ecx");
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

void AAsm::GameTotalLoop()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x452650, %%eax;"
        "calll *%%eax;"
        :
        :
        : "esp", "ebp", "eax", "ecx");
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

void AAsm::GameSleepLoop()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x453a50, %%eax;"
        "calll *%%eax;"
        :
        :
        : "esp", "ebp", "eax", "ecx");
#else
    __asm
    {
        mov ecx 6A9EC0
        call 0x453a50
    }
#endif
}

void AAsm::ClearObjectMemory()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%esi;"
        "movl $0x41bad0, %%eax;"
        "calll *%%eax;"

        "movl 0x6a9ec0, %%ebx;"
        "movl 0x820(%%ebx), %%esi;"
        "pushl %%esi;"
        "movl $0x445680, %%eax;"
        "calll *%%eax;"
        :
        :
        : "esp", "ebp", "eax", "ebx", "esi");
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

void AAsm::KillZombiesPreview()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl $0x40df70, %%eax;"
        "calll *%%eax;"
        :
        :
        : "esp", "ebp", "eax", "ebx");
#else
    __asm
    {
        mov ebx 6A9EC0+768
        call 0x40df70
    }
#endif
}

void AAsm::SetImprovePerformance(bool is_improve_performance)
{
    *(bool*)(0x6a66f4) = !is_improve_performance;
}

void AAsm::CheckFightExit()
{
#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl $0x4524f0, %%ecx;"
        "calll *%%ecx;"
        :
        :
        : "esp", "ebp", "eax", "ecx");
#else
    __asm
    {
        mov eax 6A9EC0
        call 0x4524f0
    }
#endif
}

void AAsm::SaveData()
{
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "pushl %%eax;"
        "movl $0x408c30, %%eax;"
        "calll *%%eax;"

        :
        :
        : "esp", "ebp", "eax");
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

void AAsm::LoadData()
{
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl 0x6a9ec0, %%eax;"
        "pushl %%eax;"
        "movl $0x44f7a0, %%eax;"
        "calll *%%eax;"

        :
        :
        : "esp", "ebp", "eax");
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

void AAsm::Rock()
{
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl 0x6a9ec0, %%ebx;"
        "movl 0x774(%%ebx), %%ebx;"
        "movl $0x486d20, %%eax;"
        "movl 0x6a9ec0, %%esi;"
        "movl $0x1, %%edi;"
        "movl $0x1, %%ebp;"
        "calll *%%eax;"

        :
        :
        : "esp", "ebp", "eax", "ebx", "esi", "edi");
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

void AAsm::_ClickScene()
{
    if ((*(APvzBase**)0x6a9ec0)->GameUi() != 3) {
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
        : "esp", "ebp", "eax", "ecx");
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

void AAsm::_Click()
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
        : "esp", "ebp", "eax", "ebx", "ecx", "edx");
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

void AAsm::_MouseClick()
{

#ifdef __MINGW32__
    __asm__ __volatile__(

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

        :
        : [__x] "m"(__x), [__y] "m"(__y), [__key] "m"(__key)
        : "esp", "ebp", "eax", "ebx", "ecx", "edx");
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
void AAsm::_ShootPao()
{

#ifdef __MINGW32__
    __asm__ __volatile__(

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

        :
        : [__x] "m"(__x), [__y] "m"(__y), [__rank] "m"(__rank)
        : "esp", "ebp", "eax", "ecx", "edi", "edx");
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
void AAsm::_PlantCard()
{

#ifdef __MINGW32__
    __asm__ __volatile__(

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

        :
        : [__x] "m"(__x), [__y] "m"(__y), [__index] "m"(__index)
        : "esp", "ebp", "eax", "ecx", "edi", "edx");
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
void AAsm::_ShovelPlant()
{

#ifdef __MINGW32__
    __asm__ __volatile__(

        "pushl $0x6;"
        "pushl $0x1;"
        "movl %[__y], %%ecx;"
        "movl %[__x], %%edx;"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%eax;"
        "movl $0x411060, %%ebx;"
        "calll *%%ebx;"

        :
        : [__x] "m"(__x), [__y] "m"(__y)
        : "esp", "ebp", "eax", "ecx", "ebx", "edi");
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
void AAsm::_ChooseCard()
{
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl $0x6A9EC0, %%eax;"
        "movl (%%eax), %%eax;"
        "movl 0x774(%%eax), %%eax;"
        "movl %[__cardType], %%edx;"
        "shll $0x4, %%edx;"
        "subl %[__cardType], %%edx;"
        "shll $0x2, %%edx;"
        "addl $0xa4, %%edx;"
        "addl %%eax, %%edx;"
        "pushl %%edx;"
        "movl $0x486030, %%ecx;"
        "calll *%%ecx;"

        :
        : [__cardType] "m"(__cardType)
        : "esp", "ebp", "eax", "ecx", "edx");
#else
    __asm {
        pushad
        mov eax,0x6A9EC0
        mov eax,[eax]
        mov eax,[eax+0x774]
        mov edx, __cardType
        shl edx,0x4
        sub edx, __cardType
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
void AAsm::_ChooseImitatorCard()
{

#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl $0x6A9EC0, %%eax;"
        "movl (%%eax), %%eax;"
        "movl 0x774(%%eax), %%eax;"

        "movl $0x3, 0x0C08(%%eax);"
        "movl $0x0, 0x0C09(%%eax);"
        "movl $0x0, 0x0C0A(%%eax);"
        "movl $0x0, 0x0C0B(%%eax);"
        "movl %[__cardType], %%edx;"
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

        :
        : [__cardType] "m"(__cardType)
        : "esp", "ebp", "eax", "ecx", "edx", "ebx");
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

void AAsm::_GetPlantRejectType()
{
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl %[__row], %%eax;"
        "pushl %[__cardType];"
        "pushl %[__col];"
        "movl $0x6A9EC0, %%ebx;"
        "movl (%%ebx), %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "pushl %%ebx;"
        "movl $0x40E020, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__reject_type];"

        :
        : [__cardType] "m"(__cardType), [__row] "m"(__row), [__col] "m"(__col), [__reject_type] "m"(__reject_type)
        : "esp", "ebp", "eax", "ecx", "ebx", "edx");
#else
    __asm {
        pushad
        esi row
        push __cardType
        push col
        push 6A9EC0+768
        call 40E020
        mov __reject_type, eax
        popad
    }
#endif
}

void AAsm::ReleaseMouse()
{
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl 0x6A9EC0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x40CD80, %%edx;"
        "calll *%%edx;"

        :
        :
        : "esp", "ebp", "eax", "edx");
#else
    __asm {
        pushad
        eax 6A9EC0+768
        call 40CD80
        popad
    }
#endif
}

int AAsm::GridToAbscissa(int row, int col)
{
    __row = row;
    __col = col;
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl 0x6A9EC0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl %[__col], %%eax;"
        "movl %[__row], %%esi;"
        "movl $0x41C680, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__x];"

        :
        : [__col] "m"(__col), [__row] "m"(__row), [__x] "m"(__x)
        : "esp", "ebp", "eax", "ecx", "esi", "edx");
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

int AAsm::GridToOrdinate(int row, int col)
{
    __row = row;
    __col = col;
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl 0x6A9EC0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl %[__col], %%ecx;"
        "movl %[__row], %%eax;"
        "movl $0x41C740, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__y];"

        :
        : [__col] "m"(__col), [__row] "m"(__row), [__y] "m"(__y)
        : "esp", "ebp", "eax", "ecx", "edx", "edx");
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

AZombie* AAsm::PutZombie(int row, int col, AZombieType type)
{
    auto zombieArray = AGetMainObject()->ZombieArray();
    auto num = AGetMainObject()->ZombieNext();
    __row = row;
    __col = col;
    __index = int(type);
#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl %[__row], %%eax;"
        "pushl %[__col];"
        "pushl %[__index];"
        "movl 0x6a9ec0,%%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl 0x160(%%ecx), %%ecx;"
        "movl $0x42a0f0, %%edx;"
        "calll *%%edx;"
        :
        : [__row] "g"(__row), [__col] "g"(__col), [__index] "g"(__index)
        : "esp", "ebp", "eax", "ecx", "edx");
#else
    __asm {
        pushad
        mov eax,x
        push y
        push index
        mov ecx,[6A9EC0]
        mov ecx,[ecx+768]
        mov ecx,[ecx+160]
        mov edx,42A0F0
        call edx
        popad
    }
#endif
    return zombieArray + num;
}

// AZombie* AAsm::PutZombie(int row, int col, AZombieType type)
// {
//     auto zombieArray = AGetMainObject()->ZombieArray();
//     auto num = AGetMainObject()->ZombieNext();
//     __wave = AGetMainObject()->Wave();
//     __row = row;
//     __col = col;
//     __index = int(type);
// #ifdef __MINGW32__
//     __asm__ __volatile__(
//
//         "pushl %[__row];"
//         "pushl %[__index];"
//         "movl %[__wave],%%ebx;"
//         "movl 0x6a9ec0,%%ecx;"
//         "movl 0x768(%%ecx), %%eax;"
//         "movl $0x40DDC0, %%edx;"
//         "calll *%%edx;"
//
//         :
//         : [__row] "g"(__row), [__index] "g"(__index), [__wave] "g"(__wave)
//         :);
// #else
//     __asm {
//         push row
//         push type
//         ebx wave
//         eax 6A9EC0+768
//         call 40DDC0
//     }
// #endif
//     return zombieArray + num;
// }

APlant* AAsm::PutPlant(int row, int col, APlantType type)
{
    if (type == AIMITATOR) {
        return nullptr; // 不可能出现这种情况
    }
    auto plantArray = AGetMainObject()->PlantArray();
    auto num = AGetMainObject()->PlantNext();
    __row = row;
    __col = col;
    __type = int(type);

    __imitatorType = -1;
    if (__type > 48) {
        __imitatorType = __type - 49;
        __type = 48;
    }
#ifdef __MINGW32__
    __asm__ __volatile__(

        "pushl %[__imitatorType];"
        "pushl %[__type];"
        "movl %[__row],%%eax;"
        "pushl %[__col];"
        "movl 0x6a9ec0,%%ebp;"
        "movl 0x768(%%ebp), %%edi;"
        "pushl %%edi;"
        "movl $0x40d120, %%edx;"
        "calll *%%edx;"

        :
        : [__imitatorType] "m"(__imitatorType), [__type] "m"(__type), [__row] "m"(__row), [__col] "m"(__col)
        : "esp", "ebp", "eax", "ecx", "edi", "edx");
#else
    __asm
    {
        push immitaterType
        push type
        push col
        push 6A9EC0 + 768
        eax row
        call 40D120
    }
#endif
    return plantArray + num;
}

void AAsm::RemovePlant(APlant* plant)
{
    __plant = plant;
#ifdef __MINGW32__
    __asm__ __volatile__(

        "pushl %[__plant];"
        "movl $0x4679B0, %%edx;"
        "calll *%%edx;"

        :
        : [__plant] "m"(__plant)
        : "esp", "ebp", "edx");
#else
    __asm {
        push plant;
        call 4679B0;
    }

#endif
}

void AAsm::RemoveZombie(AZombie* zombie)
{
    __zombie = zombie;
#ifdef __MINGW32__
    __asm__ __volatile__(

        "movl %[__zombie], %%ecx;"
        "movl $0x5302f0, %%edx;"
        "calll *%%edx;"
        :
        : [__zombie] "m"(__zombie)
        : "esp", "ebp", "edx", "ecx");
#else
    __asm {
        ecx zombie
        call 5302F0
    }
#endif
}

void* AAsm::SaveToMemory()
{
    __p = malloc(0x24);
    memset(__p, 0, 0x24);
    __board = (DWORD)AGetMainObject();

#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl %[__p], %%eax;"
        "pushl %[__board];"
        "movl $0x4819D0, %%ecx;"
        "calll *%%ecx;"
        "addl $0x4, %%esp;"
        :
        : [__p] "m"(__p), [__board] "m"(__board)
        : "esp", "ebp", "eax", "ecx");
#else
    __asm {
		mov eax,p
		push board
		mov ecx, 0x4819D0
		call ecx
    }
#endif

    ((bool*)__p)[0x21] = 1;
    return __p;
}

void AAsm::FreeMemory(void*& p)
{
    if (!p) {
        return;
    }
#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl %[p], %%ecx;"
        "movl $0x5D60C0, %%eax;"
        "calll *%%eax;"
        :
        : [p] "m"(p)
        : "esp", "ebp", "eax", "ecx");
#else
    __asm {
		mov ecx,p
		mov eax,0x5D60C0
		call eax
    }
#endif
    free(p);
    p = nullptr;
}

void AAsm::LoadFromMemory(void*& p)
{
    if (!p) {
        return;
    }
    __p = p;
    __levelprop = ((DWORD***)nullptr)[0x6A9EC0 / 4][0x768 / 4][0x160 / 4];

#ifdef __MINGW32__
    __asm__ __volatile__(
        "movl %[__levelprop], %%edi;"
        "movl $0x429E50, %%eax;"
        "calll *%%eax;"

        "movl %[__p], %%eax;"
        "movl 0x6a9ec0,%%edi;"
        "movl 0x768(%%edi), %%edi;"
        "pushl %%edi;"
        "movl $0x4819D0, %%ecx;"
        "calll *%%ecx;"
        "movl $0x481CE0, %%eax;"
        "calll *%%eax;"
        "addl $0x4, %%esp;"
        :
        : [__p] "m"(__p), [__board] "m"(__board), [__levelprop] "m"(__levelprop)
        : "esp", "ebp", "eax", "ecx", "edi", "ebx");
#else
    __asm {
		mov edi, levelprop
		mov eax, 0x429E50
		call eax
		mov eax,p
		push board
		mov ecx,0x4819D0
		call ecx
		mov edi,board
		mov eax,0x481CE0
		call eax
    }
#endif
    FreeMemory(p);
}
