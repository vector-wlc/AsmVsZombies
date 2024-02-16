
// AvZ版本：20240113

// 更新时间：20240131

/*

简介：

天台十炮

简单的P5，波长700。

天台场景不方便（懒得写）垫红眼。
9 20波收尾的时候用冰。
10波的红眼好像也比较多，也冰一下；若延迟过多就用樱桃炸一下促进刷新。
19波红眼少，就不冰了（想冰也不够冰用了，要留一个冰来消空降）

小鬼会啃炮，但一般不会直接啃穿，可以在适当的时候进行修补。

每关的前几波压力小的时候，在8列种花补充阳光，顺便当垫材。

阵代码：
4,2F 1 1 0 1 0,21 1 1 0 0 0,21 1 2 0 0 0,2C 1 3 0 0 0,21 1 3 0 0 0,1E 1 3 0 0 0,30 1 3 0 0 0,2C 1 4 0 0 0,21 1 4 0 0 0,1E 1 4 0 1 0,30 1 4 0 0 0,2C 1 5 0 0 0,21 1 5 0 0 0,1E 1 5 0 1 0,30 1 5 0 0 0,2F 1 6 0 1 0,21 1 6 0 0 0,21 1 7 0 0 0,2F 2 1 0 2 0,21 2 1 0 0 0,21 2 2 0 0 0,2A 2 3 1 0 0,21 2 3 0 0 0,1E 2 3 0 2 0,30 2 3 0 0 0,25 2 4 0 1 0,21 2 4 0 0 0,1E 2 4 0 2 0,30 2 4 0 0 0,2C 2 5 0 0 0,21 2 5 0 0 0,1E 2 5 0 1 0,30 2 5 0 0 0,2F 2 6 0 2 0,21 2 6 0 0 0,21 2 7 0 0 0,2F 3 1 0 0 0,21 3 1 0 0 0,21 3 2 0 0 0,2C 3 3 0 0 0,21 3 3 0 0 0,1E 3 3 0 1 0,30 3 3 0 0 0,2C 3 4 0 2 0,21 3 4 0 0 0,1E 3 4 0 2 0,30 3 4 0 0 0,2C 3 5 0 0 0,21 3 5 0 0 0,1E 3 5 0 0 0,30 3 5 0 0 0,2F 3 6 0 0 0,21 3 6 0 0 0,21 3 7 0 0 0,2F 4 1 0 0 0,21 4 1 0 0 0,21 4 2 0 0 0,2A 4 3 1 2 0,21 4 3 0 0 0,1E 4 3 0 2 0,30 4 3 0 0 0,25 4 4 0 0 0,21 4 4 0 0 0,1E 4 4 0 0 0,30 4 4 0 0 0,2C 4 5 0 2 0,21 4 5 0 0 0,1E 4 5 0 1 0,30 4 5 0 0 0,2F 4 6 0 1 0,21 4 6 0 0 0,21 4 7 0 0 0,2F 5 1 0 2 0,21 5 1 0 0 0,21 5 2 0 0 0,2C 5 3 0 2 0,21 5 3 0 0 0,1E 5 3 0 1 0,30 5 3 0 0 0,2C 5 4 0 2 0,21 5 4 0 0 0,1E 5 4 0 2 0,30 5 4 0 0 0,2C 5 5 0 2 0,21 5 5 0 0 0,1E 5 5 0 2 0,30 5 5 0 0 0,2F 5 6 0 2 0,21 5 6 0 0 0,21 5 7 0 0 0

-----------

我上一个作品是用稍微改动过的经典十二炮挂机（https://www.bilibili.com/video/BV1FS4y1b7eh/），那个实现起来比较简单，我就有点飘了，立马去挑战天台十炮。

试过带白花盆垫红眼，写了一堆屎山代码，效果还是不好；
企图避免1列炮被啃，在3列种了一排曾。这样1列炮是安全了，但减少冰瓜会降低对前场僵尸的伤害，6列炮更危险了；
6列炮也是小概率被小鬼啃，我也尝试过调整波长和炮落点，让巨人走远点再扔小鬼。现在看来是增加了6列炮直接被砸的风险，属于是捡芝麻丢西瓜了；

被坑惨的小菜鸡直接选择放弃。
但没想到过了一年多，还有把这坑填上的一天。
AvZ 的新功能提供了很大的帮助，向量的经典二炮脚本也给了我很大的启发，以 SL 兜底，不再执着于一些极小概率的情况，更能享受游戏的乐趣。

*/

#include <avz.h>

constexpr int TARGET_FLAG = 240000; // 目标选卡数

// 卡片顺序
constexpr int _FLOWER_POT = 1;
constexpr int _COFFEE_BEAN = 2;
constexpr int _ICE_SHROOM = 3;
constexpr int _M_ICE_SHROOM = 4;
// constexpr int _DOOM_SHROOM = 5;
// constexpr int _BLOVER = 6;
constexpr int _SUNFLOWER = 7;
constexpr int _KERNEL_PULT = 8;
constexpr int _COB_CANNON = 9;
constexpr int _CHERRY_BOMB = 10;

/// 灰烬生效时间
constexpr int FIRE_TIME_BOOM = 500;

// 屋顶场地落点位于前场（7~9列）时 1~7列玉米加农炮的飞行时间分别为 359 362 364 367 369 372 373
constexpr int COB_FLY_TIME_1 = 359;
constexpr int COB_FLY_TIME_6 = 372;
constexpr int FIRE_TIME_COB_COL_1 = FIRE_TIME_BOOM - COB_FLY_TIME_1;
constexpr int FIRE_TIME_COB_COL_6 = FIRE_TIME_BOOM - COB_FLY_TIME_6;

// 炮落点
constexpr float COB_FALL_POINT_1 = 9;
constexpr float COB_FALL_POINT_6 = 9;

/// 受损的炮，恢复cd在合适的区间时，铲掉重新种
// 炮种下后625可用
// 751玉米投手冷却
// 319/320模仿者生效（可能需要两个花盆，所以x2）
constexpr int FIX_NEED_TIME = 625 + 751 + 320 * 2;

// 记录已完成轮数
long int flagCnt = 0;
// 判断是否已达成目标轮数
bool achieveGoal = false;

int usedSLCnt = 0;

/// 修炮
// 要修的炮的位置
int cobFixingRow = -1;
int cobFixingCol = -1;

// 1列 6列炮
ACobManager cobs1;
ACobManager cobs6;

// 每帧运行
ATickRunner tickRunner;

// 在控制台显示日志
ALogger<AFile> logger("Re10_logger.txt");

// 控制跳帧停止，方便查看破阵原因
bool stopTickSkip = false;

// 获取6列炮列表
void GetCobListInCol6(std::vector<AGrid>& cobList)
{
    for (auto&& alivePlant : aAlivePlantFilter) {
        if ((ACOB_CANNON == alivePlant.Type()) && ((6 - 1) == alivePlant.Col())) {
            cobList.push_back({(alivePlant.Row() + 1), 6});
            if (5 == cobList.size()) {
                break; // 最多就5门炮了，跳出循环
            }
        }
    }
}

// 6列炮可能会丢，使用前需要重新获取炮列表
void UseCobs6()
{
    std::vector<AGrid> cobList;
    GetCobListInCol6(cobList);

    cobs6.SetList(cobList);
    cobs6.Fire(4, COB_FALL_POINT_6);
}

// 得到炮的恢复时间（抄AvZ源码的
int GetCobRecoverTime(APlant* cob)
{
    auto animationMemory = AGetPvzBase()->AnimationMain()->AnimationOffset()->AnimationArray() + cob->AnimationCode();

    switch (cob->State()) {
    case 35:
        return 125 + cob->StateCountdown();
    case 36:
        return int(125 * (1 - animationMemory->CirculationRate()) + 0.5) + 1;
    case 37:
        return 0;
    case 38:
        return 3125 + int(350 * (1 - animationMemory->CirculationRate()) + 0.5);
    default:
        return ACobManager::NO_EXIST_RECOVER_TIME;
    }
}

// 修补6列炮
void CobFixer()
{
    // 补炮
    if (cobFixingRow != -1) {
        if (ACard(_COB_CANNON, cobFixingRow, cobFixingCol) != nullptr) {
            logger.Warning("补炮成功 [#, #]", cobFixingRow, cobFixingCol);
            cobFixingRow = -1;
            return;
        }

        // 红眼会把炮和后轮的花盆砸扁，前轮的花盆幸存
        // 如果正好砸到这一轮要发射的炮，需要用樱桃来炸，
        // 所以先种前轮的玉米投手，避免花盆CD不够导致崩盘
        if (!AIsSeedUsable(AYMTS_34))
            return;
        ACard(_FLOWER_POT, cobFixingRow, cobFixingCol + 1);
        ACard(_KERNEL_PULT, cobFixingRow, cobFixingCol + 1);
        ACard(_FLOWER_POT, cobFixingRow, cobFixingCol);
        ACard(_KERNEL_PULT, cobFixingRow, cobFixingCol);

        return;
    }

    // 检测有没有炮丢失
    int aliveCobs1[5] = {0, 0, 0, 0, 0};
    int aliveCobs6[5] = {0, 0, 0, 0, 0};
    for (auto&& alivePlant : aAlivePlantFilter) {
        if ((ACOB_CANNON == alivePlant.Type())) {
            // 炮卡片够用，及时铲掉不满血的炮（可能被小鬼啃了），重新种
            auto seed = AGetMainObject()->SeedArray() + _COB_CANNON - 1;
            if ((seed->InitialCd() - seed->Cd() + 1) < 750 && alivePlant.Hp() < 300) {
                int cd = AGetCobRecoverTime(&alivePlant);
                // 炮弹的cd大于铲了再种所需的时间 且 炮弹已分离（205）
                if (cd > FIX_NEED_TIME && cd < (3475 - 205 - 1)) {
                    AShovel(alivePlant.Row() + 1, alivePlant.Col() + 1);
                    cobFixingRow = alivePlant.Row() + 1;
                    cobFixingCol = alivePlant.Col() + 1;
                    logger.Warning("修补炮 [#, #]", cobFixingRow, cobFixingCol);
                    return;
                }
            }
            if ((6 - 1) == alivePlant.Col()) {
                aliveCobs6[alivePlant.Row()] = 1;
            } else {
                aliveCobs1[alivePlant.Row()] = 1;
            }
        }
    }
    for (int i = 0; i < 5; ++i) {
        if (aliveCobs1[i] == 0) {
            cobFixingRow = i + 1;
            cobFixingCol = 1;
            // 停止跳帧
            // stopTickSkip = true;
            break;
        }
        if (aliveCobs6[i] == 0) {
            cobFixingRow = i + 1;
            cobFixingCol = 6;
            break;
        }
    }
}

// 录像回放（抄的
void Replay()
{
    // 这里需要改成自己的磁盘路径
    aReplay.SetSaveDirPath("C:\\avz_replay");
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

// 此函数会在游戏进入战斗界面后立即运行
AOnEnterFight({
    stopTickSkip = false;

    int slCnt = flagCnt / 100 + 1;
    logger.Warning("阳光: #,  flag: #,  SL 可用次数: #", AGetMainObject()->Sun(), flagCnt, slCnt - usedSLCnt);
    flagCnt += 2;

    // 目标达成后跳回主界面
    if (flagCnt > TARGET_FLAG) {
        ABackToMain();
        AMsgBox::Show("完成");
        flagCnt = 0;
        achieveGoal = true;
        logger.Warning("总SL次数 #", usedSLCnt);
    }
});

constexpr auto GAME_DAT_PATH = "C:\\ProgramData\\PopCap Games\\PlantsVsZombies\\userdata\\game1_15.dat";
constexpr auto TMP_DAT_PATH = "C:\\AvZ_SL\\tmp.dat";

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
    // SURVIVAL_ENDLESS_STAGE_1 白天
    // SURVIVAL_ENDLESS_STAGE_2 ？
    // SURVIVAL_ENDLESS_STAGE_3 白天泳池
    // SURVIVAL_ENDLESS_STAGE_4 夜晚泳池
    // SURVIVAL_ENDLESS_STAGE_5 白天天台
    AEnterGame(AAsm::SURVIVAL_ENDLESS_STAGE_5);
}

AOnExitFight({
    // 冲关完成
    if (achieveGoal)
        return;
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
        // flagCnt = std::clamp(flagCnt - 10, 0, INT_MAX);
        if (flagCnt >= 10) {
            flagCnt -= 10;
        } else {
            flagCnt = 0;
        }
    }
});

void AScript()
{
    // 多次生效
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);

    // 设置输出的日志类型
    logger.SetLevel({ALogLevel::WARNING}); // 只弹WARNING
    // 设置内置日志
    ASetInternalLogger(logger);

    // 帧运行操作
    tickRunner.Start(CobFixer);

    // 瞬时选卡
    ASelectCards({
                     AFLOWER_POT,   // 花盆
                     ACOFFEE_BEAN,  // 咖啡豆
                     AICE_SHROOM,   // 冰
                     AM_ICE_SHROOM, // 白冰
                     ADOOM_SHROOM,  // 核
                     ABLOVER,       // 三叶草
                     ASUNFLOWER,    // 花
                     AKERNEL_PULT,  // 玉米投手
                     ACOB_CANNON,   // 炮
                     ACHERRY_BOMB,  // 樱桃
                 },
        0);

    // 设置倍速
    // ASetGameSpeed(2);

    // 回放
    // Replay();

    // 跳帧
    ASkipTick([] {
        auto ptrs = AGetPlantPtrs({{1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}}, ABXGTS_44);
        for (auto ptr : ptrs) {
            if (ptr == nullptr) {
                return false;
            }
        }
        // return !stopTickSkip;// 调试用
        return true;
    }
        // , [] {
        //     AMsgBox::Show("被动丢炮了，跳帧停止");// 调试用
        // }
    );

    AConnect(ATime(1, -599), []() -> ACoroutine {
        // 设置炮列表
        cobs1.SetList({{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}});

        // 种花补充阳光
        ACard(_FLOWER_POT, 3, 8);
        ACard(_SUNFLOWER, 3, 8);
        co_await ANowDelayTime(751);
        ACard(_FLOWER_POT, 2, 8);
        ACard(_SUNFLOWER, 2, 8);
        co_await ANowDelayTime(751);
        ACard(_FLOWER_POT, 4, 8);
        ACard(_SUNFLOWER, 4, 8);
        co_await ANowDelayTime(751);
        ACard(_FLOWER_POT, 1, 8);
        ACard(_SUNFLOWER, 1, 8);
        co_await ANowDelayTime(751);
        ACard(_FLOWER_POT, 5, 8);
        ACard(_SUNFLOWER, 5, 8);
    });

    // P5
    // 主体节奏
    for (auto wave : {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19}) {
        // 发炮
        AConnect(ATime(wave, FIRE_TIME_COB_COL_1), [=] { cobs1.Fire(2, COB_FALL_POINT_1); });
        AConnect(ATime(wave, FIRE_TIME_COB_COL_6), [=] { UseCobs6(); });
        // 若6列炮在炮弹分离出去之前被干掉了，就用樱桃代替
        // 炮弹分离 205
        AConnect(ATime(wave, FIRE_TIME_COB_COL_6 + 205 + 1), [=]() -> ACoroutine {
            for (auto&& alivePlant : aAlivePlantFilter) {
                if (ACOB_CANNON == alivePlant.Type() && ((6 - 1) == alivePlant.Col()) && 38 == alivePlant.State()) {
                    co_return;
                }
            }
            co_await ATime(wave, 546 - 100); // 546收投篮
            ACard(_FLOWER_POT, 4, 9);
            ACard(_CHERRY_BOMB, 4, 9);
            logger.Warning("6列炮发射失败，使用樱桃");
            co_await ANowDelayTime(101);
        });
    }

    // 旗帜波
    for (auto wave : {10, 20}) {
        AConnect(ATime(wave, 520 - COB_FLY_TIME_1), [=] { cobs1.Fire(2, COB_FALL_POINT_1); });
        AConnect(ATime(wave, 520 - COB_FLY_TIME_6), [=] { UseCobs6(); });
    }

    // wave 9 19 20 收尾炮
    for (auto wave : {9, 19, 20}) {
        for (auto PaoTime : {500, 1200, 1900}) {
            AConnect(ATime(wave, FIRE_TIME_BOOM + PaoTime), [=] {
                if (ANowWave() == wave && AGetMainObject()->RefreshCountdown() > 200) {
                    for (auto&& zombie : aAliveZombieFilter) {
                        cobs1.Fire(2, COB_FALL_POINT_1);
                        UseCobs6();
                        break;
                    }
                }
            });
        }
    }

    // wave 9、10 红眼关冰一下 防砸炮
    if (AGetZombieTypeList()[AHY_32]) {
        AConnect(ATime(9, FIRE_TIME_BOOM), [] {
            ACard(_FLOWER_POT, 3, 8);
            ACard(_ICE_SHROOM, 3, 8);
            ACard(_COFFEE_BEAN, 3, 8);
        });
        AConnect(ATime(10, FIRE_TIME_BOOM - 320 - 199 - 1), []() -> ACoroutine {
            ACard(_FLOWER_POT, 3, 8);
            ACard(_M_ICE_SHROOM, 3, 8);
            co_await ANowDelayTime(320);
            ACard(_COFFEE_BEAN, 3, 8);

            co_await ANowDelayTime(500);
            // 再用樱桃炸一下，促进刷新
            if (ANowWave() == 10 && AGetMainObject()->RefreshCountdown() > 200) {
                ACard(_FLOWER_POT, 2, 9);
                ACard(_CHERRY_BOMB, 2, 9);
            }
        });

        // 铲花防止干扰冰
        for (int wave : {9, 19}) {
            AConnect(ATime(wave, -200), [] {
                for (auto&& alivePlant : aAlivePlantFilter) {
                    if (alivePlant.Type() == ASUNFLOWER && alivePlant.Row() == 3 - 1 && alivePlant.Col() == 8 - 1) {
                        AShovel(3, 8);
                    }
                }
            });
        }
    }

    // wave 20 冰消空降
    // 319/320模仿者生效
    // 咖啡豆种下到完成唤醒198/199cs
    // 冰生效时间100cs 冻结时间300cs
    // 空降200cs出现
    AConnect(ATime(20, 0 - 320 - 199 - 100 - 1), []() -> ACoroutine {
        // 铲花
        for (auto&& alivePlant : aAlivePlantFilter) {
            if (alivePlant.Type() == ASUNFLOWER && alivePlant.Row() == 3 - 1 && alivePlant.Col() == 8 - 1) {
                AShovel(3, 8);
            }
        }
        ACard(_FLOWER_POT, 3, 8);
        ACard(_M_ICE_SHROOM, 3, 8);
        co_await ANowDelayTime(320);
        ACard(_COFFEE_BEAN, 3, 8);
    });
    // 再冰一下20波的僵尸，防止砸炮
    // 994 巨人命中8列普通植物
    AConnect(ATime(20, 994 - 199), [=] {
        ACard(_FLOWER_POT, 3, 8);
        ACard(_ICE_SHROOM, 3, 8);
        ACard(_COFFEE_BEAN, 3, 8);
    });
}
