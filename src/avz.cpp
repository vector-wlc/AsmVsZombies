/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 */

#include "libavz.h"

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
std::string AvZ::seed_name_list[11][8] =
	{
		//豌豆射手，向日葵，樱桃炸弹，坚果，土豆地雷，寒冰射手，大嘴花，双重射手
		{"wdss", "xrk", "ytzd", "jg", "tddl", "hbss", "dzh", "scss"},
		//小喷菇，阳光菇，大喷菇，墓碑吞噬者，魅惑菇，胆小菇，寒冰菇，毁灭菇
		{"xpg", "ygg", "dpg", "mbtsz", "mhg", "dxg", "hbg", "hmg"},
		//荷叶，倭瓜，三发射手，缠绕海藻，火爆辣椒，地刺，火炬树桩，高坚果
		{"hy", "wg", "sfss", "crhz", "hblj", "dc", "hjsz", "gjg"},
		//水兵菇，路灯花，仙人掌，三叶草，裂荚射手，杨桃，南瓜头，磁力菇
		{"sbg", "ldh", "xrz", "syc", "ljss", "yt", "ngt", "clg"},
		//卷心菜投手，花盆，玉米投手，咖啡豆，大蒜，叶子保护伞，金盏花，西瓜投手
		{"jxcts", "hp", "ymts", "kfd", "ds", "yzbhs", "jzh", "xgts"},
		//机枪射手，双子向日葵，忧郁菇，香蒲，冰西瓜投手，吸金磁，地刺王，玉米加农炮
		{"jqss", "szxrk", "yyg", "xp", "bxgts", "xjc", "dcw", "ymjnp"},

		//以下为模仿者卡片名称
		//豌豆射手，向日葵，樱桃炸弹，坚果，土豆地雷，寒冰射手，大嘴花，双重射手
		{"Mwdss", "Mxrk", "Mytzd", "Mjg", "Mtddl", "Mhbss", "Mdzh", "Mscss"},
		//小喷菇，阳光菇，大喷菇，墓碑吞噬者，魅惑菇，胆小菇，寒冰菇，毁灭菇
		{"Mxpg", "Mygg", "Mdpg", "Mmbtsz", "Mmhg", "Mdxg", "Mhbg", "Mhmg"},
		//荷叶，倭瓜，三发射手，缠绕海藻，火爆辣椒，地刺，火炬树桩，高坚果
		{"Mhy", "Mwg", "Msfss", "Mcrhz", "Mhblj", "Mdc", "Mhjsz", "Mgjg"},
		//水兵菇，路灯花，仙人掌，三叶草，裂荚射手，杨桃，南瓜头，磁力菇
		{"Msbg", "Mldh", "Mxrz", "Msyc", "Mljss", "Myt", "Mngt", "Mclg"},
		//卷心菜投手，花盆，玉米投手，咖啡豆，大蒜，叶子保护伞，金盏花，西瓜投手
		{"Mjxcts", "Mhp", "Mymts", "Mkfd", "Mds", "Myzbhs", "Mjzh", "Mxgts"}};

std::map<std::string, int> AvZ::seed_name_to_index_map;
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