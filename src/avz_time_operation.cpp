/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: AvZ time operation queue
 * 				 It's important to understand these!
 */

#include "libavz.h"

// 此函数每帧都需调用一次
void AvZ::update_refresh_time()
{
	// 防止错误读取僵尸刷新时间点
	if (pvz_base->gameUi() == 2)
	{
		for (auto &ele : operation_queue_vec)
		{
			ele.refresh_time = -1;
		}
		return;
	}

	int wave = main_object->wave();

	auto operation_queue_it = operation_queue_vec.begin() + wave;

	if (operation_queue_it->refresh_time != -1) // 已经读取过的不再读取
	{
		return;
	}

	if (wave == 0)
	{
		operation_queue_it->refresh_time = main_object->refreshCountdown() + main_object->gameClock();
	}
	else
	{
		if (wave + 1 == 10 || wave + 1 == 20)
		{
			// 大波
			if (main_object->refreshCountdown() <= 4)
			{
				operation_queue_it->refresh_time = main_object->hugewaveCountdown() + main_object->gameClock();
			}
		}
		else if (main_object->refreshCountdown() <= 200)
		{
			// 普通波
			operation_queue_it->refresh_time = main_object->refreshCountdown() + main_object->gameClock();
		}

		// 在 wave != 0 时，可以由初始刷新倒计时得到当前已刷出波数的刷新时间点
		--operation_queue_it;
		if (operation_queue_it->refresh_time == -1)
		{
			operation_queue_it->refresh_time = main_object->gameClock() -
											   (main_object->initialCountdown() -
												main_object->refreshCountdown());
		}
	}
}

void AvZ::insertOperation(const std::function<void()> &operation, const std::string &description)
{
	if (!is_insert_operation ||
		(time_wave_insert.time == time_wave_run.time &&
		 time_wave_insert.wave == time_wave_run.wave))
	{
		operation();
		return;
	}

	Operation operate = {operation, description};

	if (time_wave_insert.wave < 1 || time_wave_insert.wave > 20)
	{
		showErrorNotInQueue("您填写的 wave 参数为 # , 已超出 [1, 20] 的范围",
							time_wave_insert.wave);
		return;
	}

	if (!RangeIn(time_wave_insert.wave, {1, 10, 20}) &&
		time_wave_insert.time < -200 &&
		operation_queue_vec[time_wave_insert.wave - 2].wave_length == -1)
	{
		showErrorNotInQueue("第 # 波设定的时间为 #, 在前一波未设定波长的情况下, time 参数不允许小于 -200",
							time_wave_insert.wave,
							time_wave_insert.time);
		return;
	}

	auto &operation_queue = operation_queue_vec[time_wave_insert.wave - 1].queue; // 取出相应波数的队列
	auto it = operation_queue.find(time_wave_insert.time);
	if (it == operation_queue.end())
	{
		std::pair<int, std::vector<Operation>> to;
		to.first = time_wave_insert.time;
		to.second.push_back(operate);
		operation_queue.insert(to);
	}
	else
	{
		it->second.push_back(operate);
	}
}

void AvZ::waitUntil(const TimeWave &_time_wave)
{
	if (is_exited ||
		pvz_base->gameUi() == 1)
	{
		return;
	}

	block_var = true; // 唤醒游戏主循环
	setTime(_time_wave);
	insertOperation([=]() {
		block_var = false; // 唤醒 Script 线程
		while (!block_var) // 阻塞游戏主循环
		{
			exit_sleep(1);
			if (pvz_base->gameUi() == 1)
			{
				// 游戏到主界面无条件退出
				return;
			}
		}
	});

	while (block_var) // 阻塞 Script 线程
	{
		exit_sleep(1);
		if (pvz_base->gameUi() == 1)
		{
			// 游戏到主界面无条件退出
			return;
		}
	}
}

void AvZ::showQueue(const std::vector<int> &lst)
{
	insertOperation([=]() {
		std::stringstream ss;
		for (const auto &wave : lst)
		{
			if (wave < 1 || wave > 20)
			{
				continue;
			}
			ss << "---------------------------"
			   << " wave : " << wave << " ---------------------------";
			if (!operation_queue_vec[wave - 1].queue.empty())
			{
				for (const auto &each : operation_queue_vec[wave - 1].queue)
				{
					ss << "\n\t" << each.first;
					for (const auto &operation : each.second)
					{
						ss << " " << operation.description;
					}
				}
			}
			else
			{
				ss << "\n\tnone";
			}

			ss << "\n";
		}

		showErrorNotInQueue(ss.str());
	},
					"showQueue");
}

void AvZ::loadScript(const std::function<void()> func)
{
	init_address();
	operation_queue_vec.resize(20);
	time_wave_run.wave = 0;
	wavelength_it = operation_queue_vec.begin();
	setInsertOperation(false);
	item_collector.start();
	AvZ::MaidCheats::stop();
	setInsertOperation(true);
	setTime(-600, 1);
	is_loaded = true;
	func();
	block_var = true; // 唤醒游戏主循环

	// 等待游戏结束
	while (pvz_base->gameUi() != 1)
	{
		exit_sleep(1);
	}
	// 如果是多次生效则继续录入操作
	// 如果不是将退出
	if (is_multiple_effective)
	{
		is_loaded = false;
	}
	else
	{
		is_exited = true;
	}
	for (auto &thread_info : thread_vec) // 停止一切线程
	{
		*thread_info.id_ptr = -1;
	}
	SetWindowTextA(pvz_hwnd, "Plants vs. Zombies");
	exit_sleep(20);
	operation_queue_vec.clear(); // 清除一切操作
	thread_vec.clear();
	key_connector.clear();
	while (!stoped_thread_id_stack.empty())
	{
		stoped_thread_id_stack.pop();
	}
}

void AvZ::openMultipleEffective(char close_key)
{
	is_multiple_effective = true;
	key_connector.add(close_key, []() {
		is_multiple_effective = false;
		showErrorNotInQueue("已关闭多次生效");
	});
}

void AvZ::run(MainObject *level, std::function<void()> Script)
{
	if (is_exited)
	{
		return;
	}
	main_object = level;

	if (!is_loaded)
	{
		std::thread task(AvZ::loadScript, Script);
		task.detach();
		while (!is_loaded)
		{
			exit_sleep(1);
		}
	}

	if (main_object->wave() != 20)
	{
		update_refresh_time();
	}

	if (pvz_base->gameUi() != 3 ||
		pvz_base->mouseWindow()->topWindow())
	{
		return;
	}

	if (main_object->selectCardUi_m()->orizontalScreenOffset() != 0 &&
		main_object->selectCardUi_m()->orizontalScreenOffset() != 7830)
	{
		return;
	}

	for (const auto &thread_info : thread_vec)
	{
		if (*thread_info.id_ptr >= 0)
		{
			thread_info.func();
		}
	}

	// 对 20 个队列进行遍历
	// 最大比较次数 20 * 3 = 60 次
	// 卧槽，感觉好亏，不过游戏应该不会卡顿
	for (int wave = 0; wave < 20; ++wave)
	{
		if (operation_queue_vec[wave].refresh_time == -1 ||
			operation_queue_vec[wave].queue.empty() ||
			!operation_queue_vec[wave].is_time_arrived())
		{
			// 波次没有到达或队列为空或时间没有到达，跳过
			continue;
		}

		auto it = operation_queue_vec[wave].queue.begin();
		time_wave_run.wave = wave + 1;
		time_wave_run.time = it->first;

		for (const auto &ele : it->second)
		{
			ele.operation();
		}
		operation_queue_vec[wave].queue.erase(it);
	}
}