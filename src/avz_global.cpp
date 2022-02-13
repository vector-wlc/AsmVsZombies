/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:32:56
 * @Description: AvZ global var
 */

#include <map>
#include <stack>

#include "avz_cannon.h"
#include "avz_execption.h"
#include "avz_global.h"
#include "avz_tick.h"
#include "avz_time_operation.h"

namespace AvZ {
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
int __call_depth = 0;
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
} // namespace AvZ
