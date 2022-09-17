/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 16:39:44
 * @Description: cannon api
 */
#ifndef __AVZ_CANNON_H__
#define __AVZ_CANNON_H__

#include "avz_tick.h"
#include "avz_time_operation.h"
#include <set>

namespace AvZ {
class PaoOperator : public GlobalVar {
public:
    // structs
    // 用于RAWPAO函数
    struct PaoDrop {
        int pao_row;
        int pao_col;
        int drop_row;
        float drop_col;
    };

    // 记录炮的位置和落点的位置及炮弹飞行时间
    struct RoofPaoDrop {
        int vec_index;
        int drop_row;
        float drop_col;
        int fire_time;
    };

    // 屋顶炮飞行时间辅助数据
    struct RoofFlyTime {
        int min_drop_x;   // 记录该列炮最小飞行时间对应的最小的横坐标
        int min_fly_time; // 记录最小的飞行时间
    };

    struct LastestPaoMsg {
        int vec_index = -1;
        int fire_time = 0;
        bool is_writable = true;
    };

    // 选用顺序控制模式
    enum SequentialMode {
        SPACE,
        TIME
    };

public:
    void virtual beforeScript() override
    {
        initialState();
    }

protected:
    static constexpr int NO_EXIST_RECOVER_TIME = INT_MIN;
    static std::set<int> lock_pao_set; // 锁定的炮
    std::vector<int> pao_index_vec;    // 炮的内存位置
    std::vector<Grid> pao_grid_vec;    // 炮列表，记录炮的信息
    int next_pao;                      // 记录当前即将发射的下一门炮
    int sequential_mode = true;        // 顺序模式
    LastestPaoMsg lastest_pao_msg;     // 最近一颗发炮的信息
    static TickRunner tick_runner;
    static RoofFlyTime fly_time_data[8];
    // 得到炮的恢复时间
    static int get_recover_time(int index);
    // 基础发炮函数
    static void base_fire_pao(int cannon_index, int drop_row, float drop_col);
    // 获取屋顶炮飞行时间
    static int get_roof_fly_time(int pao_col, float drop_col);
    // 延迟发炮
    static void delay_fire_pao(int delay_time, int cannon_index, int row, float col);
    // 禁用 =
    void operator=(const PaoOperator& p);
    // 得到炮列表中的炮恢复时间
    // return NO_EXIST_RECOVER_TIME :  can't find pao index
    int get_recover_time_vec();

    // 更新下一门要发射的炮
    // 第一个参数用于 recoverPao
    // 第二个参数用于 roofPao
    // 返回 >=0 下一门炮可用且意义为该门炮剩余的恢复时间
    // 返回 NO_EXIST_RECOVER_TIME 下一门炮不可用
    int update_next_pao(bool is_delay_pao = false, float drop_col = -1);

    // 更新最近发炮的信息
    void update_lastest_pao_msg(int fire_time, int index)
    {
        if (lastest_pao_msg.is_writable) {
            lastest_pao_msg.fire_time = fire_time;
            lastest_pao_msg.vec_index = index;
        }
    }
    // 跳过一定数量的炮
    void skip_pao(int x)
    {
        next_pao = (next_pao + x) % pao_grid_vec.size();
    }

public:
    // *** Not In Queue
    // 炮操作类状态初始化
    static void initialState();

    // *** In Queue
    // 发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
    // *** 注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
    // *** 使用示例：
    // rawRoofPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // rawRoofPao({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void rawRoofPao(int pao_row, int pao_col, int drop_row, float drop_col);

    // *** In Queue
    // 发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
    // *** 注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
    // *** 使用示例：
    // rawRoofPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // rawRoofPao({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void rawRoofPao(const std::vector<PaoDrop>& lst);

    // *** In Queue
    // 发炮函数：用户自定义位置发射
    // *** 注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
    // *** 使用示例：
    // rawPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // rawPao({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void rawPao(int pao_row, int pao_col, int drop_row, float drop_col);

    // *** In Queue
    // 发炮函数：用户自定义位置发射
    // *** 注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 resetPaolist 更新的炮序将无效！
    // *** 使用示例：
    // rawPao(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // rawPao({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void rawPao(const std::vector<PaoDrop>& lst);

    // *** In Queue
    // 种植炮函数
    // *** 使用示例
    // plantPao(3, 4)------在三行四列位置种炮
    static void plantPao(int row, int col);

    PaoOperator();

    // *** In Queue
    // 立即修补上一枚已经发射的炮
    void fixLatestPao();

    // *** In Queue
    // 设置炮序模式
    // *** 使用示例：
    // setSequentialMode(AvZ::PaoOperator::TIME) ---- 设置时间使用模式
    // setSequentialMode(AvZ::PaoOperator::SPACE) ---- 设置空间使用模式
    void setSequentialMode(int _sequential_mode)
    {
        InsertOperation([=]() {
            sequential_mode = _sequential_mode;
        },
            "setSequentialMode");
    }

    // *** In Queue
    // 设置即将发射的下一门炮
    // 此函数只有在限制炮序的时候才可调用
    // *** 使用示例：
    // setNextPao(10)------将炮列表中第十门炮设置为下一门即将发射的炮
    // setNextPao(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
    void setNextPao(int next_pao);

    // *** In Queue
    // 设置即将发射的下一门炮
    // 此函数只有在限制炮序的时候才可调用
    // *** 使用示例：
    // setNextPao(10)------将炮列表中第十门炮设置为下一门即将发射的炮
    // setNextPao(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
    void setNextPao(int row, int col);

    // *** In Queue
    // 跳炮函数
    // *** 使用示例：
    // skipPao(2)---跳过按照顺序即将要发射的2门炮
    void skipPao(int x)
    {
        InsertOperation([=]() {
            next_pao = (next_pao + x) % pao_grid_vec.size();
        },
            "skipPao");
    }

    // *** In Queue
    // 发炮函数
    // *** 使用示例：
    // pao(2,9)----------------炮击二行，九列
    // pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
    void pao(int row, float col);

    // *** In Queue
    // 发炮函数
    // *** 使用示例：
    // pao(2,9)----------------炮击二行，九列
    // pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
    void pao(const std::vector<Position>& lst);

    // *** In Queue
    // 发炮函数 炮CD恢复自动发炮
    // *** 使用示例：
    // recoverPao(2,9)----------------炮击二行，九列
    // recoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
    void recoverPao(int row, float col);

    // *** In Queue
    // 发炮函数 炮CD恢复自动发炮
    // *** 使用示例：
    // recoverPao(2,9)----------------炮击二行，九列
    // recoverPao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
    void recoverPao(const std::vector<Position>& lst);

    // *** In Queue
    // 屋顶修正飞行时间发炮. 此函数开销较大不适合精确键控.
    // 此函数只适用于RE与ME 修正时间：387cs
    // *** 使用示例：
    // roofPao(3,7)---------------------修正飞行时间后炮击3行7列
    // roofPao({ {2,9},{5,9} })---------修正飞行时间后炮击2行9列,5行9列
    void roofPao(int row, float col);

    // *** In Queue
    // 屋顶修正飞行时间发炮. 此函数开销较大不适合精确键控.
    // 此函数只适用于RE与ME 修正时间：387cs
    // *** 使用示例：
    // roofPao(3,7)---------------------修正飞行时间后炮击3行7列
    // roofPao({ {2,9},{5,9} })---------修正飞行时间后炮击2行9列,5行9列
    void roofPao(const std::vector<Position>& lst);

    // *** In Queue
    // 重置炮列表
    // *** 使用示例:
    // resetPaoList({{3, 1},{4, 1},{3, 3},{4, 3}})-------经典四炮
    void resetPaoList(const std::vector<Grid>& lst);

    // *** In Queue
    // 自动填充炮列表
    // *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
    void autoGetPaoList();
};
} // namespace AvZ
#endif