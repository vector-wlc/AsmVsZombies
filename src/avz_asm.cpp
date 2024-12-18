#include "libavz.h"

void AAsm::GameFightLoop() {
    asm volatile(
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x415d40, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::GameTotalLoop() {
    asm volatile(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x452650, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::GameSleepLoop() {
    asm volatile(
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x453a50, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::UpdateFrame() {
    asm volatile(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x320(%%edi), %%edi;"
        "movl $0x539140, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::ClearObjectMemory() {
    asm volatile(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%esi;"
        "movl $0x41bad0, %%eax;"
        "call *%%eax;"

        "movl 0x6a9ec0, %%ebx;"
        "movl 0x820(%%ebx), %%esi;"
        "pushl %%esi;"
        "movl $0x445680, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::KillZombiesPreview() {
    asm volatile(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl $0x40df70, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::SetImprovePerformance(bool is_improve_performance) {
    *(bool*)(0x6a66f4) = !is_improve_performance;
}

void AAsm::CheckFightExit() {
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl $0x4524f0, %%ecx;"
        "call *%%ecx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::SaveData() {
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "pushl %%eax;"
        "movl $0x408c30, %%eax;"
        "call *%%eax;"

        :
        :
        : ASaveAllRegister);
}

void AAsm::LoadData() {
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "pushl %%eax;"
        "movl $0x44f7a0, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::Rock() {
    asm volatile(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x774(%%ebx), %%ebx;"
        "movl $0x486d20, %%eax;"
        "movl 0x6a9ec0, %%esi;"
        "movl $0x1, %%edi;"
        "movl $0x1, %%ebp;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::ClickScene(AMainObject* level, int x, int y, int key) {
    if (AGetPvzBase()->GameUi() != 3)
        return;
    asm volatile(
        "pushl %[key];"
        "pushl %[y];"
        "pushl %[x];"
        "movl %[level], %%ecx;"
        "movl $0x411f20, %%eax;"
        "call *%%eax;"
        :
        : [key] "rm"(key), [y] "rm"(y), [x] "rm"(x), [level] "rm"(level)
        : ASaveAllRegister);
}

void AAsm::Click(AMouseWindow* mw, int x, int y, int key) {
    asm volatile(
        "pushl %[x];"
        "movl %[key], %%ebx;"
        "movl %[y], %%eax;"
        "movl %[mw], %%ecx;"
        "movl $0x539390, %%edx;"
        "call *%%edx;"
        :
        : [x] "rm"(x), [key] "rm"(key), [y] "rm"(y), [mw] "rm"(mw)
        : ASaveAllRegister);
}

void AAsm::MouseClick(int x, int y, int key) {
    int curX = AGetPvzBase()->MouseWindow()->MouseAbscissa();
    int curY = AGetPvzBase()->MouseWindow()->MouseOrdinate();
    MouseDown(x, y, key);
    MouseUp(x, y, key);
    MouseMove(curX, curY);
}

// 鼠标按下
void AAsm::MouseDown(int x, int y, int key) {
    asm volatile(
        "pushl %[x];"
        "movl %[y], %%eax;"
        "movl %[key], %%ebx;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x320(%%ecx), %%ecx;"
        "movl $0x539390, %%edx;"
        "call *%%edx;"
        :
        : [x] "rm"(x), [y] "rm"(y), [key] "rm"(key)
        : ASaveAllRegister);
}

// 鼠标松开
void AAsm::MouseUp(int x, int y, int key) {
    asm volatile(
        "pushl %[key];"
        "pushl %[x];"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x320(%%eax), %%eax;"
        "movl %[y], %%ebx;"
        "movl $0x5392e0, %%edx;"
        "call *%%edx;"
        :
        : [x] "rm"(x), [y] "rm"(y), [key] "rm"(key)
        : ASaveAllRegister);
}

// 移动鼠标
void AAsm::MouseMove(int x, int y) {
    asm volatile(
        "movl 0x6a9ec0, %%edx;"
        "movl 0x320(%%edx), %%edx;"
        "movl %[x], %%eax;"
        "movl %[y], %%ecx;"
        "movl $0x5394a0, %%ebx;"
        "call *%%ebx;"
        :
        : [x] "rm"(x), [y] "rm"(y)
        : ASaveAllRegister);
}

void AAsm::Fire(int x, int y, int rank) {
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%edi;"
        "movl 0xac(%%edi), %%eax;"
        "imull $0x14c, %[rank], %%ecx;"
        "addl %%ecx, %%eax;"
        "pushl %[y];"
        "pushl %[x];"
        "movl $0x466d50, %%edx;"
        "call *%%edx;"
        :
        : [x] "rm"(x), [y] "rm"(y), [rank] "rm"(rank)
        : ASaveAllRegister);
}

void AAsm::PlantCard(int x, int y, int index) {
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%edi;"
        "pushl %[y];"
        "pushl %[x];"
        "pushl %%edi;"
        "movl 0x144(%%edi), %%eax;"
        "imull $0x50, %[index], %%ecx;"
        "addl %%ecx, %%eax;"
        "addl $0x28, %%eax;"
        "pushl %%eax;"
        "movl $0x488590, %%ecx;"
        "call *%%ecx;"
        "movl $0x1, %%ecx;"
        "movl $0x40fd30, %%edx;"
        "call *%%edx;"
        :
        : [x] "rm"(x), [y] "rm"(y), [index] "rm"(index)
        : ASaveAllRegister);
}

void AAsm::ShovelPlant(int x, int y) {
    asm volatile(
        "pushl $0x6;"
        "pushl $0x1;"
        "movl %[y], %%ecx;"
        "movl %[x], %%edx;"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x411060, %%ebx;"
        "call *%%ebx;"
        :
        : [x] "rm"(x), [y] "rm"(y)
        : ASaveAllRegister);
}

void AAsm::ChooseCard(int cardType) {
    asm volatile(
        "movl $0x6a9ec0, %%eax;"
        "movl (%%eax), %%eax;"
        "movl 0x774(%%eax), %%eax;"
        "movl %[cardType], %%edx;"
        "shll $0x4, %%edx;"
        "subl %[cardType], %%edx;"
        "shll $0x2, %%edx;"
        "addl $0xa4, %%edx;"
        "addl %%eax, %%edx;"
        "pushl %%edx;"
        "movl $0x486030, %%ecx;"
        "call *%%ecx;"
        :
        : [cardType] "rm"(cardType)
        : ASaveAllRegister);
}

void AAsm::ChooseImitatorCard(int cardType) {
    asm volatile(
        "movl $0x6a9ec0, %%eax;"
        "movl (%%eax), %%eax;"
        "movl 0x774(%%eax), %%eax;"

        "movl $0x3, 0x0c08(%%eax);"
        "movl $0x0, 0x0c09(%%eax);"
        "movl $0x0, 0x0c0a(%%eax);"
        "movl $0x0, 0x0c0b(%%eax);"
        "movl %[cardType], %%edx;"
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
        "call *%%edx;"
        "movl $0x4866e0, %%edx;"
        "call *%%edx;"
        :
        : [cardType] "rm"(cardType)
        : ASaveAllRegister);
}

int AAsm::GetPlantRejectType(int cardType, int row, int col) {
    int ret;
    asm volatile(
        "pushl %[cardType];"
        "pushl %[col];"
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "pushl %%eax;"
        "movl %[row], %%eax;"
        "movl $0x40e020, %%edx;"
        "call *%%edx;"
        "movl %%eax, %[ret];"
        : [ret] "=rm"(ret)
        : [cardType] "rm"(cardType), [row] "rm"(row), [col] "rm"(col)
        : "esp", "eax", "ecx", "edx");
    return ret;
}

void AAsm::ReleaseMouse() {
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x40cd80, %%edx;"
        "call *%%edx;"
        :
        :
        : ASaveAllRegister);
}

int AAsm::GridToAbscissa(int row, int col) {
    int ret;
    asm volatile(
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl %[col], %%eax;"
        "movl %[row], %%esi;"
        "movl $0x41c680, %%edx;"
        "call *%%edx;"
        "movl %%eax, %[ret];"
        : [ret] "=rm"(ret)
        : [col] "rm"(col), [row] "rm"(row)
        : "esp", "eax", "ecx", "edx", "esi");
    return ret;
}

int AAsm::GridToOrdinate(int row, int col) {
    int ret;
    asm volatile(
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl %[col], %%ecx;"
        "movl %[row], %%eax;"
        "movl $0x41c740, %%edx;"
        "call *%%edx;"
        "movl %%eax, %[ret];"
        : [ret] "=rm"(ret)
        : [col] "rm"(col), [row] "rm"(row)
        : "esp", "eax", "ecx", "edx", "ebx");
    return ret;
}

AZombie* AAsm::PutZombie(int row, int col, AZombieType type) {
    auto ret = AGetMainObject()->ZombieArray() + AGetMainObject()->ZombieNext();
    asm volatile(
        "movl %[row], %%eax;"
        "pushl %[col];"
        "pushl %[index];"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl 0x160(%%ecx), %%ecx;"
        "movl $0x42a0f0, %%edx;"
        "call *%%edx;"
        :
        : [row] "rm"(row), [col] "rm"(col), [index] "rm"(int(type))
        : ASaveAllRegister);
    return ret;
}

APlant* AAsm::PutPlant(int row, int col, APlantType type) {
    if (type == AIMITATOR)
        return nullptr; // 不可能出现这种情况

    auto ret = AGetMainObject()->PlantArray() + AGetMainObject()->PlantNext();

    int imitatorType = -1;
    if (type >= AM_PEASHOOTER) {
        imitatorType = type - AM_PEASHOOTER;
        type = AIMITATOR;
    }
    asm volatile(
        "pushl %[imitatorType];"
        "pushl %[type];"
        "movl %[row], %%eax;"
        "pushl %[col];"
        "movl 0x6a9ec0, %%ebp;"
        "movl 0x768(%%ebp), %%edi;"
        "pushl %%edi;"
        "movl $0x40d120, %%edx;"
        "call *%%edx;"
        :
        : [imitatorType] "rm"(imitatorType), [type] "rm"(type), [row] "rm"(row), [col] "rm"(col)
        : ASaveAllRegister);

    return ret;
}

void AAsm::RemovePlant(APlant* plant) {
    asm volatile(
        "pushl %[plant];"
        "movl $0x4679b0, %%edx;"
        "call *%%edx;"
        :
        : [plant] "rm"(plant)
        : ASaveAllRegister);
}

void AAsm::KillZombie(AZombie* zombie) {
    asm volatile(
        "movl %[zombie], %%ecx;"
        "movl $0x5302f0, %%edx;"
        "call *%%edx;"
        :
        : [zombie] "rm"(zombie)
        : ASaveAllRegister);
}

void AAsm::RemoveZombie(AZombie* zombie) {
    asm volatile(
        "movl %[zombie], %%ecx;"
        "movl $0x530510, %%edx;"
        "call *%%edx;"
        :
        : [zombie] "rm"(zombie)
        : ASaveAllRegister);
}

void* AAsm::SaveToMemory() {
    void* p = malloc(0x24);
    memset(p, 0, 0x24);

    asm volatile(
        "movl %[p], %%eax;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "pushl %%ecx;"
        "movl $0x4819d0, %%ecx;"
        "call *%%ecx;"
        "addl $0x4, %%esp;"
        :
        : [p] "rm"(p)
        : ASaveAllRegister);

    ((bool*)p)[0x21] = 1;
    return p;
}

void AAsm::FreeMemory(void*& p) {
    if (!p)
        return;
    asm volatile(
        "movl %[p], %%ecx;"
        "movl $0x5d60c0, %%eax;"
        "call *%%eax;"
        :
        : [p] "rm"(p)
        : ASaveAllRegister);
    free(p);
    p = nullptr;
}

void AAsm::LoadFromMemory(void*& p) {
    if (!p)
        return;
    asm volatile(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl 0x160(%%edi), %%edi;"
        "movl $0x429e50, %%eax;"
        "call *%%eax;"

        "movl %[p], %%eax;"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "pushl %%edi;"
        "movl $0x4819d0, %%ecx;"
        "call *%%ecx;"
        "movl $0x481ce0, %%eax;"
        "call *%%eax;"
        "addl $0x4, %%esp;"
        :
        : [p] "rm"(p)
        : ASaveAllRegister);
    FreeMemory(p);
}

bool AAsm::IsSeedUsable(ASeed* seed) {
    bool ret;
    asm volatile(
        "movl %[ptr], %%esi;"
        "movl $0x488500, %%eax;"
        "call *%%eax;"
        "movb %%al, %[ret];"
        : [ret] "=rm"(ret)
        : [ptr] "rm"((char*)seed + 0x28)
        : "esp", "eax", "ecx", "edx", "esi");
    return ret;
}

int AAsm::GetSeedSunVal(int type, int iType) {
    int ret;
    asm volatile(
        "movl %[type], %%eax;"
        "movl %[iType], %%edx;"
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl $0x41dae0, %%ecx;"
        "call *%%ecx;"
        "movl %%eax, %[ret];"
        : [ret] "=rm"(ret)
        : [type] "rm"(type), [iType] "rm"(iType)
        : "esp", "eax", "ecx", "edx", "edi");
    return ret;
}

void AAsm::UpdateMousePos() {
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x40eab0, %%ecx;"
        "call *%%ecx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::MakePvzString(const char* str, void* strObj) {
    asm volatile(
        "movl %[str], %%ecx;"
        "pushl %%ecx;"
        "movl %[strObj], %%ecx;"
        "movl $0x404450, %%eax;"
        "call *%%eax;"
        :
        : [str] "rm"(str), [strObj] "rm"(strObj)
        : ASaveAllRegister);
}

void AAsm::FreePvzString(void* strObj) {
    asm volatile(
        "movl %[strObj], %%ecx;"
        "movl $0x404420, %%eax;"
        "call *%%eax;"
        :
        : [strObj] "rm"(strObj)
        : ASaveAllRegister);
}

void AAsm::MakeNewBoard() {
    int scene = AGetMainObject()->Scene();
    asm volatile(
        // MakeNewBoard
        "movl 0x6a9ec0, %%ecx;"
        "movl $0x44f5f0, %%eax;"
        "call *%%eax;"

        // ProcessSafeDeleteList
        "movl 0x6a9ec0, %%ecx;"
        "pushl %%ecx;"
        "movl $0x5518f0, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
    AGetMainObject()->Scene() = scene;
}

// 此函数是为了防止 LoadGame 过长的
void __ABeforeMakeNewBoard(std::vector<int>& zombieMusicRefCnts, std::vector<int>& zombieMusicIdxs) {
    for (auto idx : zombieMusicIdxs) {
        zombieMusicRefCnts.push_back(AMRef<int>(0x6a9ec0, 0x784, 0xa4 * idx + 0x4));
        if (zombieMusicRefCnts.back() != 0) {
            // 有音效要维持住
            AAsm::PlayFoleyPitch(idx);
        }
    }
    // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6a9ec0, 0x784, 0xa4 * AAsm::JACK_MUSIC_IDX + 0x4));

    // 舞王和小丑的音效
    auto&& mjMusicRefCnt = AMRef<int>(0x6a9ec0, 0x784, 0xa4 * AAsm::MJ_MUSIC_IDX + 0x4);
    if (mjMusicRefCnt != 0) {
        int mjCnt = 0;
        for (auto&& zombie : aAliveZombieFilter) {
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
    for (auto&& zombie : aAliveZombieFilter) {
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
        int refCnt = AMRef<int>(0x6a9ec0, 0x784, 0xa4 * AAsm::MJ_MUSIC_IDX + 0x4);
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
        // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6a9ec0, 0x784, 0xa4 * JACK_MUSIC_IDX + 0x4));
        std::vector<int> zombieMusicRefCnts;
        std::vector<int> zombieMusicIdxs = {AAsm::JACK_MUSIC_IDX, AAsm::DIGGER_MUSIC_IDX};
        __ABeforeMakeNewBoard(zombieMusicRefCnts, zombieMusicIdxs);
        MakeNewBoard();
        // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6a9ec0, 0x784, 0xa4 * JACK_MUSIC_IDX + 0x4));

        if (i == 0) // 首先尝试调用 LawnLoadGame
            asm volatile(
                // LawnLoadGame
                "pushl %[tmpPtr];"
                "movl 0x6a9ec0, %%ecx;"
                "movl 0x768(%%ecx), %%ecx;"
                "movl $0x481fe0, %%eax;"
                "call *%%eax;"
                "addl $0x4, %%esp;"
                :
                : [tmpPtr] "rm"(tmpPtr)
                : ASaveAllRegister);
        else // 场景切换需要调用 LoadGame
            asm volatile(
                // LoadGame
                "movl %[tmpPtr], %%eax;"
                "movl 0x6a9ec0, %%edi;"
                "movl 0x768(%%edi), %%edi;"
                "movl $0x408de0, %%ecx;"
                "call *%%ecx;"
                :
                : [tmpPtr] "rm"(tmpPtr)
                : ASaveAllRegister);
        // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6a9ec0, 0x784, 0xa4 * JACK_MUSIC_IDX + 0x4));
        __AAfterLoadGame(zombieMusicRefCnts, zombieMusicIdxs);
        // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6a9ec0, 0x784, 0xa4 * JACK_MUSIC_IDX + 0x4));

        // 这说明需要切换场景
        if (scene == AGetMainObject()->Scene())
            break;
    }

    asm volatile(
        // continue board
        "movl $0, %%eax;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x4127a0, %%edx;"
        "call *%%edx;"
        :
        : [tmpPtr] "rm"(tmpPtr)
        : ASaveAllRegister);
    // aLogger->Info("{} {}", __LINE__, AMRef<int>(0x6a9ec0, 0x784, 0xa4 * _JACK_MUSIC_IDX + 0x4));

    FreePvzString(&pvzStr);
}

void AAsm::SaveGame(const std::string& file) {
    for (auto&& zombie : AObjSelector(&AZombie::Type, AJACK_IN_THE_BOX_ZOMBIE, &AZombie::State, 15))
        if (zombie.FixationCountdown() != 0 || zombie.FreezeCountdown() != 0)
            zombie.MRef<bool>(0x104) = zombie.MRef<bool>(0xba);
    uint8_t pvzStr[28] = {0};
    void* tmpPtr = &pvzStr;
    MakePvzString(file.c_str(), &pvzStr);
    asm volatile(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "pushl %[tmpPtr];"
        "movl $0x4820d0, %%ecx;"
        "call *%%ecx;"
        "addl $0x4, %%esp;"
        :
        : [tmpPtr] "rm"(tmpPtr)
        : ASaveAllRegister);
    FreePvzString(&pvzStr);
}

bool AAsm::CanSpawnZombies(int row) {
    bool ret;
    asm volatile(
        "movl %[row], %%eax;"
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x768(%%ecx), %%ecx;"
        "movl $0x416110, %%edx;"
        "call *%%edx;"
        "movb %%al, %[ret];"
        : [ret] "=rm"(ret)
        : [row] "rm"(row)
        : "esp", "eax", "ecx", "edx");
    return ret;
}

bool AAsm::IsNight() {
    bool ret;
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x41c010, %%ecx;"
        "call *%%ecx;"
        "movb %%al, %[ret];"
        : [ret] "=rm"(ret)
        :
        : "esp", "eax", "ecx", "edx");
    return ret;
}

bool AAsm::IsRoof() {
    bool ret;
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x41c0b0, %%ecx;"
        "call *%%ecx;"
        "movb %%al, %[ret];"
        : [ret] "=rm"(ret)
        :
        : "esp", "eax", "ecx", "edx");
    return ret;
}

bool AAsm::HasGrave() {
    bool ret;
    asm volatile(
        "movl 0x6a9ec0, %%edx;"
        "movl 0x768(%%edx), %%edx;"
        "movl $0x41c040, %%ecx;"
        "call *%%ecx;"
        "movb %%al, %[ret];"
        : [ret] "=rm"(ret)
        :
        : "esp", "eax", "ecx", "edx");
    return ret;
}

bool AAsm::HasPool() {
    bool ret;
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x768(%%eax), %%eax;"
        "movl $0x41c0d0, %%ecx;"
        "call *%%ecx;"
        "movb %%al, %[ret];"
        : [ret] "=rm"(ret)
        :
        : "esp", "eax", "ecx", "edx");
    return ret;
}

int AAsm::ZombieTotalHp(int wave) {
    int ret;
    asm volatile(
        "pushl %[wave];"
        "movl 0x6a9ec0, %%ebx;"
        "movl 0x768(%%ebx), %%ebx;"
        "movl $0x412e30, %%ecx;"
        "call *%%ecx;"
        "movl %%eax, %[ret];"
        : [ret] "=rm"(ret)
        : [wave] "rm"(wave)
        : "esp", "eax", "ecx", "edx", "ebx");
    return ret;
}

void AAsm::EnterGame(int gameMode) {
    auto gameUi = AGetPvzBase()->GameUi();
    if (gameUi == LEVEL_INTRO || gameUi == PLAYING)
        return; // 在选卡界面或者战斗界面此函数无效

    if (gameUi == 0 || gameUi == 1) {
        if (gameUi == 0) // 载入界面，需要直接删除载入界面进入主界面
            asm volatile(
                "movl 0x6a9ec0, %%ecx;"
                "movl $0x452cb0, %%eax;"
                "call *%%eax;"
                :
                :
                : ASaveAllRegister);

        asm volatile( // 删除主界面
            "movl 0x6a9ec0, %%esi;"
            "movl $0x44f9e0, %%eax;"
            "call *%%eax;"
            :
            :
            : ASaveAllRegister);
    }

    if (gameUi == 7)  // 选项卡界面
        asm volatile( // 删除选项卡界面
            "movl 0x6a9ec0, %%esi;"
            "movl $0x44fd00, %%eax;"
            "call *%%eax;"
            :
            :
            : ASaveAllRegister);

    // 进入战斗或者选卡界面
    bool ok = 1;
    asm volatile(
        "push %[ok];"
        "pushl %[gameMode];"
        "movl 0x6a9ec0, %%esi;"
        "movl $0x44f560, %%eax;"
        "call *%%eax;"
        :
        : [ok] "rm"(ok), [gameMode] "rm"(gameMode)
        : ASaveAllRegister);
}

void AAsm::DoBackToMain() {
    auto gameUi = AGetPvzBase()->GameUi();
    if (gameUi != PLAYING)
        return; // 在非战斗界面此函数无效
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl $0x44feb0, %%ecx;"
        "call *%%ecx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::PickZombieWaves() {
    asm volatile(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl $0x4092e0, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::PickRandomSeeds() {
    asm volatile(
        "movl 0x6a9ec0, %%eax;"
        "movl 0x774(%%eax), %%eax;"
        "pushl %%eax;"
        "movl $0x4859b0, %%ecx;"
        "call *%%ecx;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::PlayFoleyPitch(int idx) {
    asm volatile(
        "movl 0x6a9ec0, %%ecx;"
        "movl 0x784(%%ecx), %%ecx;"
        "movl %[idx], %%eax;"
        "pushl $0;"
        "movl $0x515020, %%edx;"
        "call *%%edx;"
        :
        : [idx] "rm"(idx)
        : ASaveAllRegister);
}

void AAsm::StopFoley(int idx) {
    asm volatile(
        "movl 0x6a9ec0, %%edi;"
        "movl 0x784(%%edi), %%edi;"
        "movl %[idx], %%eax;"
        "movl $0x515290, %%ecx;"
        "call *%%ecx;"
        :
        : [idx] "rm"(idx)
        : ASaveAllRegister);
}

// 播放僵尸的出场音效
void AAsm::PlayZombieAppearSound(AZombie* zombie) {
    asm volatile(
        "movl %[zombie], %%ecx;"
        "movl $0x530640, %%eax;"
        "call *%%eax;"
        :
        : [zombie] "rm"(zombie)
        : ASaveAllRegister);
}

// 播放音乐
void AAsm::PlaySample(int idx) {
    asm volatile(
        "movl 0x6a9ec0, %%ecx;"
        "pushl %[idx];"
        "movl $0x4560c0, %%eax;"
        "call *%%eax;"
        :
        : [idx] "rm"(idx)
        : ASaveAllRegister);
}

void AAsm::UpdateCursorObjectAndPreview() {
    asm volatile(
        // CursorObject::Update
        "movl 0x6a9ec0, %%esi;"
        "movl 0x768(%%esi), %%esi;"
        "movl 0x138(%%esi), %%esi;"
        "movl $0x438780, %%eax;"
        "call *%%eax;"

        // CursorPreview::Update
        "movl 0x6a9ec0, %%edi;"
        "movl 0x768(%%edi), %%edi;"
        "movl 0x13c(%%edi), %%edi;"
        "movl $0x438da0, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

void AAsm::RefreshAllSeedPackets() {
    asm volatile(
        "movl 0x6a9ec0, %%esi;"
        "movl 0x768(%%esi), %%esi;"
        "movl 0x144(%%esi), %%esi;"
        "pushl %%esi;"
        "movl $0x489d50, %%eax;"
        "call *%%eax;"
        :
        :
        : ASaveAllRegister);
}

// // 重新开始循环的音效
// void AAsm::RestartLoopingSounds()
// {
//     // DWORD ptr[0x55];
//     // ptr[0x54] = DWORD(AGetPvzBase());
//     // void* tmpPtr = &ptr;

//     // asm volatile(
//     //     "movl %[tmpPtr], %%edi;"
//     //     "movl $0x4335d0, %%eax;"
//     //     "call *%%eax;"
//     //     :
//     //     : [tmpPtr] "rm"(tmpPtr)
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
