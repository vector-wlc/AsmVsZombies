#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include "judge.h"
#include <algorithm>
#include <array>

// 各种情况的应对处理
class Processor : public AStateHook {
public:
    // 使用一个一维数组代替二维数组
    // 不然内存管理比较麻烦
    template <typename T, size_t colMax>
    class Vector2d : public std::vector<T> {
    public:
        T& At2d(int row, int col)
        {
            return (*this)[row * colMax + col];
        }
    };

    // 观测场上的僵尸情况
    void Observe()
    {
        if (_bloverActiveCd > 0) {
            --_bloverActiveCd;
        }
        static int lock = -1;
        // 一帧只观测一次
        if (lock == AGetMainObject()->GameClock()) {
            return;
        }
        lock = AGetMainObject()->GameClock();
        _gigaVec.clear();
        _gigaVec.resize(6 * 9);
        _boxZombieVec.clear();
        _zomboniVec.clear();
        _minBalloonX = 800;
        for (auto&& zombie : aAliveZombieFilter) {
            switch (AZombieType(zombie.Type())) {
            case AHY_32: {
                // 这里 -11 是为了垫材好放置
                // 红眼最左垫材锤击第 x 格垫材满足以下公式
                // x = (红眼横坐标 - 11) / 80 + 1
                int col = (zombie.Abscissa() - 11) / 80;
                col = std::clamp(col, 0, 8);
                _gigaVec.At2d(zombie.Row(), col).push_back(&zombie);
                break;
            }
            case AQQ_16: {
                _minBalloonX = std::min(_minBalloonX, zombie.Abscissa());
                break;
            }
            case AXC_15: {
                _boxZombieVec.push_back(&zombie);
                break;
            }
            case ABC_12: {
                _zomboniVec.push_back(&zombie);
                break;
            }
            default: {
                break;
            }
            }
        }
    }

    // 生成有破阵危险的位置列表
    void GenDangerGridVec()
    {
        static int lock = -1;
        // 一帧只观测一次
        if (lock == AGetMainObject()->GameClock()) {
            return;
        }
        lock = AGetMainObject()->GameClock();
        _dangerGridVec.clear();
        // 第一步：生成最靠左的红眼的位置列表

        // 这里其实开四个内存就够了，开六个内存是为了写的更方便
        // 120 是南瓜头的坐标， 30 是红眼左攻击偏移，80 是南瓜头右防御偏移
        // 40 是我瞎调的
        static std::array<int, 6> leftX = {40, 120 + 30 + 80, -1000, -1000, 120 + 30 + 80, 40};
        _minGigaX = {800, 800, 800, 800, 800, 800};

        for (auto row : {0, 1, 4, 5}) {
            auto futureDamage = row == _jalapenoRow ? 1800 : 0;
            for (int col = 0; col < 7; ++col) {
                auto&& gigaPtrVec = _gigaVec.At2d(row, col);
                if (gigaPtrVec.empty()) {
                    continue;
                }
                for (auto gigaPtr : gigaPtrVec) {
                    if (Judge::IsGigaIoDead(gigaPtr, futureDamage)) {
                        continue;
                    }
                    _minGigaX[row] = std::min(_minGigaX[row], gigaPtr->Abscissa() - leftX[row]);
                }
                if (_minGigaX[row] < 800) {
                    _dangerGridVec.push_back({row, col});
                    break;
                }
            }
        }

        // 第二步：根据坐标对每行的位置进行排序
        std::sort(_dangerGridVec.begin(), _dangerGridVec.end(),
            [this](const AGrid& lhs, const AGrid& rhs) {
                return _minGigaX[lhs.row] < _minGigaX[rhs.row];
            });
    }

    // 使用小喷菇
    void UsePuff()
    {
        if (!_puffSeed->IsUsable()) {
            return;
        }
        auto grid = _GenFodderGrid();
        if (grid.row < 0) {
            return;
        }
        if (!_IsFodderPlantable(grid.row, grid.col)) {
            return;
        }
        ACard(AXPG_8, grid.row + 1, grid.col + 1);
    }

    // 使用三叶草
    void UseBlover()
    {
        if (!_bloverSeed->IsUsable()) {
            return;
        }

        // 如果当前气球僵尸太靠近家了，无条件立即使用三叶草
        if (_minBalloonX < 50) {
            auto grid = _GenTemporaryGrid();
            if (grid.row < 0) {
                grid = _GenPlantableGrid();
            }
            _bloverActiveCd = 51;
            _PauseCard(ASYC_27, grid.row, grid.col, 51);
            return;
        }

        // 如果当前岸路压力太大，三叶草临时当作垫材使用
        for (auto row : {1, 4}) {
            if (_minGigaX[row] < 20 && _IsPlantable(row, 2, true)) {
                if (!_IsFodderPlantable(row, 2)) {
                    return;
                }
                _bloverActiveCd = 51;
                _PauseCard(ASYC_27, row, 2, 51);
                return;
            }
        }

        // 如果气球僵尸离家很远，不需要使用三叶草
        if (_minBalloonX > 150) {
            return;
        }

        auto grid = _GenTemporaryGrid();
        if (grid.row < 0) {
            return;
        }
        if (!_IsFodderPlantable(grid.row, grid.col)) {
            return;
        }
        // 遍历当前格子的所有红眼
        // 如果全部红眼都在冻住状态，则不种植三叶草
        bool isFreeze = true;
        for (auto zombiePtr : _gigaVec.At2d(grid.row, grid.col)) {
            if (zombiePtr->FreezeCountdown() == 0) {
                isFreeze = false;
                break;
            }
        }

        if (isFreeze) {
            return;
        }
        _bloverActiveCd = 51;
        _PauseCard(ASYC_27, grid.row, grid.col, 51);
    }

    // 使用倭瓜
    void UseSquash()
    {
        if (!_squashSeed->IsUsable()) {
            return;
        }
        auto grid = _GenSquashGrid();
        if (grid.row < 0) {
            return;
        }
        if (!_IsAshPlantable(grid.row, grid.col)) {
            return;
        }
        _PauseCard(AWG_17, grid.row, grid.col);
    }

    // 使用辣椒
    void UseJalapeno()
    {
        // co_await bool Functor() 用法
        // 只要 Functor() 不 return true, Functor 就会一直被执行
        // 并且此协程会一直被阻塞
        ACoLaunch([this]() -> ACoroutine {
            co_await [this] {
                if (!_jalapenoSeed->IsUsable() || _bloverActiveCd > 0) {
                    return false;
                }
                Observe();
                auto grid = _GenJalapenoGrid();
                if (!_IsAshPlantable(grid.row, grid.col)) {
                    return false;
                }
                _PauseCard(AJALAPENO, grid.row, grid.col);
                _jalapenoRow = -1;
                return true;
            };
        });
    }

    // 使用樱桃
    void UseCherry()
    {
        // co_await bool Functor() 用法
        // 只要 Functor() 不 return true, Functor 就会一直被执行
        // 并且此协程会一直被阻塞
        ACoLaunch([this]() -> ACoroutine {
            co_await [this] {
                if (!_cherrySeed->IsUsable() || _bloverActiveCd > 0) {
                    return false;
                }
                Observe();
                auto grid = _GenCherryGrid();
                if (!_IsAshPlantable(grid.row, grid.col)) {
                    return false;
                }
                _otherPainter.Draw(AText("樱桃种植在:" + std::to_string(grid.row + 1) + "行" + std::to_string(grid.col + 1) + "列", 300, 0), 1200);
                _PauseCard(ACHERRY_BOMB, grid.row, grid.col);
                return true;
            };
        });
    }

    // 使用核
    void UseDoom()
    {
        static auto iter = _doomGridVec.begin();
        if (AGetMainObject()->Wave() == 2) {
            iter = _doomGridVec.begin();
        }
        ACoLaunch([this]() -> ACoroutine {
            co_await [this] {
                return _doomSeed->IsUsable() && _lilySeed->IsUsable() && //
                    AAsm::GetPlantRejectType(AHY_16, iter->row - 1, iter->col - 1) == AAsm::NIL;
            };
            ACard(ALILY_PAD, iter->row, iter->col);
            _PauseCard(ADOOM_SHROOM, iter->row - 1, iter->col - 1);
            ++iter;
        });
    }

    // 使用冰
    void UseIce()
    {
        static bool isUseBlue = false;
        if (AGetMainObject()->Wave() <= 1) {
            isUseBlue = false;
        }
        if (isUseBlue) {
            AConnect(ANowDelayTime(320), [this] { _UseBlueIce(); });
        } else {
            _UseWhiteIce();
        }
        isUseBlue = !isUseBlue;
    }

    // 使用南瓜头
    void UsePumpkin()
    {
        // 这里不可能溢出
        if (--_plantPumpkinCd > 0) {
            return;
        }
        if (!_pumpkinSeed->IsUsable()) {
            return;
        }
        // 包含种南瓜和铲南瓜
        static std::vector<AGrid> gridList = {{2, 1}, {2, 2}, {3, 6}, {4, 6}, {5, 1}, {5, 2}};
        static std::vector<AGrid> gigaGridList = {{1, 2}, {4, 2}};
        auto pumpkinVec = AGetPlantIndices(gridList, APUMPKIN);
        auto plantArray = AGetMainObject()->PlantArray();
        constexpr int MAX_FIX_HP = 3000 * 2 / 3;
        int minHp = MAX_FIX_HP;
        auto minGrid = gridList.front();
        for (int idx = 0; idx < gridList.size(); ++idx) {
            if (pumpkinVec[idx] < 0) { // 南瓜消失了
                ACard(APUMPKIN, gridList[idx].row, gridList[idx].col);
                return;
            }
            // 对曾的南瓜施加一个血量偏移
            int hp = plantArray[pumpkinVec[idx]].Hp();
            if (hp > MAX_FIX_HP) {
                continue;
            }
            // 这里的 800 是凭感觉设置的
            int hpOffset = (ARangeIn(gridList[idx].row, {2, 5}) && gridList[idx].col == 2) ? 800 : 0;
            auto currentHp = plantArray[pumpkinVec[idx]].Hp() - hpOffset;
            if (currentHp < minHp) {
                minHp = currentHp;
                minGrid = gridList[idx];
            }
        }

        if (minHp < MAX_FIX_HP) {
            ACard(APUMPKIN, minGrid.row, minGrid.col);
        }
    }

    // 铲除南瓜头
    void ShovelPumpkin()
    {
        // 只需要扫描第三列的红眼即可
        static std::vector<AGrid> gigaGridList = {{2, 3}, {5, 3}};
        for (auto&& gigaGrid : gigaGridList) {
            int x, y;
            AGridToCoordinate(gigaGrid.row, gigaGrid.col - 1, x, y);
            for (auto gigaPtr : _gigaVec.At2d(gigaGrid.row - 1, gigaGrid.col - 1)) {
                if (!Judge::IsGigaHammer(gigaPtr, APUMPKIN, {gigaGrid.row - 1, gigaGrid.col - 2})) {
                    continue;
                }

                if (Judge::HammerRate(gigaPtr) < -0.01) {
                    continue;
                }
                // 即将锤扁南瓜头但是仍需判断栈位大小
                auto fodderIdx = AGetPlantIndex(gigaGrid.row, gigaGrid.col);
                auto pumpkinIdx = AGetPlantIndex(gigaGrid.row, gigaGrid.col - 1, APUMPKIN);
                if (fodderIdx >= 0 && fodderIdx < pumpkinIdx) {
                    // 虽然巨人马上就要锤扁植物，但是垫材的栈位小于南瓜头，南瓜头没事
                    continue;
                }
                if (pumpkinIdx < 0) { // 没有套不用铲
                    continue;
                }

                // 没招了，只能铲套
                AShovel(gigaGrid.row, gigaGrid.col - 1, true);
                _plantPumpkinCd = 30;
                break;
            }
        }
    }

    void GenJalapenoAndCherryRow()
    {
        if (AGetMainObject()->Wave() == 1) {
            _jalapenoRow = 0;
            return;
        }
        _jalapenoRow = -1;
        Observe();
        GenDangerGridVec();

        if (_dangerGridVec.empty()) {
            _jalapenoRow = 0;
            _cherryRow = 4;
            return;
        }

        // 查看第六列樱桃是否能炸到
        // 第六列樱桃能够炸到第五列以右的红眼
        // 所以直接查看第四列以左有没有红眼
        for (auto&& dangerGrid : _dangerGridVec) {
            if (dangerGrid.row == 1 || dangerGrid.row == 4) {
                // 判断岸路是否快要砸曾，如果要砸曾，要用樱桃炸
                if (_minGigaX[dangerGrid.row] < 80) {
                    _jalapenoRow = dangerGrid.row == 1 ? 5 : 0;
                    _cherryRow = _jalapenoRow == 0 ? 4 : 0;
                    return;
                }
            } else if (dangerGrid.col < 4) { // 边路
                _jalapenoRow = dangerGrid.row;
                _cherryRow = _jalapenoRow == 0 ? 4 : 0;
                return;
            }
        }

        int topGigaCnt = 0;
        int bottomGigaCnt = 0;
        // 边路第四列以左没有红眼，则查看哪边樱桃炸的多
        int leftColOffset = _dangerGridVec.front().col;
        for (int col = 0; col < 5; ++col) {
            for (int row = 0; row < 2; ++row) {
                topGigaCnt += _gigaVec.At2d(row, col + leftColOffset).size();
            }

            for (int row = 4; row < 6; ++row) {
                bottomGigaCnt += _gigaVec.At2d(row, col + leftColOffset).size();
            }
        }

        _jalapenoRow = topGigaCnt > bottomGigaCnt ? 5 : 0;
        _cherryRow = _jalapenoRow == 0 ? 4 : 0;
    }

    void GenDoomRow()
    {
        _doomGridVec[6].row = _jalapenoRow == 0 ? 4 : 3;
    }

    bool IsPaused() const
    {
        return _isPaused;
    }

    void Pause() { _isPaused = true; }

    // 显示一些信息
    void Show()
    {
        // 显示是否工作
        _otherPainter.Draw(AText(std::string("人工智障 : ") + (_isPaused ? "暂停" : "工作"), 0, 0));

        // 显示各个位置的红眼僵尸数量
        for (int row : {0, 1, 4, 5}) {
            for (int col = 0; col < 9; ++col) {
                if (_gigaVec.At2d(row, col).empty()) {
                    continue;
                }
                int x;
                int y;
                AGridToCoordinate(row + 1, col + 1, x, y);
                _otherPainter.Draw(AText("数量:" + std::to_string(_gigaVec.At2d(row, col).size()), x, y));
            }
        }

        // 显示最具威胁的红眼的位置
        for (int idx = 0; idx < _dangerGridVec.size(); ++idx) {
            int x;
            int y;
            AGridToCoordinate(_dangerGridVec[idx].row + 1, _dangerGridVec[idx].col + 1, x, y);
            y -= 20;
            std::string text = "危险 "
                + std::to_string(idx + 1)
                + " "
                + std::to_string(int(_minGigaX[_dangerGridVec[idx].row]));
            _dangerPainter.Draw(AText(text, x, y));
        }

        // 显示使用火爆辣椒的位置
        if (_jalapenoRow >= 0) {
            int x;
            int y;
            AGridToCoordinate(_jalapenoRow + 1, 1, x, y);
            y += 20;
            _jalapenoPainter.Draw(AText("火爆辣椒预备", x, y));
            _jalapenoPainter.Draw(ARect(x + 120, y, 80 * 8, 10));
        }
    }

protected:
    virtual void _EnterFight() override
    {
        _gigaVec.clear();
        _gigaVec.resize(6 * 9);
        _boxZombieVec.clear();
        _dangerGridVec.clear();
        _jalapenoPainter.SetTextColor(AArgb(0xff, 0xff, 0xff, 0));
        _jalapenoPainter.SetRectColor(AArgb(0x7f, 0xff, 0, 0));
        _dangerPainter.SetTextColor(AArgb(0xff, 0xff, 0xff, 0));

        _jalapenoRow = -1;
        _minBalloonX = 800;
        _whiteIceGrid = {4 - 1, 4 - 1};
        _isPaused = false;
        _plantPumpkinCd = 0;

        _lilySeed = _GetSeedPtr(ALILY_PAD);
        _pumpkinSeed = _GetSeedPtr(APUMPKIN);
        _blueIceSeed = _GetSeedPtr(AICE_SHROOM);
        _whiteIceSeed = _GetSeedPtr(AM_ICE_SHROOM);
        _doomSeed = _GetSeedPtr(ADOOM_SHROOM);
        _cherrySeed = _GetSeedPtr(ACHERRY_BOMB);
        _jalapenoSeed = _GetSeedPtr(AJALAPENO);
        _squashSeed = _GetSeedPtr(ASQUASH);
        _puffSeed = _GetSeedPtr(APUFF_SHROOM);
        _bloverSeed = _GetSeedPtr(ABLOVER);
    }

    // 使用后让 Processor 暂停工作的卡片
    void _PauseCard(APlantType type, int row, float col, int pauseTime = 101)
    {
        if (row != 2 && row != 3) { // 不在水路直接铲
            AShovel(row + 1, col + 1);
        }
        // 做最后一个是否能够种植的检查
        if (AAsm::GetPlantRejectType(type, row, int(col + 0.5)) != AAsm::NIL) {
            return;
        }
        _isPaused = true;
        ACard(type, row + 1, col + 1);
        AConnect(ANowDelayTime(pauseTime), [this] {
            _isPaused = false;
        });
    }

    // 生成永久垫材的位置
    // 包括: 小喷菇
    AGrid _GenFodderGrid()
    {
        // 遍历危险列表
        // 如果目前危险位置不存在垫材且能种植则直接种植
        // 如果不能种植，则向后面扫描，如果一直不能种植就放弃此行
        // 遍历下一个位置
        for (auto dangerGrid : _dangerGridVec) {
            int minCol = ARangeIn(dangerGrid.row, {2 - 1, 4 - 1}) ? 3 - 1 : 1 - 1;
            do {
                if (_IsPlantable(dangerGrid.row, dangerGrid.col, true)) {
                    return dangerGrid;
                }
            } while (--dangerGrid.col > minCol); // 要种植的位置已经太靠左了，放弃种植
        }

        // 如果没有危险位置，就在下面四个位置中依次种植。
        static std::vector<AGrid> gridList = {{1, 3}, {4, 3}, {1, 2}, {4, 2}};
        for (auto&& grid : gridList) {
            if (_IsPlantable(grid.row, grid.col, true)) {
                return grid;
            }
        }
        // 生成失败
        return {-1, -1};
    }

    // 生成临时垫材的位置
    // 包括: 蓝冰，三叶草
    AGrid _GenTemporaryGrid()
    {
        GenDangerGridVec();
        // 遍历危险列表
        // 如果目前危险位置不存在垫材且能种植则直接种植
        // 如果不能种植，则遍历下一个位置
        for (auto&& dangerGrid : _dangerGridVec) {
            if (_IsPlantable(dangerGrid.row, dangerGrid.col, true)) {
                return dangerGrid;
            }
        }
        return {-1, -1};
    }

    // 生成火爆辣椒的位置
    AGrid _GenJalapenoGrid()
    {
        for (int col = 0; col < 9; ++col) {
            auto&& gigaPtrVec = _gigaVec.At2d(_jalapenoRow, col);
            if (gigaPtrVec.empty()) {
                continue;
            }
            for (auto gigaPtr : gigaPtrVec) {
                if (gigaPtr->Hp() < 1800) {
                    continue;
                }

                if (_IsPlantable(_jalapenoRow, col, false)) {
                    return {_jalapenoRow, col};
                }
            }
        }
        return {_jalapenoRow, 0};
    }

    // 生成樱桃的位置
    AGrid _GenCherryGrid()
    {
        if (AGetMainObject()->Wave() == 1) {
            return {5, 6};
        }
        std::vector<int> scanRowVec = {0, 1};
        if (_cherryRow == 4) {
            scanRowVec = {4, 5};
        }

        // 第一步: 找到三列以右最左的红眼
        float minX = 800;
        for (int scanRow : scanRowVec) {
            for (int scanCol = 2; scanCol < 4; ++scanCol) {
                for (auto gigaPtr : _gigaVec.At2d(scanRow, scanCol)) {
                    if (!Judge::IsGigaIoDead(gigaPtr)) {
                        minX = std::min(minX, gigaPtr->Abscissa());
                    }
                }
            }
        }

        // 从右向左扫描
        int cherryCol = 5;
        if (minX < 612) {
            for (; cherryCol > 2; --cherryCol) {
                if (Judge::IsCherryExplode(minX, cherryCol)) {
                    break;
                }
            }
        }

        // 选出一个最右的位置种植樱桃
        for (; cherryCol > 2; --cherryCol) {
            for (int rowOffset = 0; rowOffset < 2; ++rowOffset) {
                if (_IsPlantable(_cherryRow + rowOffset, cherryCol, false)) {
                    return {_cherryRow + rowOffset, cherryCol};
                }
            }
        }

        return {0, 0}; // 生成失败
    }

    // 生成倭瓜的位置
    AGrid _GenSquashGrid()
    {
        // 如果辣椒的行数已经确定，倭瓜则优先砸辣椒的旁路
        if (_jalapenoRow >= 0) {
            int squashRow = _jalapenoRow == 0 ? 1 : 4;
            int minX = _minGigaX[squashRow];
            if (minX < 160) {
                // 小于 2 格，比较危险了
                return {squashRow, (minX + 230 - 11) / 80};
            }
        }

        // 遍历危险列表
        // 如果目前危险位置不存在垫材且能种植则直接种植
        // 如果不能种植，则遍历下一个位置
        for (auto&& dangerGrid : _dangerGridVec) {
            if (dangerGrid.col < 2 && ARangeIn(dangerGrid.row, {1, 4})) { // 岸路小于 3 列
                return {dangerGrid.row, 2};
            }
            if (_IsPlantable(dangerGrid.row, dangerGrid.col, false)) {
                return dangerGrid;
            }
        }
        return {-1, -1};
    }

    // 生成一个能够种植的格子
    AGrid _GenPlantableGrid()
    {
        // 暴力遍历每一个格子
        for (int row = 0; row < 6; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (AAsm::GetPlantRejectType(AXPG_8, row, col) == AAsm::NIL) {
                    return {row, col};
                }
            }
        }
        return {0, 0};
    }

    // 下面两个函数做短暂的危险检查
    bool _IsFodderPlantable(int row, int col)
    {
        // 种植的是垫材，检查是否有丑爆和巨人锤击
        if (!_IsAshPlantable(row, col)) {
            return false;
        }
        for (auto gigaPtr : _gigaVec.At2d(row, col)) {
            if (Judge::IsGigaHammer(gigaPtr, AXPG_8, {row, col})) {
                return false;
            }
        }
        return true;
    }

    bool _IsAshPlantable(int row, int col)
    {
        // 种植的是灰烬，检查是否有丑爆
        for (auto boxPtr : _boxZombieVec) {
            if (Judge::IsBoxExplode(boxPtr, ASYC_27, {row, col})) {
                return false;
            }
        }
        return true;
    }

    // 此函数只做最基础的检查
    // 通过此函数的检查只能证明这个格子确实可以种植物
    // 但是种完可能直接就死了
    bool _IsPlantable(int row, int col, bool isFodder)
    {
        bool isPlantable = AAsm::GetPlantRejectType(ASYC_27, row, col) == AAsm::NIL;
        if (!isFodder) {
            return isPlantable || AGetPlantIndex(row + 1, col + 1, AXPG_8) >= 0;
        }

        if (!isPlantable) {
            return false;
        }

        // 如果种植的是垫材，还需要检查冰车
        for (auto zomboniPtr : _zomboniVec) {
            if (Judge::IsWillBeCrushed(zomboniPtr, row, col)) {
                return false;
            }
        }
        return true;
    }

    ASeed* _GetSeedPtr(APlantType type)
    {
        return AGetMainObject()->SeedArray() + AGetCardIndex(type);
    }

    // 使用蓝冰
    void _UseBlueIce()
    {
        ACoLaunch([this]() -> ACoroutine {
            AGrid grid;
            co_await [this, &grid] {
                if (!_blueIceSeed->IsUsable() || _bloverActiveCd > 0) {
                    return false;
                }
                Observe();
                grid = _GenTemporaryGrid();
                if (grid.row < 0 || !_IsFodderPlantable(grid.row, grid.col)) {
                    grid = _GenPlantableGrid();
                }
                return true;
            };
            aPainter.Draw(AText("使用蓝冰", 0, 20), 500);
            _PauseCard(AICE_SHROOM, grid.row, grid.col);
        });
    }

    // 使用白冰
    void _UseWhiteIce()
    {
        ACoLaunch([this]() -> ACoroutine {
            co_await [this] {
                if (!_whiteIceSeed->IsUsable() || _bloverActiveCd > 0) {
                    return false;
                }
                if (AAsm::GetPlantRejectType(AHBG_14, _whiteIceGrid.row, _whiteIceGrid.col) != AAsm::NIL) {
                    // 没有荷叶
                    if (!_lilySeed->IsUsable()) {
                        return false;
                    }
                    ACard(AHY_16, _whiteIceGrid.row + 1, _whiteIceGrid.col + 1);
                }
                return true;
            };

            aPainter.Draw(AText("使用白冰", 0, 20), 500);
            ACard(AM_ICE_SHROOM, _whiteIceGrid.row + 1, _whiteIceGrid.col + 1);
        });
    }

    // 存储红眼僵尸的分布情况
    Vector2d<std::vector<AZombie*>, 9> _gigaVec;
    std::vector<AZombie*> _boxZombieVec;
    std::vector<AZombie*> _zomboniVec;
    std::vector<AGrid> _dangerGridVec;
    std::array<float, 6> _minGigaX;

    // 控制是否工作
    bool _isPaused;

    // 火爆辣椒的种植行数
    int _jalapenoRow;

    // 樱桃炸弹的种植行数
    int _cherryRow;

    // 气球僵尸的最小横坐标
    float _minBalloonX;

    // 种植南瓜倒计时
    int _plantPumpkinCd;

    // 白冰的使用位置
    AGrid _whiteIceGrid;

    // 三叶草生效倒计时
    int _bloverActiveCd;

    // 核的使用位置
    std::vector<AGrid> _doomGridVec = {
        {4, 8}, // wave 2
        {3, 7}, // wave 4
        {4, 7}, // wave 6
        {3, 5}, // wave 8
        {3, 4}, // wave 10
        {3, 7}, // wave 11
        {4, 8}, // wave 13 这波待定
        {4, 7}, // wave 15
        {3, 5}, // wave 17
        {4, 5}, // wave 19
        {3, 7}, // wave 20 这里这个核坑不好
    };

    // 各个卡片的内存指针
    ASeed* _lilySeed;
    ASeed* _pumpkinSeed;
    ASeed* _blueIceSeed;
    ASeed* _whiteIceSeed;
    ASeed* _doomSeed;
    ASeed* _cherrySeed;
    ASeed* _jalapenoSeed;
    ASeed* _squashSeed;
    ASeed* _puffSeed;
    ASeed* _bloverSeed;

    APainter _dangerPainter;
    APainter _jalapenoPainter;
    APainter _otherPainter;
};

#endif