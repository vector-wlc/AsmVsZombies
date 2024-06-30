#include "libavz.h"

namespace {
AMainObject* __level;
AMouseWindow* __mw;
int __x;
int __y;
int __key;
int __rank;
int __index;
int __cardType;
int __row;
int __col;
int __reject_type;
APlant* __plant;
AZombie* __zombie;
int __type;
int __imitatorType;
void* __p;
DWORD __board;
DWORD __levelprop;
}

void AAsm::ClickScene(AMainObject* level, int x, int y, int key) {
    __level = level;
    __x = x;
    __y = y;
    __key = key;
    _ClickScene();
}

void AAsm::Click(AMouseWindow* mw, int x, int y, int key) {
    __mw = mw;
    __x = x;
    __y = y;
    __key = key;
    _Click();
}

void AAsm::MouseClick(int x, int y, int key) {
    __x = x;
    __y = y;
    __key = key;
    _MouseClick();
}

void AAsm::Fire(int x, int y, int rank) {
    __x = x;
    __y = y;
    __rank = rank;
    _ShootPao();
}

void AAsm::PlantCard(int x, int y, int index) {
    __x = x;
    __y = y;
    __index = index;
    _PlantCard();
}

void AAsm::ShovelPlant(int x, int y) {
    __x = x;
    __y = y;
    _ShovelPlant();
}

// 选择卡片
void AAsm::ChooseCard(int cardType) {
    __cardType = cardType;
    _ChooseCard();
}

// 选择模仿者卡片
void AAsm::ChooseImitatorCard(int cardType) {
    __cardType = cardType;
    _ChooseImitatorCard();
}

int AAsm::GetPlantRejectType(int cardType, int row, int col) {
    __cardType = cardType;
    __row = row;
    __col = col;
    _GetPlantRejectType();
    return __reject_type;
}

void AAsm::GameFightLoop() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x415d40, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::GameTotalLoop() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x452650, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::GameSleepLoop() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x453a50, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::UpdateFrame() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x320(%%edi), %%edi;"
        "movl $0x539140, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::ClearObjectMemory() {
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

void AAsm::KillZombiesPreview() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl $0x40df70, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::SetImprovePerformance(bool is_improve_performance) {
    *(bool*)(0x6a66f4) = !is_improve_performance;
}

void AAsm::CheckFightExit() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl $0x4524f0, %%ecx;"
        "calll *%%ecx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::SaveData() {
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

void AAsm::LoadData() {
    __asm__ __volatile__(

        "movl 0x6a9ec0, %%eax;"
        "pushl %%eax;"
        "movl $0x44f7a0, %%eax;"
        "calll *%%eax;"

        :
        :
        : ASaveAllRegister);
}

void AAsm::Rock() {
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

void AAsm::_ClickScene() {
    if ((*(APvzBase**)0x6a9ec0)->GameUi() != 3)
        return;
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

void AAsm::_Click() {
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

void AAsm::_MouseClick() {
    int curX = AGetPvzBase()->MouseWindow()->MouseAbscissa();
    int curY = AGetPvzBase()->MouseWindow()->MouseOrdinate();
    MouseDown(__x, __y, __key);
    MouseUp(__x, __y, __key);
    MouseMove(curX, curY);
}

// 鼠标按下
void AAsm::MouseDown(int x, int y, int key) {
    __asm__ __volatile__(
        "pushl %[x];"
        "movl %[y], %%eax;"
        "movl %[key], %%ebx;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x320(%%ecx), %%ecx;"
        "movl $0x539390, %%edx;"
        "calll *%%edx;"
        :
        : [x] "m"(x), [y] "m"(y), [key] "m"(key)
        : ASaveAllRegister);
}

// 鼠标松开
void AAsm::MouseUp(int x, int y, int key) {
    __asm__ __volatile__(
        "pushl %[key];"
        "pushl %[x];"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x320(%%eax), %%eax;"
        "movl %[y], %%ebx;"
        "movl $0x5392e0, %%edx;"
        "calll *%%edx;"
        :
        : [x] "m"(x), [y] "m"(y), [key] "m"(key)
        : ASaveAllRegister);
}

// 移动鼠标
void AAsm::MouseMove(int x, int y) {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edx;"
        "movl 0x320(%%edx), %%edx;"
        "movl %[x], %%eax;"
        "movl %[y], %%ecx;"
        "movl $0x5394a0, %%ebx;"
        "calll *%%ebx;"
        :
        : [x] "m"(x), [y] "m"(y)
        : ASaveAllRegister);
}

void AAsm::_ShootPao() {
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

void AAsm::_PlantCard() {
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
void AAsm::_ShovelPlant() {
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

void AAsm::_ChooseCard() {
    __asm__ __volatile__(
        "movl $0x6a9ec0, %%eax;"
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

void AAsm::_ChooseImitatorCard() {
    __asm__ __volatile__(
        "movl $0x6a9ec0, %%eax;"
        "movl (%%eax), %%eax;"
        "movl 0x774(%%eax), %%eax;"

        "movl $0x3, 0x0c08(%%eax);"
        "movl $0x0, 0x0c09(%%eax);"
        "movl $0x0, 0x0c0a(%%eax);"
        "movl $0x0, 0x0c0b(%%eax);"
        "movl %[__cardType], %%edx;"
        "movl %%edx, 0x0c18(%%eax);"
        "movl $0x0, 0x0c19(%%eax);"
        "movl $0x0, 0x0c1a(%%eax);"
        "movl $0x0, 0x0c1b(%%eax);"

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
        "movl $0x4866e0, %%edx;"
        "calll *%%edx;"
        :
        : [__cardType] "m"(__cardType)
        : ASaveAllRegister);
}

void AAsm::_GetPlantRejectType() {
    __asm__ __volatile__(
        "movl %[__row], %%eax;"
        "pushl %[__cardType];"
        "pushl %[__col];"
        "movl $0x6a9ec0, %%ebx;"
        "movl (%%ebx), %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "pushl %%ebx;"
        "movl $0x40e020, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__reject_type];"
        :
        : [__cardType] "m"(__cardType), [__row] "m"(__row), [__col] "m"(__col), [__reject_type] "m"(__reject_type)
        : ASaveAllRegister);
}

void AAsm::ReleaseMouse() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x40cd80, %%edx;"
        "calll *%%edx;"
        :
        :
        : ASaveAllRegister);
}

int AAsm::GridToAbscissa(int row, int col) {
    __row = row;
    __col = col;
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl %[__col], %%eax;"
        "movl %[__row], %%esi;"
        "movl $0x41c680, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__x];"
        :
        : [__col] "m"(__col), [__row] "m"(__row), [__x] "m"(__x)
        : ASaveAllRegister);

    return __x;
}

int AAsm::GridToOrdinate(int row, int col) {
    __row = row;
    __col = col;
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl %[__col], %%ecx;"
        "movl %[__row], %%eax;"
        "movl $0x41c740, %%edx;"
        "calll *%%edx;"
        "movl %%eax, %[__y];"
        :
        : [__col] "m"(__col), [__row] "m"(__row), [__y] "m"(__y)
        : ASaveAllRegister);

    return __y;
}

AZombie* AAsm::PutZombie(int row, int col, AZombieType type) {
    auto zombieArray = AGetMainObject()->ZombieArray();
    auto num = AGetMainObject()->ZombieNext();
    __row = row;
    __col = col;
    __index = int(type);
    __asm__ __volatile__(
        "movl %[__row], %%eax;"
        "pushl %[__col];"
        "pushl %[__index];"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl 0x160(%%ecx), %%ecx;"
        "movl $0x42a0f0, %%edx;"
        "calll *%%edx;"
        :
        : [__row] "g"(__row), [__col] "g"(__col), [__index] "g"(__index)
        : ASaveAllRegister);

    return zombieArray + num;
}

APlant* AAsm::PutPlant(int row, int col, APlantType type) {
    if (type == AIMITATOR)
        return nullptr; // 不可能出现这种情况
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
        "movl %[__row], %%eax;"
        "pushl %[__col];"
        "movl 0x6a9ec0, %%ebp;"
        "movl 0x768(%%ebp), %%edi;"
        "pushl %%edi;"
        "movl $0x40d120, %%edx;"
        "calll *%%edx;"
        :
        : [__imitatorType] "m"(__imitatorType), [__type] "m"(__type), [__row] "m"(__row), [__col] "m"(__col)
        : ASaveAllRegister);

    return plantArray + num;
}

void AAsm::RemovePlant(APlant* plant) {
    __plant = plant;
    __asm__ __volatile__(
        "pushl %[__plant];"
        "movl $0x4679b0, %%edx;"
        "calll *%%edx;"
        :
        : [__plant] "m"(__plant)
        : ASaveAllRegister);
}

void AAsm::KillZombie(AZombie* zombie) {
    __zombie = zombie;
    __asm__ __volatile__(
        "movl %[__zombie], %%ecx;"
        "movl $0x5302f0, %%edx;"
        "calll *%%edx;"
        :
        : [__zombie] "m"(__zombie)
        : ASaveAllRegister);
}

void AAsm::RemoveZombie(AZombie* zombie) {
    __zombie = zombie;
    __asm__ __volatile__(
        "movl %[__zombie], %%ecx;"
        "movl $0x530510, %%edx;"
        "calll *%%edx;"
        :
        : [__zombie] "m"(__zombie)
        : ASaveAllRegister);
}

void* AAsm::SaveToMemory() {
    __p = malloc(0x24);
    memset(__p, 0, 0x24);
    __board = (DWORD)AGetMainObject();

    __asm__ __volatile__(
        "movl %[__p], %%eax;"
        "pushl %[__board];"
        "movl $0x4819d0, %%ecx;"
        "calll *%%ecx;"
        "addl $0x4, %%esp;"
        :
        : [__p] "m"(__p), [__board] "m"(__board)
        : ASaveAllRegister);

    ((bool*)__p)[0x21] = 1;
    return __p;
}

void AAsm::FreeMemory(void*& p) {
    if (!p)
        return;
    __p = p;
    __asm__ __volatile__(
        "movl %[__p], %%ecx;"
        "movl $0x5d60c0, %%eax;"
        "calll *%%eax;"
        :
        : [__p] "m"(__p)
        : ASaveAllRegister);

    free(p);
    p = nullptr;
}

void AAsm::LoadFromMemory(void*& p) {
    if (!p)
        return;
    __p = p;
    __levelprop = ((DWORD***)nullptr)[0x6a9ec0 / 4][0x768 / 4][0x160 / 4];

    __asm__ __volatile__(
        "movl %[__levelprop], %%edi;"
        "movl $0x429e50, %%eax;"
        "calll *%%eax;"

        "movl %[__p], %%eax;"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "pushl %%edi;"
        "movl $0x4819d0, %%ecx;"
        "calll *%%ecx;"
        "movl $0x481ce0, %%eax;"
        "calll *%%eax;"
        "addl $0x4, %%esp;"
        :
        : [__p] "m"(__p), [__board] "m"(__board), [__levelprop] "m"(__levelprop)
        : ASaveAllRegister);

    FreeMemory(p);
}

bool AAsm::IsSeedUsable(ASeed* seed) {
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

int AAsm::GetSeedSunVal(int type, int iType) {
    int ret = 0;
    __asm__ __volatile__(
        "movl %[type], %%eax;"
        "movl %[iType], %%edx;"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl $0x41dae0, %%ebx;"
        "calll *%%ebx;"
        "movl %%eax, %[ret];"
        :
        : [type] "m"(type), [iType] "m"(iType), [ret] "m"(ret)
        : ASaveAllRegister);

    return ret;
}

void AAsm::UpdateMousePos() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x40eab0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::MakePvzString(const char* str, void* strObj) {
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

void AAsm::FreePvzString(void* strObj) {
    __asm__ __volatile__(
        "movl %[strObj], %%ecx;"
        "movl $0x404420, %%eax;"
        "calll *%%eax;"
        :
        : [strObj] "m"(strObj)
        : ASaveAllRegister);
}

void AAsm::MakeNewBoard() {
    int scene = AGetMainObject()->Scene();
    __asm__ __volatile__(
        // MakeNewBoard
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x44f5f0, %%eax;"
        "calll *%%eax;"

        // ProcessSafeDeleteList
        "movl 0x6a9ec0, %%ecx;"
        "pushl %%ecx;"
        "movl $0x5518F0, %%eax;"
        "calll *%%eax;"
        :
        :
        : ASaveAllRegister);
    AGetMainObject()->Scene() = scene;
}

// 此函数是为了防止 LoadGame 过长的
void __ABeforeMakeNewBoard(std::vector<int>& zombieMusicRefCnts, std::vector<int>& zombieMusicIdxs) {
    for (auto idx : zombieMusicIdxs) {
        zombieMusicRefCnts.push_back(AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * idx + 0x4));
        if (zombieMusicRefCnts.back() != 0) {
            // 有音效要维持住
            AAsm::PlayFoleyPitch(idx);
        }
    }
    // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * AAsm::JACK_MUSIC_IDX + 0x4));

    // 舞王和小丑的音效
    auto&& mjMusicRefCnt = AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * AAsm::MJ_MUSIC_IDX + 0x4);
    if (mjMusicRefCnt != 0) {
        int mjCnt = 0;
        for (auto&& zombie : AAliveFilter<AZombie>()) {
            int type = zombie.Type();
            mjCnt += int(type == AWW_8 || type == ABW_9);
        }
        mjMusicRefCnt = mjCnt + 1;
    }
}

// 此函数是为了防止 LoadGame 过长的
void __AAfterLoadGame(std::vector<int>& zombieMusicRefCnts, std::vector<int>& zombieMusicIdxs) {
    // 如果没有舞王和舞伴就删除音效
    int mjCnt = 0;
    for (auto&& zombie : AAliveFilter<AZombie>()) {
        int type = zombie.Type();
        mjCnt += int(type == AWW_8 || type == ABW_9);

        if (zombie.FixationCountdown() == 0 && zombie.FreezeCountdown() == 0) {
            int type = zombie.Type();
            int state = zombie.State();
            // 处于活动状态，进行音乐播放
            if (type == AXC_15 && state == 15) {
                AAsm::PlayFoleyPitch(AAsm::JACK_MUSIC_IDX);
                zombie.MRef<bool>(0x104) = true;
            } else if (type == AKG_17 && state == 32) {
                AAsm::PlayFoleyPitch(AAsm::DIGGER_MUSIC_IDX);
                zombie.MRef<bool>(0x104) = true;
            }
        } else if (type == AXC_15) {
            // 冻结的僵尸没有持有音乐
            zombie.MRef<bool>(0x104) = false;
        }
    }
    if (mjCnt == 0) {
        int refCnt = AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * AAsm::MJ_MUSIC_IDX + 0x4);
        for (int i = 0; i < refCnt; ++i)
            AAsm::StopFoley(AAsm::MJ_MUSIC_IDX);
    }

    // 把之前增加的减回去
    for (int i = 0; i < zombieMusicIdxs.size(); ++i)
        if (zombieMusicRefCnts[i] != 0)
            AAsm::StopFoley(zombieMusicIdxs[i]);
}

void AAsm::LoadGame(const std::string& file) {
    // 此函数需要根据场景调用不同的函数
    // 如果没有切换场景，调用 LawnLoadGame 481FE0
    // 如果切换场景，调用 LoadGame 408DE0
    uint8_t pvzStr[28] = {0};
    void* tmpPtr = pvzStr;
    int scene = AGetMainObject()->Scene();
    MakePvzString(file.c_str(), &pvzStr);

    for (int i = 0; i < 2; ++i) {
        // 先增加音效的引用计数
        // 防止 MakeNewBoard 停止音乐
        // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * JACK_MUSIC_IDX + 0x4));
        std::vector<int> zombieMusicRefCnts;
        std::vector<int> zombieMusicIdxs = {AAsm::JACK_MUSIC_IDX, AAsm::DIGGER_MUSIC_IDX};
        __ABeforeMakeNewBoard(zombieMusicRefCnts, zombieMusicIdxs);
        MakeNewBoard();
        // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * JACK_MUSIC_IDX + 0x4));

        if (i == 0) // 首先尝试调用 LawnLoadGame
            __asm__ __volatile__(
                // LawnLoadGame
                "pushl %[tmpPtr];"
                "movl 0x6a9ec0, %%ecx;"
                "movl 0x768(%%ecx), %%ecx;"
                "movl $0x481fe0, %%eax;"
                "calll *%%eax;"
                "addl $0x4, %%esp;"
                :
                : [tmpPtr] "m"(tmpPtr)
                : ASaveAllRegister);
        else // 场景切换需要调用 LoadGame
            __asm__ __volatile__(
                // LoadGame
                "movl %[tmpPtr], %%eax;"
                "movl 0x6a9ec0, %%edi;"
                "movl 0x768(%%edi), %%edi;"
                "movl $0x408de0, %%ecx;"
                "calll *%%ecx;"
                :
                : [tmpPtr] "m"(tmpPtr)
                : ASaveAllRegister);
        // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * JACK_MUSIC_IDX + 0x4));
        __AAfterLoadGame(zombieMusicRefCnts, zombieMusicIdxs);
        // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * JACK_MUSIC_IDX + 0x4));

        // 这说明需要切换场景
        if (scene == AGetMainObject()->Scene())
            break;
    }

    __asm__ __volatile__(
        // continue board
        "movl $0, %%eax;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x4127a0, %%ebx;"
        "calll *%%ebx;"
        :
        : [tmpPtr] "m"(tmpPtr)
        : ASaveAllRegister);
    // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6A9Ec0, 0x784, 0xa4 * _JACK_MUSIC_IDX + 0x4));

    FreePvzString(&pvzStr);
}

void AAsm::SaveGame(const std::string& file) {
    for (auto&& zombie : AAliveFilter<AZombie>())
        if ((zombie.FixationCountdown() != 0 || zombie.FreezeCountdown() != 0)
            && zombie.Type() == AXC_15 && zombie.State() == 15)
            zombie.MRef<bool>(0x104) = zombie.MRef<bool>(0xba);
    uint8_t pvzStr[28] = {0};
    void* tmpPtr = &pvzStr;
    MakePvzString(file.c_str(), &pvzStr);
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "pushl %[tmpPtr];"
        "movl $0x4820d0, %%ecx;"
        "calll *%%ecx;"
        "addl $0x4, %%esp;"
        :
        : [tmpPtr] "m"(tmpPtr)
        : ASaveAllRegister);
    FreePvzString(&pvzStr);
}

bool AAsm::CanSpawnZombies(int row) {
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

bool AAsm::IsNight() {
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

bool AAsm::IsRoof() {
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

bool AAsm::HasGrave() {
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

bool AAsm::HasPool() {
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

int AAsm::ZombieTotalHp(int wave) {
    int ret;
    __asm__ __volatile__(
        "pushl %[wave];"
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl $0x412e30, %%ecx;"
        "calll *%%ecx;"
        "mov %%eax, %[ret];"
        : [ret] "=rm"(ret)
        : [wave] "rm"(wave)
        : "ebx", "ecx", "edx");
    return ret;
}

void AAsm::EnterGame(int gameMode) {
    auto gameUi = AGetPvzBase()->GameUi();
    if (gameUi == LEVEL_INTRO || gameUi == PLAYING)
        return; // 在选卡界面或者战斗界面此函数无效

    if (gameUi == 0 || gameUi == 1) {
        if (gameUi == 0) // 载入界面，需要直接删除载入界面进入主界面
            __asm__ __volatile__(
                "movl 0x6a9ec0, %%ecx;"
                "movl $0x452cb0, %%ebx;"
                "calll *%%ebx;"
                :
                :
                : ASaveAllRegister);

        __asm__ __volatile__( // 删除主界面
            "movl 0x6a9ec0, %%esi;"
            "movl $0x44f9e0, %%eax;"
            "calll *%%eax;"
            :
            :
            : ASaveAllRegister);
    }

    if (gameUi == 7)          // 选项卡界面
        __asm__ __volatile__( // 删除选项卡界面
            "movl 0x6a9ec0, %%esi;"
            "movl $0x44fd00, %%eax;"
            "calll *%%eax;"
            :
            :
            : ASaveAllRegister);

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

void AAsm::DoBackToMain() {
    auto gameUi = AGetPvzBase()->GameUi();
    if (gameUi != PLAYING)
        return; // 在非战斗界面此函数无效
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%eax;"
        "movl $0x44feb0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::PickZombieWaves() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl $0x4092e0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::PickRandomSeeds() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x774(%%ebx), %%ebx;"
        "pushl %%ebx;"
        "movl $0x4859b0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::PlayFoleyPitch(int idx) {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x784(%%ecx), %%ecx;"
        "movl %[idx], %%eax;"
        "pushl $0;"
        "movl $0x515020, %%ebx;"
        "calll *%%ebx;"
        :
        : [idx] "m"(idx)
        : ASaveAllRegister);
}

void AAsm::StopFoley(int idx) {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x784(%%edi), %%edi;"
        "movl %[idx], %%eax;"
        "movl $0x515290, %%ebx;"
        "calll *%%ebx;"
        :
        : [idx] "m"(idx)
        : ASaveAllRegister);
}

// 播放僵尸的出场音效
void AAsm::PlayZombieAppearSound(AZombie* zombie) {
    __asm__ __volatile__(
        "movl %[zombie], %%ecx;"
        "movl $0x530640, %%eax;"
        "calll *%%eax;"
        :
        : [zombie] "m"(zombie)
        : ASaveAllRegister);
}

// 播放音乐
void AAsm::PlaySample(int idx) {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%ecx;"
        "pushl %[idx];"
        "movl $0x4560C0, %%ebx;"
        "calll *%%ebx;"
        :
        : [idx] "m"(idx)
        : ASaveAllRegister);
}

void AAsm::UpdateCursorObjectAndPreview() {
    __asm__ __volatile__(
        // CursorObject::Update
        "movl 0x6a9ec0, %%esi;"
        "movl 0x768(%%esi), %%esi;"
        "movl 0x138(%%esi), %%esi;"
        "movl $0x438780, %%ebx;"
        "calll *%%ebx;"

        // CursorPreview::Update
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl 0x13C(%%edi), %%edi;"
        "movl $0x438DA0, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : "esi", "ebx", "edi");
}

void AAsm::RefreshAllSeedPackets() {
    __asm__ __volatile__(
        "movl 0x6a9ec0, %%esi;"
        "movl 0x768(%%esi), %%esi;"
        "movl 0x144(%%esi), %%esi;"
        "pushl %%esi;"
        "movl $0x489D50, %%ebx;"
        "calll *%%ebx;"
        :
        :
        : "esi", "ebx");
}

// // 重新开始循环的音效
// void AAsm::RestartLoopingSounds()
// {
//     // DWORD ptr[0x55];
//     // ptr[0x54] = DWORD(AGetPvzBase());
//     // void* tmpPtr = &ptr;

//     // __asm__ __volatile__(
//     //     "movl %[tmpPtr], %%edi;"
//     //     "movl $0x4335D0, %%eax;"
//     //     "calll *%%eax;"
//     //     :
//     //     : [tmpPtr] "m"(tmpPtr)
//     //     : ASaveAllRegister);
//     // // 这里需要再删除被冰冻的小丑和舞王的引用计数
//     // // 给宝开的 BUG 擦屁股
//     // for (auto&& zombie : AAliveFilter<AZombie>()) {
//     //     if (zombie.FixationCountdown() == 0 && zombie.FreezeCountdown() == 0) {
//     //         continue;
//     //     }
//     //     int type = zombie.Type();
//     //     if (type == AXC_15) {
//     //         StopFoley(_JACK_MUSIC_IDX);
//     //         StopFoley(_JACK_MUSIC_IDX);
//     //     } else if (type == AWW_8) {
//     //         StopFoley(_MJ_MUSIC_IDX);
//     //         StopFoley(_MJ_MUSIC_IDX);
//     //     }
//     //     zombie.MRef<bool>(0x104) = false;
//     // }
// }
