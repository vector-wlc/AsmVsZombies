/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvzfunc
 */

#include "avz_asm.h"
#include "avz_memory.h"
#include <filesystem>

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

    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x415d40, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::GameTotalLoop()
{
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x452650, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::GameSleepLoop()
{
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x453a50, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

// 5D59A0
//   54B980
void AAsm::UpdateFrame()
{
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x320(%%edi), %%edi;"
        "movl $0x539140, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
    // __asm__ __volatile__(
    //     "movl 0x6a9ec0, %%ecx;"
    //     "movl $0x54B980, %%eax;"
    //     "calll *%%eax;"
    //     :
    //     :
    //     : ASaveAllRegister);
}

void AAsm::ClearObjectMemory()
{
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
        : ASaveAllRegister);
}

void AAsm::KillZombiesPreview()
{
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl $0x40df70, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::SetImprovePerformance(bool is_improve_performance)
{
    *(bool*)(0x6a66f4) = !is_improve_performance;
}

void AAsm::CheckFightExit()
{
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl $0x4524f0, %%ecx;"
        "calll *%%ecx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::SaveData()
{
    __asm__ __volatile__(

        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "pushl %%eax;"
        "movl $0x408c30, %%eax;"
        "calll *%%eax;"

        :
        :
        : ASaveAllRegister);
}

void AAsm::LoadData()
{
    __asm__ __volatile__(

        "movl 0x6a9ec0, %%eax;"
        "pushl %%eax;"
        "movl $0x44f7a0, %%eax;"
        "calll *%%eax;"

        :
        :
        : ASaveAllRegister);
}

void AAsm::Rock()
{
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
        : ASaveAllRegister);
}

void AAsm::_ClickScene()
{
    if ((*(APvzBase**)0x6a9ec0)->GameUi() != 3) {
        return;
    }
    __asm__ __volatile__(
        "pushl %0;"
        "pushl %1;"
        "pushl %2;"
        "movl %3, %%ecx;"
        "movl $0x411f20, %%eax;"
        "calll *%%eax;"
        :
        : "m"(__key), "m"(__y), "m"(__x), "m"(__level)
        : ASaveAllRegister);
}

void AAsm::_Click()
{
    __asm__ __volatile__(
        "pushl %0;"
        "movl %1, %%ebx;"
        "movl %2, %%eax;"
        "movl %3, %%ecx;"
        "movl $0x539390, %%edx;"
        "calll *%%edx;"
        :
        : "m"(__x), "m"(__key), "m"(__y), "m"(__mw)
        : ASaveAllRegister);
}

void AAsm::_MouseClick()
{
    int curX = AGetPvzBase()->MouseWindow()->MouseAbscissa();
    int curY = AGetPvzBase()->MouseWindow()->MouseOrdinate();
    __asm__ __volatile__(

        // mouse down
        "pushl %[__x];"
        "movl %[__y], %%eax;"
        "movl %[__key], %%ebx;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x320(%%ecx), %%ecx;"
        "movl $0x539390, %%edx;"
        "calll *%%edx;"
        // mouse up
        "pushl %[__key];"
        "pushl %[__x];"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x320(%%eax), %%eax;"
        "movl %[__y], %%ebx;"
        "movl $0x5392e0, %%edx;"
        "calll *%%edx;"
        // mouse move
        "movl 0x6a9ec0, %%edx;"
        "movl 0x320(%%edx), %%edx;"
        "movl %[curX], %%eax;"
        "movl %[curY], %%ecx;"
        "movl $0x5394A0, %%ebx;"
        "calll *%%ebx;"
        :
        : [__x] "m"(__x), [__y] "m"(__y), [curX] "m"(curX), [curY] "m"(curY), [__key] "m"(__key)
        : ASaveAllRegister);
}
void AAsm::_ShootPao()
{

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
        : ASaveAllRegister);
}

void AAsm::_PlantCard()
{
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
        : ASaveAllRegister);
}
void AAsm::_ShovelPlant()
{

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
        : ASaveAllRegister);
}
void AAsm::_ChooseCard()
{
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
        : ASaveAllRegister);
}
void AAsm::_ChooseImitatorCard()
{

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
        : ASaveAllRegister);
}

void AAsm::_GetPlantRejectType()
{
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
        : ASaveAllRegister);
}

void AAsm::ReleaseMouse()
{
    __asm__ __volatile__(

        "movl 0x6A9EC0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x40CD80, %%edx;"
        "calll *%%edx;"

        :
        :
        : ASaveAllRegister);
}

int AAsm::GridToAbscissa(int row, int col)
{
    __row = row;
    __col = col;
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
        : ASaveAllRegister);

    return __x;
}

int AAsm::GridToOrdinate(int row, int col)
{
    __row = row;
    __col = col;
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
        : ASaveAllRegister);

    return __y;
}

AZombie* AAsm::PutZombie(int row, int col, AZombieType type)
{
    auto zombieArray = AGetMainObject()->ZombieArray();
    auto num = AGetMainObject()->ZombieNext();
    __row = row;
    __col = col;
    __index = int(type);
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
        : ASaveAllRegister);

    return zombieArray + num;
}

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
    if (__type >= AM_PEASHOOTER) {
        __imitatorType = __type - AM_PEASHOOTER;
        __type = AIMITATOR;
    }
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
        : ASaveAllRegister);

    return plantArray + num;
}

void AAsm::RemovePlant(APlant* plant)
{
    __plant = plant;
    __asm__ __volatile__(

        "pushl %[__plant];"
        "movl $0x4679B0, %%edx;"
        "calll *%%edx;"

        :
        : [__plant] "m"(__plant)
        : ASaveAllRegister);
}

void AAsm::RemoveZombie(AZombie* zombie)
{
    __zombie = zombie;
    __asm__ __volatile__(

        "movl %[__zombie], %%ecx;"
        "movl $0x5302f0, %%edx;"
        "calll *%%edx;"
        :
        : [__zombie] "m"(__zombie)
        : ASaveAllRegister);
}

void* AAsm::SaveToMemory()
{
    __p = malloc(0x24);
    memset(__p, 0, 0x24);
    __board = (DWORD)AGetMainObject();

    __asm__ __volatile__(
        "movl %[__p], %%eax;"
        "pushl %[__board];"
        "movl $0x4819D0, %%ecx;"
        "calll *%%ecx;"
        "addl $0x4, %%esp;"
        :
        : [__p] "m"(__p), [__board] "m"(__board)
        : ASaveAllRegister);

    ((bool*)__p)[0x21] = 1;
    return __p;
}

void AAsm::FreeMemory(void*& p)
{
    if (!p) {
        return;
    }
    __p = p;
    __asm__ __volatile__(
        "movl %[__p], %%ecx;"
        "movl $0x5D60C0, %%eax;"
        "calll *%%eax;"
        :
        : [__p] "m"(__p)
        : ASaveAllRegister);

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
        : ASaveAllRegister);

    FreeMemory(p);
}

bool AAsm::IsSeedUsable(ASeed* seed)
{
    uint8_t* ptr = (uint8_t*)seed;
    ptr += 0x28;
    int ret = 0;
    __asm__ __volatile__(
        "movl %[ptr], %%esi;"
        "movl $0x488500, %%ebx;"
        "calll *%%ebx;"
        "movl %%eax, %[ret];"
        :
        : [ptr] "m"(ptr), [ret] "m"(ret)
        : ASaveAllRegister);

    return *((bool*)(&ret));
}

int AAsm::GetSeedSunVal(int type, int iType)
{
    int ret = 0;
    __asm__ __volatile__(
        "movl %[type], %%eax;"
        "movl %[iType], %%edx;"
        "movl 0x6a9ec0,%%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl $0x41DAE0, %%ebx;"
        "calll *%%ebx;"
        "movl %%eax, %[ret];"
        :
        : [type] "m"(type), [iType] "m"(iType), [ret] "m"(ret)
        : ASaveAllRegister);

    return ret;
}

void AAsm::UpdateMousePos()
{
    __asm__ __volatile__(
        "movl 0x6a9ec0,%%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x40EAB0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::MakePvzString(const char* str, void* strObj)
{
    __asm__ __volatile__(
        "movl %[str], %%ecx;"
        "pushl %%ecx;"
        "movl %[strObj], %%ecx;"
        "movl $0x404450, %%eax;"
        "calll *%%eax;"
        :
        : [str] "m"(str), [strObj] "m"(strObj)
        : ASaveAllRegister);
}

void AAsm::FreePvzString(void* strObj)
{
    __asm__ __volatile__(

        "movl %[strObj], %%ecx;"
        "movl $0x404420, %%eax;"
        "calll *%%eax;"

        :
        : [strObj] "m"(strObj)
        : ASaveAllRegister);
}

void AAsm::MakeNewBoard()
{
    int scene = AGetMainObject()->Scene();
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x44F5F0, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
    __aig.mainObject = AGetPvzBase()->MainObject();
    AGetMainObject()->Scene() = scene;
}

void AAsm::LoadGame(const std::string& file)
{
    uint8_t pvzStr[28] = {0};
    void* tmpPtr = pvzStr;

    // 此函数需要根据场景调用不同的函数
    // 如果没有切换场景，调用 LawnLoadGame 481FE0
    // 如果切换场景，调用 LoadGame 408DE0
    int scene = AGetMainObject()->Scene();
    MakeNewBoard();
    MakePvzString(file.c_str(), &pvzStr);
    __asm__ __volatile__(
        // LawnLoadGame
        "pushl %[tmpPtr];"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x481FE0, %%eax;"
        "calll *%%eax;"
        "addl $0x4, %%esp;"
        :
        : [tmpPtr] "m"(tmpPtr)
        : ASaveAllRegister);

    if (scene != AGetMainObject()->Scene()) {
        MakeNewBoard();
        __asm__ __volatile__(
            // LoadGame
            "movl %[tmpPtr], %%eax;"
            "movl 0x6a9ec0, %%edi;"
            "movl 0x768(%%edi), %%edi;"
            "movl $0x408DE0, %%ecx;"
            "calll *%%ecx;"
            :
            : [tmpPtr] "m"(tmpPtr)
            : ASaveAllRegister);
    }

    __asm__ __volatile__(
        // continue board
        "movl $0, %%eax;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x4127A0, %%ebx;"
        "calll *%%ebx;"
        :
        : [tmpPtr] "m"(tmpPtr)
        : ASaveAllRegister);

    FreePvzString(&pvzStr);
}

void AAsm::SaveGame(const std::string& file)
{
    uint8_t pvzStr[28] = {0};
    void* tmpPtr = &pvzStr;
    MakePvzString(file.c_str(), &pvzStr);
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "pushl %[tmpPtr];"
        "movl $0x4820D0, %%ecx;"
        "calll *%%ecx;"
        "addl $0x4, %%esp;"
        :
        : [tmpPtr] "m"(tmpPtr)
        : ASaveAllRegister);
    FreePvzString(&pvzStr);
}

bool AAsm::CanSpawnZombies(int row)
{
    int ret;
    __asm__ __volatile__(
        "movl %[row], %%eax;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x416110, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[ret];"
        :
        : [row] "m"(row), [ret] "m"(ret)
        : "esp", "ebp", "eax", "ecx", "edx");
    return ret & 0xff;
}

bool AAsm::IsNight()
{
    int ret;
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x41c010, %%ecx;"
        "calll *%%ecx;"
        "movl %%eax, %[ret];"
        :
        : [ret] "m"(ret)
        : "esp", "ebp", "eax", "ecx");
    return ret & 0xff;
}

bool AAsm::IsRoof()
{
    int ret;
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x41c0b0, %%ecx;"
        "calll *%%ecx;"
        "movl %%eax, %[ret];"
        :
        : [ret] "m"(ret)
        : "esp", "ebp", "eax", "ecx");
    return ret & 0xff;
}

bool AAsm::HasGrave()
{
    int ret;
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edx;"
        "movl 0x768(%%edx), %%edx;"
        "movl $0x41c040, %%ecx;"
        "calll *%%ecx;"
        "movl %%eax, %[ret];"
        :
        : [ret] "m"(ret)
        : "esp", "ebp", "ecx", "edx");
    return ret & 0xff;
}

bool AAsm::HasPool()
{
    int ret;
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x41c0d0, %%ecx;"
        "calll *%%ecx;"
        "movl %%eax, %[ret];"
        :
        : [ret] "m"(ret)
        : "esp", "ebp", "eax", "ecx");
    return ret & 0xff;
}

void AAsm::EnterGame(int gameMode)
{
    auto gameUi = AMPtr<APvzBase>(0x6a9ec0)->GameUi();
    if (gameUi == LEVEL_INTRO || gameUi == PLAYING) {
        return; // 在选卡界面或者战斗界面此函数无效
    }

    if (gameUi == 0 || gameUi == 1) {
        if (gameUi == 0) { // 载入界面，需要直接删除载入界面进入主界面
            __asm__ __volatile__(
                "movl 0x6a9ec0, %%ecx;"
                "movl $0x452CB0, %%ebx;"
                "calll *%%ebx;"
                :
                :
                : ASaveAllRegister);
        }

        __asm__ __volatile__( // 删除主界面
            "movl 0x6a9ec0, %%esi;"
            "movl $0x44F9E0, %%eax;"
            "calll *%%eax;"
            :
            :
            : ASaveAllRegister);
    }

    if (gameUi == 7) {        // 选项卡界面
        __asm__ __volatile__( // 删除选项卡界面
            "movl 0x6a9ec0, %%esi;"
            "movl $0x44FD00, %%eax;"
            "calll *%%eax;"
            :
            :
            : ASaveAllRegister);
    }

    // 进入战斗或者选卡界面
    bool ok = 1;
    __asm__ __volatile__(
        "push %[ok];"
        "pushl %[gameMode];"
        "movl 0x6a9ec0, %%esi;"
        "movl $0x44f560, %%eax;"
        "calll *%%eax;"
        :
        : [ok] "m"(ok), [gameMode] "m"(gameMode)
        : ASaveAllRegister);
}

void AAsm::DoBackToMain()
{
    auto gameUi = AMPtr<APvzBase>(0x6a9ec0)->GameUi();
    if (gameUi != PLAYING) {
        return; // 在非战斗界面此函数无效
    }
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl $0x44FEB0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::PickZombieWaves()
{
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl $0x4092e0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::PickRandomSeeds()
{
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x774(%%ebx), %%ebx;"
        "pushl %%ebx;"
        "movl $0x4859B0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : ASaveAllRegister);
}