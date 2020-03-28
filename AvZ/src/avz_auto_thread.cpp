/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: old auto thread CLASS
 */

#include "avz.h"

void AvZ::VThread::pushFunc(const std::function<void()> &run)
{
	//如果没有找到停下来的线程则创建新线程
	if (stoped_thread_id_stack.empty())
	{
		thread_vec.push_back({run, &thread_id});
		thread_id = thread_vec.size() - 1;
	}
	else
	{
		thread_id = stoped_thread_id_stack.top();
		thread_vec[thread_id] = {run, &thread_id};
		stoped_thread_id_stack.pop();
	}
}

void AvZ::ItemCollector::run()
{
	if (main_object->gameClock() % time_interval != 0 ||
		pvz_base->gameUi() != 3 ||
		main_object->gamePaused() ||
		main_object->mouseAttribution()->type() != 0)
	{
		return;
	}

	if (is_paused)
	{
		return;
	}

	auto item = main_object->itemArray();
	for (int index = 0; index < main_object->itemTotal(); ++index, ++item)
	{
		if (item->is_collected() || item->isDisappeared())
		{
			continue;
		}
		float item_x = item->abscissa();
		float item_y = item->ordinate();
		if (item_x >= 0.0 && item_y >= 70)
		{
			safeClick();
			int x = static_cast<int>(item_x + 30);
			int y = static_cast<int>(item_y + 30);
			leftClick(x, y);
			safeClick();
			break;
		}
	}
}

////////////////////////////////////////
//  IceFiller
////////////////////////////////////////

void AvZ::IceFiller::start(const std::vector<Grid> &lst)
{
	insertOperation([=]() {
		ice_seed_index_vec.clear();
		if (!thread_examine())
		{
			return;
		}
		is_paused = false;
		int ice_seed_index;
		ice_seed_index = getSeedIndex(HBG_14);
		if (ice_seed_index != -1)
		{
			ice_seed_index_vec.push_back(ice_seed_index);
		}
		ice_seed_index = getSeedIndex(HBG_14, true);
		if (ice_seed_index != -1)
		{
			ice_seed_index_vec.push_back(ice_seed_index);
		}
		coffee_seed_index = getSeedIndex(KFD_35);
		fill_ice_grid_vec = lst;
		pushFunc([=]() { run(); });
	});
}

void AvZ::IceFiller::run()
{
	if (pvz_base->gameUi() != 3 && main_object->gamePaused())
	{
		return;
	}

	if (is_paused)
	{
		return;
	}

	static auto plant = main_object->plantArray();
	static auto seed = main_object->seedArray();
	static std::vector<int> ice_plant_index_vec;
	static decltype(ice_plant_index_vec.begin()) ice_plant_index_it;
	static decltype(ice_seed_index_vec.begin()) ice_seed_index_it;
	static decltype(fill_ice_grid_vec.begin()) fill_ice_grid_it;
	static bool is_get_indexs = false;

	is_get_indexs = false;
	fill_ice_grid_it = fill_ice_grid_vec.begin();

	for (ice_seed_index_it = ice_seed_index_vec.begin();
		 ice_seed_index_it != ice_seed_index_vec.end();
		 ++ice_seed_index_it)
	{
		seed = main_object->seedArray() + *ice_seed_index_it;
		if (!seed->isUsable())
		{
			continue;
		}
		if (!is_get_indexs)
		{
			getPlantIndexs(fill_ice_grid_vec, HBG_14, ice_plant_index_vec);
			ice_plant_index_it = ice_plant_index_vec.begin();
			is_get_indexs = true;
		}

		for (; ice_plant_index_it != ice_plant_index_vec.end();
			 ++fill_ice_grid_it, ++ice_plant_index_it)
		{
			if ((*ice_plant_index_it) == -1)
			{
				AvZ::cardNotInQueue(*ice_seed_index_it + 1, fill_ice_grid_it->row, fill_ice_grid_it->col);
				++fill_ice_grid_it;
				++ice_plant_index_it;
				break;
			}
		}
	}
}

void AvZ::IceFiller::coffee()
{
	insertOperation([=]() {
		if (coffee_seed_index == -1)
		{
			popErrorWindowNotInQueue("你没有选择咖啡豆卡片!");
			return;
		}

		safeClick();
		clickSeed(coffee_seed_index + 1);
		auto fill_grid_it = fill_ice_grid_vec.end();
		do
		{
			--fill_grid_it;
			clickGrid(fill_grid_it->row, fill_grid_it->col);
		} while (fill_grid_it != fill_ice_grid_vec.begin());
	});
}

/////////////////////////////////////////////////
//    PlantFixer
/////////////////////////////////////////////////

void AvZ::PlantFixer::autoGetFixList()
{
	grid_lst.clear();
	auto plant = main_object->plantArray();
	int plant_cnt_max = main_object->plantCountMax();
	Grid grid;
	for (int index = 0; index < plant_cnt_max; ++index, ++plant)
	{
		if (!plant->isCrushed() && !plant->isDisappeared() && plant->type() == plant_type)
		{
			grid.col = plant->col() + 1;
			grid.row = plant->row() + 1;
			grid_lst.push_back(grid);
		}
	}
}

bool AvZ::PlantFixer::use_seed_(int seed_index, int row, float col, bool is_need_shovel)
{
	if (is_use_coffee)
	{
		if (coffee_seed_index == -1)
		{
			return false;
		}
		auto coffee_seed = main_object->seedArray();
		if (!coffee_seed->isUsable())
		{
			return false;
		}
	}
	if (is_need_shovel)
	{
		if (plant_type == NGT_30)
		{
			AvZ::shovelNotInQueue(row, col, true);
		}
		else
		{
			AvZ::shovelNotInQueue(row, col);
		}
	}
	cardNotInQueue(seed_index + 1, row, col);
	if (is_use_coffee)
	{
		cardNotInQueue(coffee_seed_index + 1, row, col);
	}
	return true;
}

void AvZ::PlantFixer::get_seed_list()
{
	seed_index_vec.clear();
	int seed_index;
	seed_index = getSeedIndex(plant_type);
	if (-1 != seed_index)
	{
		seed_index_vec.push_back(seed_index);
	}
	seed_index = getSeedIndex(plant_type, true);

	if (-1 != seed_index)
	{
		seed_index_vec.push_back(seed_index);
	}
	if (seed_index_vec.size() == 0)
	{
		popErrorWindowNotInQueue("您没有选择修补该植物的卡片！");
	}
	leaf_seed_index = getSeedIndex(HY_16);
	coffee_seed_index = getSeedIndex(KFD_35);
}

void AvZ::PlantFixer::start(int _plant_type, const std::vector<Grid> &lst, int _fix_hp)
{
	insertOperation([=]() {
		if (!thread_examine())
		{
			return;
		}
		is_paused = false;
		if (_plant_type >= JQSS_40)
		{
			popErrorWindowNotInQueue("修补植物类仅支持绿卡");
			return;
		}

		plant_type = _plant_type;
		fix_hp = _fix_hp;
		get_seed_list();
		//如果没有给出列表信息
		if (lst.size() == 0)
		{
			autoGetFixList();
		}
		else
		{
			grid_lst = lst;
		}
		pushFunc([=]() { run(); });
	});
}

void AvZ::PlantFixer::run()
{
	if (pvz_base->gameUi() != 3 && main_object->gamePaused())
	{
		return;
	}

	if (is_paused)
	{
		return;
	}

	static Seed *seed_memory;
	static Seed *leaf_seed_memory = main_object->seedArray() + leaf_seed_index;
	static Plant *plant;
	static std::vector<int> plant_index_vec;
	static Grid need_plant_grid; //记录要使用植物的格子
	static int min_hp;			 //记录要使用植物的格子
	static bool is_seed_used;	//种子是否被使用
	static decltype(seed_index_vec.begin()) usable_seed_index_it;
	static decltype(plant_index_vec.begin()) plant_index_it;
	static decltype(grid_lst.begin()) grid_it;

	usable_seed_index_it = seed_index_vec.begin();

	if (usable_seed_index_it == seed_index_vec.end())
	{
		return;
	}

	do
	{
		seed_memory = main_object->seedArray();
		seed_memory += *usable_seed_index_it;
		if (seed_memory->isUsable())
		{
			break;
		}
		++usable_seed_index_it;
	} while (usable_seed_index_it != seed_index_vec.end());

	// 没找到可用的卡片
	if (usable_seed_index_it == seed_index_vec.end())
		return;

	getPlantIndexs(grid_lst, plant_type, plant_index_vec);

	is_seed_used = false;
	need_plant_grid.row = need_plant_grid.col = 0; //格子信息置零
	min_hp = fix_hp;							   //最小生命值重置

	for (grid_it = grid_lst.begin(), plant_index_it = plant_index_vec.begin();
		 grid_it != grid_lst.end();
		 ++grid_it, ++plant_index_it)
	{
		//如果此处存在除植物类植物的植物
		if (*plant_index_it == -2)
		{
			continue;
		}

		if (*plant_index_it == -1)
		{
			//如果为池塘场景而且在水路
			if ((main_object->scene() == 2 || main_object->scene() == 3) &&
				(grid_it->row == 3 || grid_it->row == 4))
			{ //如果不存在荷叶
				if (getPlantIndex(grid_it->row, grid_it->col, 16) == -1)
				{
					//如果荷叶卡片没有恢复
					if (leaf_seed_index == -1 || !leaf_seed_memory->isUsable())
						continue;

					cardNotInQueue(leaf_seed_index + 1, grid_it->row, grid_it->col);
				}
			}
			is_seed_used = use_seed_((*usable_seed_index_it), grid_it->row, grid_it->col, false);
			break;
		}
		else
		{
			plant = main_object->plantArray();
			plant += *plant_index_it;
			int plant_hp = plant->hp();
			//如果当前生命值低于最小生命值，记录下来此植物的信息
			if (plant_hp < min_hp)
			{
				min_hp = plant_hp;
				need_plant_grid.row = grid_it->row;
				need_plant_grid.col = grid_it->col;
			}
		}
	}

	//如果有需要修补的植物且植物卡片能用则进行种植
	if (need_plant_grid.row && !is_seed_used)
	{
		//种植植物
		use_seed_((*usable_seed_index_it), need_plant_grid.row, need_plant_grid.col, true);
	}
}

/////////////////////////////////////////////////
//    PlantFixer
/////////////////////////////////////////////////

void AvZ::KeyConnector::add(char key, std::function<void()> operate)
{
	if (key >= 'a' && key <= 'z')
		key -= 32;

	for (const auto &key_operation : key_operation_vec)
	{
		if (key_operation.first == key)
		{
			popErrorWindowNotInQueue("按键 # 绑定了多个操作", key);
			return;
		}
	}

	key_operation_vec.push_back(std::pair<char, std::function<void()>>(key, operate));

	if (thread_id < 0)
	{
		pushFunc([=]() {
			for (const auto &key_operation : key_operation_vec)
			{
				if ((GetAsyncKeyState(key_operation.first) & 0x8001) == 0x8001)
				{
					setTime(nowTimeWave());
					key_operation.second();
					return;
				}
			}
		});
	}
}