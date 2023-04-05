#ifndef __AVZ_SMART_H__
#define __AVZ_SMART_H__

#include "avz_tick_runner.h"
#include <unordered_set>

class ACobManager : public AOrderedStateHook<-1> {
    __ADeleteCopyAndMove(ACobManager);

public:
    // structs
    // 用于RAWPAO函数
    struct FireDrop {
        int cobRow;
        int cobCol;
        int dropRow;
        float dropCol;
    };

    // 记录炮的位置和落点的位置及炮弹飞行时间
    struct RoofFireDrop {
        int vecIndex;
        int dropRow;
        float dropCol;
        int fireTime;
    };

    // 屋顶炮飞行时间辅助数据
    struct RoofFlyTime {
        int minDropX;   // 记录该列炮最小飞行时间对应的最小的横坐标
        int minFlyTime; // 记录最小的飞行时间
    };

    struct LastestMsg {
        int vecIndex = -1;
        int fireTime = 0;
        bool isWritable = true;
    };

    // 选用顺序控制模式
    enum SequentialMode {
        SPACE,
        TIME
    };

public:
    ACobManager() = default;
    static constexpr int NO_EXIST_RECOVER_TIME = INT_MIN;

    // 发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
    // *** 注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与 SetList 更新的炮序将无效！
    // *** 使用示例：
    // RawRoofFire(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // RawRoofFire({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void RawRoofFire(int cobRow, int cobCol, int dropRow, float dropCol);

    // 发炮函数：用户自定义位置发射，屋顶修正飞行时间发炮.
    // *** 注意：尽量不要使用此函数操作位于炮列表中的炮，因为使用此函数后自动识别的炮序与 SetList 更新的炮序将无效！
    // *** 使用示例：
    // RawRoofFire(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // RawRoofFire({ {1,2,2,9},{1,3,5,9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void RawRoofFire(const std::vector<FireDrop>& lst);

    // 发炮函数：用户自定义位置发射
    // *** 注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 SetList 更新的炮序将无效！
    // *** 使用示例：
    // RawFire(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // RawFire({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void RawFire(int cobRow, int cobCol, int dropRow, float dropCol);

    // 发炮函数：用户自定义位置发射
    // *** 注意：尽量不要使用此函数操作位于有炮序炮列表中的炮，因为使用此函数后自动识别的炮序与 SetList 更新的炮序将无效！
    // *** 使用示例：
    // RawFire(1,2,2,9)-----------------------将位置为（1，2）的炮发射到（2，9）
    // RawFire({{1, 2, 2, 9}, {1, 3, 5, 9}})-------将位置为（1，2）的炮发射到（2，9），将位置为（1，3）的炮发射到（5，9）
    static void RawFire(const std::vector<FireDrop>& lst);

    // 种植炮函数
    // *** 使用示例
    // Plant(3, 4)------在三行四列位置种炮
    static void Plant(int row, int col);

    // 立即修补上一枚已经发射的炮
    void FixLatest();

    // 设置炮序模式
    // *** 使用示例：
    // SetSequentialMode(ACobManager::TIME) ---- 设置时间使用模式
    // SetSequentialMode(ACobManager::SPACE) ---- 设置空间使用模式
    void SetSequentialMode(int sequentialMode)
    {
        _sequentialMode = sequentialMode;
    }

    // 设置即将发射的下一门炮
    // 此函数只有在限制炮序的时候才可调用
    // *** 使用示例：
    // SetNext(10)------将炮列表中第十门炮设置为下一门即将发射的炮
    // SetNext(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
    void SetNext(int next);
    void SetNext(int row, int col);

    // 跳炮函数
    // *** 使用示例：
    // Skip(2)---跳过按照顺序即将要发射的2门炮
    void Skip(int x)
    {
        _next = (_next + x) % _gridVec.size();
    }

    // 发炮函数
    // *** 注意，此函数返回 >=0 的值代表使用的炮在炮列表中的位置
    // 如返回 0 就是使用的列表中的第一门炮
    // 如返回 1 就是使用的列表中的第二门炮
    // 如果 返回的值 < 0 则代表发射失败
    // *** 使用示例：
    // Fire(2,9)----------------炮击二行，九列
    // Fire({{2, 9}, {5, 9}})-----炮击二行，九列，五行，九列
    int Fire(int row, float col);
    std::vector<int> Fire(const std::vector<APosition>& lst);

    // 发炮函数 炮CD恢复自动发炮
    // *** 使用示例：
    // RecoverFire(2,9)----------------炮击二行，九列
    // RecoverFire({{2, 9}, {5, 9}})-----炮击二行，九列，五行，九列
    int RecoverFire(int row, float col);
    std::vector<int> RecoverFire(const std::vector<APosition>& lst);

    // 屋顶修正飞行时间发炮.
    // 此函数只适用于RE与ME 修正时间：387cs
    // *** 使用示例：
    // RoofFire(3,7)---------------------修正飞行时间后炮击3行7列
    // RoofFire({{2, 9}, {5, 9}})---------修正飞行时间后炮击2行9列,5行9列
    int RoofFire(int row, float col);
    std::vector<int> RoofFire(const std::vector<APosition>& lst);

    // 发炮函数 屋顶炮 CD 恢复自动发炮
    // *** 使用示例：
    // RecoverRoofFire(2,9)----------------炮击二行，九列
    // RecoverRoofFire({{2, 9}, {5, 9}})-----炮击二行，九列，五行，九列
    int RecoverRoofFire(int row, float col);
    std::vector<int> RecoverRoofFire(const std::vector<APosition>& lst);

    // 重置炮列表
    // *** 使用示例:
    // SetList({{3, 1},{4, 1},{3, 3},{4, 3}})-------经典四炮
    void SetList(const std::vector<AGrid>& lst);

    // 自动填充炮列表
    // *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
    void AutoGetList();

    // 得到可用的炮的内存指针
    // *** 注意: 如果没有炮可用返回 nullptr
    // *** 使用示例
    // auto cobPtr = GetUsablePtr() ---- 得到可用的炮的内存指针
    __ANodiscard APlant* GetUsablePtr();

    // 得到可用的屋顶炮的内存指针
    // 需要提供炮射击的列数
    // 如果没有炮可用返回 nullptr
    // *** 使用示例
    // auto cobPtr = GetRoofUsablePtr(9) ---- 得到发往九列可用的屋顶炮的内存指针
    __ANodiscard APlant* GetRoofUsablePtr(float col);

    // 得到恢复时间最短的炮的内存指针
    // *** 注意: 如果发生内部错误返回 nullptr
    // *** 使用示例
    // auto cobPtr = GetRecoverPtr() ---- 得到恢复时间最短的炮的内存指针
    __ANodiscard APlant* GetRecoverPtr();

    // 得到恢复时间最短的屋顶炮的内存指针
    // *** 注意: 如果发生内部错误返回 nullptr
    // *** 使用示例
    // auto cobPtr = GetRoofRecoverPtr(9) ---- 得到发往九列恢复时间最短的屋顶炮的内存指针
    __ANodiscard APlant* GetRoofRecoverPtr(float col);

    // 获取屋顶炮飞行时间
    // *** 使用示例:
    // GetRoofFlyTime(1, 7) ----- 得到 1 列屋顶炮发往 7 列飞行时间
    static int GetRoofFlyTime(int cobCol, float dropCol);

protected:
    static std::unordered_set<int> _lockSet; // 锁定的炮
    static ATickRunner _tickRunner;
    static RoofFlyTime _flyTimeData[8];
    std::vector<int> _indexVec;  // 炮的内存位置
    std::vector<AGrid> _gridVec; // 炮列表，记录炮的信息
    int _next;                   // 记录当前即将发射的下一门炮
    int _sequentialMode = true;  // 顺序模式
    LastestMsg _lastestMsg;      // 最近一颗发炮的信息

    // 基础发炮函数
    static void _BasicFire(int cobIndex, int dropRow, float dropCol);

    // 延迟发炮
    static void _DelayFire(int delayTime, int cobIndex, int row, float col);

    // 恢复发炮
    int _RecoverBasicFire(int row, float col, bool isRoof);

    // 得到炮列表中的炮恢复时间
    // return NO_EXIST_RECOVER_TIME :  can't find cob index
    int _GetRecoverTimeVec();

    // 更新下一门要发射的炮
    // 第一个参数用于 RecoverFire
    // 第二个参数用于 RoofFire
    // 返回 >=0 下一门炮可用且意义为该门炮剩余的恢复时间
    // 返回 NO_EXIST_RECOVER_TIME 下一门炮不可用
    int _UpdateNextCob(bool isDelay = false, float dropCol = -1, bool isShowError = true);

    // 更新最近发炮的信息
    void _UpdateLastestMsg(int fireTime, int index)
    {
        if (_lastestMsg.isWritable) {
            _lastestMsg.fireTime = fireTime;
            _lastestMsg.vecIndex = index;
        }
    }

    // 得到可用的炮的指针
    // 如果没有炮可用返回 nullptr
    __ANodiscard APlant* _BasicGetPtr(bool isRecover, float col);
    virtual void _BeforeScript() override;
    virtual void _EnterFight() override;
};

class AItemCollector : public ATickRunnerWithNoStart {

    __ADeleteCopyAndMove(AItemCollector);

public:
    AItemCollector() = default;
    void Start();
    void SetInterval(int timeInterval);

protected:
    int _timeInterval = 10;
    void _Run();
    virtual void _EnterFight() override;
};

class AIceFiller : public ATickRunnerWithNoStart {
    __ADeleteCopyAndMove(AIceFiller);

protected:
    std::vector<AGrid> _fillIceGridVec;
    std::vector<int> _iceSeedIdxVec;
    int _coffeeSeedIdx;
    void _Run();

public:
    AIceFiller() = default;
    // 重置冰卡
    // *** 注意：该函数需要使用在 start 函数之后才能生效
    // *** 使用示例
    // SetIceSeedList({ICE_SHROOM}) ------ 只使用原版冰
    // SetIceSeedList({M_ICE_SHROOM, ICE_SHROOM}) ----- 优先使用模仿冰，再使用原版冰
    void SetIceSeedList(const std::vector<int>& lst);

    // 重置存冰位置
    // *** 使用示例：
    // SetList({{3,4},{5,6}})-----将存冰位置重置为{3，4}，{5，6}
    void SetList(const std::vector<AGrid>& lst)
    {
        this->_fillIceGridVec = lst;
    }

    // 线程开始工作
    // *** 使用示例：
    // Start({{3,4},{5,6}})-----在{3，4}，{5，6}位置存冰
    void Start(const std::vector<AGrid>& lst);

    // 使用咖啡豆函数
    // *** 使用示例：
    // Coffee()-----自动使用执行次序低的存冰位
    void Coffee();
};

class APlantFixer : public ATickRunnerWithNoStart {
    __ADeleteCopyAndMove(APlantFixer);

protected:
    bool _isUseCoffee = false;
    int _plantType;
    int _fixHp = 0;
    int _coffeeSeedIdx;
    std::vector<int> _seedIdxVec;
    std::vector<AGrid> _gridLst;
    void _GetSeedList();
    void _Run();
    void _UseSeed(int seedIndex, int row, float col, bool isNeedShovel);

public:
    APlantFixer() = default;
    // 重置植物修补位置
    // *** 使用示例：
    // SetList({{2, 3},{3, 4}})-------位置被重置为{2，3}，{3，4}
    void SetList(const std::vector<AGrid>& lst)
    {
        _gridLst = lst;
    }
    // 自动得到修补的位置列表
    void AutoGetList();

    // 线程开始工作，此函数开销较大，不建议多次调用
    // 第一个参数为植物类型
    // 第二个参数不填默认全场
    // 第三个参数不填默认植物血量为150以下时修补
    // *** 使用示例：
    // Start(23)-------修补全场的高坚果
    // Start(30,{{1,3},{2,3}})-----修补位置为{1，3}，{2，3}位置的南瓜头
    // Start(3,{{1,3},{2,3}},300)------修补位置为{1，3}，{2，3}位置的坚果，血量降至300开始修补
    void Start(int plantType, const std::vector<AGrid>& lst = {}, int fixHp = 150);

    // 重置修补血量
    // *** 使用示例：
    // SetHp(200)------将修补触发血量改为200
    void SetHp(int _fix_hp)
    {
        _fixHp = _fix_hp;
    }

    // 是否使用咖啡豆
    void SetIsUseCoffee(bool isUseCoffee)
    {
        _isUseCoffee = isUseCoffee;
    }
};

#endif