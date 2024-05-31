#ifndef __AVZ_COB_MANAGER_H__
#define __AVZ_COB_MANAGER_H__

#include "avz_tick_runner.h"
#include <unordered_set>

class ACobManager : public AOrderedBeforeScriptHook<-1> {
    __ADeleteCopyAndMove(ACobManager);

public:
    // 用于 RawFire 函数
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
        TIME,
        PRIORITY,
    };

public:
    ACobManager(SequentialMode sequentialMode = TIME)
        : _sequentialMode(sequentialMode)
    {
    }

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
    // SetSequentialMode(ACobManager::SPACE) ---- 设置为空间模式
    // SetSequentialMode(ACobManager::PRIORITY) ---- 设置为优先级模式
    void SetSequentialMode(SequentialMode sequentialMode)
    {
        _sequentialMode = sequentialMode;
    }

    // 设置即将发射的下一门炮
    // 此函数在优先级模式下无效
    // *** 使用示例：
    // SetNext(10)------将炮列表中第十门炮设置为下一门即将发射的炮
    // SetNext(2, 8)------将炮列表中位于 (2, 8) 的炮设置为下一门即将发射的炮
    void SetNext(int next);
    void SetNext(int row, int col);

    // 跳过炮列表中接下来的 n 门炮
    // 此函数在优先级模式下无效
    // *** 使用示例：
    // Skip(2)---跳过按照顺序即将要发射的两门炮
    void Skip(int n);

    // 从炮列表中删除一门或多门炮
    // *** 使用示例：
    // EraseFromList(2, 7)------将位于 (2, 7) 的炮从炮列表中删除
    // EraseFromList({{1, 7}, {2, 7}})------将位于 (1, 7) 和 (2, 7) 的炮从炮列表中删除
    void EraseFromList(int row, int col);
    void EraseFromList(const std::vector<AGrid>& lst);

    // 将一门或多门炮的优先级升至最高
    // 此函数仅在优先级模式下有效
    // 如果指定的炮不在炮列表中，会自动将其添加至炮列表
    // *** 使用示例：
    // MoveToListTop(2, 7)------将位于 (2, 7) 的炮优先级升至最高
    // MoveToListTop({{1, 7}, {2, 7}})------将位于 (1, 7) 和 (2, 7) 的炮优先级升至最高，(1, 7) 在前
    void MoveToListTop(int row, int col);
    void MoveToListTop(const std::vector<AGrid>& lst);

    // 将一门或多门炮的优先级降至最低
    // 此函数仅在优先级模式下有效
    // 如果指定的炮不在炮列表中，会自动将其添加至炮列表
    // *** 使用示例：
    // MoveToListBottom(2, 7)------将位于 (2, 7) 的炮优先级降至最低
    // MoveToListBottom({{1, 7}, {2, 7}})------将位于 (1, 7) 和 (2, 7) 的炮优先级降至最低，(2, 7) 在后
    void MoveToListBottom(int row, int col);
    void MoveToListBottom(const std::vector<AGrid>& lst);

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
    // SetList({{3, 1}, {4, 1}, {3, 3}, {4, 3}})-------经典四炮
    void SetList(const std::vector<AGrid>& lst);

    // 获取炮列表
    // *** 使用示例:
    // std::vector<AGrid> lst = GetList();
    const std::vector<AGrid>& GetList() const
    {
        return _gridVec;
    }

    // 自动填充炮列表
    // *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
    void AutoSetList();

    // 自动填充炮列表
    // *** 注意：此函数无条件将场地上的所有炮填充至此炮列表
    // 此函数已弃用，请使用 AutoSetList();
    __ADeprecated("请使用 AutoSetList()") void AutoGetList()
    {
        AutoSetList();
    }

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

    struct RecoverInfo {
        APlant* ptr;
        int recoverTime;
    };

    // 得到恢复时间列表
    // *** 注意当得到的指针为 nullptr 或者回复时间为小于 0 的值时，此炮不可用
    // *** 使用示例
    // auto lst = aCobManager.GetRecoverList();
    // for (auto&& [ptr, recoverTime] : lst) {
    //    // 这里应该写相应的处理代码
    //    // 示例就打印一下算了
    //    logger.Info("指针: #, 恢复时间: #", ptr, recoverTime);
    // }
    __ANodiscard std::vector<RecoverInfo> GetRecoverList();

    // 得到屋顶炮恢复时间列表
    // *** 注意当得到的指针为 nullptr 或者回复时间为小于 0 的值时，此炮不可用
    // *** 使用示例
    // auto lst = aCobManager.GetRoofRecoverList(9); // 得到发往第九列的屋顶炮的恢复时间列表
    // for (auto&& [ptr, recoverTime] : lst) {
    //    // 这里应该写相应的处理代码
    //    // 示例就打印一下算了
    //    logger.Info("指针: #, 恢复时间: #", ptr, recoverTime);
    // }
    __ANodiscard std::vector<RecoverInfo> GetRoofRecoverList(float col);

    // 得到可用列表
    // *** 使用示例
    // auto lst = aCobManager.GetUsableList();
    // for (auto ptr : lst) {
    //    // 这里应该写相应的处理代码
    //    // 示例就打印一下算了
    //    logger.Info("指针: #", ptr);
    // }
    __ANodiscard std::vector<APlant*> GetUsableList();

    // 得到屋顶炮可用列表
    // *** 使用示例
    // auto lst = aCobManager.GetRoofUsableList(9); // 得到发往第九列的屋顶炮的可用列表
    // for (auto ptr : lst) {
    //    // 这里应该写相应的处理代码
    //    // 示例就打印一下算了
    //    logger.Info("指针: #", ptr);
    // }
    __ANodiscard std::vector<APlant*> GetRoofUsableList(float col);

    // 获取屋顶炮飞行时间
    // *** 使用示例:
    // GetRoofFlyTime(1, 7) ----- 得到 1 列屋顶炮发往 7 列飞行时间
    __ANodiscard static int GetRoofFlyTime(int cobCol, float dropCol);

protected:
    static std::unordered_set<int> _lockSet; // 锁定的炮
    static ATickRunner _tickRunner;
    static constexpr RoofFlyTime _flyTimeData[8] = {
        {515, 359},
        {499, 362},
        {515, 364},
        {499, 367},
        {515, 369},
        {499, 372},
        {511, 373},
        {511, 373},
    };
    std::vector<int> _indexVec;     // 炮的内存位置
    std::vector<AGrid> _gridVec;    // 炮列表，记录炮的信息
    int _next;                      // 记录当前即将发射的下一门炮
    SequentialMode _sequentialMode; // 顺序模式
    LastestMsg _lastestMsg;         // 最近一颗发炮的信息

    void _Skip(int n);

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

    __ANodiscard std::vector<RecoverInfo> _BasicGetRecoverList(float col);

    virtual void _BeforeScript() override;
};

#endif
