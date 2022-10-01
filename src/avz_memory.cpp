/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: API memory
 */

#include "avz_execption.h"
#include "avz_global.h"
#include "avz_time_operation.h"
#include "pvzfunc.h"

namespace AvZ {
extern MainObject* __main_object;
extern PvZ* __pvz_base;

// *** Not In Queue
// 游戏主要信息
MainObject* GetMainObject()
{
    extern MainObject* __main_object;
    return __main_object;
}

// *** Not In Queue
// 游戏基址
PvZ* GetPvzBase()
{
    extern PvZ* __pvz_base;
    return __pvz_base;
}

// *** Not In Queue
// 返回鼠标所在行
int MouseRow()
{
    extern MainObject* __main_object;
    static int last_value = -1;
    auto memory_value = __main_object->mouseExtraAttribution()->row();
    if (memory_value >= 0) {
        last_value = memory_value + 1;
    }
    return last_value;
}

// *** Not In Queue
// 返回鼠标所在列
float MouseCol()
{
    extern MainObject* __main_object;
    static float last_value = -1;
    auto memory_value = __main_object->mouseAttribution()->abscissa();
    if (memory_value >= 0) {
        last_value = float(memory_value + 25) / 80;
    }
    return last_value;
}

int GetSeedIndex(int type, bool imitator)
{
    auto seed = __main_object->seedArray();
    int cnt = seed->count();
    for (int index = 0; index < cnt; ++index, ++seed) {
        if (imitator) {
            if (seed->type() == 48 && seed->imitatorType() == type) {
                return index;
            }
        } else if (seed->type() == type) {
            return index;
        }
    }

    return -1;
}

int GetPlantIndex(int row, int col, int type)
{
    auto plant = __main_object->plantArray();
    int plants_count_max = __main_object->plantCountMax();
    int plant_type;
    for (int i = 0; i < plants_count_max; ++i, ++plant) {
        if ((!plant->isDisappeared()) && (!plant->isCrushed()) && (plant->row() + 1 == row) && (plant->col() + 1 == col)) {
            plant_type = plant->type();
            if (type == -1) {
                //如果植物存在	且不为南瓜花盆荷叶
                if ((plant_type != 16) && (plant_type != 30) && (plant_type != 33))
                    return i; //返回植物的对象序列
            } else {
                if (plant_type == type) {
                    return i;
                } else if (type != 16 && type != 30 && type != 33 && plant_type != 16 && plant_type != 30 && plant_type != 33) {
                    return -2;
                }
            }
        }
    }
    return -1; //没有符合要求的植物返回-1
}

void GetPlantIndices(const std::vector<Grid>& lst_in_, int type,
    std::vector<int>& indexs_out_)
{
    auto plant = __main_object->plantArray();
    indexs_out_.assign(lst_in_.size(), -1);
    Grid grid;

    for (int index = 0; index < __main_object->plantCountMax();
         ++index, ++plant) {
        if (plant->isCrushed() || plant->isDisappeared()) {
            continue;
        }
        grid.row = plant->row() + 1;
        grid.col = plant->col() + 1;

        auto it_vec = FindSameEle<Grid>(lst_in_, grid);
        if (it_vec.empty()) {
            continue;
        }
        if (plant->type() == type) {
            for (const auto& ele : it_vec) {
                indexs_out_[ele - lst_in_.begin()] = index;
            }
        } else if (type != 16 && type != 30 && type != 33 && plant->type() != 16 && plant->type() != 30 && plant->type() != 33) {
            for (const auto& ele : it_vec) {
                indexs_out_[ele - lst_in_.begin()] = -2;
            }
        }
    }
}

std::vector<int> GetPlantIndices(const std::vector<Grid>& lst, int type)
{
    std::vector<int> indexs;
    GetPlantIndices(lst, type, indexs);
    return indexs;
}

bool IsZombieExist(int type, int row)
{
    auto zombie = __main_object->zombieArray();
    int zombies_count_max = __main_object->zombieTotal();
    for (int i = 0; i < zombies_count_max; ++i, ++zombie) {
        if (zombie->isExist() && !zombie->isDead()) {
            if (type < 0 && row < 0) {
                return true;
            } else if (type >= 0 && row >= 0) {
                if (zombie->row() == row - 1 && zombie->type() == type)
                    return true;
            } else if (type < 0 && row >= 0) {
                if (zombie->row() == row - 1)
                    return true;
            } else // if (type >= 0 && row < 0)
            {
                if (zombie->type() == type)
                    return true;
            }
        }
    }

    return false;
}

void SetPlantActiveTime(PlantType plant_type, int delay_time)
{
    extern TimeWave __time_wave_insert;
    auto temp = __time_wave_insert;
    Delay(delay_time - 95);
    InsertOperation([=]() {
        // 这里不做植物类型检测
        auto plant = __main_object->plantArray();
        for (int index = 0; index < __main_object->plantCountMax();
             ++index, ++plant) {
            if (!plant->isDisappeared() && !plant->isCrushed() && plant->type() == plant_type && plant->state() == 2) {
                if (std::abs(plant->explodeCountdown() - 96) < 3) {
                    plant->explodeCountdown() = 96;
                } else {
                    ShowErrorNotInQueue("SetPlantActiveTime 不允许修改的生效时间超过 3cs");
                }
                return;
            }
        }
    },
        "SetPlantActiveTime");
    SetTime(temp);
}

void UpdateZombiesPreview()
{
    // 去掉当前画面上的僵尸
    Asm::killZombiesPreview();
    // 重新生成僵尸
    __main_object->selectCardUi_m()->isCreatZombie() = false;
}

void SetZombies(const std::vector<int>& zombie_type)
{
    std::vector<int> zombie_type_vec;

    for (const auto& type : zombie_type) {
        // 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
        if (!RangeIn(type, {QZ_1, BW_9, XQ_13, XR_19, BJ_20, XG_24})) {
            zombie_type_vec.push_back(type);
        }
    }
    auto zombie_list = __main_object->zombieList();
    for (int index = 0; index < 1000; ++index, ++zombie_list) {
        (*zombie_list) = zombie_type_vec[index % zombie_type_vec.size()];
    }

    // 生成旗帜
    for (auto index : {450, 950}) {
        (*(__main_object->zombieList() + index)) = QZ_1;
    }

    // 生成蹦极
    for (auto index : {451, 452, 453, 454, 951, 952, 953, 954}) {
        (*(__main_object->zombieList() + index)) = BJ_20;
    }

    if (__pvz_base->gameUi() == 2) {
        UpdateZombiesPreview();
    }
}

void SetWaveZombies(int wave, const std::vector<int>& zombie_type)
{
    std::vector<int> zombie_type_vec;

    for (const auto& type : zombie_type) {
        // 做一些处理，出怪生成不应大量含有 旗帜 舞伴 雪橇小队 雪人 蹦极 小鬼
        if (!RangeIn(type, {QZ_1, BW_9, XQ_13, XR_19, BJ_20, XG_24})) {
            zombie_type_vec.push_back(type);
        }
    }
    auto zombie_list = __main_object->zombieList() + (wave - 1) * 50;
    for (int index = 0; index < 50; ++index, ++zombie_list) {
        (*zombie_list) = zombie_type_vec[index % zombie_type_vec.size()];
    }

    // 生成旗帜
    for (auto index : {450, 950}) {
        (*(__main_object->zombieList() + index)) = QZ_1;
    }
}

uint8_t* GetZombieTypeList()
{
    return __main_object->zombieTypeList();
}

void SetWavelength(const std::vector<WaveTime>& lst)
{
    extern std::vector<OperationQueue> __operation_queue_vec;
    extern TimeWave __time_wave_insert;
    extern std::vector<OperationQueue>::iterator __wavelength_it;
    // 需要恢复的状态
    auto temp = __time_wave_insert;

    for (const auto& ele : lst) {
        if (ele.wave < 1 || RangeIn(ele.wave, {9, 19, 20}) || ele.wave > 20) {
            ShowErrorNotInQueue(
                "setWavelength : 您当前设定的 wave 参数为 #, 超出有效范围",
                ele.wave);
            continue;
        }

        if (ele.time < 601 || ele.time > 2510) {
            ShowErrorNotInQueue(
                "setWavelength : 您当前设定第 # 波 的 time 参数为 #, "
                "超出有效范围",
                ele.wave, ele.time);
            continue;
        }

        __operation_queue_vec[ele.wave - 1].wave_length = ele.time;

        InsertGuard insert_guard(true);
        SetTime(1, ele.wave);
        InsertOperation([=]() {
            __main_object->zombieRefreshHp() = 0;
            int now_time = __main_object->gameClock() - __operation_queue_vec[ele.wave - 1].refresh_time;
            int countdown = ele.time - now_time;
            if (countdown < 0) {
                ShowErrorNotInQueue(
                    "您在第 # 波设定的波长为 #，但是当前时刻点为 "
                    "#，已超出可设定的时间范围，波长设定失败！",
                    ele.wave, ele.time, now_time);
                return;
            }
            __main_object->refreshCountdown() = countdown;
            __main_object->initialCountdown() = ele.time;

            if (__wavelength_it - __operation_queue_vec.begin() < ele.wave - 1) {
                __wavelength_it = __operation_queue_vec.begin() + ele.wave - 1;
            }

            // 设定刷新时间点
            for (; __wavelength_it != __operation_queue_vec.end() - 1; ++__wavelength_it) {
                if (__wavelength_it->refresh_time == __DEFAULT_START_TIME || __wavelength_it->wave_length == -1) {
                    break;
                }
                (__wavelength_it + 1)->refresh_time = __wavelength_it->refresh_time + __wavelength_it->wave_length;
            }
        },
            "writeWavelength");
    }

    SetTime(temp);
}

void SetGameSpeed(float x)
{
    if (x < 0.05 || x > 10) {
        ShowErrorNotInQueue(
            "setGameSpeed : 倍速设置失败，倍速设置的合法范围为 [0.05, 10]");
        return;
    }
    int ms = int(10 / x + 0.5);

    __pvz_base->tickMs() = ms;
}
} // namespace AvZ