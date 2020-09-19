/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: old auto thread CLASS
 */

#include "libavz.h"

// *** Not In Queue
// 通过得到线程的状态
// *** 返回值：
// 停止状态：return STOPED
// 暂停状态：return PAUSED
// 运行状态：return RUNNING
AvZ::TickRunner::ThreadStatus AvZ::TickRunner::getStatus() const
{
	if (thread_id < 0)
	{
		return STOPPED;
	}

	if (is_paused)
	{
		return PAUSED;
	}

	return RUNNING;
}

void AvZ::TickRunner::pushFunc(const std::function<void()> &_run)
{
	if (!thread_examine())
	{
		return;
	}
	is_paused = false;
	auto run = [=]() {
		if (is_paused)
		{
			return;
		}

		_run();
	};
	//如果没有找到停下来的线程则创建新线程
	if (stopped_thread_id_stack.empty())
	{
		thread_vec.push_back({run, &thread_id});
		thread_id = thread_vec.size() - 1;
	}
	else
	{
		thread_id = stopped_thread_id_stack.top();
		thread_vec[thread_id] = {run, &thread_id};
		stopped_thread_id_stack.pop();
	}
}

void AvZ::ItemCollector::run()
{
	if (main_object->gameClock() % time_interval != 0 ||
		main_object->mouseAttribution()->type() != 0)
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

void AvZ::IceFiller::resetIceSeedList(const std::vector<int> &lst)
{
	insertOperation([=]() {
		ice_seed_index_vec.clear();
		for (const auto &ice_index : lst)
		{
			auto seed_memory = main_object->seedArray() + ice_index - 1;
			if (seed_memory->type() != ICE_SHROOM || seed_memory->imitatorType() != ICE_SHROOM)
			{
				showErrorNotInQueue("请检查第 # 张卡片是否为冰卡", ice_index);
			}
			ice_seed_index_vec.push_back(ice_index - 1);
		}
	},
					"resetIceSeedList");
}

void AvZ::IceFiller::start(const std::vector<Grid> &lst)
{
	insertOperation([=]() {
		ice_seed_index_vec.clear();
		int ice_seed_index;
		ice_seed_index = getSeedIndex(ICE_SHROOM);
		if (ice_seed_index != -1)
		{
			ice_seed_index_vec.push_back(ice_seed_index);
		}
		ice_seed_index = getSeedIndex(ICE_SHROOM, true);
		if (ice_seed_index != -1)
		{
			ice_seed_index_vec.push_back(ice_seed_index);
		}
		coffee_seed_index = getSeedIndex(COFFEE_BEAN);
		fill_ice_grid_vec = lst;
		pushFunc([=]() { run(); });
	},
					"startFillIce");
}

void AvZ::IceFiller::run()
{
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
			getPlantIndexs(fill_ice_grid_vec, ICE_SHROOM, ice_plant_index_vec);
			ice_plant_index_it = ice_plant_index_vec.begin();
			is_get_indexs = true;
		}

		for (; ice_plant_index_it != ice_plant_index_vec.end();
			 ++fill_ice_grid_it, ++ice_plant_index_it)
		{
			if ((*ice_plant_index_it) == -1)
			{
				//如果为池塘场景而且在水路
				if ((main_object->scene() == 2 || main_object->scene() == 3) &&
					(fill_ice_grid_it->row == 3 || fill_ice_grid_it->row == 4))
				{
					//如果不存在荷叶
					if (getPlantIndex(fill_ice_grid_it->row, fill_ice_grid_it->col, LILY_PAD) == -1)
					{
						continue;
					}
				}

				// 如果是天台场景
				if (main_object->scene() == 4 || main_object->scene() == 5)
				{
					if (getPlantIndex(fill_ice_grid_it->row, fill_ice_grid_it->col, FLOWER_POT) == -1)
					{
						continue;
					}
				}

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
			showErrorNotInQueue("你没有选择咖啡豆卡片!");
			return;
		}

		if (fill_ice_grid_vec.empty())
		{
			showErrorNotInQueue("你还未为自动存冰对象初始化存冰列表");
			return;
		}
		std::vector<int> ice_plant_index_vec;
		getPlantIndexs(fill_ice_grid_vec, ICE_SHROOM, ice_plant_index_vec);

		auto fill_grid_it = fill_ice_grid_vec.end();
		do
		{
			--fill_grid_it;
			if (ice_plant_index_vec[fill_grid_it - fill_ice_grid_vec.begin()] > -1)
			{
				safeClick();
				cardNotInQueue(coffee_seed_index + 1,
							   fill_grid_it->row,
							   fill_grid_it->col);
				safeClick();
				return;
			}
		} while (fill_grid_it != fill_ice_grid_vec.begin());

		showErrorNotInQueue("coffee : 未找到可用的存冰");
	},
					"coffee");
}

/////////////////////////////////////////////////
//    PlantFixer
/////////////////////////////////////////////////

void AvZ::PlantFixer::auto_get_fix_list()
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

void AvZ::PlantFixer::autoGetFixList()
{
	insertOperation([=]() {
		auto_get_fix_list();
	},
					"autoGetFixList");
}

void AvZ::PlantFixer::use_seed_(int seed_index, int row, float col, bool is_need_shovel)
{
	if (is_need_shovel)
	{
		AvZ::shovelNotInQueue(row, col, plant_type == PUMPKIN);
	}
	cardNotInQueue(seed_index + 1, row, col);
	if (is_use_coffee)
	{
		cardNotInQueue(coffee_seed_index + 1, row, col);
	}
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
		showErrorNotInQueue("您没有选择修补该植物的卡片！");
	}
	coffee_seed_index = getSeedIndex(COFFEE_BEAN);
}

void AvZ::PlantFixer::start(int _plant_type, const std::vector<Grid> &lst, int _fix_hp)
{
	if (_plant_type == COFFEE_BEAN)
	{
		showErrorNotInQueue("PlantFixer 不支持修补咖啡豆");
		return;
	}

	if (_plant_type >= GATLING_PEA)
	{
		showErrorNotInQueue("修补植物类仅支持绿卡");
		return;
	}

	insertOperation([=]() {
		plant_type = _plant_type;
		fix_hp = _fix_hp;
		get_seed_list();
		//如果没有给出列表信息
		if (lst.size() == 0)
		{
			auto_get_fix_list();
		}
		else
		{
			grid_lst = lst;
		}
		pushFunc([=]() { run(); });
	},
					"startFixPlant");
}

void AvZ::PlantFixer::run()
{
	static Seed *seed_memory;
	static Plant *plant;
	static std::vector<int> plant_index_vec;
	static Grid need_plant_grid; //记录要使用植物的格子
	static int min_hp;			 //记录要使用植物的格子
	static bool is_seed_used;	 //种子是否被使用
	static decltype(seed_index_vec.begin()) usable_seed_index_it;
	static decltype(plant_index_vec.begin()) plant_index_it;
	static decltype(grid_lst.begin()) grid_it;

	usable_seed_index_it = seed_index_vec.begin();

	if (usable_seed_index_it == seed_index_vec.end())
	{
		return;
	}

	if (is_use_coffee)
	{
		if (coffee_seed_index == -1)
		{
			return;
		}
		auto coffee_seed = main_object->seedArray() + coffee_seed_index;
		if (!coffee_seed->isUsable())
		{
			return;
		}
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
	{
		return;
	}
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
			{
				//如果不存在荷叶
				if (getPlantIndex(grid_it->row, grid_it->col, LILY_PAD) == -1 && plant_type != LILY_PAD)
				{
					continue;
				}
			}

			// 如果是天台场景
			if (main_object->scene() == 4 || main_object->scene() == 5)
			{
				if (getPlantIndex(grid_it->row, grid_it->col, FLOWER_POT) == -1 && plant_type != FLOWER_POT)
				{
					continue;
				}
			}

			use_seed_((*usable_seed_index_it), grid_it->row, grid_it->col, false);
			is_seed_used = true;
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
			showErrorNotInQueue("按键 # 绑定了多个操作", key);
			return;
		}
	}

	key_operation_vec.push_back(std::pair<char, std::function<void()>>(key, operate));

	if (getStatus() == STOPPED)
	{
		pushFunc([=]() {
			for (const auto &key_operation : key_operation_vec)
			{
				if ((GetAsyncKeyState(key_operation.first) & 0x8001) == 0x8001 &&
					GetForegroundWindow() == pvz_hwnd) // 检测 pvz 是否为顶层窗口
				{
					InsertGuard insert_guard(false);
					key_operation.second();
					return;
				}
			}
		});
	}
}