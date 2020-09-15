/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 */

#include "libavz.h"

bool RangeIn(int num, std::initializer_list<int> lst)
{
	for (auto _num : lst)
	{
		if (_num == num)
		{
			return true;
		}
	}
	return false;
}

void AvZ::init_address()
{
	pvz_base = *(PvZ **)0x6a9ec0;
	pvz_hwnd = FindWindowW(L"MainWindow", L"Plants vs. Zombies");
	pvz_handle = INVALID_HANDLE_VALUE;
}

std::vector<AvZ::OperationQueue> AvZ::operation_queue_vec;
AvZ::TimeWave AvZ::time_wave_insert;
AvZ::TimeWave AvZ::time_wave_run;
bool AvZ::is_loaded = false;
bool AvZ::is_multiple_effective = false;
bool AvZ::is_exited = false;
bool AvZ::is_insert_operation = true;
bool AvZ::block_var = false;
int AvZ::error_mode = AvZ::POP_WINDOW;
std::vector<AvZ::OperationQueue>::iterator AvZ::wavelength_it;
HWND AvZ::pvz_hwnd;
HANDLE AvZ::pvz_handle = nullptr;
PvZ *AvZ::pvz_base;
MainObject *AvZ::main_object;

std::map<int, int> AvZ::seed_name_to_index_map;
std::vector<AvZ::Grid> AvZ::select_card_vec;
std::vector<AvZ::ThreadInfo> AvZ::thread_vec;
std::stack<int> AvZ::stoped_thread_id_stack;
AvZ::PaoOperator::RoofFlyTime AvZ::PaoOperator::fly_time_data[8] = {
	{515, 359},
	{499, 362},
	{515, 364},
	{499, 367},
	{515, 369},
	{499, 372},
	{511, 373},
	{511, 373},
};
AvZ::TickRunner AvZ::PaoOperator::tick_runner;
std::set<AvZ::Grid> AvZ::PaoOperator::lock_pao_set; // 锁定的炮

AvZ avz;
AvZ::ItemCollector item_collector;
AvZ::IceFiller ice_filler;
AvZ::PlantFixer plant_fixer;
AvZ::PaoOperator pao_operator;
AvZ::KeyConnector key_connector;