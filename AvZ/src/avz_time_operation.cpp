/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: AvZ time operation queue
 * 				 It's important to understand these!
 */

#include "avz.h"

// 此函数每帧都需调用一次
void AvZ::update_refresh_time()
{
	int wave = main_object->wave();
	if (wave == 20)
	{
		return;
	}
	if (operation_queue_vec[wave].refresh_time != -1 // 已经读取过的不再读取
		&& wave != 0								 // wave 1 需要不断读取
	)
	{
		return; // 已经读取到了的刷新时间点不再读取
	}
	int flag_countdown = (wave + 1 == 1 ? 0xFFFF : 200); // 读取刷新倒计时标志时间
	if (wave + 1 == 10 || wave + 1 == 20)
	{
		// 大波
		if (main_object->refreshCountdown() <= 4)
		{
			operation_queue_vec[wave].refresh_time = main_object->hugewaveCountdown() + main_object->gameClock();
		}
	}
	else
	{
		if (main_object->refreshCountdown() <= flag_countdown)
		{
			operation_queue_vec[wave].refresh_time = main_object->refreshCountdown() + main_object->gameClock();
		}
	}
}

void AvZ::insertOperation(const std::function<void()> &operation, const std::string &description)
{
	Operation operate = {operation, description};
	auto &operation_queue = operation_queue_vec[time_wave.wave - 1].queue; // 取出相应波数的队列
	auto it = operation_queue.find(time_wave.time);
	if (it == operation_queue.end())
	{
		std::pair<int, std::vector<Operation>> to;
		to.first = time_wave.time;
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
	while (pvz_base->gameUi() == 3)
	{
		exit_sleep(1);
		if (operation_queue_vec[_time_wave.wave - 1].refresh_time == -1) // 当前波的刷新时间未读出
		{
			continue;
		}
		if (_time_wave.time - 2 <= nowTime(_time_wave.wave))
		{
			break;
		}
	}
	setTime(_time_wave);
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

void AvZ::loadScript(const std::function<void()> func)
{
	is_loaded = true;
	init_address();
	operation_queue_vec.resize(20);
	setTime(-600, 1);
	item_collector.start();
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
	operation_queue_vec.clear(); // 清除一切操作
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

	update_refresh_time();

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
		time_wave.wave = wave + 1;
		time_wave.time = it->first;
		// 此处使用范围 for 由于迭代器更新的原因会出问题
		for (int i = 0; i < it->second.size(); ++i)
		{
			it->second[i].operation();
		}
		operation_queue_vec[wave].queue.erase(it);
	}
}
