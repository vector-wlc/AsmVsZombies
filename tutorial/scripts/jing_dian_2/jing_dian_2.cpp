// 这是一个挂机脚本
// SL 频率大概是每 2000f 一次

// 这个脚本的注意点
// 对于慢速关，带

#include <avz.h>

AOnAfterInject(AEnterGame());

ALogger<AConsole> logger;

APlantFixer ngtFixer;
APlantFixer dpgFixer;
ATickRunner yygFixer;
ATickRunner tickRunner;

std::vector<AZombie*> gigaList;
std::vector<AZombie*> footList;
std::vector<AZombie*> bcList;
bool isBoxExplode = false;
bool isCobUsable = true;
bool isNeedSyc = false;
bool isNeedFire = false;
int usedSLCnt = 0;
int flagCnt = 0;

AOnEnterFight({
    isBoxExplode = false;
    isCobUsable = true;
    isNeedSyc = false;
    isNeedFire = false;
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

constexpr auto GAME_DAT_PATH = "C:\\ProgramData\\PopCap Games\\PlantsVsZombies\\userdata\\game1_13.dat";
constexpr auto TMP_DAT_PATH = "tmp.dat";

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
    AEnterGame();
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

// 观测
void Observe()
{
    static int lastClock = -1;
    int nowClock = AGetMainObject()->GameClock();
    if (lastClock == nowClock) { // 一帧只检测一次
        return;
    }
    gigaList.clear();
    footList.clear();
    bcList.clear();
    isBoxExplode = false;
    isNeedSyc = false;
    isNeedFire = false;

    for (auto&& zombie : aAliveZombieFilter) {
        int x = zombie.Abscissa();

        switch (zombie.Type()) {
        case AHY_32:
            isNeedFire = true;
        case ABY_23:
            gigaList.push_back(&zombie);
            if (x < 361 + 80) {
                isNeedFire = true;
            }
            break;
        case AGL_7:
            footList.push_back(&zombie);
            break;
        case ABC_12:
            bcList.push_back(&zombie);
            break;
        case AXC_15:
            if (zombie.State() == 16) {
                isBoxExplode = true;
            }
            break;
        case AQQ_16:
            if (x < 160) {
                isNeedSyc = true;
            }
            break;
        default:
            break;
        }
    }
}

// 返回锤击进度
// 如果 < 0 代表还未锤击
// 如果 > 0 代表已经锤击
float HammerRate(AZombie* zombie)
{
    auto animationCode = zombie->MRef<uint16_t>(0x118);
    auto animationArray = AGetPvzBase()
                              ->AnimationMain()
                              ->AnimationOffset()
                              ->AnimationArray();
    auto circulationRate = animationArray[animationCode].CirculationRate();
    return circulationRate - 0.644;
}

bool IsGigaHammer(AZombie* zombie, int plantRow, int plantX)
{
    if (!zombie->IsHammering()) {
        return false;
    }
    if (zombie->Row() != plantRow) {
        return false;
    }
    if (HammerRate(zombie) > 0) {
        return false;
    }

    std::pair<int, int> zombieAtk = {zombie->Abscissa() - 30, zombie->Abscissa() - 30 + 89};
    auto plantDef = std::make_pair(30, 50);
    plantDef.first += plantX;
    plantDef.second += plantX;
    return std::max(zombieAtk.first, plantDef.first) <= std::min(zombieAtk.second, plantDef.second);
}

int GetUsableCobCnt()
{
    int cnt = 0;
    for (auto [ptr, recoverTime] : aCobManager.GetRecoverList()) {
        if (ptr != nullptr && recoverTime == 0) {
            ++cnt;
        }
    }
    return cnt;
}

// 得到红眼的威胁坐标
const std::vector<AZombie*>& GetGigaDangerList()
{
    static int lastClock = -1;
    static std::vector<AZombie*> ret(6, nullptr);
    static std::vector<int> xOffset = {361, 441, 0, 0, 441, 361};
    static std::vector<int> HpOffset = {400, 900, 0, 0, 400, 600};
    int nowClock = AGetMainObject()->GameClock();
    if (lastClock == nowClock) { // 一帧只检测一次
        return ret;
    }
    ret.assign(6, nullptr);
    lastClock = nowClock;

    for (auto zombie : gigaList) {
        int row = zombie->Row();
        int x = zombie->Abscissa();
        if (zombie->Hp() < HpOffset[row]
            || x - xOffset[row] > 160) {
            continue;
        }
        if (ret[row] == nullptr) {
            ret[row] = zombie;
            continue;
        }
        if (x < ret[row]->Abscissa()) {
            ret[row] = zombie;
        }
    }

    std::sort(ret.begin(), ret.end(), [](AZombie* lhs, AZombie* rhs) {
        if (lhs == nullptr) {
            return false;
        }
        if (rhs == nullptr) {
            return true;
        }
        int lhsX = lhs->Abscissa() - xOffset[lhs->Row()];
        int rhsX = rhs->Abscissa() - xOffset[rhs->Row()];
        return lhsX < rhsX;
    });

    return ret;
}

// 得到橄榄的威胁坐标
const std::vector<AZombie*>& GetFootDangerList()
{
    static int lastClock = -1;
    static std::vector<AZombie*> ret(6, nullptr);
    int nowClock = AGetMainObject()->GameClock();
    if (lastClock == nowClock) { // 一帧只检测一次
        return ret;
    }
    ret.assign(6, nullptr);
    lastClock = nowClock;

    for (auto&& zombie : footList) {
        int row = zombie->Row();
        int x = zombie->Abscissa();
        if (x > 360 + 80
            || (row != 1 && row != 4)
            || zombie->OneHp() < 600) {
            continue;
        }
        if (ret[row] == nullptr) {
            ret[row] = zombie;
            continue;
        }
        if (x < ret[row]->Abscissa()) {
            ret[row] = zombie;
        }
    }

    std::sort(ret.begin(), ret.end(), [](AZombie* lhs, AZombie* rhs) {
        if (lhs == nullptr) {
            return false;
        }
        if (rhs == nullptr) {
            return true;
        }
        return lhs->Abscissa() < rhs->Abscissa();
    });

    return ret;
}

// 使用炮进行减压
void Fire()
{
    static int cd = 0;
    if (cd > 0) {
        --cd;
        return;
    }
    auto&& dangerList = GetGigaDangerList();
    if (dangerList[0] == nullptr) {
        return;
    }
    int zombieRow = dangerList[0]->Row();
    if (isCobUsable && aCobManager.GetUsablePtr() != nullptr) {
        float zombieX = 800;
        int fireRow = (zombieRow == 0 || zombieRow == 1) ? 1 : 5;
        std::vector<int> rowRange = (zombieRow == 0 || zombieRow == 1) ? std::vector<int>({0, 1}) : std::vector<int>({4, 5});
        for (auto zombie : dangerList) {
            if (zombie == nullptr) {
                continue;
            }
            if (ARangeIn(zombie->Row(), rowRange)) {
                zombieX = std::min(zombie->Abscissa(), zombieX);
            }
        }
        // 230 是炮的极限
        // 考虑到 373 飞行时间，给这个坐标再减半格
        auto fireCol = (zombieX + 230 - 40) / 80;
        fireCol = std::clamp<float>(fireCol, 6, 7.2);
        aCobManager.Fire(fireRow, fireCol);
        cd = 1000 + 373 + 1; // 等待生效 1000cs 之后再看情况;
    }
}

// 修理忧郁菇
void FixYyg()
{
    auto fixFunc = [](const std::vector<APlantType>& seedList, const std::vector<AGrid>& gridList) {
        for (auto seedType : seedList) {
            if (!AIsSeedUsable(seedType)) {
                return;
            }
        }
        auto ptrs = AGetPlantPtrs(gridList, AYYG_42);
        for (int i = 0; i < ptrs.size(); ++i) {
            if (ptrs[i] == nullptr) {
                ACard(seedList, gridList[i].row, gridList[i].col);
                static int cnt = 0;
                logger.Warning("第 # 次补曾，位置为 (#, #)", ++cnt, gridList[i].row, gridList[i].col);
                return;
            }
        }
    };

    fixFunc({AHY_16, ANGT_30, ADPG_10, AYYG_42, AKFD_35}, {{3, 7}, {4, 7}, {3, 6}, {4, 6}});
    fixFunc({AHY_16, ADPG_10, AYYG_42, AKFD_35}, {{2, 5}, {5, 5}});
}

void PutFodder()
{
    std::vector<APlantType> seedList = {AXPG_8, AYGG_9, AHP_33};
    if (AGetMainObject()->Sun() > 6000) {
        seedList.push_back(ADPG_10);
    }

    bool isOk = false;
    for (auto type : seedList) {
        if (AIsSeedUsable(type)) {
            isOk = true;
            break;
        }
    }
    if (!isOk) {
        return;
    }

    auto&& gigaDangerList = GetGigaDangerList();
    auto&& footDangerList = GetFootDangerList();
    std::vector<AGrid> grids = {{1, 5}, {6, 5}, {2, 6}, {5, 6}};

    auto put = [](int row, int col, const std::vector<APlantType>& seedList) -> APlant* {
        for (auto zombie : gigaList) {
            if (IsGigaHammer(zombie, row - 1, col * 80 - 40)) {
                return nullptr;
            }
        }
        if (AAsm::GetPlantRejectType(AHP_33, row - 1, col - 1) != AAsm::NIL) {
            return nullptr;
        }
        for (auto type : seedList) {
            if (!AIsSeedUsable(type)) {
                continue;
            }
            if (type != ASYC_27) {
                for (auto bc : bcList) {
                    if (bc->Row() == row - 1 && bc->Abscissa() < col * 80 + 10) {
                        return nullptr;
                    }
                }
            }
            return ACard(type, row, col);
        }
        return nullptr;
    };

    if (gigaDangerList[0] == nullptr && footDangerList[0] == nullptr) {
        if (!AIsSeedUsable(AXPG_8)) {
            return;
        }
        for (auto&& [row, col] : grids) {
            if (put(row, col, {AXPG_8})) {
                return;
            }
        }
        return;
    }
    std::vector<int> cols = {5, 6, 0, 0, 6, 5};

    for (auto zombie : gigaDangerList) {
        if (zombie == nullptr) {
            continue;
        }
        put(zombie->Row() + 1, cols[zombie->Row()], seedList);
    }
    for (auto zombie : footDangerList) {
        if (zombie == nullptr) {
            return;
        }
        put(zombie->Row() + 1, cols[zombie->Row()], seedList);
    }
}

void PutSyc()
{
    if (!isNeedSyc || !AIsSeedUsable(ASYC_27)) {
        return;
    }
    auto zombieList = AGetZombieTypeList();
    if (zombieList[AHY_32] || zombieList[ABY_23]) { // 有巨人陆地上是不安全的，因为可能会被锤掉
        if (AGetPlantPtr(4, 9, AYTZD_2)) {          // 樱桃临时位置
            return;
        }
        if (AIsSeedUsable(AHY_16)) {
            ACard({AHY_16, ASYC_27}, 4, 9);
        }
    } else {
        ARemovePlant(5, 6);
        ACard(ASYC_27, 5, 6);
    }
}

void SlowLevelContorlBox()
{
    // 慢速关控丑

    // 首先考虑炮控的可能性
    Observe();
    if (!isNeedFire) {
        auto ls = aCobManager.GetRecoverList();
        if (GetUsableCobCnt() == 2) {
            isCobUsable = false;
            AConnect(ATime(ANowWave(), 500 - 373), [] {
                if ((ANowWave() == 9 || ANowWave() == 19) && !AIsZombieExist(ABY_23)) {
                    aCobManager.Fire({{3, 8}, {4, 8}});
                } else {
                    aCobManager.Fire({{2, 8}, {5, 8}});
                }
                isCobUsable = true;
            });
            return;
        }
    }

    // 再考虑冰控
    if (AIsSeedUsable(AKFD_35) && AGetPlantPtr(3, 5, AICE_SHROOM) != nullptr) {
        aIceFiller.Coffee();
        return;
    }

    // 变速关的不完全控丑
    // 如果被炸了只能靠曾哥修
    AConnect(ATime(ANowWave(), 500 - 373), [] {
        auto&& dangerList = GetGigaDangerList();
        int zombieRow = dangerList[0] == nullptr ? 1 : dangerList[0]->Row();
        if (aCobManager.GetUsablePtr() != nullptr) {
            int fireRow = (zombieRow == 0 || zombieRow == 1) ? 2 : 5;
            aCobManager.Fire(fireRow, 8);
        }
        if (aCobManager.GetUsablePtr() != nullptr) {
            int fireRow = (zombieRow == 0 || zombieRow == 1) ? 5 : 2;
            aCobManager.Fire(fireRow, 8);
        }
    });
}

void SlowLevel()
{
    aIceFiller.Start({{3, 5}});

    for (int wave = 1; wave < 21; ++wave) {
        if (wave == 10 || wave == 20) {
            continue;
        }
        AConnect(ATime(wave, 0), [] {
            SlowLevelContorlBox();
        });
    }

    for (auto wave : {9, 19}) {
        AConnect(ATime(wave, 600), [] {
            if (GetUsableCobCnt() == 2) {
                aCobManager.Fire({{2, 8}, {5, 8}});
            }
        });
    }

    AConnect(ATime(20, 1200 - 373), []() -> ACoroutine {
        if (GetUsableCobCnt() == 2) {
            aCobManager.Fire({{2, 8.5}, {5, 8.5}});
            co_await 3475;
            aCobManager.Fire({{2, 7}, {5, 7}});
        } else {
            co_await [] { return AGetPlantPtr(3, 5, AICE_SHROOM) != nullptr && AIsSeedUsable(AKFD_35); };
            aIceFiller.Coffee();
        }
    });
}

void FastLevel()
{
    // 1 2   3 4 5 6 7 8   9 10 11 12 13   14 15 16 17 18  19
    // 核樱辣冰冰冰炮核樱辣冰 冰 炮 核 樱辣 冰 冰 炮 核 樱辣 冰
    // 灰烬接核能防丑并且保证核的安全

    AConnect(ATime(1, 0), [] {
        ACard({AHMG_15, AKFD_35}, {{5, 9}});
    });

    // 360 是最早啃食八列植物的时间
    AConnect(ATime(2, 360 - 100), [] {
        ACard(AYTZD_2, 6, 9);
        ARemovePlant(2, 6);
        ACard(AHBLJ_20, 2, 6);
    });

    // 6 11 要尽可能晚
    for (auto wave : {6, 16}) {
        AConnect(ATime(wave, 550 - 373), [] {
            aCobManager.Fire({{2, 9}, {5, 9}});
        });
    }

    // 第 11 波的炮要尽可能早
    AConnect(ATime(11, 601 - 200 - 373), [] {
        aCobManager.Fire({{2, 9}, {5, 9}});
    });

    AConnect(ATime(7, 0), [] {
        ACard({AHY_16, AHMG_15, AKFD_35}, {{3, 9}});
    });

    AConnect(ATime(12, 162 - 298), [] {
        ACard({AHY_16, AHMG_15, AKFD_35}, {{3, 8}});
    });

    // 上一波的僵尸已经被炮炸死了
    // 这里不存在啃核
    AConnect(ATime(17, 550 - 298), [] {
        ACard({AHY_16, AHMG_15, AKFD_35}, {{4, 8}});
    });

    for (auto wave : {3, 4, 5, 9, 14, 15, 19}) {
        AConnect(ATime(wave, 0), [] {
            aIceFiller.Coffee();
        });
    }

    // 第 13 波的生效尽可能早
    AConnect(ATime(13, 162 - 100), [] {
        ACard({AHY_16, AYTZD_2}, 4, 9);
        ARemovePlant(2, 6);
        ACard(AHBLJ_20, 2, 6);
    });

    for (auto wave : {8, 18}) {
        AConnect(ATime(wave, 440 - 100), [] {
            ACard({AHY_16, AYTZD_2}, 4, 9);
            ARemovePlant(2, 6);
            ACard(AHBLJ_20, 2, 6);
        });
    }

    AConnect(ATime(3, 0), [] {
        aIceFiller.SetList({{3, 5}});
    });

    for (int wave : {1, 10}) {
        AConnect(ATime(wave, -599), [] {
            aIceFiller.Start({{3, 5}, {5, 8}});
        });
    }
}

void Func()
{
    if (!AIsSeedUsable(AWG_17)) {
        return;
    }

    for (int col = 9; col > 0; --col) {
        if (ACard(AWG_17, 1, col)) {
            break;
        }
    }
}

void Replay()
{
    // 这里需要改成自己的磁盘路径
    aReplay.SetSaveDirPath("R:\\");
    aReplay.SetMaxSaveCnt(1000);
    aReplay.StartRecord();

    // 播放之前必须先 Stop
    AConnect('W', [] {
        aReplay.Stop();
        aReplay.StartPlay();
    });

    // 暂停播放或者录制
    AConnect('E', [] {
        if (aReplay.IsPaused()) {
            aReplay.GoOn();
        } else {
            aReplay.Pause();
        }
    });

    // 停止回放对象的工作
    AConnect('R', [] {
        aReplay.Stop();
    });

    // 播放下一帧
    AConnect('D', [] {
        aReplay.Pause();
        aReplay.ShowOneTick(aReplay.GetPlayIdx() + 1);
    });

    // 播放上一帧
    AConnect('A', [] {
        aReplay.Pause();
        aReplay.ShowOneTick(aReplay.GetPlayIdx() - 1);
    });
}

void AScript()
{
    logger.SetLevel({ALogLevel::DEBUG, ALogLevel::ERROR, ALogLevel::WARNING});
    ASetInternalLogger(logger);
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);
    auto typeList = AGetZombieTypeList();
    if (typeList[AHY_32] || typeList[ABY_23]) {
        if (typeList[AQQ_16]) {
            ASelectCards({AHBG_14, AM_HBG_14, ANGT_30, AKFD_35, ASYC_27, AXPG_8, AYGG_9, AHY_16, AYYG_42, ADPG_10}, 0);
        } else {
            ASelectCards({AHBG_14, AM_HBG_14, ANGT_30, AKFD_35, AHP_33, AXPG_8, AYGG_9, AHY_16, AYYG_42, ADPG_10}, 0);
        }
        SlowLevel();
    } else {
        if (typeList[AQQ_16]) {
            ASelectCards({AHBG_14, AM_HBG_14, ANGT_30, AKFD_35, AHMG_15, AHBLJ_20, AYTZD_2, AHY_16, ASYC_27, ADPG_10}, 0);
        } else {
            ASelectCards({AHBG_14, AM_HBG_14, ANGT_30, AKFD_35, AHMG_15, AHBLJ_20, AYTZD_2, AHY_16, AYYG_42, ADPG_10}, 0);
        }
        FastLevel();
    }

    // 这个在调试阶段使用
    // 观察破阵原因需要使用跳帧加回放功能
    // Replay();
    // ASkipTick([] {
    //     auto ptrs = AGetPlantPtrs({{1, 2}, {6, 2}}, ABXGTS_44);
    //     for (auto ptr : ptrs) {
    //         if (ptr == nullptr) {
    //             logger.Warning("寄");
    //             AMsgBox::Show("寄");
    //             return false;
    //         }
    //     }
    //     return true;
    // });

    // 这个在挂机阶段使用
    // 当跳帧检测到破阵时，启用 SL
    ASkipTick([] {
        auto ptrs = AGetPlantPtrs({{1, 2}, {6, 2}}, ABXGTS_44);
        for (auto ptr : ptrs) {
            if (ptr == nullptr) {
                SaveLoad();
                return false;
            }
        }
        return true;
    });

    ngtFixer.Start(ANGT_30, {}, 4000 / 2);

    // 减压操作
    // 这个的优先级不需要那么高
    tickRunner.Start([] {
        Observe();
        if (isBoxExplode) {
            return;
        }
        std::vector<AGrid> grids = {{1, 1}, {2, 1}, {5, 1}, {6, 1}, {2, 2}, {2, 3}, {2, 4}, {3, 6}, {3, 7}, {4, 6}, {4, 7}, {2, 4}, {5, 4}, {1, 3}, {6, 3}};
        if (!isNeedFire) {
            if (AGetPlantPtr(1, 4, ADPG_10)) {
                grids.push_back({1, 4});
            }
            if (AGetPlantPtr(6, 4, ADPG_10)) {
                grids.push_back({6, 4});
            }
        }
        ngtFixer.SetList(grids);
        if (isNeedFire) {
            Fire();
        }
        PutFodder();
        PutSyc();
    },
        ATickRunner::ONLY_FIGHT, 1);

    // 这个的修补优先级必须提高
    yygFixer.Start([] {
        FixYyg();
    },
        ATickRunner::ONLY_FIGHT, -1);

    for (auto wave : {10, 20}) {
        AConnect(ATime(wave, 395 - 298), [] {
            aIceFiller.Coffee();
            aIceFiller.SetList({{3, 5}});
            yygFixer.Start([] {
                FixYyg();
            },
                ATickRunner::ONLY_FIGHT, -1);
        });

        // 这里是为了保证咖啡豆能永远用来防止小偷
        AConnect(ATime(wave, 395 - 298 - 751), [] {
            dpgFixer.Stop();
            yygFixer.Stop();
        });
    }

    for (auto wave : {9, 19, 20}) {
        AConnect(ATime(wave, 400), [] {
            dpgFixer.Start(ADPG_10, {{1, 3}, {6, 3}, {1, 4}, {2, 4}, {5, 4}, {6, 4}}, 0);
            dpgFixer.SetIsUseCoffee(true);
        });
    }
}