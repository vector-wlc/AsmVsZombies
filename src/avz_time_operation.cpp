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
	}

	int wave = main_object->wave();

	auto operation_queue_it = operation_queue_vec.begin() + wave;

	if (operation_queue_it->refresh_time != -1) // 已经读取过的不再读取
	{
		return;
	}

	// 没有设定波长，读取游戏内的天然数据
	int flag_countdown = (wave + 1 == 1 ? 0xFFFF : 200); // 读取刷新倒计时标志时间
	if (wave + 1 == 10 || wave + 1 == 20)
	{
		// 大波
		if (main_object->refreshCountdown() <= 4)
		{
			operation_queue_it->refresh_time = main_object->hugewaveCountdown() + main_object->gameClock();
		}
	}
	else
	{
		if (main_object->refreshCountdown() <= flag_countdown)
		{
			operation_queue_it->refresh_time = main_object->refreshCountdown() + main_object->gameClock();
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
		popErrorWindowNotInQueue("您填写的 wave 参数为 # , 已超出 [1, 20] 的范围",
								 time_wave_insert.wave);
		return;
	}

	if (!RangeIn(time_wave_insert.wave, {1, 10, 20}) &&
		time_wave_insert.time < -200 &&
		operation_queue_vec[time_wave_insert.wave - 2].wave_length == -1)
	{
		popErrorWindowNotInQueue("第 # 波设定的时间为 #, 在前一波未设定波长的情况下, time 参数不允许小于 -200",
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
	if (is_exited)
	{
		return;
	}
	setTime(_time_wave);
	if (time_wave_run.time == time_wave_insert.time &&
		time_wave_run.wave == time_wave_insert.wave)
	{
		return;
	}

	while ((pvz_base->gameUi() == 3 &&
			main_object->text()->disappearCountdown() == 1001) ||
		   pvz_base->gameUi() == 2)
	{
		exit_sleep(1);
	}

	while (pvz_base->gameUi() == 3 &&
		   (operation_queue_vec[_time_wave.wave - 1].refresh_time == -1 ||
			_time_wave.time - 2 > nowTime(_time_wave.wave)))
	{
		exit_sleep(1);
	}

	while (pvz_base->gameUi() == 3 &&
		   _time_wave.time > nowTime(_time_wave.wave))
	{
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

		popErrorWindowNotInQueue(ss.str());
	},
					"showQueue");
}

void AvZ::setWavelength(const std::vector<AvZ::WaveTime> &lst)
{
	auto temp = time_wave_insert;
	for (const auto &ele : lst)
	{
		if (ele.wave < 1 || RangeIn(ele.wave, {9, 19, 20}) || ele.wave > 20)
		{
			popErrorWindowNotInQueue("setWavelength : 您当前设定的 wave 参数为 #, 超出有效范围",
									 ele.wave);
			continue;
		}

		if (ele.time < 601 || ele.time > 2500)
		{
			popErrorWindowNotInQueue("setWavelength : 您当前设定第 # 波 的 time 参数为 #, 超出有效范围",
									 ele.wave, ele.time);
			continue;
		}

		operation_queue_vec[ele.wave - 1].wave_length = ele.time;

		setTime(100, ele.wave);
		insertOperation([=]() {
			main_object->zombieRefreshHp() = 0;
			main_object->refreshCountdown() = ele.time - 100;

			if (wavelength_it - operation_queue_vec.begin() < ele.wave)
			{
				wavelength_it = operation_queue_vec.begin() + ele.wave - 1;
			}

			// 设定刷新时间点
			for (; wavelength_it != operation_queue_vec.end() - 1; ++wavelength_it)
			{
				if (wavelength_it->refresh_time == -1 || wavelength_it->wave_length == -1)
				{
					break;
				}
				(wavelength_it + 1)->refresh_time = wavelength_it->refresh_time + wavelength_it->wave_length;
			}
		},
						"writeWavelength");
	}

	setTime(temp);
}

void AvZ::loadScript(const std::function<void()> func)
{
	is_loaded = true;
	init_address();
	operation_queue_vec.resize(20);
	time_wave_run.wave = 0;
	wavelength_it = operation_queue_vec.begin();
	setInsertOperation(false);
	item_collector.start();
	setInsertOperation(true);
	setTime(-600, 1);
	func();

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
		popErrorWindowNotInQueue("已关闭多次生效");
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
		exit_sleep(10);
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