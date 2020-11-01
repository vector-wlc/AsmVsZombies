/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 15:36:43
 * @Description: 操作队列头文件
 */

#include <functional>
#include <map>
#include <vector>

#include "pvzstruct.h"
#include "avz_debug.h"

#ifndef __AVZ_TIME_OPERATION_H__
#define __AVZ_TIME_OPERATION_H__

namespace AvZ
{
    // 该部分将使用者写的操作按照时间先后顺序录入消息队列，旧接口得以实现
    // 使用 20 条操作队列，每一条操作队列储存一波的操作
    // 需要每帧进行僵尸刷新时间的读取，普通波 <= 200，wave1 : <=600   wave10||20: 750
    // 当队列记录的刷新时间点为 -1 时，continue

    struct WaveTime
    {
        int wave;
        int time;
    };

    struct Operation
    {
        std::function<void()> operation;
        std::string description;
    };

    class OperationQueue
    {
    public:
        int refresh_time = -1;
        int wave_length = -1;
        std::map<int, std::vector<Operation>> queue;
        bool is_time_arrived()
        {
            extern MainObject *__main_object;
            if (queue.begin()->first < __main_object->gameClock() - refresh_time)
            {
                ShowErrorNotInQueue("您设定时间为 #cs, 但当前时间已到 #cs, 按下确定将以当前时间执行此次操作",
                                    queue.begin()->first,
                                    __main_object->gameClock() - refresh_time);
            }
            return queue.begin()->first <= __main_object->gameClock() - refresh_time;
        }
    };

    struct LabelOperation
    {
        int label;                       // 操作能够运行的标签
        std::function<void()> operation; // 操作
    };

    class ConditionalOperation
    {
    private:
        bool is_set_condition = false;
        std::function<int()> condition;                  // 产生条件函数
        std::vector<LabelOperation> label_operation_vec; // 运行函数

    public:
        // 设定运行条件
        // *** 注意此函数一旦调用将会清空所有添加的运行函数
        void setCondition(const std::function<int()> &_condition)
        {
            label_operation_vec.clear();
            is_set_condition = true;
            condition = _condition;
        }

        // 添加操作
        void addOperation(int label, const std::function<void()> &operation)
        {
            LabelOperation label_operation = {label, operation};
            label_operation_vec.push_back(label_operation);
        }

        // 将操作插入操作队列
        void run(int time, int wave);
    };

    void __Run(MainObject *, std::function<void()> Script);
    void __Exit();

    enum EffectiveMode
    {
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
    // OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI)-------  脚本多次生效，按下 Q 键取消此效果，多次生效效果在主界面和选卡界面都会生效
    // OpenMultipleEffective('Q', AvZ::MAIN_UI)-------  脚本多次生效，按下 Q 键取消此效果，多次生效效果仅在主界面生效
    void OpenMultipleEffective(char close_key = 'C', int _effective_mode = MAIN_UI);

    // 设定操作时间点
    void SetTime(const TimeWave &_time_wave);

    // 设定操作时间点
    // *** 使用示例：
    // SetTime(-95, 1)------ 将操作时间点设为第一波僵尸刷新前 95cs
    // SetTime(-95)--------- 将操作时间点设为僵尸刷新前 95cs, 波数由上一个最近确定的波数决定
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

    // 阻塞运行直到达到目标时间点
    // 使用方法与 setTime 相同
    // *** return : 阻塞是否正常结束
    // ture 阻塞正常结束
    // false 阻塞异常结束
    bool WaitUntil(const TimeWave &_time_wave);
    bool WaitUntil(int time, int wave);

    // 得到当前时间，读取失败返回 -1
    // *** 注意得到的是以参数波刷新时间点为基准的相对时间
    // *** 使用示例：
    // NowTime(1) -------- 得到以第一波刷新时间点为基准的当前时间
    // NowTime(2) -------- 得到以第二波刷新时间点为基准的当前时间
    int NowTime(int wave);

    // 返回已经检测到的刷新波数
    // *** 注意如果脚本没有从第一波开始运行，
    // 那么返回的波数将会从开始运行的波数开始
    std::vector<int> GetRefreshedWave();

    // 将操作插入操作队列中
    void InsertOperation(const std::function<void()> &operation, const std::string &description = "unknown");
    void InsertTimeOperation(const TimeWave &time_wave, const std::function<void()> &operation, const std::string &description = "unknown");
    void InsertTimeOperation(int time, int wave, const std::function<void()> &operation, const std::string &description = "unknown");

    // 确保当前操作是否被插入操作队列
    class InsertGuard
    {
    private:
        bool _is_insert_operation;

    public:
        InsertGuard(bool is_insert)
        {
            extern bool is_insert_operation;
            _is_insert_operation = is_insert_operation;
            is_insert_operation = is_insert;
        }

        ~InsertGuard()
        {
            extern bool is_insert_operation;
            is_insert_operation = _is_insert_operation;
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
    void ShowQueue(const std::vector<int> &lst);

} // namespace AvZ
#endif