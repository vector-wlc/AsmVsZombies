/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: CLASS PaoOperator
 */

#include "avz_cannon.h"

#include "avz_card.h"
#include "avz_click.h"
#include "avz_memory.h"

namespace AvZ {
extern int __error_mode;
extern MainObject* __main_object;
extern PvZ* __pvz_base;
extern HWND __pvz_hwnd;
PaoOperator::RoofFlyTime PaoOperator::fly_time_data[8] = {
    {515, 359},
    {499, 362},
    {515, 364},
    {499, 367},
    {515, 369},
    {499, 372},
    {511, 373},
    {511, 373},
};
TickRunner PaoOperator::tick_runner;
std::set<int> PaoOperator::lock_pao_set; // 锁定的炮

void PaoOperator::initialState()
{
    lock_pao_set.clear();
}

// 得到炮的恢复时间
int PaoOperator::get_recover_time(int index)
{
    auto cannon = __main_object->plantArray() + index;
    if (cannon->isDisappeared() || cannon->type() != COB_CANNON) {
        return NO_EXIST_RECOVER_TIME;
    }
    auto animation_memory = __pvz_base->animationMain()->animationOffset()->animationArray() + cannon->animationCode();

    switch (cannon->state()) {
    case 35:
        return 125 + cannon->stateCountdown();
    case 36:
        return int(125 * (1 - animation_memory->circulationRate()) + 0.5) + 1;
    case 37:
        return 0;
    case 38:
        return 3125 + int(350 * (1 - animation_memory->circulationRate()) + 0.5);
    default:
        return NO_EXIST_RECOVER_TIME;
    }
}

//获取屋顶炮飞行时间
int PaoOperator::get_roof_fly_time(int pao_col, float drop_col)
{
    //得到落点对应的横坐标
    int drop_x = static_cast<int>(drop_col * 80);
    //得到该列炮最小飞行时间对应的最小的横坐标
    int min_drop_x = fly_time_data[pao_col - 1].min_drop_x;
    //得到最小的飞行时间
    int min_fly_time = fly_time_data[pao_col - 1].min_fly_time;
    //返回飞行时间
    return (drop_x >= min_drop_x
            ? min_fly_time
            : (min_fly_time + 1 - (drop_x - (min_drop_x - 1)) / 32));
}

void PaoOperator::base_fire_pao(int cannon_index, int drop_row, float drop_col)
{
    SafeClick();
    int x = 0;
    int y = 0;
    if (__error_mode == CONSOLE) {
        auto plant = GetMainObject()->plantArray() + cannon_index;
        std::printf("Game clock : %d || shoot from (%d, %d) to (%d, %g)\n",
            __main_object->gameClock(),
            plant->row() + 1,
            plant->col() + 1,
            drop_row,
            drop_col);
    }
    GridToCoordinate(drop_row, drop_col, x, y);
    Asm::shootPao(x, y, cannon_index);
    SafeClick();
}

void PaoOperator::delay_fire_pao(int delay_time,
    int cannon_index,
    int row,
    float col)
{
    if (delay_time != 0) {
        // 将操作动态插入消息队列
        lock_pao_set.insert(cannon_index);
        InsertGuard insert_guard(true);
        SetDelayTime(delay_time);
        InsertOperation([=]() {
            base_fire_pao(cannon_index, row, col);
            lock_pao_set.erase(cannon_index);
        },
            "delay_fire_pao");
    } else {
        base_fire_pao(cannon_index, row, col);
    }
}

// 用户自定义炮位置发炮：单发
void PaoOperator::rawPao(int pao_row, int pao_col, int drop_row, float drop_col)
{
    InsertOperation([=]() {
        int index = GetPlantIndex(pao_row, pao_col, COB_CANNON);
        if (index < 0) {
            ShowErrorNotInQueue("请检查 (#, #) 是否为炮", pao_row, pao_col);
            return;
        }

        int recover_time = get_recover_time(index);
        if (recover_time > 0) {
            ShowErrorNotInQueue("位于 (#, #) 的炮还有 # cs 恢复",
                pao_row,
                pao_col,
                recover_time);
            return;
        }
        base_fire_pao(index, drop_row, drop_col);
    },
        "rawPao");
}

//用户自定义炮位置发炮：多发
void PaoOperator::rawPao(const std::vector<PaoDrop>& lst)
{
    for (const auto& each : lst) {
        rawPao(each.pao_row, each.pao_col, each.drop_row, each.drop_col);
    }
}

//屋顶修正时间发炮，单发
void PaoOperator::rawRoofPao(int pao_row, int pao_col, int drop_row, float drop_col)
{
    InsertOperation([=]() {
        if (__main_object->scene() != 4 && __main_object->scene() != 5) {
            ShowErrorNotInQueue("rawRoofPao : RawRoofPao函数只适用于RE与ME");
            return;
        }
        int index = GetPlantIndex(pao_row, pao_col, COB_CANNON);
        if (index < 0) {
            ShowErrorNotInQueue("请检查 (#, #) 是否为炮", pao_row, pao_col);
            return;
        }

        int recover_time = get_recover_time(index);
        int delay_time = 387 - get_roof_fly_time(pao_col, drop_col);
        if (recover_time > delay_time) {
            ShowErrorNotInQueue("位于 (#, #) 的炮还有 # cs 恢复",
                pao_row,
                pao_col,
                recover_time - delay_time);
            return;
        }
        delay_fire_pao(delay_time, index, drop_row, drop_col);
    },
        "rawRoofPao");
}

//屋顶修正时间发炮 多发
void PaoOperator::rawRoofPao(const std::vector<PaoDrop>& lst)
{
    for (const auto& each : lst) {
        rawRoofPao(each.pao_row, each.pao_col, each.drop_row, each.drop_col);
    }
}

void PaoOperator::plantPao(int row, int col)
{
    InsertOperation([=]() {
        tick_runner.pushFunc([=]() {
            int ymjnp_seed_index = GetSeedIndex(COB_CANNON);
            int ymts_seed_index = GetSeedIndex(KERNEL_PULT);

            if (ymjnp_seed_index == -1 || ymts_seed_index == -1) {
                return;
            }

            for (int t_col = col; t_col < col + 2; ++t_col) {
                if (GetPlantIndex(row, t_col, KERNEL_PULT) != -1) {
                    continue;
                }

                auto seed_memory = __main_object->seedArray() + ymts_seed_index;
                if (!seed_memory->isUsable()) {
                    return;
                }

                CardNotInQueue(ymts_seed_index + 1, row, t_col);
            }

            auto seed_memory = __main_object->seedArray() + ymjnp_seed_index;
            if (!seed_memory->isUsable()) {
                return;
            }

            CardNotInQueue(ymjnp_seed_index + 1, row, col);
            InsertGuard insert_guard(false);
            tick_runner.stop();
        });
    },
        "plantPao");
}

PaoOperator::PaoOperator()
{
    sequential_mode = TIME;
    next_pao = 0;
}

//用户重置炮列表
void PaoOperator::resetPaoList(const std::vector<Grid>& lst)
{
    InsertOperation([=]() {
        next_pao = 0;

        // 重置炮列表
        pao_grid_vec = lst;
        GetPlantIndices(pao_grid_vec, COB_CANNON, pao_index_vec);
        auto pao_index_it = pao_index_vec.begin();
        auto pao_grid_it = pao_grid_vec.begin();
        while (pao_index_it != pao_index_vec.end()) {
            if ((*pao_index_it) < 0) {
                ShowErrorNotInQueue(
                    "resetPaoList : 请检查 (#, #) 位置是否为炮",
                    pao_grid_it->row,
                    pao_grid_it->col);
                return;
            }

            ++pao_grid_it;
            ++pao_index_it;
        }
    },
        "resetPaoList");
}

// *** In Queue
// 自动填充炮列表
// *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
void PaoOperator::autoGetPaoList()
{
    InsertOperation([=]() {
        pao_grid_vec.clear();
        next_pao = 0;
        Grid pao_grid = {0, 0};
        auto plant_array = __main_object->plantArray();
        for (int index = 0; index < __main_object->plantCountMax(); ++index) {
            if (!plant_array[index].isCrushed() && !plant_array[index].isDisappeared() && plant_array[index].type() == COB_CANNON) {
                pao_grid = {plant_array[index].row() + 1,
                    plant_array[index].col() + 1};
                pao_grid_vec.push_back(pao_grid);
            }
        }

        // 对得到的炮的位置进行排序
        std::sort(pao_grid_vec.begin(), pao_grid_vec.end());

        // 得到对应的炮的序号
        GetPlantIndices(pao_grid_vec, COB_CANNON, pao_index_vec);
    });
}

void PaoOperator::setNextPao(int temp_next_pao)
{
    InsertOperation([=]() {
        if (temp_next_pao > pao_grid_vec.size()) {
            ShowErrorNotInQueue(
                "setNextPao : 本炮列表中一共有 # 门炮，您设的参数已溢出",
                pao_grid_vec.size());
            return;
        }
        next_pao = temp_next_pao - 1;
    },
        "setNextPao");
}

void PaoOperator::setNextPao(int row, int col)
{
    InsertOperation([=]() {
        int temp_next_pao = 0;
        Grid grid = {row, col};
        auto it = FindInAllRange(pao_grid_vec, grid);

        if (it != pao_grid_vec.end()) {
            next_pao = it - pao_grid_vec.begin();
        } else {
            ShowErrorNotInQueue(
                "setNextPao : 请检查(#, #)是否在本炮列表中", row, col);
            return;
        }
    },
        "setNextPao");
}

void PaoOperator::fixLatestPao()
{
    InsertOperation([=]() {
        if (lastest_pao_msg.vec_index == -1) {
            ShowErrorNotInQueue("fixLastPao ：您尚未使用炮");
            return;
        }
        lastest_pao_msg.is_writable = false; // 锁定信息
        int delay_time = lastest_pao_msg.fire_time + 205 - __main_object->gameClock();
        if (delay_time < 0) {
            delay_time = 0;
        }
        InsertGuard insert_guard(true);
        SetDelayTime(delay_time);
        InsertOperation([=]() {
            lastest_pao_msg.is_writable = true; // 解锁信息
            ShovelNotInQueue(pao_grid_vec[lastest_pao_msg.vec_index].row,
                pao_grid_vec[lastest_pao_msg.vec_index].col);
        });
        plantPao(pao_grid_vec[lastest_pao_msg.vec_index].row,
            pao_grid_vec[lastest_pao_msg.vec_index].col);
    },
        "fixLatestPao");
}

int PaoOperator::get_recover_time_vec()
{
    int time = get_recover_time(pao_index_vec[next_pao]);
    if (time == NO_EXIST_RECOVER_TIME) {
        int index = GetPlantIndex(
            pao_grid_vec[next_pao].row, pao_grid_vec[next_pao].col, COB_CANNON);
        if (index < 0) // 找不到本来位置的炮
        {
            ShowErrorNotInQueue("请检查位于 (#, #) 的第 # 门炮是否存在",
                pao_grid_vec[next_pao].row,
                pao_grid_vec[next_pao].col,
                next_pao + 1);
            return NO_EXIST_RECOVER_TIME;
        }
        pao_index_vec[next_pao] = index;
        time = get_recover_time(pao_index_vec[next_pao]);
    }
    return time;
}

int PaoOperator::update_next_pao(bool is_delay_pao, float drop_col)
{
    int recover_time = 0xFFFF;

    if (sequential_mode == TIME) {
        int time;
        int _next_pao = next_pao;
        // 遍历整个炮列表
        for (int i = 0; i < pao_index_vec.size(); ++i, skip_pao(1)) {
            // 被锁定的炮不允许发射
            if (lock_pao_set.find(pao_index_vec[next_pao]) != lock_pao_set.end()) {
                continue;
            }

            time = get_recover_time_vec();
            if (time == NO_EXIST_RECOVER_TIME) {
                return NO_EXIST_RECOVER_TIME;
            }
            int roof_offset_time = drop_col < 0 ? 0 : (387 - get_roof_fly_time(pao_grid_vec[next_pao].col, drop_col));
            time -= roof_offset_time;
            if (time <= 0) { // 这里返回 roof_offset_time 目的是直接让 roofPao 使用, 对于普通的炮, roof_offset_time 一直为 0
                return roof_offset_time;
            }
            if (recover_time > time) {
                recover_time = time;
                _next_pao = next_pao;
            }
        }

        next_pao = _next_pao;
    } else { // SPACE
        recover_time = get_recover_time_vec();
        if (recover_time <= 0) {
            return recover_time;
        }
    }

    // 以上的判断条件已经解决炮是否存在以及炮当前时刻是否能用的问题
    // 如果炮当前时刻不能使用但是为 recoverPao 时则不会报错，
    // 并返回恢复时间
    if (is_delay_pao) {
        return recover_time;
    }

    std::string error_str = (sequential_mode == TIME ? "TIME 模式 : 未找到能够发射的炮，"
                                                     : "SPACE 模式 : ");
    error_str += "位于 (#, #) 的第 # 门炮还有 #cs 恢复";
    ShowErrorNotInQueue(error_str,
        pao_grid_vec[next_pao].row,
        pao_grid_vec[next_pao].col,
        next_pao + 1,
        recover_time);
    return NO_EXIST_RECOVER_TIME;
}

// 发炮函数：单发
void PaoOperator::pao(int row, float col)
{
    InsertOperation([=]() {
        if (pao_grid_vec.size() == 0) {
            ShowErrorNotInQueue("pao : 您尚未为此炮列表分配炮");
            return;
        }
        if (update_next_pao() == NO_EXIST_RECOVER_TIME) {
            return;
        }
        base_fire_pao(pao_index_vec[next_pao], row, col);
        update_lastest_pao_msg(__main_object->gameClock(), next_pao);
        skip_pao(1);
    },
        "pao");
}

//发炮函数：多发
void PaoOperator::pao(const std::vector<Position>& lst)
{
    for (const auto& each : lst) {
        pao(each.row, each.col);
    }
}

void PaoOperator::recoverPao(int row, float col)
{
    InsertOperation([=]() {
        if (pao_grid_vec.size() == 0) {
            ShowErrorNotInQueue("recoverPao : 您尚未为此炮列表分配炮");
            return;
        }
        int delay_time = update_next_pao(true);
        if (delay_time == NO_EXIST_RECOVER_TIME) {
            return;
        }
        delay_fire_pao(delay_time, pao_index_vec[next_pao], row, col);
        update_lastest_pao_msg(__main_object->gameClock() + delay_time,
            next_pao);
        skip_pao(1);
    },
        "recoverPao");
}

void PaoOperator::recoverPao(const std::vector<Position>& lst)
{
    for (const auto& each : lst) {
        recoverPao(each.row, each.col);
    }
}

void PaoOperator::roofPao(int row, float col)
{
    InsertOperation([=]() {
        if (__main_object->scene() != 4 && __main_object->scene() != 5) {
            ShowErrorNotInQueue("roofPao : RoofPao函数只适用于 RE 与 ME ");
            return;
        }
        if (pao_grid_vec.size() == 0) {
            ShowErrorNotInQueue("roofPao : 您尚未为此炮列表分配炮");
            return;
        }

        int delay_time = update_next_pao(false, col);
        if (delay_time == NO_EXIST_RECOVER_TIME) {
            return;
        }

        delay_fire_pao(delay_time, pao_index_vec[next_pao], row, col);
        update_lastest_pao_msg(__main_object->gameClock() + delay_time,
            next_pao);
        skip_pao(1);
    },
        "roofPao");
}

void PaoOperator::roofPao(const std::vector<Position>& lst)
{
    for (const auto& each : lst) {
        roofPao(each.row, each.col);
    }
}
} // namespace AvZ