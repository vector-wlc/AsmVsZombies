/*
 * *** coding: utf-8
 * *** Author: yuchenxi0_0 and vector-wlc
 * *** Date: 2020-02-06 10:22:46
 * *** Description: High-precision PVZ TAS Frameworks : Assembly vs. Zombies !
 * 				 The founder is yuchenxi0_0.
 * 				 The underlying implementation is completed by yuchenxi0_0.
 * 				 The interface compatible with other frameworks is completed by vector-wlc.
 */

#pragma once
#ifndef __LIBAVZ_H__
#define __LIBAVZ_H__

// AvZ 版本号 当前版本 20_08_10
#define __AVZ_VERSION__ 200810

#include <map>
#include <cstdlib>
#include <vector>
#include <functional>
#include <Windows.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <stack>
#include <thread>
#include <set>
#include <initializer_list>
#include "pvzfunc.h"

#define FindInAllRange(container, goal) std::find(container.begin(), container.end(), goal)

// *** 函数功能：判断数字范围
// *** 使用示例：
// RangeIn(wave, {1,2,3})------如果波数在 1 2 3 范围里返回 true
bool RangeIn(int num, std::initializer_list<int> lst);

// 寻找 vector 中相同的元素，返回其迭代器
template <typename Ele>
auto FindSameEle(const std::vector<Ele> &container, const Ele &ele_)
	-> std::vector<decltype(container.begin())>
{
	std::vector<decltype(container.begin())> result;
	for (auto it = container.begin(); it != container.end(); ++it)
	{
		if ((*it) == ele_)
		{
			result.push_back(it);
		}
	}

	return result;
}

// 寻找 vector 中相同的元素，返回其迭代器
template <typename Ele>
auto FindSameEle(std::vector<Ele> &container, const Ele &ele_)
	-> std::vector<decltype(container.begin())>
{
	std::vector<decltype(container.begin())> result;
	for (auto it = container.begin(); it != container.end(); ++it)
	{
		if ((*it) == ele_)
		{
			result.push_back(it);
		}
	}

	return result;
}

// Assembly vs. Zombies
class AvZ
{
private:
	static HWND pvz_hwnd;
	static HANDLE pvz_handle;
	static PvZ *pvz_base;
	static MainObject *main_object;

	// 读取内存函数
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

	// 改写内存函数
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
	// 使用 20 条操作队列，每一条操作队列储存一波的操作
	// 需要每帧进行僵尸刷新时间的读取，普通波 <= 200，wave1 : <=600   wave10||20: 750
	// 当队列记录的刷新时间点为 -1 时，continue
public:
	struct TimeWave
	{
		int time;
		int wave;
	};

	struct WaveTime
	{
		int wave;
		int time;
	};

	struct Operation
	{
		std::function<void()> operation;
		std::string description;
	};

	struct OperationQueue
	{
		int refresh_time = -1;
		int wave_length = -1;
		std::map<int, std::vector<Operation>> queue;
		bool is_time_arrived()
		{
			if (queue.begin()->first < main_object->gameClock() - refresh_time)
			{
				showErrorNotInQueue("您设定时间为 #cs, 但当前时间已到 #cs, 按下确定将以当前时间执行此次操作",
									queue.begin()->first,
									main_object->gameClock() - refresh_time);
			}
			return queue.begin()->first <= main_object->gameClock() - refresh_time;
		}
	};

private:
	static std::vector<OperationQueue> operation_queue_vec; // 操作队列
	static TimeWave time_wave_insert;						// 插入操作时间记录
	static TimeWave time_wave_run;							// 运行操作时间记录
	static bool is_loaded;
	static bool is_multiple_effective;
	static bool is_exited;
	static bool is_insert_operation;
	static bool block_var; // 全局阻塞变量 true : 阻塞 script 线程 false : 阻塞游戏循环
	static std::vector<OperationQueue>::iterator wavelength_it;
	static void update_refresh_time();

// 随时检测线程退出
#define exit_sleep(ms)                                          \
	do                                                          \
	{                                                           \
		int _ms = ms;                                           \
		do                                                      \
		{                                                       \
			if (is_exited)                                      \
			{                                                   \
				SetWindowTextA(pvz_hwnd, "Plants vs. Zombies"); \
				return;                                         \
			}                                                   \
			Sleep(1);                                           \
		} while (--_ms);                                        \
	} while (false)

public:
	static void
	loadScript(const std::function<void()> func);
	static void run(MainObject *, std::function<void()> Script);
	static void exit()
	{
		SetWindowTextA(pvz_hwnd, "Plants vs. Zombies");
		is_exited = true;
	}

	// *** Not In Queue
	// 使用此函数可使脚本一次注入多次运行
	// 适用于脚本完全无误后录制视频使用
	// *** 注意：使用的关闭效果按键必须在战斗界面才会生效
	// *** 使用示例
	// AvZ::openMultipleEffective() -------- 脚本多次生效，默认按下 C 键取消此效果
	// AvZ::openMultipleEffective('Q')-------  脚本多次生效，按下 Q 键取消此效果
	static void openMultipleEffective(char close_key = 'C');

	// 设定操作时间点
	static void setTime(const TimeWave &_time_wave)
	{
		time_wave_insert = _time_wave;
	}

	// 设定操作时间点
	// *** 使用示例：
	// setTime(-95, 1)------ 将操作时间点设为第一波僵尸刷新前 95cs
	// setTime(-95)--------- 将操作时间点设为僵尸刷新前 95cs, 波数由上一个最近确定的波数决定
	static void setTime(int time, int wave)
	{
		time_wave_insert.time = time;
		time_wave_insert.wave = wave;
	}

	// 设定操作时间点
	static void setTime(int time)
	{
		time_wave_insert.time = time;
	}

	// 延迟一定时间
	// *** 注意由于操作队列的优势，此函数支持负值
	// *** 使用示例：
	// delay(298) ------ 延迟 298cs
	// delay(-298) ------ 提前 298cs
	static void delay(int time)
	{
		time_wave_insert.time += time;
	}

	// 阻塞运行直到达到目标时间点
	// 使用方法与 setTime 相同
	static void waitUntil(const TimeWave &_time_wave);
	static void waitUntil(int time, int wave)
	{
		waitUntil({time, wave});
	}

	// 得到当前时间，读取失败返回 -1
	// *** 注意得到的是以参数波刷新时间点为基准的相对时间
	// *** 使用示例：
	// AvZ::nowTime(1) -------- 得到以第一波刷新时间点为基准的当前时间
	// AvZ::nowTime(2) -------- 得到以第二波刷新时间点为基准的当前时间
	static int nowTime(int wave)
	{
		if (operation_queue_vec[wave - 1].refresh_time == -1)
		{
			showErrorNotInQueue("第 # 波刷新时间未知", wave);
			return -1;
		}
		return main_object->gameClock() - operation_queue_vec[wave - 1].refresh_time;
	}

	// 将操作插入操作队列中
	static void insertOperation(const std::function<void()> &operation, const std::string &description = "unknown");
	static void insertTimeOperation(const TimeWave &time_wave, const std::function<void()> &operation, const std::string &description = "unknown")
	{
		setTime(time_wave);
		insertOperation(operation, description);
	}
	static void insertTimeOperation(int time, int wave, const std::function<void()> &operation, const std::string &description = "unknown")
	{
		setTime(time, wave);
		insertOperation(operation, description);
	}

	// 设置 insertOperation 属性函数
	// *** 使用示例：
	// AvZ::setInsertOperation(false) ---- insertOperation 将不会把操作插入操作队列中
	// AvZ::setInsertOperation(true) ---- insertOperation 将会把操作插入操作队列中
	static void setInsertOperation(bool _is_insert_operation = true)
	{
		is_insert_operation = _is_insert_operation;
	}

	// *** In Queue
	// 调试功能：显示操作队列中当前时刻及以后操作
	// *** 使用示例
	// showQueue({1, 2, 3}) ----- 显示第 1 2 3 波中未被执行的操作
	static void showQueue(const std::vector<int> &lst);

	// debug
private:
	static void utf8_to_gbk(std::string &strUTF8);

	template <typename T>
	static void string_convert(std::string &content, T t)
	{
		std::stringstream conversion;
		conversion << t;
		content.replace(content.find_first_of('#'), 1, conversion.str());
	}

	static int error_mode;

	// 该部分为调试功能部分，使用下面两个接口可以对脚本进行调试
public:
	enum ErrorMode
	{
		POP_WINDOW,
		PVZ_TITLE,
		NONE
	};

	// *** Not In Queue
	// 设置错误提示方式
	// *** 使用示例：
	// AvZ::setErrorMode(AvZ::POP_WINDOW)----- 报错将会有弹窗弹出
	// AvZ::setErrorMode(AvZ::PVZ_TITLE)----- 报错将会显示在 PvZ 窗口标题
	// AvZ::setErrorMode(AvZ::NONE)----- 取消报错功能
	static void setErrorMode(int _error_mode)
	{
		error_mode = _error_mode;
	}

	// *** Not In Queue
	// 弹出窗口提示错误或者调试信息
	// 使用方法同 printf 但是其中的 %d %f 等需要替换为 #
	template <typename... Args>
	static void showErrorNotInQueue(const std::string &content = "", Args... args)
	{
		if (error_mode == NONE)
		{
			return;
		}
		std::string _content;
		if (time_wave_run.wave == 0)
		{
			_content = "操作录入状态  \n\n" + content;
		}
		else
		{
			_content = "wave : # -- time : #   \n\n" + content;
			string_convert(_content, time_wave_run.wave);
			string_convert(_content, time_wave_run.time);
		}

		std::initializer_list<int>{(string_convert(_content, args), 0)...};

		utf8_to_gbk(_content);
		if (error_mode == POP_WINDOW)
		{
			MessageBoxA(NULL, _content.c_str(), "Error", 0);
		}

		if (error_mode == PVZ_TITLE)
		{
			SetWindowTextA(pvz_hwnd, _content.c_str());
		}
	};

	// *** In Queue
	// 弹出窗口提示错误或者调试信息
	// 使用方法同 printf 但是其中的 %d %f 等需要替换为 #
	template <typename... Args>
	static void showError(const std::string &content = "", Args... args)
	{
		insertOperation([=]() {
			showErrorNotInQueue(content, args...);
		});
	};

	// structs
public:
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
	// *** Not In Queue
	// 更新僵尸预览
	static void update_zombies_preview();

public:
	// *** Not In Queue
	// 游戏主要信息
	static MainObject *mainObject()
	{
		return main_object;
	}

	// *** Not In Queue
	// 游戏基址
	static PvZ *pvzBase()
	{
		return pvz_base;
	}

	// *** Not In Queue
	// 返回鼠标所在行
	static int mouseRow()
	{
		return main_object->mouseExtraAttribution()->row() + 1;
	}

	// *** Not In Queue
	// 返回鼠标所在列
	static float mouseCol()
	{
		return float(main_object->mouseAttribution()->abscissa() + 25) / 80;
	}

	// *** Not In Queue
	// 获取指定类型植物的卡槽对象序列 植物类型与图鉴顺序相同，从0开始
	// 返回的卡片对象序列范围：[0,9]
	// getSeedIndex(16)------------获得荷叶的卡槽对象序列
	// getSeedIndex(16, true)-------获得模仿者荷叶的卡槽对象序列
	static int getSeedIndex(int type, bool imtator = false);

	// *** Not In Queue
	// 得到指定位置和类型的植物对象序列
	// 当参数type为默认值-1时该函数无视南瓜花盆荷叶
	// *** 使用示例：
	// getPlantIndex(3, 4)------如果三行四列有除南瓜花盆荷叶之外的植物时，返回该植物的对象序列，否则返回-1
	// getPlantIndex(3, 4, 47)---如果三行四列有春哥，返回其对象序列，如果有其他植物，返回-2，否则返回-1
	static int getPlantIndex(int row, int col, int type = -1);

	// *** Not In Queue
	// 得到一组指定位置的植物的对象序列
	// 参数1：填写一组指定位置
	// 参数2：填写指定类型
	// 参数3：得到对象序列，此函数按照位置的顺序填写对象序列
	// *** 注意：如果没有植物填写-1，如果有植物但是不是指定类型，会填写-2
	static void getPlantIndexs(const std::vector<Grid> &lst_in_, int type, std::vector<int> &indexs_out_);

	// *** Not In Queue
	// 检查僵尸是否存在
	// *** 使用示例
	// isZombieExist()-------检查场上是否存在僵尸
	// isZombieExist(23)-------检查场上是否存在巨人僵尸
	// isZombieExist(-1,4)-----检查第四行是否有僵尸存在
	// isZombieExist(23,4)-----检查第四行是否有巨人僵尸存在
	static bool isZombieExist(int type = -1, int row = -1);

	// *** Not In Queue
	// 设定特定波的波长
	// *** 注意： wave 9 19 20 无法设定波长
	// 波长的设定范围为 601 - 2500
	// *** 使用示例：
	// AvZ::setWavelength({{1, 601}, {4, 1000}}) ----- 将第一波的波长设置为 601，将第四波的波长设置为 1000
	static void setWavelength(const std::vector<WaveTime> &lst);

	// 女仆秘籍
	class MaidCheats
	{
	public:
		// *** In Queue
		// 召唤舞伴
		// 舞王不前进且每帧尝试召唤舞伴
		static void callPartner()
		{
			insertOperation([=]() {
				write_memory<uint32_t>(0x00F0B890, 0x52DFC9);
			},
							"MaidCheats::callPartner");
		}

		// *** In Queue
		// 跳舞
		// 舞王不前进且不会召唤舞伴
		static void dancing()
		{
			insertOperation([=]() {
				write_memory<uint32_t>(0x0140B890, 0x52DFC9);
			},
							"MaidCheats::dancing");
		}

		// *** In Queue
		// 保持前进
		// 舞王一直前进
		static void move()
		{
			insertOperation([=]() {
				write_memory<uint32_t>(0x00E9B890, 0x52DFC9);
			},
							"MaidCheats::move");
		}

		// *** In Queue
		// 停止女仆秘籍
		// 恢复游戏原样
		static void stop()
		{
			insertOperation([=]() {
				write_memory<uint32_t>(0x838808B, 0x52DFC9);
			},
							"MaidCheats::stop");
		}
	};

	// *** Not In Queue
	// 设置出怪 此函数不管填不填蹦极都会在 wave 10 20 刷蹦极！！！！！！！！！！！！
	// 参数命名规则：前面两个字母与 PvZ Tools 出怪类型名称拼音首字母一致，后面接下划线和类型代号
	// 详情见 pvzstruct.h 页尾
	// 例如 铁桶命名为 TT_4
	// *** 使用示例：
	// AvZ::setZombies({CG_3, TT_4, BC_12, XC_15, QQ_16, FT_21, TL_22, BY_23, HY_32, TT_18});
	// 设置出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
	// AvZ::setZombies({TT_4, BC_12, BC_12});
	// 设置出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
	static void setZombies(std::initializer_list<int> zombie_type);

	// *** Not In Queue
	// 设置特定波出怪 此函数不管填不填蹦极都会在 wave 10 20 刷蹦极！！！！！！！！！！！！
	// 参数命名规则：前面两个字母与 PvZ Tools 出怪类型名称拼音首字母一致，后面接下划线和类型代号
	// 详情见 pvzstruct.h 页尾
	// 例如 铁桶命名为 TT_4
	// *** 使用示例：
	// AvZ::setZombies(1, {CG_3, TT_4, BC_12, XC_15, QQ_16, FT_21, TL_22, BY_23, HY_32, TT_18});
	// 设置第一波出怪类型为：撑杆 铁桶 冰车 小丑 气球 扶梯 投篮 白眼 红眼 跳跳
	// AvZ::setZombies(1, {TT_4, BC_12, BC_12});
	// 设置第一波出怪类型为：铁桶 冰车 并且两种僵尸的比例为 1：2
	static void setWaveZombies(int wave, std::initializer_list<int> zombie_type);

	// *** In Queue
	// 冰三函数
	// *** 使用示例：
	// Ice3(298) --------- 修正冰三时间点至当前时刻的 298cs 后
	static void ice3(int time);

	// *** Not In Queue
	// 设置游戏倍速
	// *** 注意：倍速设置的范围为 [0.05, 10]
	// *** 使用示例
	// AvZ::setGameSpeed(5) ---- 将游戏速度设置为 5 倍速
	// AvZ::setGameSpeed(0.1) --- 将游戏速度设置为 0.1 倍速
	static void setGameSpeed(float x);

	// click api
public:
	struct ShovelCrood
	{
		int row;
		float col;
		bool pumpkin = false;
	};

	// *** Not In Queue
	// 点击格子
	// *** 使用示例：
	// clickGrid(3, 4)---- 点击格子(3, 4)
	// clickGrid(3, 4, 10)---- 向下偏移10像素点击格子(3, 4)
	static void clickGrid(int row, float col, int offset = 0);

	// *** Not In Queue
	// 点击种子/卡片
	// *** 使用示例：
	// clickSeed(1) ----- 点击第一个种子
	static void clickSeed(int seed_index) { click_scene(main_object, 50 + 50 * seed_index, 70, 1); }

	// *** Not In Queue
	// 右键安全点击
	static void safeClick() { click_scene(main_object, 1, 1, -1); }

	// *** Not In Queue
	// 鼠标左击
	// leftClick(400, 300)-----点击 PVZ 窗口中央
	static void leftClick(int x, int y) { click_scene(main_object, x, y, 1); }

	static void shovelNotInQueue(int row, float col, bool pumpkin = false);

	// *** In Queue
	// 铲除植物函数
	static void shovel(int row, float col, bool pumpkin = false)
	{
		insertOperation([=]() {
			shovelNotInQueue(row, col, pumpkin);
		},
						"shovel");
	}
	// *** In Queue
	// *** 使用示例：
	// shovel(4, 6)--------铲除4行6列的植物,如果植物有南瓜保护默认铲除被保护植物
	// shovel(4, 6, true)---铲除4行6列的植物,如果植物有南瓜保护铲除南瓜
	// shovel({{3, 6},{4, 6}})------铲除3行6列，4行6列的植物
	static void shovel(const std::vector<ShovelCrood> &lst)
	{
		insertOperation([=]() {
			for (const auto &crood : lst)
			{
				shovelNotInQueue(crood.row, crood.col, crood.pumpkin);
			}
		},
						"shovel");
	}

	// card api
private:
	static std::string seed_name_list[11][8];
	static std::map<std::string, int> seed_name_to_index_map;
	static std::vector<Grid> select_card_vec;
	// 为卡片名称变量获取卡片对象序列
	static int get_seed_index_for_seed_name(const std::string &seed_name);
	// 防误触
	static void deal_wrong_click();
	static bool choose_card(int row, int col);
	static void click_btn(int x, int y);
	static void lets_rock();
	static void select_cards();

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

	// *** Not In Queue
	// 选择一堆卡片
	// 卡片名称为拼音首字母，具体参考 https://pvz.lmintlcx.com/cvz/  页尾的命名
	// 请注意与 CppVsZombies 书写形式的不同！！！！！！
	// 使用此函数概率导致 PvZ 程序崩溃，尚未修复！！！
	// *** 使用示例：
	// AvZ::selectCards({"hbg", "Mhbg", "kfd", "hmg", "hy", "wg", "ytzd", "syc", "ngt", "xpg"});
	///////////////////// 寒冰菇 模仿寒冰菇 咖啡豆 毁灭菇  荷叶   倭瓜  樱桃炸弹  三叶草  南瓜头  小喷菇
	static void selectCards(const std::vector<std::string> &lst);

	// 用卡函数 Not In Queue
	// *** 使用示例：
	// cardNotInQueue(1, 2, 3)---------选取第1张卡片，放在2行,3列
	// cardNotInQueue(1, {{2, 3}, {3, 4}})--------选取第1张卡片，优先放在2行,3列，其次放在3行,4列
	static void cardNotInQueue(int seed_index, int row, float col);
	static void cardNotInQueue(int seed_index, const std::vector<Crood> &lst);

	// *** In Queue
	// 用卡函数
	// *** 使用示例：
	// card(1, 2, 3)---------选取第1张卡片，放在2行,3列
	// card({{1, 2, 3}, {2, 3, 4}})------选取第1张卡片，放在2行,3列，选取第2张卡片，放在3行,4列
	// card(1, {{2, 3}, {3, 4}})--------选取第1张卡片，优先放在2行,3列，其次放在3行,4列
	// 以下用卡片名称使用 card,卡片名称为拼音首字母，具体参考 https://pvz.lmintlcx.com/cvz/  页尾的命名
	// card({{"ytzd", 2, 3}, {"Mhblj", 3, 4}})---------选取樱桃卡片，放在2行,3列，选取辣椒卡片，放在3行,4列
	static void card(const std::vector<CardIndex> &lst);
	static void card(const std::vector<CardName> &lst);
	static void card(int seed_index, int row, float col);
	static void card(int seed_index, const std::vector<Crood> &lst);
	static void card(const std::string &seed_name, int row, float col);
	static void card(const std::string &seed_name, const std::vector<Crood> &lst);

	// thread
	// 使用 TickRunner 达到伪线程的效果
private:
	struct ThreadInfo
	{
		std::function<void()> func;
		int *id_ptr;
	};
	static std::vector<ThreadInfo> thread_vec;
	static std::stack<int> stoped_thread_id_stack;

public:
	// CLASS TickRunner
	// 使用此类使得操作每帧都运行
	class TickRunner
	{
	protected:
		int thread_id = -1;
		bool is_paused = false;
		bool thread_examine() // 线程出现异常返回 false
		{
			if (thread_id >= 0)
			{
				showErrorNotInQueue("一个自动线程类不允许同时拥有两个线程！");
				return false;
			}
			return true;
		}

	public:
		// *** Not In Queue
		void pushFunc(const std::function<void()> &run);

		// *** In Queue
		void stop()
		{
			insertOperation([=]() {
				stoped_thread_id_stack.push(thread_id);
				thread_id = -1;
			},
							"stop");
		}

		// *** In Queue
		void pause()
		{
			insertOperation([=]() {
				is_paused = true;
			},
							"pause");
		}
		// *** In Queue
		void goOn()
		{
			insertOperation([=]() {
				is_paused = false;
			},
							"goOn");
		}
	};

	class ItemCollector : public TickRunner
	{
	private:
		int time_interval = 10;
		void run();

	public:
		// *** In Queue
		void start()
		{
			insertOperation([=]() {
				if (!thread_examine())
				{
					return;
				}
				is_paused = false;
				pushFunc([=]() { run(); });
			},
							"startCollect");
		}
		// *** In Queue
		void setInterval(int _time_interval)
		{
			insertOperation([=]() {
				this->time_interval = _time_interval;
			},
							"setInterval");
		}
	};

	class IceFiller : public TickRunner
	{
	private:
		std::vector<Grid> fill_ice_grid_vec;
		std::vector<int> ice_seed_index_vec;
		int coffee_seed_index;
		void run();

	public:
		// *** In Queue
		// 重置冰卡
		// *** 使用示例
		// resetIceSeedList({1}) ------ 只使用第一张卡片
		// resetIceSeedList({1, 4}) ----- 使用第一、四张卡片
		void resetIceSeedList(const std::vector<int> &lst);

		// *** In Queue
		// 重置存冰位置
		// *** 使用示例：
		// resetFillList({{3,4},{5,6}})-----将存冰位置重置为{3，4}，{5，6}
		void resetFillList(const std::vector<Grid> &lst)
		{
			insertOperation([=]() {
				this->fill_ice_grid_vec = lst;
			},
							"resetFillList");
		}

		// *** In Queue
		// 线程开始工作
		// *** 使用示例：
		// start({{3,4},{5,6}})-----在{3，4}，{5，6}位置存冰
		void start(const std::vector<Grid> &lst);

		// *** In Queue
		// 使用咖啡豆函数
		// *** 使用示例：
		// coffee()-----自动使用优先级低的存冰位
		void coffee();
	};

	class PlantFixer : public TickRunner
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
		void use_seed_(int seed_index, int row, float col, bool is_need_shovel);
		void auto_get_fix_list();

	public:
		// *** In Queue
		// 重置植物修补位置
		// *** 使用示例：
		// resetFixList({{2, 3},{3, 4}})-------位置被重置为{2，3}，{3，4}
		void resetFixList(const std::vector<Grid> &lst)
		{
			insertOperation([=]() {
				grid_lst = lst;
			},
							"resetFixList");
		}
		// *** In Queue
		// 自动得到修补的位置列表
		void autoGetFixList();

		// *** In Queue
		// 线程开始工作，此函数开销较大，不建议多次调用
		// 第一个参数为植物类型
		// 第二个参数不填默认全场
		// 第三个参数不填默认植物血量为150以下时修补
		// *** 使用示例：
		// start(23)-------修补全场的高坚果
		// start(30,{{1,3},{2,3}})-----修补位置为{1，3}，{2，3}位置的南瓜头
		// start(3,{{1,3},{2,3}},300)------修补位置为{1，3}，{2，3}位置的坚果，血量降至300开始修补
		void start(int _plant_type, const std::vector<Grid> &lst = {}, int _fix_hp = 150);

		// *** In Queue
		// 重置修补血量
		// *** 使用示例：
		// resetFixHp(200)------将修补触发血量改为200
		void resetFixHp(int _fix_hp)
		{
			insertOperation([=]() {
				fix_hp = _fix_hp;
			},
							"resetFixHp");
		}

		// *** In Queue
		// 是否使用咖啡豆
		void isUseCoffee(bool _is_use_coffee)
		{
			insertOperation([=]() {
				is_use_coffee = _is_use_coffee;
			},
							"isUseCoffee");
		}
	};

	class KeyConnector : public TickRunner
	{
	private:
		std::vector<std::pair<char, std::function<void()>>> key_operation_vec;

	public:
		// *** Not In Queue
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
	public: // structs
		// 用于RAWPAO函数
		struct PaoDrop
		{
			int pao_row;
			int pao_col;
			int drop_row;
			float drop_col;
		};

		// 记录炮的位置和落点的位置及炮弹飞行时间
		struct RoofPaoDrop
		{
			int vec_index;
			int drop_row;
			float drop_col;
			int fire_time;
		};

		// 屋顶炮飞行时间辅助数据
		struct RoofFlyTime
		{
			int min_drop_x;	  // 记录该列炮最小飞行时间对应的最小的横坐标
			int min_fly_time; // 记录最小的飞行时间
		};

		struct LastestPaoMsg
		{
			int vec_index = -1;
			int fire_time = 0;
			bool is_writable = true;
		};

		// 选用顺序控制模式
		enum SequentialMode
		{
			SPACE,
			TIME
		};

	private:
		static std::set<Grid> lock_pao_set; // 锁定的炮
		std::vector<int> pao_index_vec;		// 炮的内存位置
		std::vector<Grid> pao_grid_vec;		// 炮列表，记录炮的信息
		int next_pao;						// 记录当前即将发射的下一门炮
		int sequential_mode = true;			// 顺序模式
		LastestPaoMsg lastest_pao_msg;		// 最近一颗发炮的信息
		static TickRunner tick_runner;
		static RoofFlyTime fly_time_data[8];
		// 得到炮的恢复时间
		static int get_recover_time(int index);
		// 基础发炮函数
		static void base_fire_pao(int pao_row, int pao_col, float drop_row, float drop_col);
		// 获取屋顶炮飞行时间
		static int get_roof_fly_time(int pao_col, float drop_col);
		// 延迟发炮
		static void delay_fire_pao(int delay_time, int pao_row, int pao_col, int row, float col);
		// 禁用 =
		void operator=(const PaoOperator &p);
		// 得到炮列表中的炮恢复时间
		// return -1 :  can't find pao index
		int get_recover_time_vec();

		// 更新下一门要发射的炮
		// 返回 >=0 下一门炮可用且意义为该门炮剩余的恢复时间
		// 返回 -1 下一门炮不可用
		int update_next_pao(int delay_time = 0, bool is_delay_pao = false);

		// 更新最近发炮的信息
		void update_lastest_pao_msg(int fire_time, int index)
		{
			if (lastest_pao_msg.is_writable)
			{
				lastest_pao_msg.fire_time = fire_time;
				lastest_pao_msg.vec_index = index;
			}
		}
		// 跳过一定数量的炮
		void skip_pao(int x)
		{
			next_pao = (next_pao + x) % pao_grid_vec.size();
		}

	public:
		// *** In Queue
		// 发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
		// *** 注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
		// *** 使用示例：
		// rawRoofPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
		// rawRoofPao({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
		static void rawRoofPao(int pao_row, int pao_col, int drop_row, float drop_col);

		// *** In Queue
		// 发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
		// *** 注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
		// *** 使用示例：
		// rawRoofPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
		// rawRoofPao({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
		static void rawRoofPao(const std::vector<PaoDrop> &lst);

		// *** In Queue
		// 发炮函数：用户自定义位置发射
		// *** 注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
		// *** 使用示例：
		// rawPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
		// rawPao({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
		static void rawPao(int pao_row, int pao_col, int drop_row, float drop_col);

		// *** In Queue
		// 发炮函数：用户自定义位置发射
		// *** 注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
		// *** 使用示例：
		// rawPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
		// rawPao({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
		static void rawPao(const std::vector<PaoDrop> &lst);

		// *** In Queue
		// 种植炮函数
		// *** 使用示例
		// plantPao(3, 4)------在三行四列位置种炮
		static void plantPao(int row, int col);

		PaoOperator();

		// *** In Queue
		// 立即修补上一枚已经发射的炮
		void fixLatestPao();

		// *** In Queue
		// 设置炮序模式
		// *** 使用示例：
		// setSequentialMode(AvZ::PaoOperator::TIME) ---- 设置时间使用模式
		// setSequentialMode(AvZ::PaoOperator::SPACE) ---- 设置空间使用模式
		void setSequentialMode(int _sequential_mode)
		{
			insertOperation([=]() {
				sequential_mode = _sequential_mode;
			},
							"setSequentialMode");
		}

		// *** In Queue
		// 设置即将发射的下一门炮
		// 此函数只有在限制炮序的时候才可调用
		// *** 使用示例：
		// setNextPao(10)------将炮列表中第十门炮设置为下一门即将发射的炮
		// setNextPao(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
		void setNextPao(int next_pao);

		// *** In Queue
		// 设置即将发射的下一门炮
		// 此函数只有在限制炮序的时候才可调用
		// *** 使用示例：
		// setNextPao(10)------将炮列表中第十门炮设置为下一门即将发射的炮
		// setNextPao(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
		void setNextPao(int row, int col);

		// *** In Queue
		// 跳炮函数
		// *** 使用示例：
		// skipPao(2)---跳过按照顺序即将要发射的2门炮
		void skipPao(int x)
		{
			insertOperation([=]() {
				next_pao = (next_pao + x) % pao_grid_vec.size();
			},
							"skipPao");
		}

		// *** In Queue
		// 发炮函数
		// *** 使用示例：
		// pao(2,9)----------------炮击二行，九列
		// pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void pao(int row, float col);

		// *** In Queue
		// 发炮函数
		// *** 使用示例：
		// pao(2,9)----------------炮击二行，九列
		// pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void pao(const std::vector<Crood> &lst);

		// *** In Queue
		// 发炮函数 炮CD恢复自动发炮
		// *** 使用示例：
		// recoverPao(2,9)----------------炮击二行，九列
		// recoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void recoverPao(int row, float col);

		// *** In Queue
		// 发炮函数 炮CD恢复自动发炮
		// *** 使用示例：
		// recoverPao(2,9)----------------炮击二行，九列
		// recoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
		void recoverPao(const std::vector<Crood> &lst);

		// *** In Queue
		// 屋顶修正飞行时间发炮. 此函数开销较大不适合精确键控.
		// 此函数只适用于RE与ME 修正时间：387cs
		// *** 使用示例：
		// roofPao(3,7)---------------------修正飞行时间后炮击3行7列
		// roofPao({ {2,9},{5,9} })---------修正飞行时间后炮击2行9列,5行9列
		void roofPao(int row, float col);

		// *** In Queue
		// 屋顶修正飞行时间发炮. 此函数开销较大不适合精确键控.
		// 此函数只适用于RE与ME 修正时间：387cs
		// *** 使用示例：
		// roofPao(3,7)---------------------修正飞行时间后炮击3行7列
		// roofPao({ {2,9},{5,9} })---------修正飞行时间后炮击2行9列,5行9列
		void roofPao(const std::vector<Crood> &lst);

		// *** In Queue
		// 重置炮列表
		// *** 使用示例:
		// resetPaoList({{3, 1},{4, 1},{3, 3},{4, 3}})-------经典四炮
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
#define NowTime AvZ::nowTime
#define StartMaidCheats AvZ::MaidCheats::dancing
#define StopMaidCheats AvZ::MaidCheats::stop
#define SelectCards AvZ::selectCards
#define SetZombies AvZ::setZombies
#define SetWaveZombies AvZ::setWaveZombies
#define OpenMultipleEffective AvZ::openMultipleEffective
#define ShowQueue AvZ::showQueue
#define SetWavelength AvZ::setWavelength

#endif