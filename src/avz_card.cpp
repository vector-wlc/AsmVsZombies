/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: old card
 */

#include "libavz.h"

void AvZ::choose_card(int row, int col)
{
	int yp, xp;
	if (row > 6) // 模仿者卡片
	{
		row -= 6;
		do
		{
			click_btn(490, 550, 1);
			exit_sleep(100);
		} while (!read_memory<int>(0x6A9EC0, 0x320, 0x94));
		exit_sleep(100);
		yp = 160;
		xp = 215;
	}
	else
	{
		yp = 160;
		xp = 50;
	}
	yp += (row - 1) * 70;
	xp += (col - 1) * 50;
	click(pvz_base->mouseWindow(), xp, yp, 1);
}

void AvZ::click_btn(int x, int y, int t_ms)
{
	PostMessage(pvz_hwnd, WM_LBUTTONDOWN, 0, (y & 0xFFFF) << 16 | (x & 0xFFFF));
	exit_sleep(t_ms);
	PostMessage(pvz_hwnd, WM_LBUTTONUP, 0, (y & 0xFFFF) << 16 | (x & 0xFFFF));
}

void AvZ::deal_wrong_click()
{
	int z_cnt_max = main_object->zombieTotal();
	auto zombie_memory = main_object->zombieArray();
	for (int index = 0; index < z_cnt_max; ++index, ++zombie_memory)
	{
		if (zombie_memory->abscissa() > 800 &&
			zombie_memory->abscissa() < 930 &&
			zombie_memory->ordinate() > 370)
		{
			zombie_memory->abscissa() = 930;
		}
	}
}

void AvZ::lets_rock()
{
	exit_sleep(90);
	// click 这里会出现比较玄学的按下去抬不上来的现象，所以就用 PostMessage 了。
	click_btn(234, 565, 75);

	exit_sleep(250);
	//出现警告框时
	while (pvz_base->mouseWindow()->topWindow())
	{
		click_btn(320, 400, 75);
		exit_sleep(150);
	}
}

void AvZ::selectCards(const std::vector<std::string> &lst)
{
	// 等待 "Survival Endless" 消失
	while (main_object->text()->disappearCountdown())
	{
		exit_sleep(1);
	}

	if (pvz_base->gameUi() != 2)
	{
		return;
	}

	while (main_object->selectCardUi_m()->orizontalScreenOffset() != 4250)
	{
		exit_sleep(1);
	}

	exit_sleep(50);

	if (pvz_base->mouseWindow()->isInWindow())
	{
		showErrorNotInQueue("检测到您的鼠标在游戏窗口内，这种行为可能会导致选卡失败，选卡时请尽量将鼠标移到窗口外");
	}
	deal_wrong_click();

	bool is_find;
	int col;
	while (true)
	{
		for (const auto &seed_name : lst)
		{
			is_find = false;
			for (int row = 0; row < 11; ++row)
			{
				for (col = 0; col < 8; ++col)
				{
					if (seed_name == seed_name_list[row][col])
					{
						is_find = true;
						++row;
						++col;
						break;
					}
				}

				if (is_find)
				{
					if (pvz_base->gameUi() != 2 || pvz_base->mouseWindow()->topWindow())
					{
						return;
					}
					choose_card(row, col);
					exit_sleep(80);
					break;
				}
			}
		}

		if (pvz_base->selectCardUi_p()->letsRockBtn()->isUnusable())
		{
			for (int i = 0; i < 10; ++i)
			{
				click(pvz_base->mouseWindow(), 100, 50, 1);
				exit_sleep(30);
			}
		}
		else
		{
			break;
		}
	}

	lets_rock();
}

void AvZ::cardNotInQueue(int seed_index, int row, float col)
{
	if (seed_index > 10 || seed_index < 1)
	{
		showErrorNotInQueue("Card : 您填写的参数 # 已溢出，请检查卡片名字是否错写为单引号", seed_index);
		return;
	}

	auto seed = main_object->seedArray() + seed_index - 1;
	if (!seed->isUsable())
	{
		showErrorNotInQueue("Card : 第 # 张卡片还有 #cs 才能使用", seed_index, seed->initialCd() - seed->cd() + 1); // PvZ计算问题导致+1
		return;
	}
	safeClick();
	clickSeed(seed_index);
	clickGrid(row, col);
	safeClick();
}

void AvZ::cardNotInQueue(int seed_index, const std::vector<Crood> &lst)
{
	if (seed_index > 10 || seed_index < 1)
	{
		showErrorNotInQueue("Card : 您填写的参数 # 已溢出，请检查卡片名字是否错写为单引号", seed_index);
		return;
	}

	auto seed = main_object->seedArray() + seed_index - 1;
	if (!seed->isUsable())
	{
		showErrorNotInQueue("Card : 第 # 张卡片还有 #cs 才能使用", seed_index, seed->initialCd() - seed->cd() + 1); // PvZ计算问题导致+1
		return;
	}
	safeClick();
	clickSeed(seed_index);
	for (const auto &crood : lst)
	{
		clickGrid(crood.row, crood.col);
	}
	safeClick();
}

int AvZ::get_seed_index_for_seed_name(const std::string &seed_name)
{
	if (!is_get_seed_index)
	{
		auto seed = main_object->seedArray();
		if (pvz_base->gameUi() == 3)
		{
			int seed_counts = seed->count();
			int seed_type;
			std::pair<std::string, int> seed_info;
			for (int i = 0; i < seed_counts; ++i, ++seed)
			{
				seed_type = seed->type();
				//如果是模仿者卡片
				if (seed_type == 48)
				{
					seed_type = seed->imitatorType();
					seed_info.first = seed_name_list[seed_type / 8 + 6][seed_type % 8];
					seed_info.second = i;
				}
				else //if(seed_info != 48)
				{
					seed_info.first = seed_name_list[seed_type / 8][seed_type % 8];
					seed_info.second = i;
				}
				seed_name_to_index_map.insert(seed_info);
			}
			is_get_seed_index = true;
		}
		else
		{
			is_get_seed_index = false;
		}
	}
	auto it = seed_name_to_index_map.find(seed_name);
	if (it == seed_name_to_index_map.end())
	{
		showErrorNotInQueue("卡片名称'#'未被录入 AvZ ,或者您没有选择该卡片", seed_name.c_str());
		return -1;
	}
	else
	{
		return it->second;
	}
}

void AvZ::card(int seed_index, int row, float col)
{
	insertOperation([=]() {
		cardNotInQueue(seed_index, row, col);
	},
					"card");
}

void AvZ::card(const std::vector<CardIndex> &lst)
{
	insertOperation([=]() {
		for (const auto &each : lst)
		{
			cardNotInQueue(each.seed_index, each.row, each.col);
		}
	},
					"card");
}

void AvZ::card(const std::string &seed_name, int row, float col)
{
	insertOperation([=]() {
		int seed_index = get_seed_index_for_seed_name(seed_name);
		if (seed_index == -1)
		{
			return;
		}
		cardNotInQueue(seed_index + 1, row, col);
	},
					"card");
}

void AvZ::card(const std::vector<CardName> &lst)
{
	insertOperation([=]() {
		for (const auto &each : lst)
		{
			int seed_index = get_seed_index_for_seed_name(each.seed_name);
			if (seed_index == -1)
			{
				return;
			}
			cardNotInQueue(seed_index + 1, each.row, each.col);
		}
	},
					"card");
}

void AvZ::card(int seed_index, const std::vector<Crood> &lst)
{
	insertOperation([=]() {
		cardNotInQueue(seed_index, lst);
	},
					"card");
}

void AvZ::card(const std::string &seed_name, const std::vector<Crood> &lst)
{
	insertOperation([=]() {
		int seed_index = get_seed_index_for_seed_name(seed_name);
		if (seed_index == -1)
		{
			return;
		}
		cardNotInQueue(seed_index + 1, lst);
	},
					"card");
}
