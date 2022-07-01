/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:36:43
 * @Description: 操作队列头文件
 */

#include <map>
#include <vector>

#include "avz_debug.h"
#include "avz_global.h"
#include "pvzfunc.h"
#include "pvzstruct.h"

#ifndef __AVZ_TIME_OPERATION_H__
#define __AVZ_TIME_OPERATION_H__

namespace AvZ {
// 该部分将使用者写的操作按照时间先后顺序录入消息队列，旧接口得以实现
// 使用 20 条操作队列，每一条操作队列储存一波的操作
// 需要每帧进行僵尸刷新时间的读取，普通波 <= 200，wave1 : <=600   wave10||20:750
// 当队列记录的刷新时间点为 -1 时，continue

// 实现方式
// 1. 检测刷新波数，如果下一波刷新点已知，将标志波转移到下一波，并且将上一波所有的操作搬到下一波
// 2. 插入操作时，检测刷新点之间的关系，将操作插到相应的执行波数
// 3. 当使用 SetWavelength 函数时，将小于 -200 时间点的操作搬到上波.

struct WaveTime {
    int wave;
    int time;
};

class Operation {
private:
    TimeWave insert_time_wave;
    VoidFunc<void> operation;
    std::string description;
    bool is_executed = false;

public:
    Operation() = default;

    Operation(const VoidFunc<void>& _operation, const std::string& _description,
        const TimeWave& _insert_time_wave)
        : operation(_operation)
        , description(_description)
        , insert_time_wave(_insert_time_wave)
    {
    }

    void execute()
    {
        if (Likely(!is_executed)) {
            is_executed = true;
            operation();
        }
    }

    const std::string& getDescription() const { return description; }

    const TimeWave& getInsertTimeWave() const { return insert_time_wave; }

    template <typename T>
    void setInsertTimeWave(T&& t) { insert_time_wave = t; }
};

class OperationQueue {
public:
    int refresh_time = __DEFAULT_START_TIME;
    int wave_length = -1;
    std::multimap<int, Operation> queue;
    bool is_time_arrived();
};

void __Exit();

enum EffectiveMode {
    MAIN_UI = 0,
    MAIN_UI_OR_FIGHT_UI,
};

// *** Not In Queue
// 使用此函数可使脚本一次注入多次运行
// 适用于脚本完全无误后录制视频使用
// *** 注意：使用的关闭效果按键必须在战斗界面才会生效
// *** 使用示例
// OpenMultipleEffective() -------- 脚本多次生效，默认按下 C 键取消此效果
// OpenMultipleEffective('Q')-------  脚本多次生效，按下 Q 键取消此效果
// OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI)-------
// 脚本多次生效，按下 Q 键取消此效果，多次生效效果在主界面和选卡界面都会生效
// OpenMultipleEffective('Q', AvZ::MAIN_UI)-------  脚本多次生效，按下 Q
// 键取消此效果，多次生效效果仅在主界面生效
void OpenMultipleEffective(char close_key = 'C', int _effective_mode = MAIN_UI);

// 设定操作时间点
void SetTime(const TimeWave& _time_wave);

// 设定操作时间点
// *** 使用示例：
// SetTime(-95, 1)------ 将操作时间点设为第一波僵尸刷新前 95cs
// SetTime(-95)--------- 将操作时间点设为僵尸刷新前 95cs,
// 波数由上一个最近确定的波数决定
void SetTime(int time, int wave);

// 设定操作时间点
void SetTime(int time);

// 延迟一定时间
// *** 注意由于操作队列的优势，此函数支持负值
// *** 使用示例：
// Delay(298) ------ 延迟 298cs
// Delay(-298) ------ 提前 298cs
void Delay(int time);

// 设定时间为当前时间
void SetNowTime();

// 设定延迟时间
void SetDelayTime(int time);

// 阻塞运行直到达到目标时间点
// 使用方法与 setTime 相同
// *** 注意：WaitUntil 停止阻塞的时间点是设定的时间点的下一帧
// 例如 WaitUntil(150, 1); int time = NowTime(1)
// 此时 time 的值是 151
// *** return : 阻塞是否正常结束
// ture 阻塞正常结束
// false 阻塞异常结束
bool WaitUntil(const TimeWave& _time_wave);
bool WaitUntil(int time, int wave);

// 得到当前时间，读取失败返回 -65535
// *** 注意得到的是以参数波刷新时间点为基准的相对时间
// *** 使用示例：
// NowTime(1) -------- 得到以第一波刷新时间点为基准的当前时间
// NowTime(2) -------- 得到以第二波刷新时间点为基准的当前时间
int NowTime(int wave);

// 返回已经检测到的刷新波数
// *** 注意如果脚本没有从第一波开始运行，
// 那么返回的波数将会从开始运行的波数开始
std::vector<int> GetRefreshedWave();

// 得到当前正在运行的波数
int GetRunningWave();

enum InsertState {
    NOT_INSERT = -1,
    RUN
};

// 将操作插入操作队列中
template <class Func>
void InsertOperation(Func&& _operation,
    const std::string& description = "unknown")
{
    extern PvZ* __pvz_base;
    extern MainObject* __main_object;
    extern int __error_mode;
    extern TimeWave __time_wave_insert;
    extern TimeWave __time_wave_start;
    extern bool __is_insert_operation;
    extern std::vector<OperationQueue> __operation_queue_vec;

    TimeWave __CalculateInsertTimeWave();

    auto true_insert_time_wave = __CalculateInsertTimeWave();

    if (true_insert_time_wave.wave == RUN) {
        if (__error_mode == CONSOLE) {
            Print("(%d, %d)# run operation: %s (uninserted)\n",
                __time_wave_insert.time,
                __time_wave_insert.wave,
                description.c_str());
        }

        auto temp = __time_wave_insert;
        _operation();
        __time_wave_insert = temp;
    } else if (true_insert_time_wave.wave != NOT_INSERT) {
        if (__error_mode == CONSOLE) {
            Print("(%d, %d)# insert operation : %s \n",
                __time_wave_insert.time,
                __time_wave_insert.wave,
                description.c_str());
        }

        Operation operation(_operation, description, __time_wave_insert);
        auto& operation_queue = __operation_queue_vec[true_insert_time_wave.wave - 1].queue; // 取出相应波数的队列
        operation_queue.insert({true_insert_time_wave.time, operation});
    }
}

template <class Func>
void InsertTimeOperation(const TimeWave& time_wave, Func&& operation,
    const std::string& description = "unknown")
{
    SetTime(time_wave);
    InsertOperation(operation, description);
}

template <class Func>
void InsertTimeOperation(int time, int wave, Func&& operation,
    const std::string& description = "unknown")
{
    SetTime(time, wave);
    InsertOperation(operation, description);
}

// 确保当前操作是否被插入操作队列
class InsertGuard {
private:
    bool _is_insert_operation;

public:
    InsertGuard(bool is_insert)
    {
        extern bool __is_insert_operation;
        _is_insert_operation = __is_insert_operation;
        __is_insert_operation = is_insert;
    }

    ~InsertGuard()
    {
        extern bool __is_insert_operation;
        __is_insert_operation = _is_insert_operation;
    }
};

// 设置 insertOperation 属性函数
// *** 使用示例：
// SetInsertOperation(false) ---- insertOperation 将不会把操作插入操作队列中
// SetInsertOperation(true) ---- insertOperation 将会把操作插入操作队列中
void SetInsertOperation(bool _is_insert_operation = true);

// *** In Queue
// 调试功能：显示操作队列中当前时刻及以后操作
// *** 使用示例
// ShowQueue({1, 2, 3}) ----- 显示第 1 2 3 波中未被执行的操作
void ShowQueue(const std::vector<int>& lst);

// *** Not In Queue
// 脚本退出时用户的自定义处理，主要用于释放内存资源
template <class Func>
void ScriptExitDeal(Func&& func)
{
    extern VoidFunc<void> __script_exit_deal;
    __script_exit_deal = func;
}

// *** Not In Queue
// 设定脚本开始运行时间
// *** 使用示例
// SetScriptStartTime(100, 2) ------ 将脚本的开始运行时间设定为 100, 2
void SetScriptStartTime(int time, int wave);

// *** Not In Queue
// 设定高级暂停按键
// *** 注意开启高级暂停时不能使用跳帧
// *** 使用示例
// SetAdvancedPauseKey('Q') ------ 将 Q 键设定为高级暂停管理按键，即按下 Q
// 游戏暂停，再按 Q 游戏继续运行
void SetAdvancedPauseKey(char key);

// *** In Queue
// 跳到游戏指定时刻
// *** 注意使用此函数时不能使用高级暂停
// *** 使用示例
// SkipTick(200, 1) ------ 跳到时刻点 (200, 1)
void SkipTick(int time, int wave);

// *** In Queue
// 跳到指定条件为 false 的游戏帧
// *** 注意使用此函数时不能使用高级暂停
// *** 使用示例 : 直接跳到位置为 {1, 3}, {1, 5} 春哥死亡时的游戏帧
// auto condition = [=]() {
//     std::vector<int> results;
//     GetPlantIndices({{1, 3}, {1, 5}}, YMJNP_47, results);
//
//     for (auto result : results) {
//         if (result < 0) {
//             return false;
//         }
//     }
//     return true;
// };
//
// auto callback = [=]() {
//     ShowErrorNotInQueue("春哥无了，嘤嘤嘤");
// };
//
// SkipTick(condition, callback);
template <class Condition, class Callback>
void SkipTick(Condition&& condition, Callback&& callback)
{
    InsertOperation([condition = std::move(condition),
                        callback = std::move(callback)]() {
        extern bool __is_advanced_pause;
        extern VoidFunc<bool> __skip_tick_condition;
        extern PvZ* __pvz_base;
        if (__is_advanced_pause) {
            ShowErrorNotInQueue("开启高级暂停时不能启用跳帧");
            return;
        }

        if (__skip_tick_condition()) {
            ShowErrorNotInQueue("请等待上一个跳帧条件达到后的下一帧再设定跳帧条件");
            return;
        }

        __skip_tick_condition = [condition = std::move(condition),
                                    callback = std::move(callback)]() -> bool {
            if (__pvz_base->gameUi() == 3 && condition()) {
                return true;
            }
            __skip_tick_condition = []() -> bool { return false; };
            if (__pvz_base->gameUi() == 3) {
                callback();
            }
            return false;
        };
    },
        "SkipTick");
}

template <class Condition>
void SkipTick(Condition&& condition)
{
    SkipTick(condition, []() {});
}

} // namespace AvZ
#endif