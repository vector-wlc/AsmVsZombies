/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: API  time operation queue
 *         It's important to understand these!
 */

#include <mutex>
#include <sstream>
#include <thread>

#include "avz_cannon.h"
#include "avz_debug.h"
#include "avz_execption.h"
#include "avz_global.h"
#include "avz_memory.h"
#include "avz_tick.h"
#include "avz_time_operation.h"
#include "pvzstruct.h"

namespace AvZ {
const int __DEFAULT_START_TIME = -0xffff;
extern PvZ* __pvz_base;
extern MainObject* __main_object;
extern ItemCollector item_collector;
extern IceFiller ice_filler;
extern PlantFixer plant_fixer;
extern PaoOperator pao_operator;
extern KeyConnector key_connector;
extern std::vector<ThreadInfo> __thread_vec;
extern std::stack<int> __stopped_thread_id_stack;
extern std::map<int, int> __seed_name_to_index_map;
extern std::vector<Grid> __select_card_vec;
extern std::vector<OperationQueue> __operation_queue_vec;
extern std::mutex __operation_mutex;
extern TimeWave __time_wave_insert;
extern TimeWave __time_wave_run;
extern TimeWave __time_wave_start;
extern bool __is_loaded;
extern int __effective_mode;
extern bool __is_exited;
extern bool __is_insert_operation;
extern bool __block_var;
extern std::vector<OperationQueue>::iterator __wavelength_it;
extern std::function<void()> __script_exit_deal;

// 此函数每帧都需调用一次
void UpdateRefreshTime()
{
    extern MainObject* __main_object;
    int wave = __main_object->wave();

    auto operation_queue_it = __operation_queue_vec.begin() + wave;

    if (operation_queue_it->refresh_time != -1) { // 已经读取过的不再读取
        return;
    }

    if (wave == 0) {
        operation_queue_it->refresh_time = __main_object->refreshCountdown() + __main_object->gameClock();
    } else {
        if ((wave + 1) % 10 == 0) {
            // 大波
            if (__main_object->refreshCountdown() <= 4) {
                operation_queue_it->refresh_time = __main_object->hugeWaveCountdown() + __main_object->gameClock();
            }
        } else if (__main_object->refreshCountdown() <= 200) {
            // 普通波
            operation_queue_it->refresh_time = __main_object->refreshCountdown() + __main_object->gameClock();
        }

        // 在 wave != 0 时，可以由初始刷新倒计时得到当前已刷出波数的刷新时间点
        --operation_queue_it;
        if (operation_queue_it->refresh_time == -1) {
            operation_queue_it->refresh_time = __main_object->gameClock() - (__main_object->initialCountdown() - __main_object->refreshCountdown());
        }
    }
}

void __Exit()
{
    extern HWND __pvz_hwnd;
    extern bool __is_exited;
    __is_exited = true;
}

void InsertOperation(const std::function<void()>& operation,
    const std::string& description)
{
    extern PvZ* __pvz_base;

    if (__pvz_base->gameUi() == 1 || (__time_wave_start.wave >= __time_wave_insert.wave && __time_wave_start.time > __time_wave_insert.time)) {
        return;
    }

    if (!__is_insert_operation || (__time_wave_insert.time != __DEFAULT_START_TIME && __time_wave_insert.time == NowTime(__time_wave_insert.wave))) {
        // 暂存时间插入点的状态
        auto temp = __time_wave_insert;
        operation();
        __time_wave_insert = temp;
        return;
    }

    if (__time_wave_insert.wave < 1 || __time_wave_insert.wave > __main_object->totalWave()) {
        ShowErrorNotInQueue("您填写的 wave 参数为 # , 已超出 [1, #] 的范围",
            __time_wave_insert.wave,
            __main_object->totalWave());
        return;
    }
    if ((__time_wave_insert.wave != 1 && __time_wave_insert.wave % 10 != 0) && __time_wave_insert.time < -200 && __operation_queue_vec[__time_wave_insert.wave - 2].wave_length == -1) {
        ShowErrorNotInQueue(
            "第 # 波设定的时间为 #, 在前一波未设定波长的情况下, time "
            "参数不允许小于 -200",
            __time_wave_insert.wave, __time_wave_insert.time);
        return;
    }

    __operation_mutex.lock();
    auto& operation_queue = __operation_queue_vec[__time_wave_insert.wave - 1].queue; // 取出相应波数的队列
    auto it = operation_queue.find(__time_wave_insert.time);
    Operation operate = {operation, description};
    if (it == operation_queue.end()) {
        std::pair<int, std::vector<Operation>> to;
        to.first = __time_wave_insert.time;
        to.second.push_back(operate);
        operation_queue.insert(to);
    } else {
        it->second.push_back(operate);
    }
    __operation_mutex.unlock();
}

void InsertTimeOperation(const TimeWave& time_wave,
    const std::function<void()>& operation,
    const std::string& description)
{
    SetTime(time_wave);
    InsertOperation(operation, description);
}
void InsertTimeOperation(int time, int wave,
    const std::function<void()>& operation,
    const std::string& description)
{
    SetTime(time, wave);
    InsertOperation(operation, description);
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

    // 这里的 return 是为了兼容之前的版本的代码不报错
    // 实际上已经没有任何作用了

    if (__is_exited) {
        throw Exception("script have exited\n");
    }

    {
        InsertGuard insert_guard(true);
        SetTime(_time_wave);
        InsertOperation([=]() {
            __block_var = false;   // 唤醒 Script 线程
            while (!__block_var) { // 阻塞 pvz 进程，等待操作完成
                ExitSleep(1);
            }
        });
    }

    while (__block_var) {
        // 阻塞 Script 线程
        Sleep(1);
        if (__pvz_base->gameUi() == 1) {
            throw Exception("game return main ui\n");
        }
    }

    return true;
}

bool WaitUntil(int time, int wave) { return WaitUntil({time, wave}); }

// 得到当前时间，读取失败返回 __DEFAULT_START_TIME
// *** 注意得到的是以参数波刷新时间点为基准的相对时间
// *** 使用示例：
// nowTime(1) -------- 得到以第一波刷新时间点为基准的当前时间
// nowTime(2) -------- 得到以第二波刷新时间点为基准的当前时间
int NowTime(int wave)
{
    extern MainObject* __main_object;
    if (__operation_queue_vec[wave - 1].refresh_time == -1) {
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
        if (ele.refresh_time != -1) {
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
            if (!__operation_queue_vec[wave - 1].queue.empty()) {
                for (const auto& each :
                    __operation_queue_vec[wave - 1].queue) {
                    ss << "\n\t" << each.first;
                    for (const auto& operation : each.second) {
                        ss << " " << operation.description;
                    }
                }
            } else {
                ss << "\n\tnone";
            }

            ss << "\n";
        }

        ShowErrorNotInQueue(ss.str());
    },
        "showQueue");
}

void ScriptExitDeal(const std::function<void()>& func)
{
    __script_exit_deal = func;
}

void LoadScript(const std::function<void()> func)
{
    __operation_queue_vec.resize(__main_object->totalWave());
    __time_wave_run.wave = 0;
    __wavelength_it = __operation_queue_vec.begin();
    SetInsertOperation(false);
    item_collector.start();
    MaidCheats::stop();
    __pvz_base->tickMs() = 10;
    SetInsertOperation(true);

    // 将默认时间设置为刚一进战斗界面的时间
    if (__pvz_base->gameUi() == 3) {
        UpdateRefreshTime();
        auto waves = GetRefreshedWave();
        __time_wave_start.wave = *waves.begin();
        __time_wave_start.time = NowTime(__time_wave_start.wave);
    } else {
        __time_wave_start.wave = 1;
        __time_wave_start.time = __DEFAULT_START_TIME;
    }
    SetTime(__time_wave_start);

    __is_loaded = true;

    try {
        func();

        __block_var = true;

        // 等待游戏进入战斗界面
        while (__pvz_base->gameUi() != 3) {
            if (__pvz_base->gameUi() == 1) {
                break;
            }
            ExitSleep(1);
        }

        // 等待游戏结束
        while (__pvz_base->gameUi() == 3) {
            ExitSleep(1);
        }

        if (__effective_mode != MAIN_UI_OR_FIGHT_UI) {
            // 如果战斗界面不允许重新注入则等待回主界面
            while (__pvz_base->gameUi() != 1) {
                ExitSleep(1);
            }
        }
    } catch (Exception& exce) {
        printf(exce.what());
    } catch (...) {
        ShowErrorNotInQueue("脚本触发了一个未知的异常\n");
    }

    // 停止一切线程
    for (const auto& ele : __thread_vec) {
        *ele.id_ptr = -1;
    }

    // 释放资源
    Sleep(100);
    __script_exit_deal();
    AvZ::SetErrorMode(AvZ::POP_WINDOW);
    SetInsertOperation(false);
    MaidCheats::stop();
    __pvz_base->tickMs() = 10;
    SetInsertOperation(true);

    __is_loaded = !(__effective_mode >= 0);
}

void OpenMultipleEffective(char close_key, int _effective_mode)
{
    __effective_mode = _effective_mode;
    key_connector.add(close_key, []() {
        __effective_mode = -1;
        ShowErrorNotInQueue("已关闭多次生效");
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
    auto waves = GetRefreshedWave();
    if (waves.size() == 0) {
        ShowErrorNotInQueue(
            "SetNowTime : "
            "未检测到当前时间，请在游戏进入战斗界面之后再调用此函数");
        return;
    }
    int wave = waves[waves.size() - 1];
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
        ShowErrorNotInQueue(
            "您设定时间为 #cs, 但当前时间已到 #cs, "
            "按下确定将以当前时间执行此次操作",
            queue.begin()->first, __main_object->gameClock() - refresh_time);
    }
    return queue.begin()->first <= __main_object->gameClock() - refresh_time;
}

void __Run(MainObject* level, std::function<void()> Script)
{
    try {
        extern MainObject* __main_object;
        extern PvZ* __pvz_base;
        if (__is_exited) {
            return;
        }
        __main_object = level;
        // 假进入战斗界面直接返回
        if (__main_object->loadDataState() == 1) {
            return;
        }

        if (!__is_loaded) {
            void InitAddress();
            InitAddress();

            __operation_queue_vec.clear(); // 清除一切操作
            __thread_vec.clear();
            key_connector.clear();
            __seed_name_to_index_map.clear();
            while (!__stopped_thread_id_stack.empty()) {
                __stopped_thread_id_stack.pop();
            }

            std::thread task(LoadScript, Script);
            task.detach();
            while (!__is_loaded) {
                ExitSleep(10);
            }
        }

        if (__pvz_base->gameUi() == 2 && !__select_card_vec.empty()) {
            void ChooseCards();
            ChooseCards();
        }

        if (__pvz_base->gameUi() != 3 || __pvz_base->mouseWindow()->topWindow()) {
            __seed_name_to_index_map.clear();
            return;
        }

        // 以下代码到战斗界面才能执行
        if (__pvz_base->gameUi() == 2) {
            return;
        }

        if (!__select_card_vec.empty()) {
            __select_card_vec.clear();
        }

        if (__main_object->wave() != __main_object->totalWave()) {
            UpdateRefreshTime();
        }

        // 对 __main_object->totalWave 个队列进行遍历
        // 最大比较次数 __main_object->totalWave * 3 = 60 次
        // 卧槽，感觉好亏，不过游戏应该不会卡顿

        for (int wave = 0; wave < __main_object->totalWave(); ++wave) {
            while (__operation_queue_vec[wave].refresh_time != -1 && // 波次刷新时间已知
                !__operation_queue_vec[wave].queue.empty() &&        // 有操作
                __operation_queue_vec[wave].is_time_arrived())       // 操作时间已到达
            {
                auto it = __operation_queue_vec[wave].queue.begin();
                __time_wave_run.wave = wave + 1;
                __time_wave_run.time = it->first;

                for (const auto& ele : it->second) {
                    ele.operation();
                }
                __operation_mutex.lock();
                __operation_queue_vec[wave].queue.erase(it);
                __operation_mutex.unlock();
            }
        }

        for (const auto& thread_info : __thread_vec) {
            if (*thread_info.id_ptr >= 0) {
                thread_info.func();
            }
        }
    } catch (Exception& exce) {
        printf(exce.what());
    } catch (...) {
        ShowErrorNotInQueue("脚本触发了一个未知的异常\n");
    }
}

// 将操作插入操作队列
void ConditionalOperation::run(int time, int wave)
{
    if (!is_set_condition) {
        ShowErrorNotInQueue("run : 您还未为条件操作设定条件");
    }

    auto temp = __time_wave_insert;

    SetTime(time, wave);
    InsertOperation([=]() {
        int label = condition();
        for (const auto& ele : label_operation_vec) {
            if (ele.label == label) {
                ele.operation();
            }
        }
    });

    SetTime(temp);
}

void SetScriptStartTime(int time, int wave)
{
    __time_wave_start.time = time;
    __time_wave_start.wave = wave;
}

} // namespace AvZ
