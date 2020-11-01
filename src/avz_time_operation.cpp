/*
 * @coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-02-06 10:22:46
 * @Description: AvZ time operation queue
 *         It's important to understand these!
 */

#include <sstream>
#include <thread>

#include "avz_debug.h"
#include "avz_time_operation.h"
#include "avz_global.h"
#include "avz_tick.h"
#include "avz_cannon.h"
#include "avz_memory.h"

namespace AvZ
{
    extern PvZ *__pvz_base;
    extern MainObject *__main_object;
    extern ItemCollector item_collector;
    extern IceFiller ice_filler;
    extern PlantFixer plant_fixer;
    extern PaoOperator pao_operator;
    extern KeyConnector key_connector;
    extern std::vector<ThreadInfo> __thread_vec;
    extern std::stack<int> __stopped_thread_id_stack;
    extern std::map<int, int> __seed_name_to_index_map;
    extern std::vector<Grid> __select_card_vec;

    std::vector<OperationQueue> __operation_queue_vec;
    TimeWave __time_wave_insert;
    TimeWave __time_wave_run;
    bool is_loaded = false;
    int effective_mode = -1;
    bool is_exited = false;
    bool is_insert_operation = true;
    bool block_var = false;
    std::vector<OperationQueue>::iterator __wavelength_it;

    // 此函数每帧都需调用一次
    void update_refresh_time()
    {
        extern MainObject *__main_object;
        int wave = __main_object->wave();

        auto operation_queue_it = __operation_queue_vec.begin() + wave;

        if (operation_queue_it->refresh_time != -1)
        { // 已经读取过的不再读取
            return;
        }

        if (wave == 0)
        {
            operation_queue_it->refresh_time = __main_object->refreshCountdown() + __main_object->gameClock();
        }
        else
        {
            if (wave + 1 == 10 || wave + 1 == 20)
            {
                // 大波
                if (__main_object->refreshCountdown() <= 4)
                {
                    operation_queue_it->refresh_time = __main_object->hugeWaveCountdown() + __main_object->gameClock();
                }
            }
            else if (__main_object->refreshCountdown() <= 200)
            {
                // 普通波
                operation_queue_it->refresh_time = __main_object->refreshCountdown() + __main_object->gameClock();
            }

            // 在 wave != 0 时，可以由初始刷新倒计时得到当前已刷出波数的刷新时间点
            --operation_queue_it;
            if (operation_queue_it->refresh_time == -1)
            {
                operation_queue_it->refresh_time = __main_object->gameClock() - (__main_object->initialCountdown() - __main_object->refreshCountdown());
            }
        }
    }

    void __Exit()
    {
        extern HWND __pvz_hwnd;
        SetWindowTextA(__pvz_hwnd, "Plants vs. Zombies");
        extern bool is_exited;
        is_exited = true;
    }

    void InsertOperation(const std::function<void()> &operation, const std::string &description)
    {
        extern PvZ *__pvz_base;
        if (__pvz_base->gameUi() == 1)
        {
            return;
        }

        if (!is_insert_operation ||
            (__time_wave_insert.time == __time_wave_run.time &&
             __time_wave_insert.wave == __time_wave_run.wave))
        {
            operation();
            return;
        }

        if (__time_wave_insert.wave < 1 || __time_wave_insert.wave > 20)
        {
            ShowErrorNotInQueue("您填写的 wave 参数为 # , 已超出 [1, 20] 的范围",
                                __time_wave_insert.wave);
            return;
        }

        if (!RangeIn(__time_wave_insert.wave, {1, 10, 20}) &&
            __time_wave_insert.time < -200 &&
            __operation_queue_vec[__time_wave_insert.wave - 2].wave_length == -1)
        {
            ShowErrorNotInQueue("第 # 波设定的时间为 #, 在前一波未设定波长的情况下, time 参数不允许小于 -200",
                                __time_wave_insert.wave,
                                __time_wave_insert.time);
            return;
        }

        auto &operation_queue = __operation_queue_vec[__time_wave_insert.wave - 1].queue; // 取出相应波数的队列
        auto it = operation_queue.find(__time_wave_insert.time);
        Operation operate = {operation, description};
        if (it == operation_queue.end())
        {
            std::pair<int, std::vector<Operation>> to;
            to.first = __time_wave_insert.time;
            to.second.push_back(operate);
            operation_queue.insert(to);
        }
        else
        {
            it->second.push_back(operate);
        }
    }

    void InsertTimeOperation(const TimeWave &time_wave, const std::function<void()> &operation, const std::string &description)
    {
        SetTime(time_wave);
        InsertOperation(operation, description);
    }
    void InsertTimeOperation(int time, int wave, const std::function<void()> &operation, const std::string &description)
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
        extern bool is_insert_operation;
        is_insert_operation = _is_insert_operation;
    }

    bool WaitUntil(const TimeWave &_time_wave)
    {
        extern PvZ *__pvz_base;
        block_var = true;

        if (is_exited)
        {
            return false;
        }

        {
            InsertGuard insert_guard(true);
            SetTime(_time_wave);
            InsertOperation([=]() {
                block_var = false; // 唤醒 Script 线程
                Sleep(10);         // 停滞一帧
            });
        }

        while (block_var)
        {
            // 阻塞 Script 线程
            Sleep(1);
            if (__pvz_base->gameUi() == 1)
            {
                return false;
            }
        }

        return true;
    }

    bool WaitUntil(int time, int wave)
    {
        return WaitUntil({time, wave});
    }

    // 得到当前时间，读取失败返回 -1
    // *** 注意得到的是以参数波刷新时间点为基准的相对时间
    // *** 使用示例：
    // nowTime(1) -------- 得到以第一波刷新时间点为基准的当前时间
    // nowTime(2) -------- 得到以第二波刷新时间点为基准的当前时间
    int NowTime(int wave)
    {
        extern MainObject *__main_object;
        if (__operation_queue_vec[wave - 1].refresh_time == -1)
        {
            ShowErrorNotInQueue("第 # 波刷新时间未知", wave);
            return -1;
        }
        return __main_object->gameClock() - __operation_queue_vec[wave - 1].refresh_time;
    }

    std::vector<int> GetRefreshedWave()
    {
        std::vector<int> waves;
        int wave = 0;
        for (const auto &ele : __operation_queue_vec)
        {
            ++wave;
            if (ele.refresh_time != -1)
            {
                waves.push_back(wave);
            }
        }

        return waves;
    }

    void ShowQueue(const std::vector<int> &lst)
    {
        InsertOperation([=]() {
            std::stringstream ss;
            for (const auto &wave : lst)
            {
                if (wave < 1 || wave > 20)
                {
                    continue;
                }
                ss << "---------------------------"
                   << " wave : " << wave << " ---------------------------";
                if (!__operation_queue_vec[wave - 1].queue.empty())
                {
                    for (const auto &each : __operation_queue_vec[wave - 1].queue)
                    {
                        ss << "\n\t" << each.first;
                        for (const auto &operation : each.second)
                        {
                            ss << " " << operation.description;
                        }
                    }
                }
                else
                {
                    ss << "\n\tnone";
                }

                ss << "\n";
            }

            ShowErrorNotInQueue(ss.str());
        },
                        "showQueue");
    }

    void LoadScript(const std::function<void()> func)
    {
        void InitAddress();
        InitAddress();
        __operation_queue_vec.resize(20);
        __time_wave_run.wave = 0;
        __wavelength_it = __operation_queue_vec.begin();
        SetInsertOperation(false);
        item_collector.start();
        MaidCheats::stop();
        __pvz_base->tickMs() = 10;
        SetInsertOperation(true);
        SetTime(-600, 1);
        is_loaded = true;
        func();

        // 等待游戏进入战斗界面
        while (__pvz_base->gameUi() != 3)
        {
            if (__pvz_base->gameUi() == 1)
            {
                break;
            }
            exit_sleep(1);
        }

        // 等待游戏结束
        while (__pvz_base->gameUi() == 3)
        {
            exit_sleep(1);
        }

        if (effective_mode != MAIN_UI_OR_FIGHT_UI)
        {
            // 如果战斗界面不允许重新注入则等待回主界面
            while (__pvz_base->gameUi() != 1)
            {
                exit_sleep(1);
            }
        }

        // 停止一切线程
        for (const auto &ele : __thread_vec)
        {
            *ele.id_ptr = -1;
        }

        extern HWND __pvz_hwnd;
        SetWindowTextA(__pvz_hwnd, "Plants vs. Zombies");
        __pvz_base->tickMs() = 10;
        __operation_queue_vec.clear(); // 清除一切操作
        __thread_vec.clear();
        key_connector.clear();
        __seed_name_to_index_map.clear();
        while (!__stopped_thread_id_stack.empty())
        {
            __stopped_thread_id_stack.pop();
        }
        is_loaded = !(effective_mode >= 0);
    }

    void OpenMultipleEffective(char close_key, int _effective_mode)
    {
        effective_mode = _effective_mode;
        key_connector.add(close_key, []() {
            effective_mode = -1;
            ShowErrorNotInQueue("已关闭多次生效");
        });
    }

    void SetTime(const TimeWave &_time_wave)
    {
        if (is_insert_operation)
        {
            __time_wave_insert = _time_wave;
        }
        else
        {
            ShowErrorNotInQueue("当前操作队列模式为直接运行不插入，因此 setTime 函数无效");
        }
    }

    // 设定操作时间点
    // *** 使用示例：
    // setTime(-95, 1)------ 将操作时间点设为第一波僵尸刷新前 95cs
    // setTime(-95)--------- 将操作时间点设为僵尸刷新前 95cs, 波数由上一个最近确定的波数决定
    void SetTime(int time, int wave)
    {
        if (is_insert_operation)
        {
            __time_wave_insert.time = time;
            __time_wave_insert.wave = wave;
        }
        else
        {
            ShowErrorNotInQueue("当前操作队列模式为直接运行不插入，因此 setTime 函数无效");
        }
    }

    // 设定操作时间点
    void SetTime(int time)
    {
        if (is_insert_operation)
        {
            __time_wave_insert.time = time;
        }
        else
        {
            ShowErrorNotInQueue("当前操作队列模式为直接运行不插入，因此 setTime 函数无效");
        }
    }

    void SetNowTime()
    {
        auto waves = GetRefreshedWave();
        int wave = waves[waves.size() - 1];
        int time = NowTime(wave);
        SetTime(time, wave);
    }

    // 延迟一定时间
    // *** 注意由于操作队列的优势，此函数支持负值
    // *** 使用示例：
    // delay(298) ------ 延迟 298cs
    // delay(-298) ------ 提前 298cs
    void Delay(int time)
    {
        if (is_insert_operation)
        {
            __time_wave_insert.time += time;
        }
        else
        {
            ShowErrorNotInQueue("当前操作队列模式为直接运行不插入，因此 delay 函数无效");
        }
    }

    void __Run(MainObject *level, std::function<void()> Script)
    {
        extern MainObject *__main_object;
        extern PvZ *__pvz_base;
        if (is_exited)
        {
            return;
        }
        __main_object = level;

        if (!is_loaded)
        {
            std::thread task(LoadScript, Script);
            task.detach();
            while (!is_loaded)
            {
                exit_sleep(10);
            }

            // 假进入战斗界面直接返回
            if (__pvz_base->gameUi() == 3 && __main_object->text()->disappearCountdown())
            {
                return;
            }
        }

        if (__pvz_base->gameUi() == 2 && !__select_card_vec.empty())
        {
            void select_cards();
            select_cards();
        }

        if (__pvz_base->gameUi() != 3 || __pvz_base->mouseWindow()->topWindow())
        {
            __seed_name_to_index_map.clear();
            return;
        }

        // 假进入战斗界面直接返回
        if ((__main_object->selectCardUi_m()->orizontalScreenOffset() != 0 &&
             __main_object->selectCardUi_m()->orizontalScreenOffset() != 7830 &&
             __main_object->selectCardUi_m()->orizontalScreenOffset() != 9780) ||
            __main_object->text()->disappearCountdown() == 1001 ||
            __main_object->text()->disappearCountdown() == 1000)
        {
            return;
        }

        if (!__select_card_vec.empty())
        {
            __select_card_vec.clear();
        }

        if (__main_object->wave() != 20)
        {
            update_refresh_time();
        }

        for (const auto &thread_info : __thread_vec)
        {
            if (*thread_info.id_ptr >= 0)
            {
                thread_info.func();
            }
        }

        // 对 20 个队列进行遍历
        // 最大比较次数 20 * 3 = 60 次
        // 卧槽，感觉好亏，不过游戏应该不会卡顿
        for (int wave = 0; wave < 20; ++wave)
        {
            if (__operation_queue_vec[wave].refresh_time == -1 ||
                __operation_queue_vec[wave].queue.empty() ||
                !__operation_queue_vec[wave].is_time_arrived())
            {
                // 波次没有到达或队列为空或时间没有到达，跳过
                continue;
            }

            auto it = __operation_queue_vec[wave].queue.begin();
            __time_wave_run.wave = wave + 1;
            __time_wave_run.time = it->first;

            for (const auto &ele : it->second)
            {
                ele.operation();
            }
            __operation_queue_vec[wave].queue.erase(it);
        }
    }

    // 将操作插入操作队列
    void ConditionalOperation::run(int time, int wave)
    {
        if (!is_set_condition)
        {
            ShowErrorNotInQueue("run : 您还未为条件操作设定条件");
        }

        auto temp = __time_wave_insert;

        SetTime(time, wave);
        InsertOperation([=]() {
            int label = condition();
            for (const auto &ele : label_operation_vec)
            {
                if (ele.label == label)
                {
                    ele.operation();
                }
            }
        });

        SetTime(temp);
    }

} // namespace AvZ
