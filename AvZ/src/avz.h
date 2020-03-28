/*
 * @coding: utf-8
 * @Author: yuchenxi0_0 and vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: High-precision PVZ TAS Frameworks : Assembly vs. Zombies !
 * 				 The founder is yuchenxi0_0.
 * 				 The underlying implementation is completed by yuchenxi0_0.
 * 				 The interface compatible with other frameworks is completed by vector-wlc.
 */

#pragma once

// AvZ 版本号 当前版本 20_03_16
#define __AVZ_VERSION__ 200316

#include "pvzfunc.h"
#include <map>
#include <vector>
#include <functional>
#include <Windows.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <stack>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "scripts.h"

#define FindInAllRange(container, goal) std::find(container.begin(), container.end(), goal)

// @函数功能：判断数字范围
// @使用示例：
// RangeIn(wave, {1,2,3})------如果波数在 1 2 3 范围里返回 true
bool RangeIn(int num, std::initializer_list<int> lst);

// Assembly vs. Zombies
class AvZ
{
private:
	static HWND pvz_hwnd;
	static HANDLE pvz_handle;
	static PvZ *pvz_base;
	static MainObject *main_object;

	//读取内存函数
	template <typename T, typename... Args>
	static T read_memory(Args... args)
	{
		std::initializer_list<uintptr_t> lst = {static_cast<uintptr_t>(args)...};
		uintptr_t buff = 0;
		T result = T();
		for (auto it = lst.begin(); it != lst.end(); ++it)
			if (it != lst.end() - 1)
				ReadProcessMemory(pvz_handle, (const void *)(buff + *it), &buff, sizeof(buff), nullptr);
			else
				ReadProcessMemory(pvz_handle, (const void *)(buff + *it), &result, sizeof(result), nullptr);
		return result;
	}

	//改写内存函数
	template <typename T, typename... Args>
	static void write_memory(T value, Args... args)
	{
		std::initializer_list<uintptr_t> lst = {static_cast<uintptr_t>(args)...};
		uintptr_t buff = 0;
		for (auto it = lst.begin(); it != lst.end(); it++)
			if (it != lst.end() - 1)
				ReadProcessMemory(pvz_handle, (const void *)(buff + *it), &buff, sizeof(buff), nullptr);
			else
				WriteProcessMemory(pvz_handle, (void *)(buff + *it), &value, sizeof(value), nullptr);
	}

	static void init_address();

	// time operation queue

	// 该部分将使用者写的操作按照时间先后顺序录入消息队列，旧接口得以实现
public:
	struct TimeWave
	{
		int time;
		int wave;
		friend bool operator<(const TimeWave &tq1, const TimeWave &tq2)
		{
			if (tq1.wave == tq2.wave)
			{
				return tq1.time < tq2.time;
			}
			return tq1.wave < tq2.wave;
		}
		friend bool operator==(const TimeWave &tq1, const TimeWave &tq2)
		{
			return tq1.time == tq2.time && tq1.wave == tq2.wave;
		}
	};

private:
	static std::map<TimeWave, std::vector<std::function<void()>>> operation_queue; // 操作队列
	static TimeWave time_wave;													   // 操作
	static bool is_loaded;
	static bool is_multiple_effective;
	static bool is_exited;
	static void update_refresh_time();
	static bool is_time_arrived();

// 随时检测线程退出
#define exit_sleep(ms)     \
	do                     \
	{                      \
		int _ms = ms;      \
		do                 \
		{                  \
			if (is_exited) \
			{              \
				return;    \
			}              \
			Sleep(1);      \
		} while (--_ms);   \
	} while (false)

public:
	static void loadScript(const std::function<void()> func);
	static void run(MainObject *);
	static void exit() { is_exited = true; }
	static void openMultipleEffective() { is_multiple_effective = true; }

	// 设定操作时间点
	static void setTime(const TimeWave &_time_wave)
	{
		time_wave = _time_wave;
	}
	// 设定操作时间点
	// 使用示例：
	// setTime(-95, 1)------ 将操作时间点设为第一波僵尸刷新前 95cs
	// setTime(-95)--------- 将操作时间点设为僵尸刷新前 95cs, 波数由上一个最近确定的波数决定
	static void setTime(int time, int wave)
	{
		time_wave.time = time;
		time_wave.wave = wave;
	}
	// 设定操作时间点
	static void setTime(int time)
	{
		time_wave.time = time;
	}

	// 将操作插入操作队列中
	static void insertOperation(const std::function<void()> &operation);
	static void insertTimeOperation(const TimeWave &time_wave, const std::function<void()> &operation)
	{
		setTime(time_wave);
		insertOperation(operation);
	}
	static void insertTimeOperation(int time, int wave, const std::function<void()> &operation)
	{
		setTime(time, wave);
		insertOperation(operation);
	}
	// 得到当前时间
	static TimeWave nowTimeWave()
	{
		return {main_object->gameClock() - zombie_refresh.time, zombie_refresh.wave};
	}

	// 阻塞运行直到达到目标时间点
	// 使用方法与 setTime 相同
	static void waitUntil(const TimeWave &_time_wave);

	// 阻塞运行直到达到目标时间点
	// 使用方法与 setTime 相同
	static void waitUntil(int time, int wave = 0)
	{
		waitUntil({time, wave});
	}

	// debug
private:
	static void UTF8ToGBK(std::string &strUTF8);

	template <typename T>
	static void string_convert(std::string &content, T t)
	{
		std::stringstream conversion;
		conversion << t;
		content.replace(content.find_first_of('#'), 1, conversion.str());
	}
	// 该部分为调试功能部分，使用下面两个接口可以对脚本进行调试
public:
	// Not In Queue
	// 弹出窗口提示错误或者调试信息
	// 使用方法同 printf 但是其中的 %d %f 等需要替换为 #
	template <typename... Args>
	static void popErrorWindowNotInQueue(const std::string &content, Args... args)
	{
		std::string _content = "wave : # -- time : # \n\n" + content;
		auto it = operation_queue.begin();
		string_convert(_content, it->first.wave);
		string_convert(_content, it->first.time);
		std::initializer_list<int>{(string_convert(_content, args), 0)...};

		UTF8ToGBK(_content);

		MessageBoxA(NULL, _content.c_str(), "Error", 0);
	};

	// In Queue
	// 弹出窗口提示错误或者调试信息
	// 使用方法同 printf
	template <typename... Args>
	static void popErrorWindow(const std::string &content, Args... args)
	{
		insertOperation([=]() {
			popErrorWindowNotInQueue(content, args...);
		});
	};

	///////////////////////////////////////////////////////////
	// old apis (from Python vs. Zombies or C++ vs. Zombies) //
	///////////////////////////////////////////////////////////

	struct Grid
	{
		int row;
		int col;

		friend bool operator==(const Grid &grid1, const Grid &grid2)
		{
			return grid1.row == grid2.row && grid1.col == grid2.col;
		}

		friend bool operator<(const Grid &grid1, const Grid &grid2)
		{
			if (grid1.row == grid2.row)
			{
				return grid1.col < grid2.col;
			}
			return grid1.row < grid2.row;
		}
	};

	struct Crood
	{
		int row;
		float col;
	};

	// memory func api
private:
	// Not In Queue
	// 更新僵尸预览
	static void update_zombies_preview();

public:
	// Not In Queue
	// 游戏主要信息
	static MainObject *mainObject()
	{
		return main_object;
	}

	// Not In Queue
	// 游戏基址
	static PvZ *pvzBase()
	{
		return pvz_base;
	}

	// Not In Queue
	//返回鼠标所在行
	static int mouseRow()
	{
		return main_object->mouseExtraAttribution()->row() + 1;
	}

	// Not In Queue
	//返回鼠标所在列
	static float mouseCol()
	{
		return float(main_object->mouseAttribution()->abscissa() + 25) / 80;
	}

	// Not In Queue
	//获取指定类型植物的卡槽对象序列 植物类型与图鉴顺序相同，从0开始
	//返回的卡片对象序列范围：[0,9]
	//GetSeedIndex(16)------------获得荷叶的卡槽对象序列
	//GetSeedIndex(16, true)-------获得模仿者荷叶的卡槽对象序列
	static int getSeedIndex(int type, bool imtator = false);

	// Not In Queue
	//得到指定位置和类型的植物对象序列
	//当参数type为默认值-1时该函数无视南瓜花盆荷叶
	//使用示例：
	//GetPlantIndex(3, 4)------如果三行四列有除南瓜花盆荷叶之外的植物时，返回该植物的对象序列，否则返回-1
	//GetPlantIndex(3, 4, 47)---如果三行四列有春哥，返回其对象序列，如果有其他植物，返回-2，否则返回-1
	static int getPlantIndex(int row, int col, int type = -1);

	// Not In Queue
	//得到一组指定位置的植物的对象序列
	//参数1：填写一组指定位置
	//参数2：填写指定类型
	//参数3：得到对象序列，此函数按照位置的顺序填写对象序列
	//注意：如果没有植物填写-1，如果有植物但是不是指定类型，会填写-2
	static void getPlantIndexs(const std::vector<Grid> &lst_in_, int type, std::vector<int> &indexs_out_);

	// Not In Queue
	//检查僵尸是否存在
	//使用示例
	//isZombieExist()-------检查场上是否存在僵尸
	//isZombieExist(23)-------检查场上是否存在巨人僵尸
	//isZombieExist(-1,4)-----检查第四行是否有僵尸存在
	//isZombieExist(23,4)-----检查第四行是否有巨人僵尸存在
	static bool isZombieExist(int type = -1, int row = -1);

	// In Queue
	//启动女仆秘籍
	static void startMaidCheats()
	{
		insertOperation([=]() {
			write_memory<byte>(0x71, 0x52DFE8);
		});
	}

	// In Queue
	//停止女仆秘籍
	static void stopMaidCheats()
	{
		insertOperation([=]() {
			write_memory<byte>(0x7F, 0x52DFE8);
		});
	}

	// Not In Queue
	//设置出怪 此函数不管填不填蹦极都会在 wave 10 20 刷蹦极！！！！！！！！！！！！
	//参数命名规则：前面两个字母与 PvZ Tools 出怪类型名称拼音首字母一致，后面接下划线和类型代号
	//例如 铁桶命名为 TT_4
	//使用示例：
	//AvZ::setZombies({CG_3, TT_4, BC_12, XC_15, QQ_16, FT_21, TL_22, BY_23, HY_32, TT_18});
	//设置出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
	//AvZ::setZombies({TT_4, BC_12, BC_12});
	//设置出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
	static void setZombies(std::initializer_list<int> zombie_type);

	// Not In Queue
	//设置特定波出怪 此函数不管填不填蹦极都会在 wave 10 20 刷蹦极！！！！！！！！！！！！
	//参数命名规则：前面两个字母与 PvZ Tools 出怪类型名称拼音首字母一致，后面接下划线和类型代号
	//例如 铁桶命名为 TT_4
	//使用示例：
	//AvZ::setZombies(1, {CG_3, TT_4, BC_12, XC_15, QQ_16, FT_21, TL_22, BY_23, HY_32, TT_18});
	//设置第一波出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
	//AvZ::setZombies(1, {TT_4, BC_12, BC_12});
	//设置第一波出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
	static void setWaveZombies(int wave, std::initializer_list<int> zombie_type);

	// time api
private:
	static TimeWave zombie_refresh;

public:
	static void delay(int time)
	{
		time_wave.time += time;
	}

	// In Queue
	//冰三函数
	//使用示例：
	//Ice3(298) --------- 修正冰三时间点至当前时刻的 298cs 后
	static void ice3(int time);

	// click api
public:
	// Not In Queue
	// 点击格子
	// 使用示例：
	// clickGrid(3, 4)---- 点击格子(3, 4)
	// clickGrid(3, 4, 10)---- 向下偏移10像素点击格子(3, 4)
	static void clickGrid(int row, float col, int offset = 0);

	// Not In Queue
	// 点击种子/卡片
	// 使用示例：
	// clickSeed(1) ----- 点击第一个种子
	static void clickSeed(int seed_index) { click_scene(main_object, 50 + 50 * seed_index, 70, 1); }

	// Not In Queue
	//右键安全点击
	static void safeClick() { click_scene(main_object, 1, 1, -1); }

	// Not In Queue
	// 鼠标左击
	// leftClick(400, 300)-----点击 PVZ 窗口中央
	static void leftClick(int x, int y) { click_scene(main_object, x, y, 1); }

	static void shovelNotInQueue(int row, float col, bool pumpkin = false);

	// In Queue
	//铲除植物函数
	//使用示例：
	//Shovel(4, 6)--------铲除4行6列的植物,如果植物有南瓜保护默认铲除被保护植物
	//Shovel(4, 6, true)---铲除4行6列的植物,如果植物有南瓜保护铲除南瓜
	//Shovel({{3, 6},{4, 6}})------铲除3行6列，4行6列的植物
	static void shovel(int row, float col, bool pumpkin = false)
	{
		insertOperation([=]() {
			shovelNotInQueue(row, col, pumpkin);
		});
	}
	// In Queue
	static void shovel(const std::vector<Crood> &lst)
	{
		insertOperation([=]() {
			for (const auto &crood : lst)
			{
				shovelNotInQueue(crood.row, crood.col);
			}
		});
	}

	// card api
private:
	static bool is_get_seed_index;
	static std::string seed_name_list[11][8];
	static std::map<std::string, int> seed_name_to_index_map;
	//为卡片名称变量获取卡片对象序列
	static int get_seed_index_for_seed_name(const std::string &seed_name);
	//防误触
	static void deal_wrong_click();
	static void choose_card(int row, int col);
	static void click_btn(int x, int y, int t_ms = 0);
	static void lets_rock();

public:
	struct CardIndex
	{
		int seed_index;
		int row;
		float col;
	};
	struct CardName
	{
		std::string seed_name;
		int row;
		float col;
	};

	// 选择一堆卡片 Not In Queue
	// 请注意与 CvZ 书写形式的不同！！！！！！
	// 使用示例：
	// AvZ::selectCards({"hbg", "Mhbg", "kfd", "hmg", "hy", "wg", "ytzd", "syc", "ngt", "xpg"});
	///////////////////// 寒冰菇 模仿寒冰菇 咖啡豆 毁灭菇  荷叶   倭瓜  樱桃炸弹  三叶草  南瓜头  小喷菇
	static void selectCards(const std::vector<std::string> &lst);

	//用卡函数 Not In Queue
	//使用示例：
	//cardNotInQueue(1, 2, 3)---------选取第1张卡片，放在2行,3列
	//cardNotInQueue(1, {{2, 3}, {3, 4}})--------选取第1张卡片，优先放在2行,3列，其次放在3行,4列
	static void cardNotInQueue(int seed_index, int row, float col);
	static void cardNotInQueue(int seed_index, const std::vector<Crood> &lst);

	//用卡函数 In Queue
	//使用示例：
	//card(1, 2, 3)---------选取第1张卡片，放在2行,3列
	//card({{1, 2, 3}, {2, 3, 4}})------选取第1张卡片，放在2行,3列，选取第2张卡片，放在3行,4列
	//card(1, {{2, 3}, {3, 4}})--------选取第1张卡片，优先放在2行,3列，其次放在3行,4列
	//以下用卡片名称使用 card,卡片名称为拼音首字母，具体参考pvz_data.h的命名
	//card({{"ytzd", 2, 3}, {"Mhblj", 3, 4}})---------选取樱桃卡片，放在2行,3列，选取辣椒卡片，放在3行,4列
	static void card(const std::vector<CardIndex> &lst);
	static void card(const std::vector<CardName> &lst);
	static void card(int seed_index, int row, float col);
	static void card(int seed_index, const std::vector<Crood> &lst);
	static void card(const std::string &seed_name, int row, float col);
	static void card(const std::string &seed_name, const std::vector<Crood> &lst);

	// thread
private:
	struct ThreadInfo
	{
		std::function<void()> func;
		int *id_ptr;
	};
	static std::vector<ThreadInfo> thread_vec;
	static std::stack<int> stoped_thread_id_stack;

public:
	// CLASS VThread by vector-wlc
	class VThread
	{
	protected:
		int thread_id = -1;
		bool is_paused = false;
		bool thread_examine() // 线程出现异常返回 false
		{
			if (thread_id >= 0)
			{
				popErrorWindowNotInQueue("一个自动线程类不允许同时拥有两个线程！");
				return false;
			}
			return true;
		}

	public:
		// Not In Queue
		void pushFunc(const std::function<void()> &run);

		// In Queue
		void stop()
		{
			insertOperation([=]() {
				stoped_thread_id_stack.push(thread_id);
				thread_id = -1;
			});
		}

		// In Queue
		void pause()
		{
			insertOperation([=]() {
				is_paused = true;
			});
		}
		// In Queue
		void goOn()
		{
			insertOperation([=]() {
				is_paused = false;
			});
		}
	};

	class ItemCollector : public VThread
	{
	private:
		int time_interval = 10;
		void run();

	public:
		// In Queue
		void start()
		{
			insertOperation([=]() {
				if (!thread_examine())
				{
					return;
				}
				is_paused = false;
				pushFunc([=]() { run(); });
			});
		}
		// In Queue
		void setInterval(int _time_interval)
		{
			insertOperation([=]() {
				this->time_interval = _time_interval;
			});
		}
	};

	class IceFiller : public VThread
	{
	private:
		std::vector<Grid> fill_ice_grid_vec;
		std::vector<int> ice_seed_index_vec;
		int coffee_seed_index;
		void run();

	public:
		// In Queue
		//重置存冰位置
		//使用示例：
		//resetFillList({{3,4},{5,6}})-----将存冰位置重置为{3，4}，{5，6}
		void resetFillList(const std::vector<Grid> &lst)
		{
			insertOperation([=]() {
				this->fill_ice_grid_vec = lst;
			});
		}

		// In Queue
		//线程开始工作
		//使用示例：
		//start({{3,4},{5,6}})-----在{3，4}，{5，6}位置存冰
		void start(const std::vector<Grid> &lst);

		// In Queue
		//使用咖啡豆函数
		//使用示例：
		//coffee()-----自动使用优先级低的存冰位
		void coffee();
	};

	class PlantFixer : public VThread
	{

	private:
		bool is_use_coffee = false;
		int plant_type;
		int fix_hp = 0;
		int coffee_seed_index;
		int leaf_seed_index;
		std::vector<int> seed_index_vec;
		std::vector<Grid> grid_lst;
		void get_seed_list();
		void run();
		bool use_seed_(int seed_index, int row, float col, bool is_need_shovel);

	public:
		// In Queue
		//重置植物修补位置
		//使用示例：
		//resetFixList({{2, 3},{3, 4}})-------位置被重置为{2，3}，{3，4}
		void resetFixList(const std::vector<Grid> &lst)
		{
			insertOperation([=]() {
				grid_lst = lst;
			});
		}
		// In Queue
		//自动得到修补的位置列表
		void autoGetFixList();

		// In Queue
		//线程开始工作，此函数开销较大，不建议多次调用
		//第一个参数为植物类型
		//第二个参数不填默认全场
		//第三个参数不填默认植物血量为150以下时修补
		//使用示例：
		//start(23)-------修补全场的高坚果
		//start(30,{{1,3},{2,3}})-----修补位置为{1，3}，{2，3}位置的南瓜头
		//start(3,{{1,3},{2,3}},300)------修补位置为{1，3}，{2，3}位置的坚果，血量降至300开始修补
		void start(int _plant_type, const std::vector<Grid> &lst = {}, int _fix_hp = 150);

		// In Queue
		//重置修补血量
		//使用示例：
		//resetFixHp(200)------将修补触发血量改为200
		void resetFixHp(int _fix_hp)
		{
			insertOperation([=]() {
				fix_hp = _fix_hp;
			});
		}

		// In Queue
		//是否使用咖啡豆
		void isUseCoffee(bool _is_use_coffee)
		{
			insertOperation([=]() {
				is_use_coffee = _is_use_coffee;
			});
		}
	};

	class KeyConnector : public VThread
	{
	private:
		std::vector<std::pair<char, std::function<void()>>> key_operation_vec;

	public:
		// Not In Queue
		// 添加操作
		// 不论此函数在何时被调用，指令全局有效！
		void add(char key, std::function<void()> operate);

		void clear()
		{
			key_operation_vec.clear();
		}
	};

	// pao api
public:
	class PaoOperator
	{
	public:
		//记录炮的信息
		struct PaoInfo
		{
			int row;					 //所在行
			int col;					 //所在列
			int recover_time;			 //恢复时间
			int index;					 //炮的对象序列
			int vec_index;				 //炮所在 vector 的索引
			bool is_in_list = false;	 //记录是否在炮列表内
			bool is_in_sequence = false; //记录是否被炮序限制
			friend bool operator==(const PaoInfo &pi1, const PaoInfo &pi2)
			{
				return pi1.row == pi2.row && pi1.col == pi2.col;
			}

			friend bool operator==(const PaoInfo &pi1, const Grid &pi2)
			{
				return pi1.row == pi2.row && pi1.col == pi2.col;
			}

			friend bool operator<(const PaoInfo &pi1, const PaoInfo &pi2)
			{
				if (pi1.row == pi2.row)
				{
					return pi1.col < pi2.col;
				}
				return pi1.row < pi2.row;
			}
		};

		//用于RAWPAO函数
		struct PaoDrop
		{
			int pao_row;
			int pao_col;
			int drop_row;
			float drop_col;
		};

		//记录炮的位置和落点的位置及炮弹飞行时间
		struct RoofPaoDrop
		{
			int vec_index;
			int drop_row;
			float drop_col;
			int fire_time;
		};

		//屋顶炮飞行时间辅助数据
		struct RoofFlyTime
		{
			int min_drop_x;	  //记录该列炮最小飞行时间对应的最小的横坐标
			int min_fly_time; //记录最小的飞行时间
		};

		struct LastestPaoMsg
		{
			int vec_index = -1;
			int fire_time = 0;
			bool is_writable = true;
		};

	private:
		static int next_vec_index;
		static std::vector<PaoInfo> all_pao_vec; //所有炮的信息
		std::vector<int> pao_list;				 //炮列表，记录炮的信息
		int next_pao;							 //记录当前即将发射的下一门炮
		bool limit_pao_sequence = true;			 //是否限制炮序
		static LastestPaoMsg lastest_pao_msg;	 //最近一颗发炮的信息
		static VThread vthread;
		static RoofFlyTime fly_time_data[8];
		//录入新的一门炮
		static void new_pao(PaoInfo &np);
		//删除一门炮
		static void delete_pao(std::vector<PaoInfo>::iterator &it)
		{
			std::swap(next_vec_index, it->vec_index);
		}
		static void delete_pao(int index)
		{
			std::swap(next_vec_index, all_pao_vec[index].vec_index);
		}
		//炮是否存在
		static bool is_exist(std::vector<PaoInfo>::iterator &it)
		{
			return (it - all_pao_vec.begin()) == it->vec_index;
		}
		static bool is_exist(int index)
		{
			return index == all_pao_vec[index].vec_index;
		}
		//禁用 = 运算符
		void operator=(PaoOperator) {}
		//对炮进行一些检查
		static void pao_examine(int vec_index, int drop_row, float drop_col);
		//检查落点
		static bool is_drop_conflict(int pao_row, int pao_col, int drop_row, float drop_col);
		//基础发炮函数
		static void base_fire_pao(int vec_index, int drop_row, float drop_col);
		//获取屋顶炮飞行时间
		static int get_roof_fly_time(int pao_col, float drop_col);
		//延迟发炮
		static void delay_fire_pao(int vec_index, int delay_time, int row, float col);
		//更新最近发炮的信息
		static void update_lastest_pao_msg(int fire_time, int index)
		{
			if (lastest_pao_msg.is_writable)
			{
				lastest_pao_msg.fire_time = fire_time;
				lastest_pao_msg.vec_index = index;
			}
		}
		//跳过一定数量的炮
		void skip_pao(int x)
		{
			next_pao = (next_pao + x) % pao_list.size();
		}

	public:
		// 炮信息初始化
		static void initPaoMessage();

		// In Queue
		// 更新炮的信息
		// 炮的信息对于 roofPao 系列函数十分重要！
		// 使用示例：
		// updatePaoMessage({{2, 3}, {3, 4}}) ------- 更新 {2, 3}, {3, 4} 位置炮的信息
		static void updatePaoMessage(const std::vector<Grid> &lst);

		// In Queue
		//发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
		//注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与UpdatePaolist更新的炮序将无效！
		//使用示例：
		//rawRoofPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
		//rawRoofPao({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
		static void rawRoofPao(int pao_row, int pao_col, int drop_row, float drop_col);

		// In Queue
		//发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
		//注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与UpdatePaolist更新的炮序将无效！
		//使用示例：
		//rawRoofPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
		//rawRoofPao({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
		static void rawRoofPao(const std::vector<PaoDrop> &lst);

		// In Queue
		//发炮函数：用户自定义位置发射
		//注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
		//使用示例：
		//rawPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
		//rawPao({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
		static void rawPao(int pao_row, int pao_col, int drop_row, float drop_col);

		// In Queue
		//发炮函数：用户自定义位置发射
		//注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
		//使用示例：
		//rawPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
		//rawPao({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
		static void rawPao(const std::vector<PaoDrop> &lst);

		// In Queue
		//种植炮函数
		//使用示例
		//plantPao(3, 4)------在三行四列位置种炮
		static void plantPao(int row, int col);

		// In Queue
		// 立即修补上一枚已经发射的炮
		static void fixLatestPao();

		PaoOperator();
		~PaoOperator();

		//////////////////////////////////////////// 模式设定成员

		// In Queue
		//设置炮序限制 参数为 false 则解除炮序限制，true 则增加炮序限制
		//解除此限制后 fixPao 可铲种炮列表内的炮，tryPao 系列可使用， Pao 系列不可使用
		//增加此限制后 fixPao 不可铲种炮列表内的炮，tryPao 系列不可使用， Pao 系列可使用
		void setLimitPaoSequence(bool limit);

		/////////////////////////////////////////// 下面是关于限制炮序的相关成员

		// In Queue
		//设置即将发射的下一门炮
		//此函数只有在限制炮序的时候才可调用
		//使用示例：
		//setNextPao(10)------将炮列表中第十门炮设置为下一门即将发射的炮
		//setNextPao(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
		void setNextPao(int next_pao);

		// In Queue
		//设置即将发射的下一门炮
		//此函数只有在限制炮序的时候才可调用
		//使用示例：
		//setNextPao(10)------将炮列表中第十门炮设置为下一门即将发射的炮
		//setNextPao(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
		void setNextPao(int row, int col);

		// In Queue
		//跳炮函数
		//使用示例：
		//skipao(2)---跳过按照顺序即将要发射的2门炮
		void skipPao(int x)
		{
			insertOperation([=]() {
				next_pao = (next_pao + x) % pao_list.size();
			});
		}

		// In Queue
		//发炮函数
		//使用示例：
		//pao(2,9)----------------炮击二行，九列
		//pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void pao(int row, float col);

		// In Queue
		//发炮函数
		//使用示例：
		//pao(2,9)----------------炮击二行，九列
		//pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void pao(const std::vector<Crood> &lst);

		// In Queue
		//发炮函数 炮CD恢复自动发炮
		//使用示例：
		//recoverPao(2,9)----------------炮击二行，九列
		//recoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void recoverPao(int row, float col);

		// In Queue
		//发炮函数 炮CD恢复自动发炮
		//使用示例：
		//recoverPao(2,9)----------------炮击二行，九列
		//recoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void recoverPao(const std::vector<Crood> &lst);

		// In Queue
		//屋顶修正飞行时间发炮. 此函数开销较大不适合精确键控.
		//此函数只适用于RE与ME 修正时间：387cs
		//使用示例：
		//roofPao(3,7)---------------------修正飞行时间后炮击3行7列
		//roofPao({ {2,9},{5,9} })---------修正飞行时间后炮击2行9列,5行9列
		void roofPao(int row, float col);

		// In Queue
		//屋顶修正飞行时间发炮. 此函数开销较大不适合精确键控.
		//此函数只适用于RE与ME 修正时间：387cs
		//使用示例：
		//roofPao(3,7)---------------------修正飞行时间后炮击3行7列
		//roofPao({ {2,9},{5,9} })---------修正飞行时间后炮击2行9列,5行9列
		void roofPao(const std::vector<Crood> &lst);

		//////////////////////////////////////// 下面是不限制炮序能够使用的成员

		// In Queue
		//自动找炮函数
		//使用示例
		//tryPao(2,9)----------------在炮列表中找到可用的炮后，炮击二行，九列
		//tryPao({{2, 9}, {5, 9}})-----在炮列表中找到可用的炮后，炮击二行，九列，五行，九列
		void tryPao(int row, float col);

		// In Queue
		//自动找炮函数
		//使用示例
		//tryPao(2,9)----------------在炮列表中找到可用的炮后，炮击二行，九列
		//tryPao({{2, 9}, {5, 9}})-----在炮列表中找到可用的炮后，炮击二行，九列，五行，九列
		void tryPao(const std::vector<Crood> &lst);

		// In Queue
		//自动找炮函数
		//使用示例
		//tryRoofPao(2,9)----------------在炮列表中找到可用的炮后，炮击二行，九列
		//tryRoofPao({{2, 9},{5, 9}})-----在炮列表中找到可用的炮后，炮击二行，九列，五行，九列
		void tryRoofPao(int row, float col);

		// In Queue
		//自动找炮函数
		//使用示例
		//tryRoofPao(2,9)----------------在炮列表中找到可用的炮后，炮击二行，九列
		//tryRoofPao({{2, 9},{5, 9}})-----在炮列表中找到可用的炮后，炮击二行，九列，五行，九列
		void tryRoofPao(const std::vector<Crood> &lst);

		// In Queue
		//发炮函数 炮CD恢复自动尝试发炮
		//使用示例：
		//tryRecoverPao(2,9)----------------炮击二行，九列
		//tryRecoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void tryRecoverPao(int row, float col);

		// In Queue
		//发炮函数 炮CD恢复自动尝试发炮
		//使用示例：
		//tryRecoverPao(2,9)----------------炮击二行，九列
		//tryRecoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void tryRecoverPao(const std::vector<Crood> &lst);

		////////////////////////////////// 下面是不受模式限制使用的成员

		// In Queue
		//重置炮列表
		//使用示例:
		//resetPaoList({{3, 1},{4, 1},{3, 3},{4, 3}})-------经典四炮
		void resetPaoList(const std::vector<Grid> &lst);
	};
};

extern AvZ avz;
extern AvZ::ItemCollector item_collector;
extern AvZ::IceFiller ice_filler;
extern AvZ::PlantFixer plant_fixer;
extern AvZ::PaoOperator pao_operator;
extern AvZ::KeyConnector key_connector;

// 为了方便书写搞一些宏 /无奈

#define Prejudge AvZ::setTime
#define Until AvZ::setTime
#define Delay AvZ::delay
#define Card AvZ::card
#define Shovel AvZ::shovel
#define Ice3 AvZ::ice3
#define KeyConnect key_connector.add
#define MouseRow AvZ::mouseRow
#define MouseCol AvZ::mouseCol
#define SetTime AvZ::setTime
#define InsertOperation AvZ::insertOperation
#define InsertTimeOperation AvZ::insertTimeOperation
#define WaitUntil AvZ::waitUntil
#define NowTimeWave AvZ::nowTimeWave
#define StartMaidCheats AvZ::startMaidCheats
#define StopMaidCheats AvZ::stopMaidCheats
#define SelectCards AvZ::selectCards
#define SetZombies AvZ::setZombies
#define SetWaveZombies AvZ::setWaveZombies
#define OpenMultipleEffective AvZ::openMultipleEffective