#ifndef __AVZ_TICK_H__
#define __AVZ_TICK_H__

#include <stack>
#include <vector>

#include "avz_time_operation.h"
#include "avz_debug.h"
#include "avz_global.h"

namespace AvZ
{
    struct ThreadInfo
    {
        std::function<void()> func;
        int *id_ptr;
    };

    // CLASS TickRunner
    // 使用此类使得操作每帧都运行
    class TickRunner
    {
    private:
        int thread_id = -1;
        bool is_paused = false;
        bool thread_examine()
        { // 线程出现异常返回 false
            if (thread_id >= 0)
            {
                ShowErrorNotInQueue("一个自动线程类不允许同时拥有两个线程！");
                return false;
            }
            return true;
        }

    public:
        enum ThreadStatus
        {
            STOPPED = 0,
            PAUSED,
            RUNNING
        };

        // *** Not In Queue
        // 得到线程的状态
        // *** 返回值：
        // 停止状态：return STOPPED
        // 暂停状态：return PAUSED
        // 运行状态：return RUNNING
        ThreadStatus getStatus() const;

        // *** Not In Queue
        void pushFunc(const std::function<void()> &run);

        // *** In Queue
        void stop()
        {
            InsertOperation([=]() {
                extern std::vector<ThreadInfo> __thread_vec;
                extern std::stack<int> __stopped_thread_id_stack;
                __stopped_thread_id_stack.push(thread_id);
                thread_id = -1;
            },
                            "stop");
        }

        // *** In Queue
        void pause()
        {
            InsertOperation([=]() {
                is_paused = true;
            },
                            "pause");
        }
        // *** In Queue
        void goOn()
        {
            InsertOperation([=]() {
                is_paused = false;
            },
                            "goOn");
        }
    };

    class ItemCollector : public TickRunner
    {
    private:
        int time_interval = 10;
        void run();

    public:
        // *** In Queue
        void start()
        {
            InsertOperation([=]() {
                pushFunc([=]() {
                    run();
                });
            },
                            "startCollect");
        }
        // *** In Queue
        void setInterval(int _time_interval)
        {
            InsertOperation([=]() {
                this->time_interval = _time_interval;
            },
                            "setInterval");
        }
    };

    class IceFiller : public TickRunner
    {
    private:
        std::vector<Grid> fill_ice_grid_vec;
        std::vector<int> ice_seed_index_vec;
        int coffee_seed_index;
        void run();

    public:
        // *** In Queue
        // 重置冰卡
        // *** 注意：该函数需要使用在 start 函数之后才能生效
        // *** 使用示例
        // resetIceSeedList({ICE_SHROOM}) ------ 只使用原版冰
        // resetIceSeedList({M_ICE_SHROOM, ICE_SHROOM}) ----- 优先使用模仿冰，再使用原版冰
        void resetIceSeedList(const std::vector<int> &lst);

        // *** In Queue
        // 重置存冰位置
        // *** 使用示例：
        // resetFillList({{3,4},{5,6}})-----将存冰位置重置为{3，4}，{5，6}
        void resetFillList(const std::vector<Grid> &lst)
        {
            InsertOperation([=]() {
                this->fill_ice_grid_vec = lst;
            },
                            "resetFillList");
        }

        // *** In Queue
        // 线程开始工作
        // *** 使用示例：
        // start({{3,4},{5,6}})-----在{3，4}，{5，6}位置存冰
        void start(const std::vector<Grid> &lst);

        // *** In Queue
        // 使用咖啡豆函数
        // *** 使用示例：
        // coffee()-----自动使用优先级低的存冰位
        void coffee();
    };

    class PlantFixer : public TickRunner
    {

    private:
        bool is_use_coffee = false;
        int plant_type;
        int fix_hp = 0;
        int coffee_seed_index;
        std::vector<int> seed_index_vec;
        std::vector<Grid> grid_lst;
        void get_seed_list();
        void run();
        void use_seed_(int seed_index, int row, float col, bool is_need_shovel);
        void auto_get_fix_list();

    public:
        // *** In Queue
        // 重置植物修补位置
        // *** 使用示例：
        // resetFixList({{2, 3},{3, 4}})-------位置被重置为{2，3}，{3，4}
        void resetFixList(const std::vector<Grid> &lst)
        {
            InsertOperation([=]() {
                grid_lst = lst;
            },
                            "resetFixList");
        }
        // *** In Queue
        // 自动得到修补的位置列表
        void autoGetFixList();

        // *** In Queue
        // 线程开始工作，此函数开销较大，不建议多次调用
        // 第一个参数为植物类型
        // 第二个参数不填默认全场
        // 第三个参数不填默认植物血量为150以下时修补
        // *** 使用示例：
        // start(23)-------修补全场的高坚果
        // start(30,{{1,3},{2,3}})-----修补位置为{1，3}，{2，3}位置的南瓜头
        // start(3,{{1,3},{2,3}},300)------修补位置为{1，3}，{2，3}位置的坚果，血量降至300开始修补
        void start(int _plant_type, const std::vector<Grid> &lst = {}, int _fix_hp = 150);

        // *** In Queue
        // 重置修补血量
        // *** 使用示例：
        // resetFixHp(200)------将修补触发血量改为200
        void resetFixHp(int _fix_hp)
        {
            InsertOperation([=]() {
                fix_hp = _fix_hp;
            },
                            "resetFixHp");
        }

        // *** In Queue
        // 是否使用咖啡豆
        void isUseCoffee(bool _is_use_coffee)
        {
            InsertOperation([=]() {
                is_use_coffee = _is_use_coffee;
            },
                            "isUseCoffee");
        }
    };

    class KeyConnector : public TickRunner
    {
    private:
        std::vector<std::pair<char, std::function<void()>>> key_operation_vec;

    public:
        // *** Not In Queue
        // 添加操作
        // 不论此函数在何时被调用，指令全局有效！
        void add(char key, std::function<void()> operate);

        void clear()
        {
            key_operation_vec.clear();
        }
    };

} // namespace AvZ
#endif