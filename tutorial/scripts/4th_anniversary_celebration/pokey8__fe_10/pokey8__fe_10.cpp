#include "judge.h"
#include <avz.h>
#include <unordered_map>
#include <vector>

AOnAfterInject(AEnterGame());

bool yzbhsIsFodder = true;
ALogger<AConsole> logger;

// 判断是否是快速波
bool IsFast()
{
    bool isFast = true;
    auto zombieTypeList = AGetZombieTypeList();
    bool isHasHongYan = zombieTypeList[AHY_32];
    bool isHasBaiYan = zombieTypeList[ABY_23];
    if (isHasBaiYan) {
        isFast = false;
    } else if (isHasHongYan) {
        auto zombieList = AGetMainObject()->ZombieList();
        int wave = ANowTime().wave;

        // 统计红眼数量
        int cnt = 0;
        for (int i = (wave - 1) * 50; i < wave * 50; i++) {
            if (zombieList[i] == AHY_32) {
                cnt++;
            }
        }

        // 大于一个则为慢速波
        if (cnt > 1) {
            isFast = false;
        }
    }
    return isFast;
}

// 一大堆垫材逻辑
void PlantFodder()
{
    std::vector<std::vector<char>> isHammer(9, {false, false, false, false, false, false});
    std::vector<std::vector<char>> hasGigas(9, {false, false, false, false, false, false});
    std::vector<std::vector<char>> hasFootballs(9, {false, false, false, false, false, false});
    std::vector<std::vector<char>> hasXiaoChous(9, {false, false, false, false, false, false});
    std::vector<std::vector<char>> hasPoles(9, {false, false, false, false, false, false});
    std::vector<std::vector<char>> hasTizi(9, {false, false, false, false, false, false});
    bool hasQq = false;

    // 先统计一下僵尸
    for (auto&& zombie : aAliveZombieFilter) {
        int type = zombie.Type();
        int x = zombie.Abscissa();
        int row = zombie.Row();
        const int GigaHammerCol0 = 523 - 80 * 5;
        const int XiaoChouHitCol0 = 368 + 15 - 80 * 4;
        for (int col = 0; col < 9; col++) {
            if ((type == AHY_32 || type == ABY_23) && (GigaHammerCol0 + (col - 1) * 80 < x && x < GigaHammerCol0 + col * 80)) {
                hasGigas[col][row] = true;
                isHammer[col][row] = isHammer[col][row] || Judge::IsGigaHammer(&zombie, AXPG_8, {row, 5});
            } else if (type == AQQ_16 && x < 80) {
                hasQq = true;
            } else {
                // hasFootballs[col][row] = hasFootballs[col][row] || (type == AGL_7 && (FootballHitCol0 + (col - 1) * 80 < x && x < FootballHitCol0 + col * 80));
                hasXiaoChous[col][row] = hasXiaoChous[col][row] || (type == AXC_15 && (XiaoChouHitCol0 + (col - 1) * 80 < x && x < XiaoChouHitCol0 + col * 80));
                hasTizi[col][row] = hasTizi[col][row] || (type == AFT_21 && (XiaoChouHitCol0 + (col - 1) * 80 < x && x < XiaoChouHitCol0 + col * 80));
                // hasPoles[col][row] = hasPoles[col][row] || (type == ACG_3 && x < PoleHitCol0 + col * 80 && x > PoleHitCol0 + col * 80 - 80);
            }
        }
    }

    // 种草
    if (hasQq && AIsSeedUsable(ASYC_27)) {
        ACard(ASYC_27, {{1, 1}});
    }

    // 查看是否能种植大喷菇
    // if (AIsSeedUsable(ADPG_10)) {
    //     std::vector<AGrid> dpgGrids = {{1, 6}, {6, 6}, {1, 5}, {6, 5}};
    //     auto dpgPtrs = AGetPlantPtrs(dpgGrids, ADPG_10);
    //     for (std::size_t i = 0; i < dpgGrids.size(); ++i) {
    //         if (dpgPtrs[i] != nullptr) {
    //             continue;
    //         }
    //         if (hasPoles[dpgGrids[i].col - 1][dpgGrids[i].row - 1]) {
    //             continue;
    //         }
    //         if (AAsm::GetPlantRejectType(ADPG_10, dpgGrids[i].row - 1, dpgGrids[i].col - 1) == AAsm::NIL) {
    //             ACard(ADPG_10, dpgGrids[i].row, dpgGrids[i].col);
    //             break;
    //         }
    //     }
    // }

    // 垫小丑
    for (auto&& [row, col] : std::vector<AGrid> {{2, 7}, {5, 7}}) {
        if (!hasXiaoChous[col - 1][row - 1]) {
            continue;
        }
        for (auto&& fodderType : {AXPG_8, AM_XPG_8, AYGG_9, AHP_33, ADXG_13, AXRK_1, ADS_36, ADPG_10, ASYC_27}) {

            if (!AIsSeedUsable(fodderType)) {
                continue;
            }
            if (AAsm::GetPlantRejectType(fodderType, row - 1, col - 1) != AAsm::NIL) {
                continue;
            }
            ACard(fodderType, row, col);
            break;
        }
    }

    // 垫梯子
    for (auto&& [row, col] : std::vector<AGrid> {{2, 7}, {5, 7}}) {
        if (!hasTizi[col - 1][row - 1]) {
            continue;
        }
        for (auto&& fodderType : {AXPG_8, AM_XPG_8, AYGG_9, AHP_33, ADXG_13, AXRK_1, ADS_36, ADPG_10, ASYC_27}) {

            if (!AIsSeedUsable(fodderType)) {
                continue;
            }
            if (AAsm::GetPlantRejectType(fodderType, row - 1, col - 1) != AAsm::NIL) {
                continue;
            }
            ACard(fodderType, row, col);
            break;
        }
    }

    // 能不能种植垫材
    std::vector<AGrid> fodderGrids = {{2, 7}, {5, 7}, {1, 7}, {6, 7}};
    for (auto&& [row, col] : fodderGrids) {
        if (isHammer[col - 1][row - 1]) {
            continue;
        }
        if (!hasGigas[col - 1][row - 1] && !hasFootballs[col - 1][row - 1]) {
            continue;
        }
        if (AAsm::GetPlantRejectType(AXPG_8, row - 1, col - 1) != AAsm::NIL
            || AGetPlantPtr(row, col, AHP_33) != nullptr) {
            break;
        }
        for (auto&& fodderType : {AXPG_8, AM_XPG_8, AYGG_9, AHP_33, ADXG_13, AXRK_1, ADS_36, ADPG_10, ASYC_27}) {
            if (!AIsSeedUsable(fodderType)) {
                continue;
            }
            ACard(fodderType, row, col);
            break;
        }
    }

    // 常驻垫才提前种好
    // 如果非常驻垫材都CD了，那就别浪费CD去垫还没威胁的路
    if (AIsSeedUsable(AYGG_9)) {
        for (auto&& [row, col] : fodderGrids) {
            for (auto&& fodderType : {AXPG_8, AM_XPG_8}) {
                if (!AIsSeedUsable(fodderType)) {
                    continue;
                }
                if (AAsm::GetPlantRejectType(fodderType, row - 1, col - 1) != AAsm::NIL
                    || AGetPlantPtr(row, col, AHP_33) != nullptr) {
                    continue;
                }
                ACard(fodderType, row, col);
                break;
            }
        }
    }

    // // 冰海豚减压
    // if (!ARangeIn(nowTime.wave, {6, 7, 8, 9, 16, 17, 18, 19})) {
    //     if (nowTime.time == 530 - 100) {
    //         if (AIsZombieExist(AHT_14) && !AIsZombieExist(ABJ_20) && !AIsSeedUsable(ANGT_30) && AIsSeedUsable(AHBG_14)) {
    //                 ACard(AHBG_14, 2, 1);
    //         }
    //     }
    // }

    ATime nowTime = ANowTime();
    float paoCol = 8.5;

    if (ARangeIn(nowTime.wave, {9, 19, 20})) {
        int start = 3475 / 5 + 1 - 200 - 373;
        int cd = 3475 / 5;
        for (int t = start + cd; t < 4500; t += cd) {
            if (nowTime.time == t) {
                if (AIsZombieExist()) {
                    aCobManager.RecoverFire({{2, paoCol}, {5, paoCol}});
                }
            }
        }
    }

    // 补白南瓜逻辑
    int safeBaiNGTTime = 761 - 320;

    if (nowTime.time < safeBaiNGTTime) {
        std::vector<AGrid> fodderGrids = {{3, 7}, {4, 7}};
        for (auto&& [row, col] : fodderGrids) {
            if (AIsSeedUsable(AM_NGT_30) && (AAsm::GetPlantRejectType(AM_NGT_30, row - 1, col - 1) == AAsm::NIL)) {
                if (!AIsZombieExist(AHT_14, row)) {
                    ACard(AM_NGT_30, row, col);
                }
            }
        }
    }
}

ATickRunner tickRunner;
APlantFixer ngtFixer;
APlantFixer gjgFixer;
APlantFixer hyFixer;

bool isNeedSL = false;
int usedSLCnt = 20;
int flagCnt = 2020;

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

void Replay()
{
    aReplay.SetSaveDirPath("E:\\game\\pvzRep");
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

constexpr auto GAME_DAT_PATH = "C:\\ProgramData\\PopCap Games\\PlantsVsZombies\\userdata\\game4_13.dat";
constexpr auto TMP_DAT_PATH = "tmp.dat";

// 在进入战斗界面之前复制游戏的存档
AOnBeforeScript({
    std::filesystem::copy(GAME_DAT_PATH, TMP_DAT_PATH + std::to_string(flagCnt % 10),
        std::filesystem::copy_options::overwrite_existing);
});

AOnExitFight({
    if (isNeedSL) {
        // 直接倒退回前 8f 的存档 SL
        // 防止死亡循环
        std::filesystem::copy(TMP_DAT_PATH + std::to_string(flagCnt % 10),
            GAME_DAT_PATH, std::filesystem::copy_options::overwrite_existing);
        flagCnt -= 10;
    }
    isNeedSL = false;
});

// 增加容错率的 SL 大法
void SaveLoad()
{
    // 这两个函数实现了一个组合拳
    // 但是这两个函数都不是立即执行的函数
    // 它相当于是给 AvZ 下达了一个命令
    // AvZ 会在合适的时刻点运行这些命令
    ++usedSLCnt;
    ABackToMain();
    AEnterGame();
    isNeedSL = true;
}

void AScript()
{

    logger.SetLevel({ALogLevel::DEBUG, ALogLevel::ERROR, ALogLevel::WARNING});
    ASetInternalLogger(logger);
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);

    ASelectCards(
        {
            ANGT_30,
            // AM_NGT_30,
            ASYC_27,
            AHBG_14,
            AXPG_8,
            AM_XPG_8,
            AYGG_9,
            AHP_33,
            ADXG_13,
            AXRK_1,
            ADS_36,
        },
        0);

    // // 观察破阵原因需要使用跳帧加回放功能
    // Replay();
    // ASkipTick([] {
    //     auto ptrs = AGetPlantPtrs({{1, 4}, {2, 4}, {5, 4}, {6, 4}}, ABXGTS_44);
    //     for (auto ptr : ptrs) {
    //         if (ptr == nullptr) {
    //             logger.Warning("寄");
    //             AMsgBox::Show("寄");
    //             return false;
    //         }
    //     }
    //     return true;
    // });

    // 当跳帧检测到破阵时，启用 SL
    ASkipTick([] {
        auto ptrs = AGetPlantPtrs({{1, 4}, {2, 4}, {5, 4}, {6, 4}}, ABXGTS_44);
        for (auto ptr : ptrs) {
            if (ptr == nullptr) {
                if (usedSLCnt > flagCnt / 100 + 1) {
                    logger.Warning("这次是真的寄了");
                    AMsgBox::Show("寄");
                } else {
                    logger.Warning("虽然寄了，但是我们有 SL 大法！");
                    SaveLoad();
                }

                return false;
            }
        }
        return true;
    });

    float paoCol = 8.5;

    AMaidCheats::Dancing();

    AConnect(ATime(20, 3475 / 5 + 1 - 200), [] {
        AMaidCheats::Stop();
    });

    tickRunner.Start(PlantFodder);
    ngtFixer.Start(ANGT_30, {}, 4000 / 3 * 2);
    // gjgFixer.Start(AGJG_23, {{3, 5}}, 150);
    // hyFixer.Start(AHY_16, {{3, 5}});

    AConnect(ATime(20, -101), [] {
        ACard(AHBG_14, 2, 1);
    });

    for (int wave = 1; wave < 21; ++wave) {
        AConnect(ATime(wave, 3475 / 5 + 1 - 200 - 373), [paoCol] {
            aCobManager.RecoverFire({{2, paoCol}, {5, paoCol}});
        });

        // if (ARangeIn(wave, {10, 20})) {
        //     AConnect(ATime(wave, bengjiIceTime), [wave] {
        //         if (AIsZombieExist(ABJ_20)) {
        //             ACard(AHBG_14, 2, 1);
        //         }
        //     });
        // }
    }

    // 种伞逻辑，已弃用
    // for (auto wave : {9, 19}) {
    //     AConnect(ATime(wave, 3475 / 2 + 1 - 200 - 751), []() -> ACoroutine {
    //         yzbhsIsFodder = false;
    //         co_await [] { return AIsSeedUsable(AYZBHS_37); };
    //         ARemovePlant(2, 5);
    //         ACard(AYZBHS_37, 2, 5);
    //         co_await [] { return AIsSeedUsable(AYZBHS_37); };
    //         ARemovePlant(5, 5);
    //         ACard(AYZBHS_37, 5, 5);
    //     });
    // }
    // for (auto wave : {10, 20}) {
    //     AConnect(ATime(wave, 396), [] {
    //         yzbhsIsFodder = true;
    //         ARemovePlant(2, 5, {{AYZBHS_37, AYZBHS_37}});
    //         ARemovePlant(5, 5, {{AYZBHS_37, AYZBHS_37}});
    //     });
    // }

    // tickRunner.Start([] {
    //     for (auto&& zombie : aAliveZombieFilter) {
    //         logger.Info("#", zombie.Abscissa());
    //     }
    // });
};