/*
    这里是shallow一dream，注意中间那个东西不是短杠，而是一（yi），嗯没错就是短杠的那个名字已经被注册了（
    B站ID：shallow一dream
    抖音ID：Fighting-hawk
    一个是浅梦，一个是斗鹰哈哈哈
    接下来说一下脚本思路

    阵型是不经典16炮，陆路每路一个冰瓜，保证若有若无的减速以及显著降低炮损，原阵经典十六要是也这么打P6打一个慢速关可能底线炮都要被啃掉了
    发炮采用332332的数量排布，尽可能不遇到刷新延迟，即使是这样仍然会遇到不少刷新延迟，延迟处理采用樱桃和核弹，因为多数波都有三炮炸实，实际上只用樱桃没准也够
    必须要吹气球，不要以为P6再加上能处理刷新延迟就可以不管气球！
    水路为了防止一些离谱的铁桶还在炮前加了两个南瓜，南瓜损耗不大，靠天也补得过来，因此运阵全程没有偷花，补底线炮也是靠天
    阳光消耗除了补炮、补南瓜之外，还有冰消珊瑚、处理刷新延迟，但是靠天统统够用，天降阳光可太猛了。
    为什么要采用冰消珊瑚呢？因为有时候收尾会遇到比较离谱的情况，在w20只剩一对炮了，然后其中一门炮用来消珊瑚了，就寄，干脆直接采用冰消珊瑚了。
    发炮用了两个炮列表，底线炮和其他位置的炮各用一个，防止多个底线炮同时被啃掉的情况或者一个炮马上要CD好了但是被啃掉了需要补的情况。
    然后就是即使这样也会遇到在某一波没有炮用的情况，这时候就要用核弹刷新。
    修改收尾逻辑可能可以防止这一点。
    这个阵不怕夹零，因为每波波长都是601，也无所谓快慢变极。

    没了，这个阵已经是很简单的挂机阵了，稳定性反正10000f都不用SL，建议新手入坑优先选择的阵型。
    不要用那个经典十六炮！！！！！！
*/
#include <avz.h>
#include <cstdlib>

bool cobCannonUsable = true;
AOnEnterFight(cobCannonUsable = true);
int slNum = 0;
int flagCnt = -2;
bool isNeedSyc = false;
bool isNeedSL = false;
constexpr auto GAME_DAT_PATH = "C:\\ProgramData\\PopCap Games\\PlantsVsZombies\\userdata\\game3_13.dat";
constexpr auto TMP_DAT_PATH = "tmp.dat";

ATickRunner tickRunner;
ALogger<AFile> fileLogger("logger.txt");
AAsm saveAndLoad;
ACobManager diXianCob; // 底线炮列表
APlantFixer lilyPadFixer;
APlantFixer pumpkinFixer;
ATickRunner sl;

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

AOnEnterFight({
    isNeedSyc = false;
    flagCnt += 2;
});

// 放置三叶草
void PutBlover()
{
    bool isNeedSyc = false;
    std::vector<AZombie*> bcPtrs;
    for (auto&& zombie : aAliveZombieFilter) {
        if (zombie.Type() == AQQ_16 && zombie.Abscissa() < 80) {
            isNeedSyc = true;
        }
    }

    // 放置三叶草
    if (AIsSeedUsable(ASYC_27) && isNeedSyc) {
        ACard(ASYC_27, 4, 9);
    }
}

void DealWave()
{
    if (aCobManager.GetUsablePtr() == nullptr) {
        AConnect(ATime(ANowDelayTime(74)), [=] {
            // consoleLogger.Warning("使用核弹激活。");
            ACard(ADOOM_SHROOM, 3, 9);
            ACard(ACOFFEE_BEAN, 3, 9);
        });
    }
    aCobManager.Fire({{2, 9}, {5, 9}});
    if (AGetMainObject()->Wave() % 3 == 0) {
        AConnect(ANowDelayTime(110), [=] { diXianCob.Fire(2, 9); });
    } else if (AGetMainObject()->Wave() % 3 == 1) {
        AConnect(ANowDelayTime(110), [=] { diXianCob.Fire(5, 9); });
    }
}

void DealDelay(int wave)
{
    if (!AIsSeedUsable(ADOOM_SHROOM) && !AIsSeedUsable(ACHERRY_BOMB))
        return;
    if (AGetMainObject()->Wave() == wave) {
        if (wave != 10) {
            ACard(ADOOM_SHROOM, 3, 9);
            ACard(ACOFFEE_BEAN, 3, 9);
        } else {
            if (AGetMainObject()->Wave() % 3 == 0)
                ACard(ACHERRY_BOMB, 2, 9);
            else
                ACard(ACHERRY_BOMB, 5, 9);
        }
    }
}

// 增加容错率的 SL 大法
void SaveLoad()
{
    // 这两个函数实现了一个组合拳
    // 但是这两个函数都不是立即执行的函数
    // 它相当于是给 AvZ 下达了一个命令
    // AvZ 会在合适的时刻点运行这些命令
    ABackToMain();
    AEnterGame();
    isNeedSL = true;
}

void Replay()
{
    aReplay.SetSaveDirPath("Z:\\");
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

void normal_solve()
{
    for (auto wave : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}) {
        AConnect(ATime(wave, 336 - 373), [=] { DealWave(); });
    }

    for (auto wave : {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18}) {
        AConnect(ATime(wave, 650), [=] { DealDelay(wave); });
    }
}

void fast_solve()
{
    for (int wave = 1; wave < 21; wave++) {
        AConnect(ATime(wave, 336 - 373), [=] { aCobManager.Fire({{2, 9}, {5, 9}}); });
        AConnect(ATime(wave, 602), [=] {
            if (AGetMainObject()->Wave() == wave && wave != 10) {
                diXianCob.Fire({{2, 9}, {5, 9}});
            }
        });
    }
    AConnect(ATime(10, 500 - 373), [=] { diXianCob.Fire({{2, 9}, {5, 9}}); });
}

void FixYMJNP()
{
    if (!cobCannonUsable) {
        return;
    }

    auto fixFunc = [](const std::vector<APlantType>& seedList, const std::vector<AGrid>& gridList) {
        for (auto seedType : seedList) {
            if (!AIsSeedUsable(seedType)) {
                return;
            }
        }
        auto ptrs = AGetPlantPtrs(gridList, AYMJNP_47);
        for (int i = 0; i < ptrs.size(); ++i) {
            if (ptrs[i] == nullptr) {
                ACard(seedList, gridList[i].row, gridList[i].col);
                ACard(seedList, gridList[i].row, gridList[i].col + 1);
                AConnect(ANowDelayTime(320), [=] { ACard(ACOB_CANNON, gridList[i].row, gridList[i].col); });
                return;
            }
        }
    };

    fixFunc({AKERNEL_PULT, AM_KERNEL_PULT}, {{1, 1}, {2, 1}, {5, 1}, {6, 1}});
}

void AScript()
{
    // 设置游戏倍速
    ASetGameSpeed(10);
    // 冲关必备，连续运行代码
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);

    ASetInternalLogger(fileLogger);
    fileLogger.SetLevel({ALogLevel::DEBUG, ALogLevel::WARNING});

    // 设置出怪测试代码
    // auto typelist = ACreateRandomTypeList({}, {ABY_23});
    // ASetZombies(typelist, ASetZombieMode::INTERNAL);

    ASelectCards({AYTZD_2, AYMJNP_47, ASYC_27, AYMTS_34, AM_YMTS_34, AHY_16, AKFD_35, ANGT_30, AHBG_14, AHMG_15}, 1);

    tickRunner.Start([] {
        FixYMJNP();
        PutBlover(); });
    AConnect(ATime(1, -599), [=] {
        lilyPadFixer.Start(ALILY_PAD, {{3, 9}, {4, 9}}, 100);
        pumpkinFixer.Start(APUMPKIN, {{3, 9}, {4, 9}}, 1000);
        aCobManager.SetList({{1, 6}, {2, 6}, {3, 1}, {3, 3}, {3, 5}, {3, 7}, {4, 1}, {4, 3}, {4, 5}, {4, 7}, {5, 6}, {6, 6}});
        diXianCob.SetList({{1, 1}, {2, 1}, {5, 1}, {6, 1}});
        if (flagCnt % 100 == 0) {
            slNum = slNum + 1;
            fileLogger.Warning("过了 # f了, 现在有 # 次SL机会啦 ! ! !", flagCnt, slNum);
        } else if (flagCnt % 10 == 0)
            fileLogger.Debug("过了 # f了", flagCnt);
    });

    if (AGetZombieTypeList()[AHY_32] || AGetZombieTypeList()[ABY_23]) {
        normal_solve();
    } else {
        fast_solve();
    }

    // 自动收尾
    for (auto wave : {9, 19, 20}) {
        AConnect(ATime(wave, 564), [wave]() -> ACoroutine {
            for (int i = 0; i < 3; ++i) {
                co_await [] { return aCobManager.GetUsablePtr() != nullptr; };
                if (AGetMainObject()->Wave() != wave) {
                    co_return;
                }
                for (auto&& zombie : aAliveZombieFilter) {
                    if (zombie.Hp() > 0) {
                        aCobManager.RecoverFire({{2, 8.5}, {5, 8.5}});
                        break;
                    }
                }
                co_await (601);
            }
        });
        if (wave == 19)
            AConnect(ATime(wave, 564), [] { diXianCob.Fire(5, 8.5); });
    }

    // 冰消珊瑚
    AConnect(ATime(20, -2 - 298), [] {
        ACard(AICE_SHROOM, 1, 8);
        ACard(ACOFFEE_BEAN, 1, 8);
    });

    sl.Start([=] {
        auto ptrs = AGetPlantPtrs({{1, 6}, {2, 6}, {3, 7}, {4, 7}, {5, 6}, {6, 6}}, AYMJNP_47);
        for (auto ptr : ptrs) {
            if (ptr == nullptr) {
                if (slNum == 0) {
                } else {
                    slNum = slNum - 1;
                    SaveLoad();
                }
            }
        }
    });

    // 回放观察破阵原因
    // Replay();

    // 失败了就寄的跳帧
    // ASkipTick([] {
    //     auto ptrs = AGetPlantPtrs({{1, 6}, {2, 6}, {5, 6}, {6, 6}, {3, 7}, {4, 7}}, AYMJNP_47);
    //     for (auto ptr : ptrs){
    //         if (ptr == nullptr){
    //             AMsgBox::Show("寄！！！");
    //             return false;
    //         }
    //     }
    //     return true;
    // });

    // 失败了还能SL的跳帧
    ASkipTick([] {
        auto ptrs = AGetPlantPtrs({{1, 6}, {2, 6}, {3, 7}, {4, 7}, {5, 6}, {6, 6}}, AYMJNP_47);
        for (auto ptr : ptrs) {
            if (ptr == nullptr) {
                if (slNum == 0) {
                    fileLogger.Debug("这次是真的寄了! ! !");
                    AMsgBox::Show("寄！！！");
                } else {
                    fileLogger.Debug("现在是第 # f, SL, 启动！！！", flagCnt);
                    slNum = slNum - 1;
                    SaveLoad();
                }
                return false;
            }
        }
        if (flagCnt >= 10000) {
            return false;
        }
        return true;
    });
}