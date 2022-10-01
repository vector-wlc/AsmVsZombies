/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: API  time operation queue
 *         It's important to understand these!
 */

#include <mutex>
#include <sstream>

#include "avz_cannon.h"
#include "avz_debug.h"
#include "avz_execption.h"
#include "avz_global.h"
#include "avz_memory.h"
#include "avz_tick.h"
#include "avz_time_operation.h"
#include "pvzstruct.h"

namespace AvZ {
extern PvZ* __pvz_base;
extern MainObject* __main_object;
extern ItemCollector item_collector;
extern IceFiller ice_filler;
extern PlantFixer plant_fixer;
extern PaoOperator pao_operator;
extern KeyConnector key_connector;
extern std::map<int, int> __seed_name_to_index_map;
extern std::vector<Grid> __select_card_vec;
extern std::vector<OperationQueue> __operation_queue_vec;
extern TimeWave __time_wave_insert;
extern TimeWave __time_wave_start;
extern bool __is_loaded;
extern int __effective_mode;
extern bool __is_exited;
extern bool __is_insert_operation;
extern bool __block_var;
extern std::vector<OperationQueue>::iterator __wavelength_it;
extern std::vector<OperationQueue>::iterator __run_wave_iter;
extern VoidFunc<void> __script_exit_deal;
extern bool __is_advanced_pause;
extern VoidFunc<bool> __skip_tick_condition;
extern int __error_mode;

// 此函数每帧都需调用一次
void __UpdateRefreshTime()
{
    extern MainObject* __main_object;
    int wave = __main_object->wave();

    auto operation_queue_it = __operation_queue_vec.begin() + wave;

    if (Unlikely(operation_queue_it->refresh_time != __DEFAULT_START_TIME)) { // 已经读取过的不再读取
        return;
    }

    int refresh_time = __main_object->refreshCountdown();
    int game_clock = __main_object->gameClock();

    if (Likely(wave != 0)) {
        if (Likely((wave + 1) % 10 != 0)) { // 普通波
            if (Unlikely(refresh_time <= 200)) {
                operation_queue_it->refresh_time = refresh_time + game_clock;
            }
        } else { // 大波
            if (Unlikely(refresh_time <= 5)) {
                operation_queue_it->refresh_time = __main_object->hugeWaveCountdown() + game_clock;
            }
        }

        // 在 wave != 0 时，可以由初始刷新倒计时得到当前已刷出波数的刷新时间点
        --operation_queue_it;
        if (Unlikely(operation_queue_it->refresh_time == __DEFAULT_START_TIME)) {
            operation_queue_it->refresh_time = game_clock - (__main_object->initialCountdown() - refresh_time);
        }
    } else { // 第一波
        operation_queue_it->refresh_time = refresh_time + game_clock;
    }
}

// 切换运行的波
// 将上一波未执行的操作移到下一波
// 将运行波数迭代器 + 1
void __ChangeRunWave()
{
    auto next_wave_iter = __run_wave_iter + 1;
    int offset_time = __run_wave_iter->refresh_time - next_wave_iter->refresh_time;
    for (auto&& ele : __run_wave_iter->queue) {
        next_wave_iter->queue.insert({ele.first + offset_time, ele.second});
    }

    __run_wave_iter = next_wave_iter;
}

int GetRunningWave()
{
    return __run_wave_iter - __operation_queue_vec.begin() + 1;
}

void __Exit()
{
    extern HWND __pvz_hwnd;
    extern bool __is_exited;
    __is_exited = true;
}

TimeWave __CalculateInsertTimeWave()
{
    if (__time_wave_start.wave >= __time_wave_insert.wave && //
        __time_wave_start.time > __time_wave_insert.time) {
        return {0, NOT_INSERT};
    }

    if (!__is_insert_operation ||                           //
        (__time_wave_insert.time != __DEFAULT_START_TIME && //
            __time_wave_insert.time == NowTime(__time_wave_insert.wave))) {
        return {0, RUN};
    }

    if (__time_wave_insert.wave < 1 || __time_wave_insert.wave > __main_object->totalWave()) {
        ShowErrorNotInQueue("您填写的 wave 参数为 # , 已超出 [1, #] 的范围",
            __time_wave_insert.wave, __main_object->totalWave());

        return {0, NOT_INSERT};
    }

    if ((__time_wave_insert.wave != 1 && __time_wave_insert.wave % 10 != 0) && //
        __time_wave_insert.time < -200 &&                                      //
        __operation_queue_vec[__time_wave_insert.wave - 2].wave_length == -1) {
        ShowErrorNotInQueue(
            "第 # 波设定的时间为 #, 在前一波未设定波长的情况下, time "
            "参数不允许小于 -200",
            __time_wave_insert.wave, __time_wave_insert.time);
        return {0, NOT_INSERT};
    }

    int current_wave = __run_wave_iter - __operation_queue_vec.begin() + 1;
    int insert_time = __time_wave_insert.time;
    int insert_wave = __time_wave_insert.wave;

    if ((insert_wave != 1 && insert_wave % 10 != 0) && // 使用了 SetWavelength
        insert_time < -200 && insert_wave > current_wave) {
        while (insert_wave != 1 && insert_time < -200) {
            if (__operation_queue_vec[insert_wave - 2].wave_length == -1) {
                ShowErrorNotInQueue(
                    "第 # 波设定的时间为 #, 虽然已设定波长，但是已超出波长计算范围",
                    __time_wave_insert.wave, __time_wave_insert.time);
                return {0, NOT_INSERT};
            }
            insert_time += __operation_queue_vec[insert_wave - 2].wave_length;
            --insert_wave;
        }

        return {insert_time, insert_wave};
    }

    if (insert_wave < current_wave) { // 如果插入的波数小于当前运行的波数，将其时间点折算成当前运行波数
        auto&& queue = __operation_queue_vec[insert_wave - 1].queue;
        int offset_time = __operation_queue_vec[insert_wave - 1].refresh_time - __run_wave_iter->refresh_time;
        return {offset_time + insert_time, current_wave};
    }

    return __time_wave_insert;
}

// 设置 insertOperation 属性函数
// *** 使用示例：
// SetInsertOperation(false) ---- insertOperation 将不会把操作插入操作队列中
// SetInsertOperation(true) ---- insertOperation 将会把操作插入操作队列中
void SetInsertOperation(bool _is_insert_operation)
{
    extern bool __is_insert_operation;
    __is_insert_operation = _is_insert_operation;
}

bool WaitUntil(const TimeWave& _time_wave)
{
    extern PvZ* __pvz_base;
    __block_var = true;
    void RunScriptEveryTick();

    {
        InsertGuard insert_guard(true);
        SetTime(_time_wave);
        InsertOperation([=]() {
            __block_var = false; // 唤醒 Script
            SetTime(_time_wave.time + 1, _time_wave.wave);
        },
            "WaitUntil");
    }

    RunScriptEveryTick();

    if (Unlikely(__pvz_base->gameUi() == 1)) {
        throw Exception(STR_GAME_RET_MAIN_UI);
    }

    while (__block_var) {
        Asm::gameSleepLoop();
        if (Unlikely(!__pvz_base->mainObject())) {
            throw Exception(STR_GAME_RET_MAIN_UI);
        }
    }

    return true;
}

bool WaitUntil(int time, int wave) { return WaitUntil({time, wave}); }

// 得到当前时间，读取失败返回 __DEFAULT_START_TIME
// *** 注意得到的是以参数波刷新时间点为基准的相对时间
// *** 使用示例：
// NowTime(1) -------- 得到以第一波刷新时间点为基准的当前时间
// NowTime(2) -------- 得到以第二波刷新时间点为基准的当前时间
int NowTime(int wave)
{
    extern MainObject* __main_object;
    if (__operation_queue_vec[wave - 1].refresh_time == __DEFAULT_START_TIME) {
        return __DEFAULT_START_TIME;
    }
    return __main_object->gameClock() - __operation_queue_vec[wave - 1].refresh_time;
}

std::vector<int> GetRefreshedWave()
{
    std::vector<int> waves;
    int wave = 0;
    for (const auto& ele : __operation_queue_vec) {
        ++wave;
        if (ele.refresh_time != __DEFAULT_START_TIME) {
            waves.push_back(wave);
        }
    }

    return waves;
}

void ShowQueue(const std::vector<int>& lst)
{
    InsertOperation([=]() {
        std::stringstream ss;
        for (const auto& wave : lst) {
            if (wave < 1 || wave > __main_object->totalWave()) {
                continue;
            }
            ss << "---------------------------"
               << " wave : " << wave << " ---------------------------";

            bool is_have_operation = false;

            for (const auto& operation_queue : __operation_queue_vec) {
                int time = __DEFAULT_START_TIME - 1;
                for (const auto& operation : operation_queue.queue) {
                    const auto& time_wave = operation.second.getInsertTimeWave();
                    if (time_wave.wave == wave) {
                        if (time != time_wave.time) {
                            is_have_operation = true;
                            time = time_wave.time;
                            ss << "\n\t" << time;
                        }
                        ss << " " << operation.second.getDescription();
                    }
                }
            }
            if (!is_have_operation) {
                ss << "\n\tnone";
            }

            ss << "\n";
        }

        ShowErrorNotInQueue(ss.str());
    },
        "showQueue");
}

void OpenMultipleEffective(char close_key, int _effective_mode)
{
    __effective_mode = _effective_mode;
    key_connector.add(close_key, []() {
        __effective_mode = -1;
        __is_exited = true;
    });
}

void SetTime(const TimeWave& _time_wave)
{
    if (__is_insert_operation) {
        __time_wave_insert = _time_wave;
    } else {
        ShowErrorNotInQueue(
            "当前操作队列模式为直接运行不插入，因此 setTime 函数无效");
    }
}

// 设定操作时间点
// *** 使用示例：
// setTime(-95, 1)------ 将操作时间点设为第一波僵尸刷新前 95cs
// setTime(-95)--------- 将操作时间点设为僵尸刷新前 95cs,
// 波数由上一个最近确定的波数决定
void SetTime(int time, int wave)
{
    if (__is_insert_operation) {
        __time_wave_insert.time = time;
        __time_wave_insert.wave = wave;
    } else {
        ShowErrorNotInQueue(
            "当前操作队列模式为直接运行不插入，因此 setTime 函数无效");
    }
}

// 设定操作时间点
void SetTime(int time)
{
    if (__is_insert_operation) {
        __time_wave_insert.time = time;
    } else {
        ShowErrorNotInQueue(
            "当前操作队列模式为直接运行不插入，因此 setTime 函数无效");
    }
}

void SetNowTime()
{
    auto wave = GetRunningWave();
    int time = NowTime(wave);
    SetTime(time, wave);
}

// 设定延迟时间
void SetDelayTime(int time)
{
    SetNowTime();
    Delay(time);
}

// 延迟一定时间
// *** 注意由于操作队列的优势，此函数支持负值
// *** 使用示例：
// delay(298) ------ 延迟 298cs
// delay(-298) ------ 提前 298cs
void Delay(int time)
{
    if (__is_insert_operation) {
        __time_wave_insert.time += time;
    } else {
        ShowErrorNotInQueue(
            "当前操作队列模式为直接运行不插入，因此 delay 函数无效");
    }
}

bool OperationQueue::is_time_arrived()
{
    extern MainObject* __main_object;
    if (queue.begin()->first < __main_object->gameClock() - refresh_time && queue.begin()->first != __DEFAULT_START_TIME) {
        auto insert_wave_time = queue.begin()->second.getInsertTimeWave();
        ShowErrorNotInQueue(
            "您设定时间为 #cs, 但当前时间已到 #cs, "
            "按下确定将以当前时间执行此次操作",
            insert_wave_time.time,
            __main_object->gameClock() - __operation_queue_vec[insert_wave_time.wave - 1].refresh_time);
    }
    return queue.begin()->first <= __main_object->gameClock() - refresh_time;
}

void SetScriptStartTime(int time, int wave)
{
    __time_wave_start.time = time;
    __time_wave_start.wave = wave;
}

void SetAdvancedPause(bool is_advanced_pause)
{
    __is_advanced_pause = is_advanced_pause;
}

void SetAdvancedPauseKey(char key)
{
    key_connector.add(key, [=]() {
        if (__skip_tick_condition()) {
            ShowErrorNotInQueue("开启跳帧时不能启用高级暂停");
            return;
        }
        __is_advanced_pause = !__is_advanced_pause;
    });
}

void SkipTick(int time, int wave)
{
    SkipTick([=]() {
        int now_time = NowTime(wave);
        if (now_time == __DEFAULT_START_TIME || now_time < time) { // 时间未到达
            return true;
        }
        if (now_time > time) { // 时间已到达
            ShowErrorNotInQueue("无法回跳时间点");
        }
        return false;
    });
}

} // namespace AvZ
