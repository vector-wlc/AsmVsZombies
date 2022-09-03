/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: API card
 */

#include "avz_card.h"

#include "avz_click.h"
#include "avz_global.h"
#include "avz_time_operation.h"
#include "pvzfunc.h"
#include <cstdio>

namespace AvZ {
extern std::map<int, int> __seed_name_to_index_map;
extern std::vector<int> __select_card_vec;
extern PvZ* __pvz_base;
extern MainObject* __main_object;
extern HWND __pvz_hwnd;

/*************************************************
 ** 2021_08_20 版本后弃用
bool ChooseCard(int row, int col)
{
    int yp, xp;
    if (row > 6) // 模仿者卡片
    {
        row -= 6;

        Asm::click(__pvz_base->mouseWindow(), 490, 550, 1);
        if (!ReadMemory<int>(0x6A9EC0, 0x320, 0x94)) {
            return false;
        }
        yp = 160;
        xp = 215;
    } else {
        yp = 160;
        xp = 50;
    }
    yp += (row - 1) * 70;
    xp += (col - 1) * 50;
    Asm::click(__pvz_base->mouseWindow(), xp, yp, 1);

    return true;
}

void ClickBtn(int x, int y)
{
    PostMessage(__pvz_hwnd, WM_LBUTTONDOWN, 0,
        (y & 0xFFFF) << 16 | (x & 0xFFFF));
    PostMessage(__pvz_hwnd, WM_LBUTTONUP, 0, (y & 0xFFFF) << 16 | (x & 0xFFFF));
}

void DealWrongClick()
{
    int z_cnt_max = __main_object->zombieTotal();
    auto zombie_memory = __main_object->zombieArray();
    for (int index = 0; index < z_cnt_max; ++index, ++zombie_memory) {
        if ((zombie_memory->standState() == -2 || zombie_memory->standState() == -3 || zombie_memory->standState() == -4) && zombie_memory->abscissa() > 800 && zombie_memory->abscissa() < 930 && zombie_memory->ordinate() > 370) {
            zombie_memory->abscissa() = 930;
        }
    }
}
*************************************************/

void __ChooseCards()
{
    static auto it = __select_card_vec.begin();

    if (__main_object->text()->disappearCountdown() || __main_object->selectCardUi_m()->orizontalScreenOffset() != 4250) {
        it = __select_card_vec.begin();
        return;
    }

    if (__main_object->globalClock() % 17 != 0) {
        return; // 检测间隔为 17cs
    }

    if (it != __select_card_vec.end()) {
        if (*it >= 49) {
            Asm::chooseImitatorCard(*it - 49);
        } else {
            Asm::chooseCard(*it);
        }
        ++it;
        return;
    }

    if (__pvz_base->selectCardUi_p()->letsRockBtn()->isUnusable()) {
        static int cnt = 0;
        ++cnt;
        Asm::click(__pvz_base->mouseWindow(), 100, 50, 1);
        if (cnt == 10) {
            cnt = 0;
            it = __select_card_vec.begin();
        }

        return;
    }

    // 等待最后一个卡片进入卡槽
    int index = *(it - 1);
    if (index > 48) {
        index = 48;
    }

    if (__pvz_base->selectCardUi_p()->cardMoveState(index) == 1) {
        Asm::rock();
    }
}

void SelectCards(const std::vector<int>& lst)
{
    bool is_find;
    Grid grid;

    __select_card_vec.clear();
    std::set<int> repetitive_type_set;
    bool is_imitator_selected = false;
    for (const auto& card_type : lst) {
        if (card_type > 87) {
            ShowErrorNotInQueue("您选择的代号为 # 的卡片在 PvZ 中不存在",
                card_type);
            return;
        }

        if (repetitive_type_set.find(card_type) == repetitive_type_set.end()) { // 没有被选择的卡片
            repetitive_type_set.insert(card_type);
        } else {
            ShowErrorNotInQueue("您重复选择了代号为 # 的卡片", card_type);
            return;
        }

        if (!is_imitator_selected) {
            is_imitator_selected = (card_type > IMITATOR);
        } else if (card_type > IMITATOR) {
            ShowErrorNotInQueue("您重复选择了模仿者卡片");
            return;
        }
    }

    __select_card_vec = lst;
}

void CardNotInQueue(int seed_index, int row, float col)
{
    auto seed_count = __main_object->seedArray()->count();
    if (seed_index > seed_count || seed_index < 1) {
        ShowErrorNotInQueue(
            "Card : 您填写的参数 # 已溢出，请检查卡片名字是否错写为单引号",
            seed_index);
        return;
    }

    SafeClick();
    auto seed = __main_object->seedArray() + seed_index - 1;
    if (!seed->isUsable()) {
        ShowErrorNotInQueue(
            "Card : 第 # 张卡片还有 #cs 才能使用", seed_index,
            seed->initialCd() - seed->cd() + 1); // PvZ计算问题导致+1
        return;
    }
    SafeClick();
    extern int __error_mode;
    if (__error_mode == CONSOLE) {
        Print("Plant Card (%d) to (%d, %g)\n",
            seed_index, row, col);
    }
    int x;
    int y;
    col = int(col + 0.5);
    GridToCoordinate(row, col, x, y);
    Asm::plantCard(x, y, seed_index - 1);
    SafeClick();
}

void CardNotInQueue(int seed_index, const std::vector<Position>& lst)
{
    auto seed_count = __main_object->seedArray()->count();
    if (seed_index > seed_count || seed_index < 1) {
        ShowErrorNotInQueue("Card : 您填写的参数 # 已溢出", seed_index);
        return;
    }

    SafeClick();
    auto seed = __main_object->seedArray() + seed_index - 1;
    if (!seed->isUsable()) {
        ShowErrorNotInQueue(
            "Card : 第 # 张卡片还有 #cs 才能使用", seed_index,
            seed->initialCd() - seed->cd() + 1); // PvZ计算问题导致+1
        return;
    }
    SafeClick();

    int x = 0;
    int y = 0;
    int col = 0;
    extern int __error_mode;
    for (const auto& crood : lst) {
        col = int(crood.col + 0.5);
        GridToCoordinate(crood.row, col, x, y);
        std::printf("Game clock : %d ||", __main_object->gameClock());
        if (__error_mode == CONSOLE) {
            std::printf("Try Plant Card (%d) to (%d, %g) | ",
                seed_index, crood.row, crood.col);
        }
        Asm::plantCard(x, y, seed_index - 1);
    }
    if (__error_mode == CONSOLE) {
        std::printf("\n");
    }
    SafeClick();
}

int GetCardIndex(PlantType plant_type)
{
    if (__seed_name_to_index_map.empty()) {
        auto seed = __main_object->seedArray();
        if (__pvz_base->gameUi() == 3) {
            int seed_counts = seed->count();
            int seed_type;
            std::pair<int, int> seed_info;
            for (int i = 0; i < seed_counts; ++i, ++seed) {
                seed_type = seed->type();
                //如果是模仿者卡片
                if (seed_type == 48) {
                    seed_type = seed->imitatorType();
                    seed_info.first = seed_type + 49;
                    seed_info.second = i;
                } else { // if(seed_info != 48)
                    seed_info.first = seed_type;
                    seed_info.second = i;
                }
                __seed_name_to_index_map.insert(seed_info);
            }
        }
    }
    auto it = __seed_name_to_index_map.find(plant_type);
    if (it == __seed_name_to_index_map.end()) {
        ShowErrorNotInQueue("你没有选择卡片代号为 # 的植物", plant_type);
        return -1;
    } else {
        return it->second;
    }
}

void Card(int seed_index, int row, float col)
{
    InsertOperation([=]() { CardNotInQueue(seed_index, row, col); },
        "card");
}

void Card(PlantType plant_type, int row, float col)
{
    InsertOperation([=]() {
        int seed_index = GetCardIndex(plant_type);
        if (seed_index == -1) {
            return;
        }
        CardNotInQueue(seed_index + 1, row, col);
    },
        "card");
}

void Card(const std::vector<CardName>& lst)
{
    for (const auto& each : lst) {
        Card(each.plant_type, each.row, each.col);
    }
}

void Card(int seed_index, const std::vector<Position>& lst)
{
    InsertOperation([=]() { CardNotInQueue(seed_index, lst); },
        "card");
}

void Card(PlantType plant_type, const std::vector<Position>& lst)
{
    InsertOperation([=]() {
        int seed_index = GetCardIndex(plant_type);
        if (seed_index == -1) {
            return;
        }
        CardNotInQueue(seed_index + 1, lst);
    },
        "card");
}
} // namespace AvZ
