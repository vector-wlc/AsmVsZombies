/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 */

#include "avz.h"

bool RangeIn(int num, std::initializer_list<int> lst)
{
	for (auto _num : lst)
		if (_num == num)
			return true;
	return false;
}

void AvZ::init_address()
{
	pvz_hwnd = FindWindowW(L"MainWindow", L"Plants vs. Zombies");
	DWORD pid;
	GetWindowThreadProcessId(pvz_hwnd, &pid);
	if (pvz_handle == nullptr)
	{
		pvz_handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	}
	PvZ **base_ptr = (PvZ **)0x6a9ec0;
	pvz_base = *base_ptr;
}

std::map<AvZ::TimeWave, std::vector<std::function<void()>>> AvZ::operation_queue;
AvZ::TimeWave AvZ::time_wave;
bool AvZ::is_loaded = false;
bool AvZ::is_multiple_effective = false;
bool AvZ::is_exited = false;
HWND AvZ::pvz_hwnd;
HANDLE AvZ::pvz_handle = nullptr;
PvZ *AvZ::pvz_base;
MainObject *AvZ::main_object;
AvZ::TimeWave AvZ::zombie_refresh;
bool AvZ::is_get_seed_index = false;
std::string AvZ::seed_name_list[11][8] =
	{
		{"wdss", "xrk", "ytzd", "jg", "tddl", "hbss", "dzh", "scss"},
		{"xpg", "ygg", "dpg", "mbtsz", "mhg", "dxg", "hbg", "hmg"},
		{"hy", "wg", "sfss", "crhz", "hblj", "dc", "hjsz", "gjg"},
		{"sbg", "ldh", "xrz", "syc", "ljss", "yt", "ngt", "clg"},
		{"jxcts", "hp", "ymts", "kfd", "ds", "yzbhs", "jzh", "xgts"},
		{"jqss", "szxrk", "yyg", "xp", "bxgts", "xjc", "dcw", "ymjnp"},
		{"Mwdss", "Mxrk", "Mytzd", "Mjg", "Mtddl", "vhbss", "Mdzh", "Mscss"},
		{"Mxpg", "Mygg", "Mdpg", "Mmbtsz", "Mmhg", "Mdxg", "Mhbg", "Mhmg"},
		{"Mhy", "Mwg", "Msfss", "Mcrhz", "Mhblj", "Mdc", "Mhjsz", "Mgjg"},
		{"Msbg", "Mldh", "Mxrz", "Msyc", "Mljss", "Myt", "Mngt", "Mclg"},
		{"Mjxcts", "Mhp", "Mymts", "Mkfd", "Mds", "Myzbhs", "Mjzh", "Mxgts"}};

std::map<std::string, int> AvZ::seed_name_to_index_map;
std::vector<AvZ::ThreadInfo> AvZ::thread_vec;
std::stack<int> AvZ::stoped_thread_id_stack;
std::vector<AvZ::PaoOperator::PaoInfo> AvZ::PaoOperator::all_pao_vec;
int AvZ::PaoOperator::next_vec_index = 0;
AvZ::PaoOperator::RoofFlyTime AvZ::PaoOperator::fly_time_data[8] = {
	{515, 359}, {499, 362}, {515, 364}, {499, 367}, {515, 369}, {499, 372}, {511, 373}, {511, 373}};
AvZ::PaoOperator::LastestPaoMsg AvZ::PaoOperator::lastest_pao_msg;
AvZ::VThread AvZ::PaoOperator::vthread;

AvZ avz;
AvZ::ItemCollector item_collector;
AvZ::IceFiller ice_filler;
AvZ::PlantFixer plant_fixer;
AvZ::PaoOperator pao_operator;
AvZ::KeyConnector key_connector;