/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2023-01-08 10:00:46
 * @Description:
 */
#ifndef __JUDGE_H__
#define __JUDGE_H__

#include <avz.h>

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