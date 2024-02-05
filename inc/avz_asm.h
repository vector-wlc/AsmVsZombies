/*
 * @coding: utf-8
 * @Author: yuchexi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: pvz func click_scene
 */
#ifndef __AVZ_ASM_H__
#define __AVZ_ASM_H__

#include "avz_pvz_struct.h"
#include "avz_types.h"

#define ASaveAllRegister "esp", "ebp", "eax", "ebx", "ecx", "edx", "esi", "edi"

// 使用汇编代码调用 PvZ 游戏内部的函数
class AAsm {

public:
    // key 1: left click
    // key -1: right click
    // key 3: middle click
    static void ClickScene(AMainObject* level, int x, int y, int key);

    static void Click(AMouseWindow* mw, int x, int y, int key);

    static void SaveData();

    static void LoadData();

    // 进入战斗页面，开始游戏
    static void Rock();

    static void MouseClick(int x, int y, int key);

    // 发射炮
    static void Fire(int x, int y, int rank);

    // 种卡函数
    static void PlantCard(int x, int y, int index);

    // 铲除函数
    static void ShovelPlant(int x, int y);

    // 选择卡片
    static void ChooseCard(int card_type);

    // 选择模仿者卡片
    static void ChooseImitatorCard(int card_type);

    enum PlantReject {
        NIL, // 没有植物, 即可以种植
        NOT_HERE,
        ONLY_ON_GRAVES,
        ONLY_IN_POOL,
        ONLY_ON_GROUND,
        NEEDS_POT,
        NOT_ON_ART,
        NOT_PASSED_LINE,
        NEEDS_UPGRADE,
        NOT_ON_GRAVE,
        NOT_ON_CRATER,
        NOT_ON_WATER,
        NEEDS_GROUND,
        NEEDS_SLEEPING
    };

    // 是否可种卡片
    // 返回值意义查看 `enum PlantReject`
    // *** 使用示例:
    // if(AAsm::GetPlantRejectType(AXRK_1, 1, 1) == AAsm::NIL) { // NIL 表示可以种植物， 检查一下 2 行 2 列是否可以种植物
    //
    // }
    __ANodiscard static int GetPlantRejectType(int cardType, int row, int col);

    // 战斗界面刷新函数
    static void GameFightLoop();

    // 游戏整体刷新函数
    static void GameTotalLoop();

    // 游戏最完整刷新函数，带帧率控制
    static void GameSleepLoop();

    // 控件管理器在一次更新中递归地更新其中所有的控件
    static void UpdateFrame();

    static void ClearObjectMemory();

    static void CheckFightExit();

    static void KillZombiesPreview();

    static void SetImprovePerformance(bool is_improve_performance);

    static void ReleaseMouse();

    __ANodiscard static int GridToAbscissa(int row, int col);

    __ANodiscard static int GridToOrdinate(int row, int col);

    // 放置一个植物
    // PutPlant(1, 1, ANGT_30) ----- 在2行2列放置一个南瓜头
    static APlant* PutPlant(int row, int col, APlantType type);

    // 放置一个僵尸
    // PutZombie(1, 1, APJ_0) ----- 在2行2列放置一个普通僵尸
    static AZombie* PutZombie(int row, int col, AZombieType type);

    static void RemovePlant(APlant* plant);

    static void RemoveZombie(AZombie* zombie);

    static bool IsSeedUsable(ASeed* seed);
    static int GetSeedSunVal(int type, int iType);
    static void UpdateMousePos();

    static void MakePvzString(const char* str, void* strObj);
    static void FreePvzString(void* strObj);
    static void MakeNewBoard();
    static void LoadGame(const std::string& file);
    static void SaveGame(const std::string& file);

    static bool CanSpawnZombies(int row);
    static bool IsNight();
    static bool IsRoof();
    static bool HasGrave();
    static bool HasPool();

    static int ZombieTotalHp(int wave);

    enum GameMode {
        ADVENTURE,
        SURVIVAL_NORMAL_STAGE_1,
        SURVIVAL_NORMAL_STAGE_2,
        SURVIVAL_NORMAL_STAGE_3,
        SURVIVAL_NORMAL_STAGE_4,
        SURVIVAL_NORMAL_STAGE_5,
        SURVIVAL_HARD_STAGE_1,
        SURVIVAL_HARD_STAGE_2,
        SURVIVAL_HARD_STAGE_3,
        SURVIVAL_HARD_STAGE_4,
        SURVIVAL_HARD_STAGE_5,
        SURVIVAL_ENDLESS_STAGE_1,
        SURVIVAL_ENDLESS_STAGE_2,
        SURVIVAL_ENDLESS_STAGE_3,
        SURVIVAL_ENDLESS_STAGE_4,
        SURVIVAL_ENDLESS_STAGE_5,
        CHALLENGE_WAR_AND_PEAS,
        CHALLENGE_WALLNUT_BOWLING,
        CHALLENGE_SLOT_MACHINE,
        CHALLENGE_RAINING_SEEDS,
        CHALLENGE_BEGHOULED,
        CHALLENGE_INVISIGHOUL,
        CHALLENGE_SEEING_STARS,
        CHALLENGE_ZOMBIQUARIUM,
        CHALLENGE_BEGHOULED_TWIST,
        CHALLENGE_LITTLE_TROUBLE,
        CHALLENGE_PORTAL_COMBAT,
        CHALLENGE_COLUMN,
        CHALLENGE_BOBSLED_BONANZA,
        CHALLENGE_SPEED,
        CHALLENGE_WHACK_A_ZOMBIE,
        CHALLENGE_LAST_STAND,
        CHALLENGE_WAR_AND_PEAS_2,
        CHALLENGE_WALLNUT_BOWLING_2,
        CHALLENGE_POGO_PARTY,
        CHALLENGE_FINAL_BOSS,
        CHALLENGE_ART_CHALLENGE_WALLNUT,
        CHALLENGE_SUNNY_DAY,
        CHALLENGE_RESODDED,
        CHALLENGE_BIG_TIME,
        CHALLENGE_ART_CHALLENGE_SUNFLOWER,
        CHALLENGE_AIR_RAID,
        CHALLENGE_ICE,
        CHALLENGE_ZEN_GARDEN,
        CHALLENGE_HIGH_GRAVITY,
        CHALLENGE_GRAVE_DANGER,
        CHALLENGE_SHOVEL,
        CHALLENGE_STORMY_NIGHT,
        CHALLENGE_BUNGEE_BLITZ,
        CHALLENGE_SQUIRREL,
        TREE_OF_WISDOM,
        SCARY_POTTER_1,
        SCARY_POTTER_2,
        SCARY_POTTER_3,
        SCARY_POTTER_4,
        SCARY_POTTER_5,
        SCARY_POTTER_6,
        SCARY_POTTER_7,
        SCARY_POTTER_8,
        SCARY_POTTER_9,
        SCARY_POTTER_ENDLESS,
        PUZZLE_I_ZOMBIE_1,
        PUZZLE_I_ZOMBIE_2,
        PUZZLE_I_ZOMBIE_3,
        PUZZLE_I_ZOMBIE_4,
        PUZZLE_I_ZOMBIE_5,
        PUZZLE_I_ZOMBIE_6,
        PUZZLE_I_ZOMBIE_7,
        PUZZLE_I_ZOMBIE_8,
        PUZZLE_I_ZOMBIE_9,
        PUZZLE_I_ZOMBIE_ENDLESS,
        UPSELL,
        INTRO,
        NUM_GAME_MODES
    };

    // 进入游戏
    static void EnterGame(int gameMode);

    // 直接返回主界面
    static void DoBackToMain();

    enum GameScenes {
        LOADING,
        MENU,
        LEVEL_INTRO,
        PLAYING,
        ZOMBIES_WON,
        AWARD,
        CREDIT,
        CHALLENGE
    };

    // 内置生成僵尸列表的函数
    static void PickZombieWaves();

    // 选卡界面点击调试试玩按钮时，以随机选卡填充卡槽，立即完成所有选卡的移动，然后结束选卡。
    static void PickRandomSeeds();

    __ADeprecated() static void* SaveToMemory();
    __ADeprecated() static void LoadFromMemory(void*& p);
    __ADeprecated() static void FreeMemory(void*& p);

protected:
    static void _ClickScene();
    static void _Click();
    static void _MouseClick();
    static void _ShootPao();
    static void _PlantCard();
    static void _ShovelPlant();
    static void _ChooseCard();
    static void _ChooseImitatorCard();
    static void _GetPlantRejectType();
    static void _RemovePlant();
};

#endif
