#ifndef __AVZ_SMART_H__
#define __AVZ_SMART_H__

#include "avz_tick_runner.h"

class AItemCollector : public ATickRunnerWithNoStart,
                       public AOrderedEnterFightHook<-1> {
    __ADeleteCopyAndMove(AItemCollector);

public:
    AItemCollector();
    void Start();
    void SetInterval(int timeInterval);

    // 设置收集类型
    // 1银币
    // 2金币
    // 3钻石
    // 4阳光
    // 5小阳光
    // 6大阳光
    // 8奖杯
    // 9铲子
    // 10图鉴
    // 11钥匙
    // 12透视空花瓶
    // 13绿水壶
    // 14玉米卷
    // 15信
    // 16植物卡片
    // 17礼物盒子
    // 18钱袋
    // 19礼物盒子
    // 20钱袋
    // 21银向日葵
    // 22金向日葵
    // 23巧克力
    // 24巧克力
    // 25礼物盒子
    // 26礼物盒子
    // 27礼物盒子
    // 使用示例：
    // SetTypeList({1, 2, 3}); -------- 只收集银币金币和钻石
    void SetTypeList(const std::vector<int>& types);

protected:
    int _timeInterval = 10;
    static constexpr int _TYPE_SIZE = 28;
    std::array<bool, _TYPE_SIZE> _types;
    void _Run();
    virtual void _EnterFight() override;
};

class AIceFiller : public ATickRunnerWithNoStart,
                   public AOrderedEnterFightHook<-1> {
    __ADeleteCopyAndMove(AIceFiller);

protected:
    std::vector<AGrid> _fillIceGridVec;
    std::vector<AGrid> _tempIceGridVec;
    std::vector<int> _iceSeedIdxVec;
    int _seedType;
    int _coffeeSeedIdx;
    void _Run();

public:
    AIceFiller() = default;
    // 重置冰卡
    // *** 注意：该函数需要使用在 start 函数之后才能生效
    // *** 使用示例
    // SetIceSeedList({AICE_SHROOM}) ------ 只使用原版冰
    // SetIceSeedList({AM_ICE_SHROOM, AICE_SHROOM}) ----- 优先使用模仿冰，再使用原版冰
    void SetIceSeedList(const std::vector<int>& lst);

    // 重置存冰位置
    // *** 使用示例：
    // SetList({{3,4},{5,6}})-----将存冰位置重置为{3，4}，{5，6}
    void SetList(const std::vector<AGrid>& lst) {
        _fillIceGridVec = lst;
    }

    const std::vector<AGrid>& GetList() const {
        return _fillIceGridVec;
    }

    // 删除一个或多个存冰位
    void EraseFromList(int row, int col);
    void EraseFromList(const std::vector<AGrid>& lst);

    // 将一个或多个存冰位的优先级升至最高（最先存，最后用）
    // 如果指定的存冰位不在存冰位列表中，会自动将其添加至存冰位列表
    void MoveToListTop(int row, int col);
    void MoveToListTop(const std::vector<AGrid>& lst);

    // 将一个或多个存冰位的优先级降至最低（最后存，最先用）
    // 如果指定的存冰位不在存冰位列表中，会自动将其添加至存冰位列表
    void MoveToListBottom(int row, int col);
    void MoveToListBottom(const std::vector<AGrid>& lst);

    // 设置临时存冰位
    // 临时存冰位的放置和使用优先级都高于永久存冰位置，但是临时存冰位使用一次后就会从列表中移除
    void SetTempPositions(const std::vector<AGrid>& lst) {
        _tempIceGridVec = lst;
    }

    // 设置临时存冰位
    // 临时存冰位的放置和使用优先级都高于永久存冰位置，但是临时存冰位使用一次后就会从列表中移除
    void AddTempPosition(int row, int col) {
        _tempIceGridVec.emplace_back(row, col);
    }

    // 线程开始工作
    // *** 使用示例：
    // Start({{3,4},{5,6}})-----在{3，4}，{5，6}位置存冰
    void Start(const std::vector<AGrid>& lst);

    // 使用咖啡豆函数
    // *** 使用示例：
    // Coffee()-----自动使用执行次序低的存冰位
    // Coffee(3, 1)-----使用第3行第1列的存冰，若不存在则等效于 Coffee()
    void Coffee();
    void Coffee(int row, int col);

    virtual void _EnterFight() override;
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
    void SetList(const std::vector<AGrid>& lst) {
        _gridLst = lst;
    }

    const std::vector<AGrid>& GetList() const {
        return _gridLst;
    }

    // 删除一个或多个修补位置
    void EraseFromList(int row, int col);
    void EraseFromList(const std::vector<AGrid>& lst);

    // 将一个或多个修补位置的优先级升至最高（最先修补）
    // 如果指定的修补位置不在修补位置列表中，会自动将其添加至修补位置列表
    void MoveToListTop(int row, int col);
    void MoveToListTop(const std::vector<AGrid>& lst);

    // 将一个或多个修补位置的优先级降至最低（最后修补）
    // 如果指定的修补位置不在修补位置列表中，会自动将其添加至修补位置列表
    void MoveToListBottom(int row, int col);
    void MoveToListBottom(const std::vector<AGrid>& lst);

    // 自动得到修补的位置列表
    void AutoSetList();

    // 自动得到修补的位置列表
    __ADeprecated("请使用 AutoSetList()") void AutoGetList() {
        AutoSetList();
    }

    // 线程开始工作，此函数开销较大，不建议多次调用
    // 第一个参数为植物类型
    // 第二个参数不填默认全场
    // 第三个参数不填默认植物血量为150以下时修补
    // *** 使用示例：
    // Start(AGJG_23)-------修补全场的高坚果
    // Start(APUMPKIN, {{1,3},{2,3}})-----修补位置为{1，3}，{2，3}位置的南瓜头
    // Start(AJG_3,{{1,3},{2,3}},300)------修补位置为{1，3}，{2，3}位置的坚果，血量降至300开始修补
    void Start(int plantType, const std::vector<AGrid>& lst = {}, int fixHp = 150);

    // 重置修补血量
    // *** 使用示例：
    // SetHp(200)------将修补触发血量改为200
    void SetHp(int _fix_hp) {
        _fixHp = _fix_hp;
    }

    // 是否使用咖啡豆
    void SetIsUseCoffee(bool isUseCoffee) {
        _isUseCoffee = isUseCoffee;
    }
};

#endif
