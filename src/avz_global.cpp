/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:32:56
 * @Description: AvZ global var
 */

#include <map>
#include <stack>

#include "libavz.h"

namespace AvZ {
std::set<GlobalVar*> __global_var_set;
HWND __pvz_hwnd;
HANDLE __pvz_handle = nullptr;
PvZ* __pvz_base;
MainObject* __main_object;
AnimationMain* __animation_main;
std::map<int, int> __seed_name_to_index_map;
std::vector<int> __select_card_vec;
std::vector<OperationQueue> __operation_queue_vec;
TimeWave __time_wave_insert;
TimeWave __time_wave_start;
bool __is_loaded = false;
int __effective_mode = -1;
bool __is_exited = false;
bool __is_insert_operation = true;
bool __block_var = false;
int __error_mode = POP_WINDOW;
bool __is_advanced_pause = false;
bool __is_run_enter_fight = false;
bool __is_run_exit_fight = false;
int __call_depth = 0;
int __tick_ms = 10;
std::vector<OperationQueue>::iterator __wavelength_it;
std::vector<OperationQueue>::iterator __run_wave_iter;

VoidFunc<bool> __skip_tick_condition = []() -> bool {
    return false;
};

VoidFunc<void> __script_exit_deal = []() {};

bool RangeIn(int num, std::initializer_list<int> lst)
{
    for (auto _num : lst) {
        if (_num == num) {
            return true;
        }
    }
    return false;
}

void __InitAddress()
{
    __pvz_base = *(PvZ**)0x6a9ec0;
    __pvz_hwnd = FindWindowW(L"MainWindow", L"Plants vs. Zombies");
    __pvz_handle = INVALID_HANDLE_VALUE;
}

void __BeforeScript()
{
    __is_run_enter_fight = false;
    __is_run_exit_fight = false;
    __operation_queue_vec.resize(__main_object->totalWave() + 1);
    __wavelength_it = __operation_queue_vec.begin();
    SetInsertOperation(false);
    item_collector.start();
    MaidCheats::stop();
    __tick_ms = __pvz_base->tickMs();
    SetInsertOperation(true);

    __skip_tick_condition = []() -> bool {
        return false;
    };

    Asm::setImprovePerformance(false);
    key_connector.clear();
    __seed_name_to_index_map.clear();

    for (auto&& global_var : __global_var_set) {
        global_var->beforeScript();
    }

    std::vector<int> waves;
    // 将默认时间设置为刚一进战斗界面的时间
    if (__pvz_base->gameUi() == 3) {
        void __UpdateRefreshTime();
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
}

void __AfterScript()
{
    for (auto&& global_var : __global_var_set) {
        global_var->afterScript();
    }
}

void __EnterFight()
{
    for (auto&& global_var : __global_var_set) {
        global_var->enterFight();
    }
}

void __ExitFight()
{
    if (!__is_exited) {
        __script_exit_deal();
        __operation_queue_vec.clear(); // 清除一切操作
        for (auto&& global_var : __global_var_set) {
            global_var->exitFight();
        }
    }

    AvZ::SetErrorMode(AvZ::POP_WINDOW);
    TickRunner::clear();
    SetInsertOperation(false);
    MaidCheats::stop();
    __pvz_base->tickMs() = __tick_ms;
    SetInsertOperation(true);
}

} // namespace AvZ
