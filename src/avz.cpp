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

void __UpdateRefreshTime();
void __InitAddress();
void __ChooseCards();
void __ChangeRunWave();

void __LoadScript()
{
    extern PvZ* __pvz_base;
    extern MainObject* __main_object;
    extern int __error_mode;
    extern int __effective_mode;
    extern TimeWave __time_wave_start;
    extern bool __is_insert_operation;
    extern std::vector<OperationQueue> __operation_queue_vec;
    extern std::vector<OperationQueue>::iterator __wavelength_it;
    extern bool __is_loaded;
    extern bool __block_var;
    extern VoidFunc<void> __script_exit_deal;

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

    __operation_queue_vec.resize(__main_object->totalWave() + 1);
    __wavelength_it = __operation_queue_vec.begin();
    SetInsertOperation(false);
    item_collector.start();
    MaidCheats::stop();
    __pvz_base->tickMs() = 10;
    SetInsertOperation(true);

    __skip_tick_condition = []() -> bool {
        return false;
    };

    Asm::setImprovePerformance(false);
    key_connector.clear();
    __seed_name_to_index_map.clear();
    PaoOperator::initialState();

    std::vector<int> waves;
    // 将默认时间设置为刚一进战斗界面的时间
    if (__pvz_base->gameUi() == 3) {
        __UpdateRefreshTime();
        waves = GetRefreshedWave();
    }

    if (waves.size() == 0) {
        __time_wave_start.wave = 1;
        __time_wave_start.time = __DEFAULT_START_TIME;
    } else {
        __time_wave_start.wave = *waves.begin();
        __time_wave_start.time = NowTime(__time_wave_start.wave);
    }

    __run_wave_iter = __operation_queue_vec.begin() + __time_wave_start.wave - 1;

    SetTime(__time_wave_start);

    try {
        __block_var = true;
        Script();
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

        if (__effective_mode != MAIN_UI_OR_FIGHT_UI) {
            // 如果战斗界面不允许重新注入则等待回主界面
            while (__pvz_base->mainObject()) {
                Asm::gameSleepLoop();
            }
        }
    } catch (Exception& exce) {
        printf(exce.what());
    } catch (...) {
        ShowErrorNotInQueue("脚本触发了一个未知的异常\n");
    }
    __script_exit_deal();
    AvZ::SetErrorMode(AvZ::POP_WINDOW);
    __operation_queue_vec.clear(); // 清除一切操作
    TickRunner::clear();
    SetInsertOperation(false);
    MaidCheats::stop();
    __pvz_base->tickMs() = 10;
    SetInsertOperation(true);

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
        printf(exce.what());
    } catch (...) {
        ShowErrorNotInQueue("脚本触发了一个未知的异常\n");
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
        Asm::gameFightLoop();
        Asm::clearObjectMemory();
        Asm::gameExit();
    }
}

Memory memory;

// call script() instead of game_loop()
void InstallHook(Memory& memory)
{
    DWORD temp;
    VirtualProtect((void*)0x400000, 0x35E000, PAGE_EXECUTE_READWRITE, &temp);
    *(uint32_t*)0x667bc0 = (uint32_t)&ManageScript;
}

void UninstallHook(Memory& memory)
{
    DWORD temp;
    VirtualProtect((void*)0x400000, 0x35E000, PAGE_EXECUTE_READWRITE, &temp);
    *(uint32_t*)0x667bc0 = 0x452650;
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        // attach to process
        // return FALSE to fail DLL load
        memory.OpenSelf();
        InstallHook(memory);
        break;

    case DLL_PROCESS_DETACH:
        // detach from process
        AvZ::__Exit();
        Sleep(10);

        UninstallHook(memory);
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
