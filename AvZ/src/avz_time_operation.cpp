/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: AvZ time operation queue
 * 				 It's important to understand these!
 */

#include "avz.h"

void AvZ::update_refresh_time()
{
	static int wave = 0;
	if (wave != operation_queue.begin()->first.wave)
	{
		wave = operation_queue.begin()->first.wave;
		if (wave == main_object->wave()) // 已刷新
		{
			zombie_refresh.time = main_object->gameClock() - main_object->initialCountdown() + main_object->refreshCountdown();
			zombie_refresh.wave = wave;
		}
		else if (wave - 1 == main_object->wave()) // 未刷新
		{
			int prejudge_std_time = (wave == 1 ? 600 : 200);
			if (wave == 10 || wave == 20) //大波
			{
				if (main_object->refreshCountdown() > 4)
				{
					--wave;
					return;
				}
				zombie_refresh.time = main_object->hugewaveCountdown() + main_object->gameClock();
				zombie_refresh.wave = wave;
			}
			else if (main_object->refreshCountdown() <= prejudge_std_time) //普通波
			{
				zombie_refresh.time = main_object->refreshCountdown() + main_object->gameClock();
				zombie_refresh.wave = wave;
			}
			else
			{
				--wave;
				return;
			}
		}
		else
		{
			--wave;
			return;
		}
	}
}

bool AvZ::is_time_arrived()
{
	update_refresh_time();
	const auto &operation = operation_queue.begin()->first;
	if (zombie_refresh.wave == operation.wave)
	{
		int now_time = main_object->gameClock() - zombie_refresh.time;
		if (now_time >= operation.time)
		{
			if (now_time > operation.time && operation.time != -600)
			{
				popErrorWindowNotInQueue("您设定的时间已过去，现在的时间已到 : #，点击确定按照当前时间执行接下来的操作", now_time);
			}
			return true;
		}
	}
	// 如果现在的波数已经大于设定的波数 + 2，直接放行
	return main_object->wave() > operation.wave + 2;
}

void AvZ::insertOperation(const std::function<void()> &operation)
{
	auto it = operation_queue.find(time_wave);
	if (it == operation_queue.end())
	{
		std::pair<TimeWave, std::vector<std::function<void()>>> to;
		to.first = time_wave;
		to.second.push_back(operation);
		operation_queue.insert(to);
	}
	else
	{
		it->second.push_back(operation);
	}
}

void AvZ::waitUntil(const TimeWave &_time_wave)
{
	TimeWave now_time;
	while (pvz_base->gameUi() == 3)
	{
		now_time = nowTimeWave();
		if (_time_wave.time - 2 <= now_time.time && _time_wave.wave == now_time.wave)
		{
			break;
		}
		exit_sleep(1);
	}
	setTime(_time_wave);
}

void AvZ::loadScript(const std::function<void()> func)
{
	is_loaded = true;
	init_address();
	setTime(-600, 1);
	item_collector.start();
	pao_operator.initPaoMessage();
	func();

	while (pvz_base->gameUi() != 1)
	{
		exit_sleep(1);
	}
	if (is_multiple_effective)
	{
		is_loaded = false;
	}
	for (auto &thread_info : thread_vec) // 停止一切线程
	{
		*thread_info.id_ptr = -1;
	}
	exit_sleep(20);
	operation_queue.clear(); // 清除一切操作
	thread_vec.clear();
	key_connector.clear();
	while (!stoped_thread_id_stack.empty())
	{
		stoped_thread_id_stack.pop();
	}
}

void AvZ::run(MainObject *level)
{
	main_object = level;

	if (!is_loaded)
	{
		std::thread task(AvZ::loadScript, Script);
		task.detach();
		exit_sleep(10);
	}
	if (pvz_base->gameUi() != 3 || pvz_base->mouseWindow()->topWindow())
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

	if (!operation_queue.empty())
	{
		if (is_time_arrived())
		{
			auto it = operation_queue.begin();
			for (const auto &operation : it->second)
			{
				operation();
			}
			operation_queue.erase(it);
		}
	}
}
