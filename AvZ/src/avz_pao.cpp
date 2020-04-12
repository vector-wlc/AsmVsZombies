/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: old CLASS PaoOperator
 */

#include "avz.h"

//录入新的一门炮
void AvZ::PaoOperator::new_pao(PaoInfo &np)
{
	np.vec_index = next_vec_index;
	if (next_vec_index == all_pao_vec.size())
	{
		all_pao_vec.push_back(np);
		++next_vec_index;
	}
	else
	{
		int temp_index = all_pao_vec[next_vec_index].vec_index;
		all_pao_vec[next_vec_index] = np;
		next_vec_index = temp_index;
	}
}

// 炮信息初始化
void AvZ::PaoOperator::initPaoMessage()
{
	all_pao_vec.clear();
	next_vec_index = 0;
	auto cannon = main_object->plantArray();
	PaoInfo pao_message;
	int plant_count_max = main_object->plantCountMax();
	int cannon_status;
	int now_time = main_object->gameClock();
	//遍历场上所有炮的信息
	for (int i = 0; i < plant_count_max; ++i, ++cannon)
	{
		if (cannon->type() == 47 && !cannon->isCrushed() && !cannon->isDisappeared())
		{
			pao_message.row = cannon->row() + 1;
			pao_message.col = cannon->col() + 1;
			pao_message.index = i;
			cannon_status = cannon->state();

			//计算炮恢复时间
			if (cannon_status == 37)
				pao_message.recover_time = now_time;
			else if (cannon_status == 38) //如果正在发射则认为该炮的
				pao_message.recover_time = now_time + 3475;
			else //如果炮不能用，则恢复时间为现在时间 + 倒计时 + 125
				pao_message.recover_time = now_time + cannon->stateCountdown() + 125;

			new_pao(pao_message);
		}
	}
}

AvZ::PaoOperator::PaoOperator()
{
	sequential_mode = TIME;
	next_pao = 0;
}

AvZ::PaoOperator::~PaoOperator()
{
}

void AvZ::PaoOperator::updatePaoMessage(const std::vector<Grid> &lst)
{
	insertOperation([=]() {
		// 这里代码写的很垃圾 =_=
		// 删掉场上不存在的炮
		std::vector<Grid> all_pao_grid;
		std::vector<int> cannon_index_vec;
		Grid grid;
		for (const auto &pao_msg : all_pao_vec)
		{
			grid.row = pao_msg.row;
			grid.col = pao_msg.col;
			all_pao_grid.push_back(grid);
		}
		getPlantIndexs(all_pao_grid, YMJNP_47, cannon_index_vec);
		auto cannon_index_it = cannon_index_vec.begin();

		while (cannon_index_it != cannon_index_vec.end())
		{
			if ((*cannon_index_it) < 0)
			{
				delete_pao(cannon_index_it - cannon_index_vec.begin());
			}
			++cannon_index_it;
		}

		// 进行炮信息的更新
		Plant *cannon;
		getPlantIndexs(lst, YMJNP_47, cannon_index_vec);
		auto grid_it = lst.begin();
		cannon_index_it = cannon_index_vec.begin();
		int plant_count_max = main_object->plantCountMax();
		int cannon_status;
		int now_time = main_object->gameClock();

		while (grid_it != lst.end())
		{
			auto all_pao_it = FindInAllRange(all_pao_vec, *grid_it);
			if ((*cannon_index_it) >= 0) // 炮存在
			{
				PaoInfo pao_info;
				if (all_pao_it != all_pao_vec.end())
				{
					delete_pao(all_pao_it);
				}

				cannon = main_object->plantArray() + (*cannon_index_it);
				pao_info.index = *cannon_index_it;
				cannon_status = cannon->state();
				pao_info.row = cannon->row() + 1;
				pao_info.col = cannon->col() + 1;

				if (cannon_status == 37)
					pao_info.recover_time = now_time;
				else if (cannon_status == 38)
					pao_info.recover_time = now_time + 3475;
				else
					pao_info.recover_time = now_time + cannon->stateCountdown() + 125;

				new_pao(pao_info);
			}

			++grid_it;
			++cannon_index_it;
		}
	});
}

//用户重置炮列表
void AvZ::PaoOperator::resetPaoList(const std::vector<Grid> &lst)
{
	insertOperation([=]() {
		next_pao = 0;

		//重置炮列表
		pao_list.clear();
		for (const auto &pao_grid : lst)
		{
			//在总炮信息里面找不到则报错
			auto it = FindInAllRange(all_pao_vec, pao_grid);
			if (it == all_pao_vec.end() || !is_exist(it))
			{
				popErrorWindowNotInQueue("resetPaoList : 请检查 (#, #) 是否为炮", pao_grid.row, pao_grid.col);
				return;
			}
			pao_list.push_back(it - all_pao_vec.begin());
		}
	});
}

void AvZ::PaoOperator::setNextPao(int temp_next_pao)
{
	insertOperation([=]() {
		if (temp_next_pao > pao_list.size())
		{
			popErrorWindowNotInQueue("setNextPao : 本炮列表中一共有 # 门炮，您设的参数已溢出", pao_list.size());
			return;
		}
		next_pao = temp_next_pao - 1;
	});
}

void AvZ::PaoOperator::setNextPao(int row, int col)
{
	insertOperation([=]() {
		int temp_next_pao = 0;
		Grid grid = {row, col};
		auto it = FindInAllRange(all_pao_vec, grid);

		if (it != all_pao_vec.end())
		{
			next_pao = it - all_pao_vec.begin();
		}
		else
		{
			popErrorWindowNotInQueue("setNextPao : 请检查(#, #)是否在本炮列表中", row, col);
			return;
		}
	});
}

//对炮进行一些检查
bool AvZ::PaoOperator::pao_examine(int vec_index, int drop_row, float drop_col)
{
	if (!is_exist(vec_index))
	{
		popErrorWindowNotInQueue("pao_examine : 位于 (#, #) 的炮已失效！",
								 all_pao_vec[vec_index].row,
								 all_pao_vec[vec_index].col);
		return false;
	}
	int now_time = main_object->gameClock();
	auto cannon = main_object->plantArray() + all_pao_vec[vec_index].index;
	if (cannon->state() != 37)
	{
		if (all_pao_vec[vec_index].recover_time > now_time)
		{
			popErrorWindowNotInQueue("pao_examine : 位于 (#, #) 的炮还有 #cs 恢复",
									 all_pao_vec[vec_index].row,
									 all_pao_vec[vec_index].col,
									 all_pao_vec[vec_index].recover_time - now_time);
		}
		else
		{
			popErrorWindowNotInQueue("pao_examine : 位于 (#, #) 的炮未恢复，若您手动发射了该炮，请及时调用 updatePaoMessage 以更新炮的信息",
									 all_pao_vec[vec_index].row, all_pao_vec[vec_index].col);
		}
		return false;
	}

	return true;
}

void AvZ::PaoOperator::base_fire_pao(int vec_index, int drop_row, float drop_col)
{
	safeClick();

	for (int i = 0; i < 3; ++i)
	{
		clickGrid(all_pao_vec[vec_index].row, all_pao_vec[vec_index].col);
	}
	main_object->clickPaoCountdown() = 0;
	clickGrid(drop_row, drop_col);

	safeClick();
	update_lastest_pao_msg(main_object->gameClock(), vec_index);
	all_pao_vec[vec_index].recover_time = main_object->gameClock() + 3475;
}

//用户自定义炮位置发炮：单发
void AvZ::PaoOperator::rawPao(int pao_row, int pao_col, int drop_row, float drop_col)
{
	insertOperation([=]() {
		int now_time = main_object->gameClock();
		Grid grid = {pao_row, pao_col};
		auto it = FindInAllRange(all_pao_vec, grid);
		if (it == all_pao_vec.end())
		{
			popErrorWindowNotInQueue("rawPao : 请检查 (#, #) 是否为炮", pao_row, pao_col);
			return;
		}
		int vec_index = it - all_pao_vec.begin();
		if (!pao_examine(vec_index, drop_row, drop_col))
		{
			return;
		}
		base_fire_pao(vec_index, drop_row, drop_col);
	});
}

//用户自定义炮位置发炮：多发
void AvZ::PaoOperator::rawPao(const std::vector<PaoDrop> &lst)
{
	for (const auto &each : lst)
	{
		rawPao(each.pao_row, each.pao_col, each.drop_row, each.drop_col);
	}
}

void AvZ::PaoOperator::plantPao(int row, int col)
{
	insertOperation([=]() {
		vthread.pushFunc([=]() {
			static int ymjnp_seed_index = getSeedIndex(YMJNP_47);
			static int ymts_seed_index = getSeedIndex(YMTS_34);
			static Seed *seed_memory;

			if (ymjnp_seed_index == -1 || ymts_seed_index == -1)
			{
				return;
			}

			for (int t_col = col; t_col < col + 2; ++t_col)
			{
				if (getPlantIndex(row, t_col, YMTS_34) != -1)
				{
					continue;
				}

				seed_memory = main_object->seedArray() + ymts_seed_index;
				if (!seed_memory->isUsable())
				{
					return;
				}

				cardNotInQueue(ymts_seed_index + 1, row, t_col);
			}

			seed_memory = main_object->seedArray() + ymjnp_seed_index;
			if (!seed_memory->isUsable())
			{
				return;
			}

			cardNotInQueue(ymjnp_seed_index + 1, row, col);
			setTime(nowTimeWave());
			vthread.stop();
			updatePaoMessage({{row, col}});
		});
	});
}

void AvZ::PaoOperator::shovelPao(int row, int col)
{
	insertOperation([=]() {
		Grid grid = {row, col};
		auto it = FindInAllRange(all_pao_vec, grid);
		if (it == all_pao_vec.end())
		{
			popErrorWindowNotInQueue("请检查 (#, #) 是否为炮", it->row, it->col);
		}
		delete_pao(it);
		shovelNotInQueue(row, col);
	});
}

void AvZ::PaoOperator::fixLatestPao()
{
	insertOperation([=]() {
		if (lastest_pao_msg.vec_index == -1)
		{
			popErrorWindowNotInQueue("fixLastPao ：您尚未使用炮");
			return;
		}
		lastest_pao_msg.is_writable = false; // 锁定信息
		int time = nowTimeWave().time + lastest_pao_msg.fire_time - main_object->gameClock() + 205;
		SetTime(time);
		insertOperation([=]() {
			lastest_pao_msg.is_writable = true; // 解锁信息
			shovelNotInQueue(all_pao_vec[lastest_pao_msg.vec_index].row, all_pao_vec[lastest_pao_msg.vec_index].col);
			delete_pao(lastest_pao_msg.vec_index);
		});
		plantPao(all_pao_vec[lastest_pao_msg.vec_index].row, all_pao_vec[lastest_pao_msg.vec_index].col);
	});
}

// 找到恢复时间最早的炮
int AvZ::PaoOperator::find_min_recover_time_pao()
{
	// 寻找第一个未被铲的炮
	auto vec_index_it = pao_list.begin();
	while (!is_exist(*vec_index_it))
	{
		++vec_index_it;
	}

	// 寻找 cd 最小的炮
	auto min_time_it = vec_index_it;
	do
	{
		if (is_exist(*vec_index_it) &&														  // is_exist?
			all_pao_vec[*min_time_it].recover_time > all_pao_vec[*vec_index_it].recover_time) // is_recoverd?
		{
			min_time_it = vec_index_it;
		}
		++vec_index_it;
	} while (vec_index_it != pao_list.end());

	return *min_time_it;
}

//发炮函数：单发
void AvZ::PaoOperator::pao(int row, float col)
{
	insertOperation([=]() {
		if (pao_list.size() == 0)
		{
			popErrorWindowNotInQueue("pao : 您尚未为此炮列表分配炮");
			return;
		}
		int vec_index = get_next_pao();
		if (!pao_examine(vec_index, row, col))
		{
			return;
		}
		base_fire_pao(vec_index, row, col);
		skip_pao(1);
	});
}

//发炮函数：多发
void AvZ::PaoOperator::pao(const std::vector<Crood> &lst)
{
	for (const auto &each : lst)
	{
		pao(each.row, each.col);
	}
}

void AvZ::PaoOperator::delay_fire_pao(int vec_index, int delay_time, int row, float col)
{
	all_pao_vec[vec_index].recover_time = delay_time + main_object->gameClock() + 3475;
	if (delay_time <= 0)
	{
		if (!pao_examine(vec_index, row, col))
		{
			return;
		}
		base_fire_pao(vec_index, row, col);
	}
	else
	{
		update_lastest_pao_msg(main_object->gameClock() + delay_time, vec_index);
		// 将操作动态插入消息队列
		time_wave = nowTimeWave();
		time_wave.time += delay_time;
		insertOperation([=]() {
			if (!pao_examine(vec_index, row, col))
			{
				return;
			}
			base_fire_pao(vec_index, row, col);
		});
	}
}

void AvZ::PaoOperator::recoverPao(int row, float col)
{
	insertOperation([=]() {
		if (pao_list.size() == 0)
		{
			popErrorWindowNotInQueue("recoverPao : 您尚未为此炮列表分配炮");
			return;
		}
		int vec_index = get_next_pao();
		int delay_time = all_pao_vec[vec_index].recover_time - main_object->gameClock();
		if (delay_time < 0)
		{
			delay_time = 0;
		}
		delay_fire_pao(vec_index, delay_time, row, col);
		skip_pao(1);
	});
}

void AvZ::PaoOperator::recoverPao(const std::vector<Crood> &lst)
{
	for (const auto &each : lst)
	{
		recoverPao(each.row, each.col);
	}
}

//获取屋顶炮飞行时间
int AvZ::PaoOperator::get_roof_fly_time(int pao_col, float drop_col)
{
	//得到落点对应的横坐标
	int drop_x = static_cast<int>(drop_col * 80);
	//得到该列炮最小飞行时间对应的最小的横坐标
	int min_drop_x = fly_time_data[pao_col - 1].min_drop_x;
	//得到最小的飞行时间
	int min_fly_time = fly_time_data[pao_col - 1].min_fly_time;
	//返回飞行时间
	return (drop_x >= min_drop_x ? min_fly_time : (min_fly_time + 1 - (drop_x - (min_drop_x - 1)) / 32));
}

void AvZ::PaoOperator::roofPao(int row, float col)
{
	insertOperation([=]() {
		if (main_object->scene() != 4 && main_object->scene() != 5)
		{
			popErrorWindowNotInQueue("roofPao : RoofPao函数只适用于 RE 与 ME ");
			return;
		}
		if (pao_list.size() == 0)
		{
			popErrorWindowNotInQueue("roofPao : 您尚未为此炮列表分配炮");
			return;
		}

		int vec_index = get_next_pao();
		int delay_time = 387 - get_roof_fly_time(all_pao_vec[vec_index].col, col);
		int fire_time = delay_time + main_object->gameClock();
		//如果炮可用
		if (all_pao_vec[vec_index].recover_time <= fire_time)
		{
			delay_fire_pao(vec_index, delay_time, row, col);
			skip_pao(1);
		}
		else
		{
			popErrorWindowNotInQueue("roofPao : 位于 (#, #) 的炮还有 #cs 恢复", all_pao_vec[vec_index].row, all_pao_vec[vec_index].col, all_pao_vec[vec_index].recover_time - fire_time);
		}
	});
}

void AvZ::PaoOperator::roofPao(const std::vector<Crood> &lst)
{
	for (const auto &each : lst)
	{
		roofPao(each.row, each.col);
	}
}

//屋顶修正时间发炮，单发
void AvZ::PaoOperator::rawRoofPao(int pao_row, int pao_col, int drop_row, float drop_col)
{
	insertOperation([=]() {
		if (main_object->scene() != 4 && main_object->scene() != 5)
		{
			popErrorWindowNotInQueue("rawRoofPao : RawRoofPao函数只适用于RE与ME");
			return;
		}
		Grid grid = {pao_row, pao_col};
		auto it = FindInAllRange(all_pao_vec, grid);
		if (it == all_pao_vec.end())
		{
			popErrorWindowNotInQueue("rawRoofPao : 请检查 (#, #) 是否为炮", pao_row, pao_col);
			return;
		}
		int delay_time = 387 - get_roof_fly_time(pao_col, drop_col);
		delay_fire_pao(it - all_pao_vec.begin(), delay_time, drop_row, drop_col);
	});
}

//屋顶修正时间发炮 多发
void AvZ::PaoOperator::rawRoofPao(const std::vector<PaoDrop> &lst)
{
	for (const auto &each : lst)
	{
		rawRoofPao(each.pao_row, each.pao_col, each.drop_row, each.drop_col);
	}
}