/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: old memory apis
 */

#include "libavz.h"

int AvZ::getSeedIndex(int type, bool imitator)
{
	auto seed = main_object->seedArray();
	int cnt = seed->count();
	for (int index = 0; index < cnt; ++index, ++seed)
	{
		if (imitator)
		{
			if (seed->type() == 48 && seed->imitatorType() == type)
			{
				return index;
			}
		}
		else if (seed->type() == type)
		{
			return index;
		}
	}

	return -1;
}

int AvZ::getPlantIndex(int row, int col, int type)
{
	auto plant = main_object->plantArray();
	int plants_count_max = main_object->plantCountMax();
	int plant_type;
	for (int i = 0; i < plants_count_max; ++i, ++plant)
	{
		if ((!plant->isDisappeared()) && (!plant->isCrushed()) &&
			(plant->row() + 1 == row) && (plant->col() + 1 == col))
		{
			plant_type = plant->type();
			if (type == -1)
			{
				//如果植物存在	且不为南瓜花盆荷叶
				if ((plant_type != 16) && (plant_type != 30) && (plant_type != 33))
					return i; //返回植物的对象序列
			}
			else
			{
				if (plant_type == type)
				{
					return i;
				}
				else if (type != 16 && type != 30 && type != 33 &&
						 plant_type != 16 && plant_type != 30 && plant_type != 33)
				{
					return -2;
				}
			}
		}
	}
	return -1; //没有符合要求的植物返回-1
}

void AvZ::getPlantIndexs(const std::vector<Grid> &lst_in_, int type, std::vector<int> &indexs_out_)
{
	auto plant = main_object->plantArray();
	indexs_out_.assign(lst_in_.size(), -1);
	Grid grid;

	for (int index = 0; index < main_object->plantCountMax(); ++index, ++plant)
	{
		if (plant->isCrushed() || plant->isDisappeared())
		{
			continue;
		}
		grid.row = plant->row() + 1;
		grid.col = plant->col() + 1;

		auto it_vec = FindSameEle<Grid>(lst_in_, grid);
		if (it_vec.empty())
		{
			continue;
		}
		if (plant->type() == type)
		{
			for (const auto &ele : it_vec)
			{
				indexs_out_[ele - lst_in_.begin()] = index;
			}
		}
		else if (type != 16 && type != 30 && type != 33 &&
				 plant->type() != 16 && plant->type() != 30 && plant->type() != 33)
		{
			for (const auto &ele : it_vec)
			{
				indexs_out_[ele - lst_in_.begin()] = -2;
			}
		}
	}
}

bool AvZ::isZombieExist(int type, int row)
{
	auto zombie = main_object->zombieArray();
	int zombies_count_max = main_object->zombieTotal();
	for (int i = 0; i < zombies_count_max; ++i, ++zombie)
	{
		if (zombie->isExist() && !zombie->isDead())
			if (type < 0 && row < 0)
				return true;
			else if (type >= 0 && row >= 0)
			{
				if (zombie->row() == row - 1 && zombie->type() == type)
					return true;
			}
			else if (type < 0 && row >= 0)
			{
				if (zombie->row() == row - 1)
					return true;
			}
			else //if (type >= 0 && row < 0)
			{
				if (zombie->type() == type)
					return true;
			}
	}

	return false;
}

void AvZ::update_zombies_preview()
{
	// 去掉当前画面上的僵尸
	auto zombie_memory = main_object->zombieArray();
	for (int i = 0; i < main_object->zombieTotal(); ++i, ++zombie_memory)
	{
		zombie_memory->isDisappeared() = true;
		zombie_memory->state() = 3;
	}
	// 重新生成僵尸
	main_object->selectCardUi_m()->isCreatZombie() = false;
}

void AvZ::setZombies(std::initializer_list<int> zombie_type)
{
	while (main_object->text()->disappearCountdown())
	{
		exit_sleep(1);
	}
	std::vector<int> zombie_type_vec;

	for (const auto &type : zombie_type)
	{
		// 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
		if (!RangeIn(type, {QZ_1, BW_9, XQ_13, XR_19, BJ_20, XG_24}))
		{
			zombie_type_vec.push_back(type);
		}
	}
	auto zombie_list = main_object->zombieList();
	for (int index = 0; index < 1000; ++index, ++zombie_list)
	{
		(*zombie_list) = zombie_type_vec[index % zombie_type_vec.size()];
	}

	// 生成旗帜
	for (auto index : {450, 950})
	{
		(*(main_object->zombieList() + index)) = QZ_1;
	}

	// 生成蹦极
	for (auto index : {451, 452, 453, 454, 951, 952, 953, 954})
	{
		(*(main_object->zombieList() + index)) = BJ_20;
	}

	if (pvz_base->gameUi() == 2)
	{
		update_zombies_preview();
	}
}

void AvZ::setWaveZombies(int wave, std::initializer_list<int> zombie_type)
{
	while (main_object->text()->disappearCountdown())
	{
		exit_sleep(1);
	}
	std::vector<int> zombie_type_vec;

	for (const auto &type : zombie_type)
	{
		// 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
		if (!RangeIn(type, {QZ_1, BW_9, XQ_13, XR_19, BJ_20, XG_24}))
		{
			zombie_type_vec.push_back(type);
		}
	}
	auto zombie_list = main_object->zombieList() + (wave - 1) * 50;
	for (int index = 0; index < 50; ++index, ++zombie_list)
	{
		(*zombie_list) = zombie_type_vec[index % zombie_type_vec.size()];
	}

	// 生成旗帜
	for (auto index : {450, 950})
	{
		(*(main_object->zombieList() + index)) = QZ_1;
	}

	// 生成蹦极
	for (auto index : {451, 452, 453, 454, 951, 952, 953, 954})
	{
		(*(main_object->zombieList() + index)) = BJ_20;
	}
}

void AvZ::setWavelength(const std::vector<AvZ::WaveTime> &lst)
{
	auto temp = time_wave_insert;
	for (const auto &ele : lst)
	{
		if (ele.wave < 1 || RangeIn(ele.wave, {9, 19, 20}) || ele.wave > 20)
		{
			showErrorNotInQueue("setWavelength : 您当前设定的 wave 参数为 #, 超出有效范围",
								ele.wave);
			continue;
		}

		if (ele.time < 601 || ele.time > 2500)
		{
			showErrorNotInQueue("setWavelength : 您当前设定第 # 波 的 time 参数为 #, 超出有效范围",
								ele.wave, ele.time);
			continue;
		}

		operation_queue_vec[ele.wave - 1].wave_length = ele.time;

		setTime(1, ele.wave);
		insertOperation([=]() {
			main_object->zombieRefreshHp() = 0;
			int now_time = main_object->gameClock() - operation_queue_vec[ele.wave - 1].refresh_time;
			int countdown = ele.time - now_time;
			if (countdown < 0)
			{
				showErrorNotInQueue("您在第 # 波设定的波长为 #，但是当前时刻点为 #，已超出可设定的时间范围，波长设定失败！",
									ele.wave,
									ele.time,
									now_time);
				return;
			}
			main_object->refreshCountdown() = countdown;
			main_object->initialCountdown() = ele.time;

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