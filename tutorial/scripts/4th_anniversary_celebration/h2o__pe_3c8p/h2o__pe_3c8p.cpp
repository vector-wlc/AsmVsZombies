/*
 * @Description: 3c8p开局至冲关全程挂机脚本，运行脚本前请先确保已安装GetZombieAbscissas插件
 */

// 需要按下 Ctrl Shift P 组合键并键入 AvZ:Get AvZ Extension 后安装 GetZombieAbscissas 插件
#include "GetZombieAbscissa.h"
#include "PlantOperator.h"
#include "TickPlanterPlus.h"
#include <iomanip>

struct ZombieEnterMovement {
    int index;
    int row;
    std::vector<float> absccisas;
    int enter_time;
};

// 忧郁菇自动种植类
PlantOperator gloomShroomPlanter;
// 冰瓜自动种植类
PlantOperator winterMelonPlanter;
// 种植玉米加农炮操作类
PlantOperator cobPlanter;
// 陆路高坚果自动修补操作类
APlantFixer tallNutFixer1;
// 水路高坚果自动修补操作类
APlantFixer tallNutFixer2;
// 大蒜自动修补操作类
APlantFixer garlicFixer;
// 南瓜头自动修补操作类
APlantFixer pumpkinFixer;
// 常规操作帧运行类
ATickRunner tickRunner;

// 返回[wave]波出现的最小编号的僵尸所在的行数，若场上不存在[wave]波刷出的僵尸，返回-1
int GetZombieRefreshRow(int wave);
// 返回场上满足特定条件的植物总数
int GetPlantNum(std::function<bool(APlant*)> plants);
// 得到[row]行僵尸的最大存在时间
int GetMaxExistTime(int row);
// 发炮降低高坚果压力
void PressReductionCob();
// 在[wave]波发炮射击2行和5行消灭大部分僵尸，只留下最慢的一只领带僵尸。适用于六行场地
void SeparateLastZombie(int wave);
// 气球僵尸飞过[x]列时在[list]中寻找可以种植的格子种植三叶草
void AutoBlover(float x, std::vector<AGrid> list);
// 唤醒[list]中的蘑菇
void WakeShroom(std::vector<AGrid> list);
// 智能修补南瓜，当场上需要修补的格子附近存在僵尸时不使用模仿者种子
void PumpkinFixer(std::vector<AGrid> grid_list, int fixHp);
// 检查(row, col)的植物是否正在被僵尸啃食
bool IsBeingAten(int row, int col);
// 已完成0轮时的整轮操作
void Begin1();
// 第二次选卡时的整轮操作
void Begin2();
// 第三次选卡及以后的整轮操作
void Run();

// 第一个玉米加农炮的种植行
int first_cob_row;
// 炮对上行僵尸的作用半径
constexpr static int CANNON_LEFT_LENGTH_OF_ZOMBIE_AT_UP_ROW = 71;
// 炮对本行僵尸的作用半径
constexpr static int CANNON_LEFT_LENGTH_OF_ZOMBIE_AT_THE_ROW = 79;
// 炮对下行僵尸的作用半径
constexpr static int CANNON_LEFT_LENGTH_OF_ZOMBIE_AT_DOWN_ROW = 78;

// 游戏控制
class GameController {
protected:
    const std::vector<float> _gameSpeeds = {0.1, 0.25, 0.5, 1.0, 2.5, 5.0, 10.0};
    float _defaultSpeed = 1.0;
    char _decelerateKey = 'A';
    char _accelerateKey = 'D';
    char _resetSpeedKey = 'S';
    char _scriptStopKey = 'Q';

public:
    // 启用按键控制，默认速度倍率[speed]=1.0
    // [speed]有7个档位：{0.1, 0.25, 0.5, 1.0, 2.5, 5.0, 10.0}
    // A：速度加快一档
    // D：速度减慢一档
    // W：速度恢复原速
    // Q：脚本运行结束后不再重新载入
    void Enable(float speed = 1.0);

    // 设置减速快捷键
    void SetDecelerateKey(char key) { _decelerateKey = key; }

    // 设置加速快捷键
    void SetAccelerateKey(char key) { _accelerateKey = key; }

    // 设置恢复默认速度快捷键
    void SetResetSpeedKey(char key) { _resetSpeedKey = key; }

    // 设置脚本停止运行快捷键
    void SetScriptStopKey(char key) { _scriptStopKey = key; }
};
GameController gameController;

ALogger<AFile> fileLogger("logger.txt");

void AScript()
{
    // 解注以下代码开启存档只读
    // AMRef<uint8_t>(0x482149) = 0x2E;
    // AMRef<uint8_t>(0x54B267) = 0x70;
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);
    gameController.Enable(10);
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* time_ptr = std::localtime(&now_time);
    auto time_str = std::put_time(time_ptr, "当前时间: %Y/%m/%d %H:%M:%S");
    int round = AGetMainObject()->MPtr(0x160)->MRef<int>(0x6C);
    fileLogger.Debug("# 已完成#轮 当前阳光:#", time_str, 2 * round, AGetMainObject()->Sun());
    switch (round) {
    case 0:
        Begin1();
        break;
    case 1:
        Begin2();
        break;
    default:
        Run();
        // 这个在挂机阶段使用
        // 当跳帧检测到破阵时，停止跳帧
        ASkipTick([] {
            auto CobIndices = AGetPlantIndices({{3, 1}, {4, 1}, {3, 3}, {4, 3}, {1, 5}, {2, 5}, {5, 5}, {6, 5}}, ACOB_CANNON);
            auto GloomIndices = AGetPlantIndices({{3, 7}, {4, 7}, {1, 2}, {2, 2}, {5, 2}, {6, 2}, {3, 6}, {4, 6}}, AGLOOM_SHROOM);
            auto MelonIndices = AGetPlantIndices({{1, 1}, {2, 3}, {5, 3}, {6, 1}, {1, 3}, {6, 3}, {1, 4}, {6, 4}, {2, 4}, {5, 4}}, AWINTER_MELON);
            auto AllIndices = {CobIndices, GloomIndices, MelonIndices};
            for (const auto& Indeices : AllIndices) {
                for (const auto& index : Indeices) {
                    if (index == -1) {
                        return false;
                    }
                }
            }
            return true;
        });
        break;
    }
    AConnect('T', [] { ASkipTick([] { return true; }); });
}

int GetZombieRefreshRow(int wave)
{
    for (auto&& zombies : aAliveZombieFilter) {
        if (zombies.ExistTime() == ANowTime(wave)) {
            return zombies.Row() + 1;
        }
    }
    return -1;
}

void AutoBlover(float x, std::vector<AGrid> list)
{
    if (!AIsSeedUsable(ABLOVER))
        return;

    for (auto&& zombie : aAliveZombieFilter) {
        if (zombie.Type() == AQQ_16 && (40 + zombie.Abscissa()) / 80 <= x) {
            for (const auto& [row, col] : list) {
                if (AAsm::GetPlantRejectType(ABLOVER, row - 1, col - 1) == AAsm::NIL) {
                    ACard(ABLOVER, row, col);
                    break;
                }
            }
        }
    }
}

void WakeShroom(std::vector<AGrid> list)
{
    if (!AIsSeedUsable(ACOFFEE_BEAN))
        return;

    for (const auto& [row, col] : list) {
        if (AAsm::GetPlantRejectType(ACOFFEE_BEAN, row - 1, col - 1) == AAsm::NIL) {
            ACard(ACOFFEE_BEAN, row, col);
            break;
        }
    }
}

int GetPlantNum(std::function<bool(APlant*)> plants)
{
    std::vector<APlant*> ptrs;
    for (auto&& p : AAliveFilter(plants)) {
        ptrs.emplace_back(&p);
    }
    return ptrs.size();
}

int GetMaxExistTime(int row)
{
    std::vector<int> time;
    int max_time = 0;
    for (auto&& zombie : aAliveZombieFilter)
        if (zombie.Row() == row - 1)
            time.emplace_back(zombie.ExistTime());

    if (!time.empty())
        max_time = *std::max_element(time.begin(), time.end());

    return max_time;
}

void PressReductionCob()
{
    static int lock = -1;
    // 一帧只观测一次
    if (lock == AGetMainObject()->GameClock())
        return;

    lock = AGetMainObject()->GameClock();
    if (lock % 377 != 0)
        return;

    int zombies_num_in_the_row_2 = 0;
    int zombies_num_in_the_row_3 = 0;
    int zombies_num_in_the_row_5 = 0;
    int zombies_hp_in_the_row_2 = 0;
    int zombies_hp_in_the_row_3 = 0;
    int zombies_hp_in_the_row_5 = 0;
    for (auto&& zombies : aAliveZombieFilter) {
        if (zombies.Row() + 1 == 2 && zombies.Abscissa() <= 8 * 80.0) {
            zombies_num_in_the_row_2++;
            zombies_hp_in_the_row_2 += zombies.Hp() + zombies.OneHp() + zombies.TwoHp() / 5;
        }
        if (zombies.Row() + 1 == 3 && zombies.Abscissa() <= 765) {
            zombies_num_in_the_row_3++;
            zombies_hp_in_the_row_3 += zombies.Hp() + zombies.OneHp() + zombies.TwoHp() / 5;
        }
        if (zombies.Row() + 1 == 5 && zombies.Abscissa() <= 8 * 80.0) {
            zombies_num_in_the_row_5++;
            zombies_hp_in_the_row_5 += zombies.Hp() + zombies.OneHp() + zombies.TwoHp() / 5;
        }
    }
    int index_ready_Fire = -1;
    for (auto&& plants : aAlivePlantFilter) {
        if (plants.Type() == ACOB_CANNON && plants.State() == 37) {
            index_ready_Fire = plants.MRef<uint16_t>(0x148);
            break;
        }
    }
    if (index_ready_Fire >= 0) {
        if (zombies_num_in_the_row_2 >= 3 || zombies_hp_in_the_row_2 > 1370 || zombies_num_in_the_row_3 >= 3 || zombies_hp_in_the_row_3 > 1370 || GetMaxExistTime(2) > 2100 || GetMaxExistTime(3) > 2100) {
            AAsm::ReleaseMouse();
            AAsm::Fire(80 * 8, 55 + 85 * 2, index_ready_Fire);
            AAsm::ReleaseMouse();
        } else if (zombies_num_in_the_row_5 >= 3 || zombies_hp_in_the_row_5 > 1370 || GetMaxExistTime(5) > 2100) {
            AAsm::ReleaseMouse();
            AAsm::Fire(80 * 7, 55 + 85 * 5, index_ready_Fire);
            AAsm::ReleaseMouse();
        }
    }
}

void SeparateLastZombie(int wave)
{
    AConnect(ATime(wave, 0), [=] {
        std::vector<ZombieEnterMovement> zombieEnterMovements;
        auto zombies = AGetMainObject()->ZombieArray();
        ZombieEnterMovement temp = {};
        for (int index = 0; index < AGetMainObject()->ZombieTotal(); ++index) {
            if (!zombies[index].IsDisappeared() && !zombies[index].IsDead() && zombies[index].Row() != 2 && zombies[index].Row() != 3 && zombies[index].ExistTime() == ANowTime(wave) && ARangeIn(zombies[index].Type(), {AZOMBIE, ACONEHEAD_ZOMBIE, ABUCKETHEAD_ZOMBIE, ASCREEN_DOOR_ZOMBIE})) {
                temp.index = index;
                temp.row = zombies[index].Row() + 1;
                temp.absccisas = _qrmd::AGetZombieAbscissas(index, 820);
                for (int t = 0; t <= 820; ++t) {
                    if (temp.absccisas[t] < 765) {
                        temp.enter_time = t;
                        break;
                    }
                }
                zombieEnterMovements.emplace_back(temp);
            }
        }
        int maxEnterTime = zombieEnterMovements[0].enter_time;
        for (int i = 0; i < zombieEnterMovements.size(); ++i) {
            if (zombieEnterMovements[i].enter_time > maxEnterTime) {
                maxEnterTime = zombieEnterMovements[i].enter_time;
            }
        }
        if (maxEnterTime > 373) {
            AConnect(ATime(wave, maxEnterTime - 1 - 373), [] { aCobManager.Fire({{2, 8.8}, {5, 8.8}}); });
        } else {
            // 灰烬的作用范围是圆，所以对本行、上行、和下行的能打击到的僵尸的横坐标范围不同，因此选择最右僵尸时，应根据其(横坐标+落点行炮距-本行炮距)判断
            float maxAbsccisa = 0;
            float cannonLeftLengthOfZombieOffset = 0;
            float cannonDropAbsccisa = 0;
            for (int i = 0; i < zombieEnterMovements.size(); ++i) {
                if (zombieEnterMovements[i].row == 1 || zombieEnterMovements[i].row == 4) {
                    cannonLeftLengthOfZombieOffset = CANNON_LEFT_LENGTH_OF_ZOMBIE_AT_THE_ROW - CANNON_LEFT_LENGTH_OF_ZOMBIE_AT_UP_ROW;
                } else if (zombieEnterMovements[i].row == 2 || zombieEnterMovements[i].row == 5) {
                    cannonLeftLengthOfZombieOffset = 0;
                } else if (zombieEnterMovements[i].row == 3 || zombieEnterMovements[i].row == 6) {
                    cannonLeftLengthOfZombieOffset = CANNON_LEFT_LENGTH_OF_ZOMBIE_AT_THE_ROW - CANNON_LEFT_LENGTH_OF_ZOMBIE_AT_DOWN_ROW;
                }
                if (zombieEnterMovements[i].absccisas[373] + cannonLeftLengthOfZombieOffset > maxAbsccisa) {
                    maxAbsccisa = zombieEnterMovements[i].absccisas[373] + cannonLeftLengthOfZombieOffset;
                    cannonDropAbsccisa = maxAbsccisa - CANNON_LEFT_LENGTH_OF_ZOMBIE_AT_THE_ROW + 7 - 1;
                }
            }
            aCobManager.Fire({{2, cannonDropAbsccisa / 80}, {5, cannonDropAbsccisa / 80}});
        }
    });
}

bool IsBeingAten(int row, int col)
{
    for (auto&& p : aAlivePlantFilter) {
        if (p.MRef<int>(0xb4) != 0) {
            if (p.Row() == row - 1 && p.Col() == col - 1)
                return true;
        }
    }
    return false;
}

void PumpkinFixer(std::vector<AGrid> grid_list, int fixHp)
{
    if (!AIsSeedUsable(APUMPKIN) && !AIsSeedUsable(AM_PUMPKIN)) {
        return;
    }
    std::vector<int> grid_hp;
    for (const auto [row, col] : grid_list) {
        auto ptr = AGetPlantPtr(row, col, APUMPKIN);
        int gridhp = -1;
        if (ptr != nullptr) {
            gridhp = ptr->Hp();
        }
        grid_hp.emplace_back(gridhp);
    }
    int minhp = *std::min_element(grid_hp.begin(), grid_hp.end());
    if (minhp > fixHp) {
        return;
    }
    std::vector<AGrid> needFixGird;
    for (int i = 0; i < grid_hp.size(); ++i) {
        if (grid_hp[i] == minhp)
            needFixGird.emplace_back(grid_list[i]);
    }
    for (const auto& [row, col] : needFixGird) {
        if (AIsSeedUsable(APUMPKIN)) {
            ACard(APUMPKIN, row, col);
            break;
        }
        if (!AIsSeedUsable(APUMPKIN) && AIsSeedUsable(AM_PUMPKIN) && IsBeingAten(row, col)) {
            continue;
        }
        ACard(AM_PUMPKIN, row, col);
        break;
    }
}

void Begin1()
{
    ASelectCards({ASUNFLOWER, AM_SUNFLOWER, ATWIN_SUNFLOWER, APOTATO_MINE, AGARLIC, ATALL_NUT, ALILY_PAD, AKERNEL_PULT, ACOB_CANNON, ASQUASH}, 1);
    tickPlanterPlus.Start();
    tickRunner.Start(PressReductionCob);
    tickPlanterPlus.DelayPlantNotInQuene(ASUNFLOWER, {{1, 1}, {2, 1}, {1, 2}, {2, 2}, {1, 3}, {2, 3}});
    tickPlanterPlus.DelayPlantNotInQuene(AM_SUNFLOWER, {{5, 1}, {6, 1}, {5, 2}, {6, 2}, {5, 3}, {6, 3}});
    // wave 1 智能土豆地雷
    AConnect(ATime(1, 0), [] { tickPlanterPlus.WaitPlantNotInQuene(APOTATO_MINE, GetZombieRefreshRow(1), 5); });
    // wave 2 智能土豆地雷
    AConnect(ATime(2, 0), [] { tickPlanterPlus.DelayPlantNotInQuene(APOTATO_MINE, GetZombieRefreshRow(2), 4); });
    // wave 3, 4 智能大蒜、土豆地雷、玉米投手、高坚果
    AConnect(ATime(3, 0), [] {
        int zombie_row = GetZombieRefreshRow(3);
        int garlic_row = zombie_row <= 2 ? 6 : 1;
        first_cob_row = zombie_row <= 2 ? 5 : 2;
        tickPlanterPlus.WaitPlantNotInQuene(AGARLIC, garlic_row, 9);
        tickPlanterPlus.WaitPlantNotInQuene(ASQUASH, zombie_row, 5);
        tickPlanterPlus.WaitPlantNotInQuene(ATALL_NUT, first_cob_row, 7);
        tickPlanterPlus.DelayPlantNotInQuene(AKERNEL_PULT, {{first_cob_row, 6}, {first_cob_row, 5}});
        AConnect(ATime(4, 0), [=] {
            tickPlanterPlus.WaitPlantNotInQuene(AGARLIC, garlic_row == 6 ? 1 : 6, 9);
            tickPlanterPlus.WaitPlantNotInQuene(ATALL_NUT, first_cob_row == 5 ? 2 : 5, 7);
        });
        int squash_row = first_cob_row == 2 ? 5 : 2;
        for (int wave = 4; wave <= 18; ++wave) {
            AConnect(ATime(wave, 1), [=] {
                if (AGetPlantIndex(squash_row, 6, ASQUASH) < 0 && AGetPlantIndex(squash_row, 5, ACOB_CANNON) < 0) {
                    tickPlanterPlus.DelayPlantNotInQuene(ASQUASH, squash_row, 6);
                } else if (AGetPlantIndex(3, 8, ASQUASH) < 0) {
                    tickPlanterPlus.AddPlantNotInQuene(ASQUASH, 3, 8);
                }
            });
        }
    });
    // wave 4 边路向日葵
    AConnect(ATime(4, 0), [] {
        tickPlanterPlus.DelayPlantNotInQuene(ASUNFLOWER, {{1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {2, 4}});
        tickPlanterPlus.DelayPlantNotInQuene(AM_SUNFLOWER, {{6, 4}, {6, 5}, {6, 6}, {6, 7}, {6, 8}, {5, 4}});
    });
    // wave 5 水路大蒜、高坚果、玉米投手、向日葵
    AConnect(ATime(5, 0), [] {
        tickPlanterPlus.WaitAddPlant(AGARLIC, 4, 9);
        tickPlanterPlus.WaitAddPlant(ATALL_NUT, 3, 9);
        tickPlanterPlus.WaitPlantNotInQuene(ALILY_PAD, 3, 8);
        if (AGetPlantIndex(first_cob_row, 5, ACOB_CANNON) < 0) {
            tickPlanterPlus.WaitPlantNotInQuene(ACOB_CANNON, first_cob_row, 5);
        }
        tickPlanterPlus.AddPlantNotInQuene(ASUNFLOWER, {{3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7}});
        tickPlanterPlus.AddPlantNotInQuene(AM_SUNFLOWER, {{4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, {4, 6}, {4, 7}, {4, 8}});
    });
    // wave 7 开启高坚果和大蒜修补，种植玉米加农炮、双子向日葵
    AConnect(ATime(7, 0), [] {
        tallNutFixer1.Start(ATALL_NUT, {{2, 7}, {5, 7}}, 2000);
        tallNutFixer2.Start(ATALL_NUT, {{3, 9}}, 3000);
        garlicFixer.Start(AGARLIC, {{1, 9}, {4, 9}, {6, 9}}, 80);
        tickPlanterPlus.DelayPlantNotInQuene(ATWIN_SUNFLOWER, {{3, 5}, {4, 5}, {3, 6}, {4, 6}, {2, 4}, {5, 4}});
        cobPlanter.Start(ACOB_CANNON, {{first_cob_row == 2 ? 5 : 2, 5}, {3, 1}, {4, 1}, {3, 3}, {4, 3}, {1, 5}, {2, 5}, {5, 5}, {6, 5}});
    });
    // wave 19 拖收尾
    AConnect(ATime(19, 0), [] {
        tickRunner.Stop();
        aCobManager.AutoSetList();
        SeparateLastZombie(19);
    });
    AConnect(ATime(19, 4500 - 200 - 373), [] {
        aCobManager.AutoSetList();
        aCobManager.Fire({{2, 8.8}, {5, 8.8}});
    });

    // wave 20 炮消珊瑚、拖收尾
    AConnect(ATime(20, 223 - 373), [] {
        aCobManager.AutoSetList();
        aCobManager.Fire(4, 7.5875);
    });
    AConnect(ATime(20, 0), [] {
        aCobManager.AutoSetList();
        SeparateLastZombie(20);
    });
    AConnect(ATime(20, 5500 - 373), [] {
        aCobManager.AutoSetList();
        aCobManager.Fire({{2, 8.8}, {5, 8.8}});
    });
}

void Begin2()
{
    ASelectCards({AMELON_PULT, AWINTER_MELON, APUMPKIN, AUMBRELLA_LEAF, AFUME_SHROOM, AGLOOM_SHROOM, AM_PUMPKIN, ATALL_NUT, ABLOVER, ACOFFEE_BEAN}, 0);
    for (int wave : {10, 20}) {
        AConnect(ATime(wave, 100), [] {
            if (AIsZombieExist(ABUNGEE_ZOMBIE)) {
                aCobManager.AutoSetList();
                aCobManager.Fire({{2, 5}, {5, 5}});
            }
        });
    }
    AMaidCheats::Dancing();
    tickPlanterPlus.Start();
    tickPlanterPlus.ShovelPlantNotInQuene(ATALL_NUT, {{2, 7}, {5, 7}, {1, 7}, {6, 7}});
    tickPlanterPlus.ShovelPlantNotInQuene(AUMBRELLA_LEAF, {{2, 1}, {5, 1}});
    gloomShroomPlanter.Start(AGLOOM_SHROOM, {{3, 7}, {4, 7}, {1, 2}, {2, 2}, {5, 2}, {6, 2}});
    tickPlanterPlus.DelayPlantNotInQuene(ACOFFEE_BEAN, {{3, 7}, {4, 7}, {1, 2}, {2, 2}, {5, 2}, {6, 2}});
    winterMelonPlanter.Start(AWINTER_MELON, {{1, 1}, {2, 3}, {5, 3}, {6, 1}, {1, 3}, {6, 3}, {1, 4}, {6, 4}});
    pumpkinFixer.Start(APUMPKIN, {{3, 7}, {4, 7}, {3, 6}, {4, 6}, {3, 5}, {4, 5}, {1, 1}, {5, 1}, {1, 2}, {5, 2}, {1, 3}, {5, 3}, {1, 4}, {5, 4}, {2, 1}, {6, 1}, {2, 2}, {6, 2}, {2, 3}, {6, 3}, {2, 4}, {6, 4}}, 1500);
    tallNutFixer1.Start(ATALL_NUT, {{2, 7}, {5, 7}, {1, 7}, {6, 7}}, 1000);
    tickRunner.Start([] {
        if (AIsZombieExist(ABALLOON_ZOMBIE)) {
            AutoBlover(1, {{2, 8}});
        }
        if (ANowTime(1) % (869 * 3) == 296 && AGetMainObject()->ZombieCount() > 0) {
            aCobManager.AutoSetList();
            aCobManager.Fire({{2, 8.15}, {5, 8.2375}});
        }
        if (AGetMainObject()->GameClock() % 377 != 0)
            return;
        aCobManager.AutoSetList();
        for (auto&& zombies : aAliveZombieFilter) {
            if (zombies.Type() == ADIGGER_ZOMBIE && zombies.Abscissa() <= 120.0 && zombies.Hp() == zombies.MRef<int>(0xcc)) {
                if (ARangeIn(zombies.Row() + 1, {1, 2}))
                    aCobManager.Fire(2, 2);
                if (ARangeIn(zombies.Row() + 1, {5, 6}))
                    aCobManager.Fire(5, 2);
            }
        }
    });
}

void Run()
{
    std::vector<int> cards = {APUMPKIN, AM_PUMPKIN, ABLOVER, AICE_SHROOM, ACOFFEE_BEAN, AFUME_SHROOM, AGLOOM_SHROOM, ALILY_PAD};
    if (GetPlantNum([](APlant* p) { return p->Type() == ACOB_CANNON && p->Hp() == p->HpMax(); }) == 8 // 炮均满血
        && GetPlantNum([](APlant* p) { return p->Type() == AWINTER_MELON; }) < 10) {                  // 且冰瓜有缺失
        cards.emplace_back(AMELON_PULT);
        cards.emplace_back(AWINTER_MELON);
    } else {
        cards.emplace_back(AKERNEL_PULT);
        cards.emplace_back(ACOB_CANNON);
    }
    ASelectCards(cards, 0);
    for (int wave : {10, 20})
        AConnect(ATime(wave, 200), [] {
            if (AIsZombieExist(ABUNGEE_ZOMBIE))
                ACard({AICE_SHROOM, ACOFFEE_BEAN}, 1, 7);
        });
    AMaidCheats::Dancing();
    AShovel({{1, 7}, {2, 7}, {5, 7}, {6, 7}});
    if (AGetSeedIndex(AWINTER_MELON) >= 0)
        winterMelonPlanter.Start(AWINTER_MELON, {{1, 1}, {2, 3}, {5, 3}, {6, 1}, {1, 3}, {6, 3}, {1, 4}, {6, 4}, {2, 4}, {5, 4}}, 1500, true);

    if (AGetSeedIndex(ACOB_CANNON) >= 0)
        cobPlanter.Start(ACOB_CANNON, {{3, 1}, {4, 1}, {3, 3}, {4, 3}, {1, 5}, {2, 5}, {5, 5}, {6, 5}}, 500, true);

    gloomShroomPlanter.Start(AGLOOM_SHROOM, {{3, 7}, {4, 7}, {1, 2}, {2, 2}, {5, 2}, {6, 2}, {3, 6}, {4, 6}}, 1500, true);
    tickRunner.Start([] {
        if (AIsZombieExist(ABALLOON_ZOMBIE)) {
            AutoBlover(1, {{2, 7}, {2, 8}});
        }
        if (ANowTime(1) % 869 == 296) {
            aCobManager.AutoSetList();
            aCobManager.Fire({{2, 8.15}, {5, 8.2375}});
        }
        PumpkinFixer({{1, 1}, {5, 1}, {1, 2}, {5, 2}, {1, 3}, {5, 3}, {1, 4}, {5, 4}, {2, 1}, {6, 1}, {2, 2}, {6, 2}, {2, 3}, {6, 3}, {2, 4}, {6, 4}, {3, 7}, {4, 7}, {3, 6}, {4, 6}, {3, 5}, {4, 5}}, 1500);
        WakeShroom({{3, 7}, {4, 7}, {3, 6}, {4, 6}, {1, 2}, {2, 2}, {5, 2}, {6, 2}});
    });
}

void GameController::Enable(float speed)
{
    ASetGameSpeed(speed);
    _defaultSpeed = 10.0 / AGetPvzBase()->TickMs();
    AConnect(_decelerateKey, [this] {
        int gameSpeedGear = _gameSpeeds.size();
        float gameSpeed = 10.0 / AGetPvzBase()->TickMs();
        for (int i = 0; i < _gameSpeeds.size(); ++i) {
            if (gameSpeed <= _gameSpeeds[i]) {
                gameSpeedGear = i;
                break;
            }
        }
        if (gameSpeedGear > 0)
            ASetGameSpeed(_gameSpeeds[gameSpeedGear - 1]);
    });
    AConnect(_accelerateKey, [this] {
        int gameSpeedGear = 0;
        float game_speed = 10.0 / AGetPvzBase()->TickMs();
        for (int i = _gameSpeeds.size() - 1; i >= 0; --i) {
            if (game_speed >= _gameSpeeds[i]) {
                gameSpeedGear = i;
                break;
            }
        }
        if (gameSpeedGear < _gameSpeeds.size() - 1)
            ASetGameSpeed(_gameSpeeds[gameSpeedGear + 1]);
    });
    AConnect(_resetSpeedKey, [] { ASetGameSpeed(1); });

    AConnect(_scriptStopKey, [] { ASetReloadMode(AReloadMode::NONE); });
}