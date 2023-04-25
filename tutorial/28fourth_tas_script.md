<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2023-01-16 16:07:34
 * @Description: 
-->
# (附加) 两仪键控脚本

**请强烈注意，我不是键控技术党，我是键控工具党，所以示例脚本的目的是教会大家如何使用本框架的接口，** 
**脚本中的一些数值并不是科学的数值，科学的数值请询问当前植吧顶尖技术党或者查看相关的帖子和B站动态等，但由于本人早已不是键控技术党，**
**而且没有精力和兴趣去学相关的键控技术知识。**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具** 

这个脚本由一个源文件和两个头文件组成

## script.cpp
```C++
/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2023-01-02 10:36:03
 * @Description: 此脚本并不能保证任何成功率，并且目前不能挂机，主要演示如何使用 AvZ 编写无炮键控脚本
 */
#include "processor.h"

#if __AVZ_VERSION__ < 230501
#error "此脚本需要 AvZ2 2.3.3 230501 版本才能运行"
#endif

Processor processor;
ATickRunner tickRunner;

ACoroutine AScript()
{
    ASetZombies({ATT_4, AHY_32, ABY_23, AGL_7, ABC_12, AXC_15, AQQ_16, AQS_11, AKG_17, APJ_0, AHT_14, ABJ_20, ATL_22, AWW_8});
    ASelectCards({AHY_16, ANGT_30, AHBG_14, AM_HBG_14, AHMG_15, AYTZD_2, AHBLJ_20, AWG_17, AXPG_8, ASYC_27}, 1);
    ASetReloadMode(AReloadMode::MAIN_UI);
    ACard(AWG_17, 6, 9);
    tickRunner.Start([] {
        processor.Observe();
        processor.GenDangerGridVec();
        if (!processor.IsPaused()) {
            processor.UseSquash();
            processor.UsePuff();
            processor.UseBlover();
        }
        processor.ShovelPumpkin();
        processor.UsePumpkin();
        processor.Show();
    });

    for (int wave = 1; wave < 21; ++wave) {
        AConnect(ATime(wave, 2000), [] {
            // 使用灰烬之前暂停工作
            processor.Pause();
        });
        AConnect(ATime(wave, 900 - 320 - 100), [] {
            processor.UseIce();
        });
    }

    for (auto wave : {1, 3, 5, 7, 9, 10, 12, 14, 16, 18}) {
        AConnect(ATime(wave, 900), [] {
            processor.GenJalapenoAndCherryRow();
        });
        AConnect(ATime(wave, 2500 - 200 - 100), [] {
            processor.UseCherry();
        });
    }

    for (auto wave : {2, 4, 6, 8, 11, 13, 15, 17, 19, 20}) {
        AConnect(ATime(wave, 266), [] {
            processor.UseJalapeno();
        });
        AConnect(ATime(wave, 2500 - 200 - 100), [] {
            processor.UseDoom();
        });
    }

    AConnect(ATime(9, 2500), [] {
        processor.UseJalapeno();
        processor.UseIce();
    });
    AConnect(ATime(10, 395 - 100), [] { // 核杀小偷
        processor.UseDoom();
    });
    AConnect(ATime(12, 1000), [] { // 第十二波核的使用位置和辣椒有关
        processor.GenDoomRow();
    });
    AConnect(ATime(19, 2500), [] {
        processor.UseIce();
    });
    AConnect(ATime(20, 0), [] {
        processor.GenJalapenoAndCherryRow();
    });
    AConnect(ATime(20, 395 - 100), [] { // 樱杀小偷
        ACard(AHY_16, 3, 4);
        ACard(ACHERRY_BOMB, 3, 4);
    });
    AConnect(ATime(20, 2500), [] {
        processor.UseIce();
    });
    co_return;
}
```

## judge.h
```C++
#ifndef __JUDGE_H__
#define __JUDGE_H__

#include "libavz.h"

// 包含一些伤害判定功能
// 其中部分功能取自 https://github.com/Rottenham/avz-testing/blob/main/inc/avz_testing.h
class Judge {
public:
    // 这个函数没有计算植物的横向纵向偏移
    static bool IsBoxExplode(AZombie* zombie, APlantType plantType, const AGrid& plantGrid)
    {
        int plantX = plantGrid.col * 80 + 40;
        int plantY = plantGrid.row * 85 + 80;
        return IsBoxExplode(zombie, plantType, plantX, plantY);
    }

    static bool IsBoxExplode(AZombie* zombie, APlant* plant)
    {
        return IsBoxExplode(zombie, APlantType(plant->Type()), plant->Abscissa(), plant->Ordinate());
    }

    static bool IsBoxExplode(AZombie* zombie, APlantType plantType, int plantX, int plantY)
    {
        if (zombie->State() != 16) { // 16 是小丑的爆炸状态
            return false;
        }

        int x = zombie->Abscissa();
        int y = zombie->Ordinate();
        int yDistance = 0;
        if (y < plantY - 60) {
            yDistance = (plantY - 60) - y;
        } else if (y > plantY + 20) {
            yDistance = y - (plantY + 20);
        }
        if (yDistance > 90) {
            return false;
        }
        int xDistance = std::sqrt(90 * 90 - yDistance * yDistance);
        auto def = _GetExplodeDefenseRange(plantType);
        return plantX + def.first - xDistance <= x && x <= plantX + def.second + xDistance;
    }

    // 这个函数没有计算植物的横向偏移
    static bool IsGigaHammer(AZombie* zombie, APlantType plantType, const AGrid& plantGrid)
    {
        return IsGigaHammer(zombie, plantType, plantGrid.row, plantGrid.col * 80 + 40);
    }

    static bool IsGigaHammer(AZombie* zombie, APlant* plant)
    {
        return IsGigaHammer(zombie, APlantType(plant->Type()), plant->Row(), plant->Abscissa());
    }

    static bool IsGigaHammer(AZombie* zombie, APlantType plantType, int plantRow, int plantX)
    {
        if (!zombie->IsHammering()) {
            return false;
        }
        if (zombie->Row() != plantRow) {
            return false;
        }
        if (HammerRate(zombie) > 0) {
            return false;
        }

        std::pair<int, int> zombieAtk = {zombie->Abscissa() - 30, zombie->Abscissa() - 30 + 89};
        auto plantDef = _GetHitDefenseRange(APlantType(plantType));
        plantDef.first += plantX;
        plantDef.second += plantX;
        return std::max(zombieAtk.first, plantDef.first) <= std::min(zombieAtk.second, plantDef.second);
    }

    // 返回锤击进度
    // 如果 < 0 代表还未锤击
    // 如果 > 0 代表已经锤击
    static float HammerRate(AZombie* zombie)
    {
        auto animationCode = zombie->MRef<uint16_t>(0x118);
        auto animationArray = AGetPvzBase()
                                  ->AnimationMain()
                                  ->AnimationOffset()
                                  ->AnimationArray();
        auto circulationRate = animationArray[animationCode].CirculationRate();
        return circulationRate - HAMMER_CIRCULATION_RATE;
    }

    // 计算红眼是否可以被 IO 打死
    // 此函数模糊计算，不计算垫材对僵尸速度的影响
    static bool IsGigaIoDead(AZombie* zombie, int futureDamage = 0)
    {
        int zombieX = zombie->Abscissa();
        int rightMoveTime = 0;
        if (ARangeIn(zombie->Row(), {0, 5})) { // 红眼在边路
            rightMoveTime = _MoveTime(zombie, zombieX - _SIDE_IO_RIGHT);
            int moveTime = _MoveTime(zombie, zombieX - _SIDE_IO_LEFT) - rightMoveTime;
            return moveTime * _GLOOM_DAMAGE_PER_FRAME > zombie->Hp() - futureDamage;
        }

        rightMoveTime = _MoveTime(zombie, zombieX - _ONE_SHORE_IO_RIGHT);
        int oneIoMoveTime = _MoveTime(zombie, zombieX - _ONE_SHORE_IO_LEFT);
        int twoIoMoveTime = _MoveTime(zombie, zombieX - _TWO_SHORE_IO_LEFT);
        int totalDamage = (oneIoMoveTime - rightMoveTime) * _GLOOM_DAMAGE_PER_FRAME
            + (twoIoMoveTime - oneIoMoveTime) * _GLOOM_DAMAGE_PER_FRAME * 2;
        return totalDamage > zombie->Hp() - futureDamage;
    }

    static bool IsCherryExplode(AZombie* zombie, int cherryCol)
    {
        return IsCherryExplode(zombie->Abscissa(), cherryCol);
    }

    static bool IsCherryExplode(float zombieX, int cherryCol)
    {
        int leftX = _6_COL_CHERRY_LEFT + (cherryCol - 5) * 80;
        int rightX = _6_COL_CHERRY_RIGHT + (cherryCol - 5) * 80;
        return zombieX >= leftX && zombieX <= rightX;
    }

    static bool IsWillBeCrushed(AZombie* zombie, int plantRow, int plantCol)
    {
        int zombieX = zombie->Abscissa();
        int zombieRow = zombie->Row();
        //  这里加 40 是为了防止植物放下去什么都没做然后过几帧被冰车直接碾压
        return zombieRow == plantRow && zombieX < plantCol * 80 + 40;
    }

protected:
    static std::pair<int, int> _GetExplodeDefenseRange(APlantType type)
    {
        switch (type) {
        case ATALL_NUT:
            return {-50, 30};
        case APUMPKIN:
            return {-60, 40};
        case ACOB_CANNON:
            return {-60, 80};
        default:
            return {-50, 10};
        }
    }

    static std::pair<int, int> _GetHitDefenseRange(APlantType type)
    {
        switch (type) {
        case ATALL_NUT:
            return {30, 60};
        case APUMPKIN:
            return {20, 80};
        case ACOB_CANNON:
            return {20, 120};
        default:
            return {30, 50};
        }
    }

    // 注意这里稍稍违反了游戏规则
    // 游戏中僵尸的冰冻倒计时是个随机数
    // 无法精准的知道冰冻倒计时
    // 但这里为了简化逻辑直接读取了冰冻倒计时
    static int _MoveTime(AZombie* zombie, float distance)
    {
        if (distance < 0) {
            return 0;
        }
        int freezeCountdown = zombie->FreezeCountdown();
        int slowCountdown = zombie->SlowCountdown() - freezeCountdown;
        float slowMaxDistance = slowCountdown * _GIGA_AVG_SPEED * 2;
        int normalDistance = distance - slowMaxDistance;
        if (normalDistance < 0) {
            return int(distance / _GIGA_AVG_SPEED * 2 + freezeCountdown);
        } else {
            return int(normalDistance / _GIGA_AVG_SPEED) + freezeCountdown + slowCountdown;
        }
    }

    // 原速红眼平均移动速度 (px/cs)
    // 1.25 是我瞎调的参数
    constexpr static float _GIGA_AVG_SPEED = float(484) / 3158 * 1.25;

    // 曾的每帧伤害 (hp/cs)
    constexpr static float _GLOOM_DAMAGE_PER_FRAME = float(80) / 200;

    // 边路 IO 左界限
    constexpr static int _SIDE_IO_LEFT = -67;

    // 边路 IO 右界限
    constexpr static int _SIDE_IO_RIGHT = 296;

    // 一曾岸路 IO 右界限
    constexpr static int _ONE_SHORE_IO_RIGHT = 616;

    // 一曾岸路 IO 左界限
    constexpr static int _ONE_SHORE_IO_LEFT = 296;

    // 二曾岸路伤害范围
    // 这里实际上还有一小段没算, 即 231 - 253, 可以计算红眼在这一段时间内只会受到 1 到 2 滴血, 因此就不计算了
    constexpr static int _TWO_SHORE_IO_LEFT = 253;

    // 边路防御底线
    constexpr static int _DEFENSE_SIDE_ROAD = -67;

    // 岸路防御底线
    constexpr static int _DEFENSE_SHORE_ROAD = 231;

    // 六列樱桃爆炸左界限
    // 这里用的范围比较小
    // +10 是因为樱桃有 1s 的生效延迟
    constexpr static int _6_COL_CHERRY_LEFT = 264 + 10;

    // 六列樱桃爆炸右界限
    constexpr static int _6_COL_CHERRY_RIGHT = 612;

    // 巨人僵尸锤击命中时的动画循环率
    constexpr static float HAMMER_CIRCULATION_RATE = 0.644;
};

#endif
```

## processor.h

```C++
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
                ALimitValue(col, 0, 8);
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
    ACoroutine UseJalapeno()
    {
        // co_await bool Functor() 用法
        // 只要 Functor() 不 return true, Functor 就会一直被执行
        // 并且此协程会一直被阻塞
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
    }

    // 使用樱桃
    ACoroutine UseCherry()
    {
        // co_await bool Functor() 用法
        // 只要 Functor() 不 return true, Functor 就会一直被执行
        // 并且此协程会一直被阻塞
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
    }

    // 使用核
    ACoroutine UseDoom()
    {
        static auto iter = _doomGridVec.begin();
        if (AGetMainObject()->Wave() == 2) {
            iter = _doomGridVec.begin();
        }
        co_await [this] {
            return _doomSeed->IsUsable() && _lilySeed->IsUsable() && //
                AAsm::GetPlantRejectType(AHY_16, iter->row - 1, iter->col - 1) == AAsm::NIL;
        };
        ACard(ALILY_PAD, iter->row, iter->col);
        _PauseCard(ADOOM_SHROOM, iter->row - 1, iter->col - 1);
        ++iter;
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
    ACoroutine _UseBlueIce()
    {
        co_await [this] {
            if (!_blueIceSeed->IsUsable() || _bloverActiveCd > 0) {
                return false;
            }
            Observe();
            auto grid = _GenTemporaryGrid();
            if (grid.row < 0 || !_IsFodderPlantable(grid.row, grid.col)) {
                grid = _GenPlantableGrid();
            }

            aPainter.Draw(AText("使用蓝冰", 0, 20), 500);
            _PauseCard(AICE_SHROOM, grid.row, grid.col);
            return true;
        };
    }

    // 使用白冰
    ACoroutine _UseWhiteIce()
    {
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
            aPainter.Draw(AText("使用白冰", 0, 20), 500);
            ACard(AM_ICE_SHROOM, _whiteIceGrid.row + 1, _whiteIceGrid.col + 1);
            return true;
        };
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
```

[目录](./0catalogue.md)