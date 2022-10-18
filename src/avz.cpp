/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 */
#include "avz.h"
#include "avz_global.h"

namespace AvZ {
ItemCollector item_collector;
IceFiller ice_filler;
PlantFixer plant_fixer;
PaoOperator pao_operator;
KeyConnector key_connector;
AliveFilter<Zombie> alive_zombie_filter;
AliveFilter<Plant> alive_plant_filter;
AliveFilter<Seed> alive_seed_filter;
AliveFilter<PlaceItem> alive_place_item_filter;
} // namespace AvZ

namespace AvZ {
extern MainObject* __main_object;
extern VoidFunc<bool> __skip_tick_condition;
extern MainObject* __main_object;
extern PvZ* __pvz_base;
extern bool __is_exited;
extern bool __is_advanced_pause;
extern bool __is_loaded;
extern std::vector<OperationQueue> __operation_queue_vec;
extern std::map<int, int> __seed_name_to_index_map;
extern std::vector<int> __select_card_vec;
extern int __error_mode;
extern int __call_depth;
extern bool __block_var;
extern std::vector<OperationQueue>::iterator __run_wave_iter;
extern PvZ* __pvz_base;
extern MainObject* __main_object;
extern int __error_mode;
extern int __effective_mode;
extern TimeWave __time_wave_start;
extern bool __is_insert_operation;
extern std::vector<OperationQueue> __operation_queue_vec;
extern std::vector<OperationQueue>::iterator __wavelength_it;
extern bool __is_loaded;
extern VoidFunc<void> __script_exit_deal;
extern std::vector<GlobalVar*> __global_var_set;
extern bool __is_run_enter_fight;
extern bool __is_run_exit_fight;

void __UpdateRefreshTime();
void __InitAddress();
void __ChooseCards();
void __ChangeRunWave();
void __BeforeScript();
void __AfterScript();
void __EnterFight();
void __ExitFight();

void __LoadScript()
{
    // 脚本初始化
    __InitAddress();

    if (__pvz_base->gameUi() == 1 || //
        (__pvz_base->gameUi() != 2 && __pvz_base->gameUi() != 3)) {
        return;
    }
    __main_object = __pvz_base->mainObject();

    // 假进入战斗界面直接返回
    if (__main_object->loadDataState() == 1) {
        return;
    }

    __is_loaded = true;

    try {
        __block_var = true;
        __BeforeScript();
        Script();
        __AfterScript();
        __block_var = true;

        void RunScriptEveryTick();
        RunScriptEveryTick();

        // 等待游戏进入战斗界面
        while (__pvz_base->gameUi() == 2 && __pvz_base->mainObject()) {
            Asm::gameSleepLoop();
        }

        // 等待游戏结束
        while (__pvz_base->gameUi() == 3 && __pvz_base->mainObject()) {
            Asm::gameSleepLoop();
        }

        if (!__is_run_exit_fight) {
            __is_run_exit_fight = true;
            __ExitFight();
        }

        if (__effective_mode != MAIN_UI_OR_FIGHT_UI) {
            // 如果战斗界面不允许重新注入则等待回主界面
            while (__pvz_base->mainObject()) {
                Asm::gameSleepLoop();
            }
        }
    } catch (Exception& exce) {
        std::string exce_msg = exce.what();
        if (exce_msg != STR_GAME_RET_MAIN_UI) {
            __is_exited = true;
            exce_msg += " ||  AvZ has stopped working !!!";
        }
        exce_msg = "catch exception: " + exce_msg;
        exce_msg += '\n';
        std::printf(exce_msg.c_str());
    } catch (...) {
        ShowErrorNotInQueue("脚本触发了一个未知的异常\n");
        __is_exited = true;
    }
    if (!__is_run_exit_fight) {
        __ExitFight();
    }
    __is_loaded = !(__effective_mode >= 0);
}

extern "C" __declspec(dllexport) void __cdecl __Run()
{
    try {
        if (Unlikely(__is_exited)) {
            return;
        }

        if (Likely(__is_loaded)) { // 运行脚本
            TickRunner::runInGlobal();

            if (Unlikely(__is_advanced_pause)) {
                return;
            }

            int game_ui = __pvz_base->gameUi();
            if (Unlikely(game_ui == 2 && !__select_card_vec.empty())) {
                __ChooseCards();
            }

            if (Unlikely(game_ui != 3 || __pvz_base->mouseWindow()->topWindow())) {
                __seed_name_to_index_map.clear();
                return;
            }

            // 以下代码到战斗界面才能执行
            if (Unlikely(game_ui == 2)) {
                return;
            }

            if (Unlikely(!__is_run_enter_fight)) {
                __is_run_enter_fight = true;
                __EnterFight();
            }

            if (Unlikely(!__select_card_vec.empty())) {
                __select_card_vec.clear();
            }

            if (Likely(__main_object->wave() != __main_object->totalWave())) {
                __UpdateRefreshTime();
            }

            int offset_time = __main_object->gameClock() - (__run_wave_iter + 1)->refresh_time;
            int currnet_wave = __run_wave_iter - __operation_queue_vec.begin() + 1;

            if (Unlikely(offset_time == -200 ||                                          //
                    (currnet_wave == 9 || currnet_wave == 19) && offset_time == -750)) { // 下一波刷新点时间已经确定
                __ChangeRunWave();
                currnet_wave = __run_wave_iter - __operation_queue_vec.begin() + 1;
            }

            while (__run_wave_iter->refresh_time != __DEFAULT_START_TIME && // 波次刷新时间已知
                !__run_wave_iter->queue.empty() &&                          // 有操作
                __run_wave_iter->is_time_arrived())                         // 操作时间已到达
            {
                auto& queue = __run_wave_iter->queue;
                int time = queue.begin()->first;
                for (auto iter = queue.begin(); iter->first == time && iter != queue.end();) {
                    if (Unlikely(__error_mode == CONSOLE)) {
                        Print("(%d, %d)# run operation: %s (inserted in %d, %d)\n",
                            iter->second.getInsertTimeWave().time,
                            iter->second.getInsertTimeWave().wave,
                            iter->second.getDescription().c_str(),
                            time, currnet_wave);
                    }
                    iter->second.execute();
                    iter = queue.erase(iter);
                }
            }
            TickRunner::runInFight();
        } else { // 载入脚本
            __LoadScript();
        }
    } catch (Exception& exce) {
        std::string exce_msg = exce.what();
        if (exce_msg != STR_GAME_RET_MAIN_UI) {
            __is_exited = true;
            exce_msg += " ||  AvZ has stopped working !!!";
        }
        exce_msg = "catch exception: " + exce_msg;
        exce_msg += '\n';
        std::printf(exce_msg.c_str());
        __is_loaded = !(__effective_mode >= 0);
    } catch (...) {
        ShowErrorNotInQueue("脚本触发了一个未知的异常, 已强制关闭脚本\n");
        __is_exited = true;
    }

    return;
}

void RunScriptEveryTick()
{
    AvZ::__Run();
    if (__is_advanced_pause) {
        return;
    }

    Asm::gameTotalLoop();
}
}

extern "C" __declspec(dllexport) void __cdecl ManageScript()
{
    AvZ::RunScriptEveryTick();
    while (AvZ::__skip_tick_condition() && AvZ::__block_var && AvZ::__pvz_base->mainObject()) {
        AvZ::__Run();
        if (AvZ::__is_advanced_pause) {
            return;
        }
        AvZ::__pvz_base->mjClock() += 1;
        Asm::gameFightLoop();
        Asm::clearObjectMemory();
        Asm::gameExit();
    }
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    void InstallHook();
    void UninstallHook();
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        // attach to process
        // return FALSE to fail DLL load
        InstallHook();
        break;

    case DLL_PROCESS_DETACH:
        // detach from process
        AvZ::__Exit();
        Sleep(10);
        UninstallHook();
        break;

    case DLL_THREAD_ATTACH:
        // attach to thread
        break;

    case DLL_THREAD_DETACH:
        // detach from thread
        break;
    }
    return TRUE; // succesful
}
