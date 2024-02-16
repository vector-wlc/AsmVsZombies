#include <avz.h>
using namespace std;

ALogger<AConsole> logger;
APlantFixer pumpkinFixer;
APlantFixer fumeShroomFixer;
APlantFixer lilyPadFixer;
ATickRunner c1User;
ATickRunner c2User;
ATickRunner smartBlover;
ATickRunner gloomShroomUser;
ATickRunner n;

int usedSLCnt = 0;
int flagCnt = 0;
bool safe = true;

AOnEnterFight({
    std::string level;
    auto typeList = AGetZombieTypeList();
    if (typeList[AHY_32] && !typeList[ABY_23]) {
        level = "变速关 ";
    } else if (typeList[AHY_32] || typeList[ABY_23]) {
        level = "慢速关 ";
    } else {
        level = "快速关 ";
    }
    int slCnt = flagCnt / 100 + 1;
    logger.Debug("阳光: # " + level + "flag: #,  SL 可用次数: #", AGetMainObject()->Sun(), flagCnt, slCnt - usedSLCnt);
    flagCnt += 2;
});

constexpr auto GAME_DAT_PATH = "C:\\ProgramData\\PopCap Games\\PlantsVsZombies\\userdata\\game1_14.dat";
constexpr auto TMP_DAT_PATH = "D:\\2345Downloads\\tmp.dat";

// 在进入战斗界面之前复制游戏的存档
AOnBeforeScript({
    std::filesystem::copy(GAME_DAT_PATH, TMP_DAT_PATH + std::to_string(flagCnt % 10),
        std::filesystem::copy_options::overwrite_existing);
});

// 增加容错率的 SL 大法
void SaveLoad()
{
    if (usedSLCnt >= flagCnt / 100 + 1) {
        logger.Warning("这次是真的寄了");
        AMsgBox::Show("寄");
        return;
    } else {
        logger.Warning("虽然寄了，但是我们有 SL 大法！");
    }
    // 这两个函数实现了一个组合拳
    // 但是这两个函数都不是立即执行的函数
    // 它相当于是给 AvZ 下达了一个命令
    // AvZ 会在合适的时刻点运行这些命令
    ABackToMain(false);
    AEnterGame(AAsm::SURVIVAL_ENDLESS_STAGE_4);
}

AOnExitFight({
    // 僵尸赢了
    // 直接跳回主界面
    // 这里肯定是不存档的
    if (AGetPvzBase()->GameUi() == AAsm::ZOMBIES_WON) {
        SaveLoad();
    }
    // 如果界面不在选卡界面，就是启动了 SL
    if (AGetPvzBase()->GameUi() != AAsm::LEVEL_INTRO) {
        // 直接倒退回前 8f 的存档 SL
        // 防止死亡循环
        ++usedSLCnt;
        std::filesystem::copy(TMP_DAT_PATH + std::to_string(flagCnt % 10),
            GAME_DAT_PATH, std::filesystem::copy_options::overwrite_existing);
        flagCnt = std::clamp(flagCnt - 10, 0, INT_MAX);
    }
});

void SCard(APlantType type, int r, int c)
{
    auto seed = AGetMainObject()->SeedArray();
    if (seed[AGetCardIndex(type)].IsUsable() && (AGetPlantIndex(r, c, 2) == -1 || AGetPlantIndex(r, c, 2) == -2) && (AGetPlantIndex(r, c, 20) == -1 || AGetPlantIndex(r, c, 20) == -2)) {
        AShovel(r, c);
        ACard(type, r, c);
    }
}

bool IsJack(int r, int a_1, int a_2, int t)
{
    bool rst = false;
    auto z = AGetMainObject()->ZombieArray();
    for (int i = 0; i < AGetMainObject()->ZombieTotal(); ++i) {
        if (z[i].Type() == AJACK_IN_THE_BOX_ZOMBIE && z[i].IsExist() && !z[i].IsDead() && z[i].Row() == r - 1 && z[i].Abscissa() >= a_1 && z[i].Abscissa() <= a_2 && z[i].StateCountdown() == t) {
            rst = true;
        }
    }
    return rst;
}

bool IsZBExist(AZombieType type, int row, int a_1, int a_2, int hp, bool ham) // 判定僵尸
{
    bool result = false;
    auto zombie = AGetMainObject()->ZombieArray();
    for (int index = 0; index < AGetMainObject()->ZombieTotal(); ++index) {
        if (zombie[index].Type() == type && zombie[index].IsExist() && !zombie[index].IsDead() && zombie[index].Row() == row - 1 && zombie[index].Abscissa() >= a_1 && zombie[index].Abscissa() <= a_2 && zombie[index].Hp() >= hp && zombie[index].IsHammering() == ham) {
            result = true;
        }
    }
    return result;
}

void SafeCard(APlantType type, int row, int col, bool judge) // 智能用卡
{
    if (IsZBExist(AZOMBONI, row, 0, 80 * col, 0, false)) {
        safe = false;
    } else {
        safe = true;
    }

    if (judge) {
        if (safe) {
            auto seed = AGetMainObject()->SeedArray();
            if (seed[AGetCardIndex(type)].IsUsable()) {
                if (AGetPlantIndex(row, col) == -1 && AGetPlantIndex(row, col, 33) == -1) {
                    ACard(type, row, col);
                }
            }
        }
    } else {
        auto seed = AGetMainObject()->SeedArray();
        if (seed[AGetCardIndex(type)].IsUsable()) {
            if (AGetPlantIndex(row, col) == -1 && AGetPlantIndex(row, col, 33) == -1) {
                ACard(type, row, col);
            }
        }
    }
}

void N()
{
    if (IsJack(2, 570, 800, 1)) {
        SCard(AJALAPENO, 2, 8);
    }
    if (IsJack(5, 570, 800, 1)) {
        SCard(AJALAPENO, 5, 8);
    }
    if (IsJack(1, 521, 601, 1) || IsJack(2, 570, 800, 1)) {
        SCard(ACHERRY_BOMB, 2, 8);
    }
    if (IsJack(6, 521, 601, 1) || IsJack(5, 570, 800, 1)) {
        SCard(ACHERRY_BOMB, 5, 8);
    }
    if (IsZBExist(AGIGA_GARGANTUAR, 1, 380, 391, 400, true)) {
        SCard(ACHERRY_BOMB, 1, 6);
    }
    if (IsZBExist(AGIGA_GARGANTUAR, 6, 380, 391, 400, true)) {
        SCard(ACHERRY_BOMB, 6, 6);
    }
    if (IsZBExist(AGIGA_GARGANTUAR, 2, 581, 601, 600, true)) {
        SCard(AJALAPENO, 2, 8);
        SCard(ACHERRY_BOMB, 2, 8);
    }
    if (IsZBExist(AGIGA_GARGANTUAR, 5, 581, 601, 600, true)) {
        SCard(AJALAPENO, 5, 8);
        SCard(ACHERRY_BOMB, 5, 8);
    }
    if (IsZBExist(ABACKUP_DANCER, 2, 480, 520, 200, false)) {
        SCard(AJALAPENO, 2, 8);
    }
}

void I()
{
    auto zt = AGetMainObject()->ZombieTypeList();
    if (zt[AGIGA_GARGANTUAR]) {
        for (int w : {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16}) {
            AConnect(ATime(w, 341 - 98), [=] {
                if (w != 9 || IsZBExist(AGIGA_GARGANTUAR, 1, 0, 854, 5000, false) || IsZBExist(AGIGA_GARGANTUAR, 1, 0, 854, 5000, true) || IsZBExist(AGIGA_GARGANTUAR, 2, 0, 854, 5000, false) || IsZBExist(AGIGA_GARGANTUAR, 2, 0, 854, 5000, true) || IsZBExist(AGIGA_GARGANTUAR, 5, 0, 854, 5000, false) || IsZBExist(AGIGA_GARGANTUAR, 5, 0, 854, 5000, true) || IsZBExist(AGIGA_GARGANTUAR, 6, 0, 854, 5000, false) || IsZBExist(AGIGA_GARGANTUAR, 6, 0, 854, 5000, true)) {
                    SCard(AICE_SHROOM, 1, 6);
                }
            });
        }
    } else {
        if (zt[AGARGANTUAR]) {
            for (int w : {1, 2, 3, 4, 5, 6, 7, 11, 12, 13, 14, 15, 16}) {
                AConnect(ATime(w, 341 - 98), [=] {
                    SCard(AICE_SHROOM, 1, 6);
                });
            }
        } else {
            for (int w : {1, 2, 3, 4, 5, 6, 11, 12, 13, 14, 15, 16}) {
                AConnect(ATime(w, 341 - 98), [=] {
                    SCard(AICE_SHROOM, 1, 6);
                });
            }
        }
    }
}

void SmartC_1()
{
    for (int r : {2, 5}) {
        if (IsZBExist(AGIGA_GARGANTUAR, r, 601, 680, 1200, false) && !IsZBExist(AGIGA_GARGANTUAR, r, 601, 686, 1200, true)) {
            for (APlantType type : {APUFF_SHROOM, AM_PUFF_SHROOM, AFLOWER_POT}) {
                SafeCard(type, r, 8, true);
            }
        }
    }
    for (int r : {1, 6}) {
        if (IsZBExist(AGIGA_GARGANTUAR, r, 361 + 80, 436 + 80, 800, false) && !IsZBExist(AGIGA_GARGANTUAR, r, 361 + 80, 436 + 80, 800, true))
            for (APlantType type : {APUFF_SHROOM, AM_PUFF_SHROOM, AFLOWER_POT}) {
                SafeCard(type, r, 6, true);
            }
    }
}

void SmartC_2()
{
    for (int r : {2, 5}) {
        if (IsZBExist(AGIGA_GARGANTUAR, r, 601, 680, 1200, false) && !IsZBExist(AGIGA_GARGANTUAR, r, 601, 686, 1200, true)) {
            for (APlantType type : {APUFF_SHROOM, AM_PUFF_SHROOM, AFUME_SHROOM}) {
                SafeCard(type, r, 8, true);
            }
        }
    }
    for (int r : {1, 6}) {
        if (IsZBExist(AGIGA_GARGANTUAR, r, 361 + 80, 436 + 80, 800, false) && !IsZBExist(AGIGA_GARGANTUAR, r, 361 + 80, 436 + 80, 800, true))
            for (APlantType type : {APUFF_SHROOM, AM_PUFF_SHROOM, AFUME_SHROOM}) {
                SafeCard(type, r, 6, true);
            }
    }
}

void SmartC_b(int w) // 智能垫材，处理舞王
{
    AConnect(ATime(w, 189), [=] {
        for (int r : {2, 5}) {
            if (IsZBExist(ADANCING_ZOMBIE, r, 0, 800, 0, false)) {
                for (APlantType t : {APUFF_SHROOM, AM_PUFF_SHROOM}) {
                    SafeCard(t, r, 9, false);
                }
            }
        }
    });
}

void SmartBlover() // 智能三叶草
{
    for (int r : {1, 2, 3, 4, 5, 6}) {
        if (IsZBExist(ABALLOON_ZOMBIE, r, 0, 20, 0, false)) {
            SafeCard(ABLOVER, 1, 1, false);
        }
    }
}

void PlantGloomShroom(int r, int c) // 补曾主体
{
    auto gloom_shroom = AGetMainObject()->SeedArray() + AGetSeedIndex(AGLOOM_SHROOM);
    if (gloom_shroom->IsUsable() && AGetPlantIndex(r, c, 10) > -1) {
        ACard(AGLOOM_SHROOM, r, c);
    }
}

void FixGloomShroom() // 补曾位置
{
    PlantGloomShroom(3, 9);
    PlantGloomShroom(4, 9);
    PlantGloomShroom(3, 8);
    PlantGloomShroom(4, 8);
    PlantGloomShroom(3, 7);
    PlantGloomShroom(4, 7);
    PlantGloomShroom(2, 7);
    PlantGloomShroom(5, 7);
    PlantGloomShroom(2, 6);
    PlantGloomShroom(5, 6);
}

void AScript()
{
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);

    logger.SetLevel({ALogLevel::DEBUG, ALogLevel::ERROR, ALogLevel::WARNING});
    ASetInternalLogger(logger);

    auto zt = AGetMainObject()->ZombieTypeList();
    AConnect(ATime(1, -599), [=] {
        fumeShroomFixer.Start(AFUME_SHROOM, {{3, 9}, {4, 9}, {3, 8}, {4, 8}, {3, 7}, {4, 7}, {2, 7}, {5, 7}, {2, 6}, {5, 6}, {1, 5}, {6, 5}, {1, 4}, {6, 4}}, 0);
        gloomShroomUser.Start(FixGloomShroom);
        pumpkinFixer.Start(APUMPKIN, {{1, 4}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {3, 6}, {3, 7}, {3, 8}, {3, 9}, {4, 6}, {4, 7}, {4, 8}, {4, 9}, {5, 1}, {5, 2}, {5, 3}, {5, 4}, {6, 1}, {6, 4}}, 2000);
        lilyPadFixer.Start(ALILY_PAD, {{3, 9}, {4, 9}, {3, 8}, {4, 8}, {3, 7}, {4, 7}}, 0);
    });
    // 选卡及分类处理
    if (zt[ABALLOON_ZOMBIE]) {
        ASelectCards({ACHERRY_BOMB, AJALAPENO, AICE_SHROOM, ABLOVER, AM_PUFF_SHROOM, APUFF_SHROOM, AGLOOM_SHROOM, APUMPKIN, ALILY_PAD, AFUME_SHROOM}, 0);
        c2User.Start(SmartC_2);
        smartBlover.Start(SmartBlover);
    } else {
        ASelectCards({ACHERRY_BOMB, AJALAPENO, AICE_SHROOM, AFLOWER_POT, AM_PUFF_SHROOM, APUFF_SHROOM, AGLOOM_SHROOM, APUMPKIN, ALILY_PAD, AFUME_SHROOM}, 0);
        c1User.Start(SmartC_1);
    }

    if (zt[AJACK_IN_THE_BOX_ZOMBIE]) {
        n.Start(N);
    }

    if (!zt[AGIGA_GARGANTUAR] && !zt[AZOMBONI] && zt[ADANCING_ZOMBIE]) {
        for (int w = 1; w <= 20; ++w) {
            SmartC_b(w);
        }
    }

    if (zt[ADANCING_ZOMBIE]) {
        for (int w : {1, 10, 20}) {
            SmartC_b(w);
        }
    }

    // 旗帜波点冰
    for (auto wave : {10, 20}) {
        AConnect(ATime(wave, 97 + 298 - 99), [=] {
            ACard(AICE_SHROOM, {{1, 1}, {2, 8}, {5, 8}, {1, 6}, {6, 6}});
        });
    }

    I();

    ASkipTick([] {
        auto ptrs = AGetPlantPtrs({{1, 3}, {6, 3}}, ABXGTS_44);
        for (auto ptr : ptrs) {
            if (ptr == nullptr) {
                SaveLoad();
                return false;
            }
        }
        return true;
    });
}