/*
 * @Author: qrmd
 * @Date: 2023-08-14 01:05:25
 * @LastEditors: qrmd
 * @LastEditTime: 2023-12-10 21:37:37
 * @Description:
 */
#ifndef __GETZOMBIEABSCISSAS_2_0__
#define __GETZOMBIEABSCISSAS_2_0__

#include "avz.h"

namespace _qrmd {

const std::vector<float> __zombie_walk_a = {
    -9.8, -8.4, -7, -5.6, -4.1, -2.7, -1.3, 0, 1.4, 2.8, 4.2, 5.7,
    7.1, 7.9, 8.8, 9.7, 10.5, 10.6, 10.8, 10.9, 11, 11, 11, 11,
    11, 13.4, 15.8, 18.1, 20.5, 22.8, 25.2, 27.6, 29.9, 31.1, 32.3, 33.5,
    34.6, 35.9, 37, 38.2, 39.4, 39.5, 39.6, 39.7, 39.8, 39.9, 40};
const std::vector<float> __zombie_walk_b = {
    -9.8, -8.5, -7.3, -6, -4.7, -3.4, -2.1, -0.9, 0.3, 1.6, 2.8, 4.1,
    5.4, 6.7, 8, 9.2, 10.5, 10.6, 10.7, 10.7, 10.8, 10.8, 10.9, 11,
    12.8, 14.5, 16.3, 18.1, 19.9, 21.6, 23.4, 25.2, 27, 28.8, 30.5, 32.3,
    34, 35.9, 37.6, 39.4, 39.5, 39.5, 39.6, 39.8, 39.9, 39.9, 40};
const std::vector<float> __zombie_dance = {
    -9.8, -9.4, -8.9, -8.4, -7.9, -7.5, -7, -6.5, -6.1, -5.6,
    -5.1, -4.7, -4.2, -3.7, -3.3, -2.8, -2.3, -1.8, -1.4, -0.9,
    -0.4, 0, 0.3, 0.8, 1.3, 1.8, 2.2, 2.6, 3.1, 3.6,
    4.1, 4.6, 5, 5.5, 6, 6.5, 6.9, 7.3, 7.8, 8.3,
    8.8, 9.3, 9.7, 10.2, 10.7, 11.1, 11.6, 12.1, 12.6, 13};
const std::vector<float> __zombie_swim = {
    -9.8, -8.4, -7, -5.6, -4.1, -2.7, -1.3, 0, 1.4, 2.8, 4.2,
    5.7, 7.1, 7.9, 8.8, 9.7, 10.5, 10.6, 10.8, 10.9, 11, 11,
    11, 11, 11, 13.4, 15.8, 18.1, 20.5, 22.8, 25.2, 27.6, 29.9,
    31.1, 32.3, 33.5, 34.6, 35.9, 37, 38.2, 39.4, 40};
const std::vector<float> __polevaulter_before_jump = {
    -59.8, -59, -58.2, -57.3, -56.5, -55.7, -54.8, -54, -53.2, -52.3,
    -51.5, -50.7, -49.8, -49, -48.2, -47.3, -46.5, -45.7, -44.8, -44,
    -43.2, -42.3, -41.5, -40.7, -39.8, -39, -38.2, -37.3, -36.5, -35.7,
    -34.8, -34, -33.2, -32.3, -31.5, -30.7, -29.8};
const std::vector<float> __polevaulter_after_jump = {
    -59.8, -59.1, -58.3, -57.6, -56.9, -55, -53, -51.1, -49.2,
    -47.2, -45.1, -43.1, -41.1, -39, -37, -35, -32.9, -30.9,
    -28.9, -27, -25, -23, -21, -19, -17, -14.7, -12.3,
    -10, -7.7, -5.8, -3.9, -2, -0.1, 1.6, 3.5, 5.4,
    7.3, 7.5, 7.8, 8, 8.2, 8.2, 8.1, 8, 8};
const std::vector<float> __newspaper_walk = {
    -59.8, -59.3, -58.8, -58.3, -57.8, -54.8, -51.8, -48.9, -45.9, -43.8,
    -41.6, -39.4, -37.3, -35.1, -33.8, -32.5, -31.2, -29.8, -28.5, -28.5,
    -28.5, -28.6, -28.6, -27.6, -26.6, -25.7, -24.7, -23.7, -21.6, -19.4,
    -17.2, -15.1, -12.9, -11.3, -9.7, -8.1, -6.5, -4.9, -4.2, -3.5,
    -2.7, -2, -2, -2, -2, -2, -2};
const std::vector<float> __football_walk = {
    -59.8, -57.4, -55, -52.6, -50.2, -47.8, -47.5, -47.3, -47, -46.7,
    -46.4, -46.1, -45.8, -44.3, -42.8, -41.3, -39.8, -38.3, -36.8, -35.3,
    -33.8, -33.5, -33.3, -33, -32.7, -32.4, -32.1, -31.8, -30.8, -29.8};
const std::vector<float> __jackson_moonwalk = {
    -9.8, -9.8, -9.8, -13.3, -16.7, -20.1, -22.2, -24.4, -26.5,
    -27.9, -29.3, -30.8, -37.5, -44.3, -51.1, -51.1, -51.1, -53.8,
    -56.9, -59.9, -62.9, -66.1, -69.3, -72.4, -78.2, -84, -89.8};
const std::vector<float> __jackson_walk = {
    -9.8, -7.4, -4.9, -2.5, -0.1, -0.1, -0.1, -0.1, -0.1, -0.1, -0.1,
    3.9, 8, 12.2, 16.3, 16.3, 16.3, 16.3, 16.3, 16.3, 18.8};
const std::vector<float> __snorkle_walk = {
    -59.8, -58.2, -56.5, -54.8, -53.2, -51.5, -49.8, -48.2, -46.5, -44.8,
    -43.2, -41.5, -39.8, -38.2, -36.5, -34.8, -33.2, -31.5, -29.8, -28.2,
    -26.5, -24.8, -23.2, -21.5, -19.8, -18.2, -16.5, -14.8, -13.2, -11.5,
    -9.8, -8.2, -6.5, -4.8, -3.2, -1.5, 0.1};
const std::vector<float> __dolphinrider_walk = {
    -59.8, -58.7, -57.6, -56.6, -55.5, -52.7, -49.9, -47.2, -45.9, -44.6,
    -43.3, -42.1, -40.9, -39.7, -38.5, -37.3, -36.1, -35, -33.8, -32.6,
    -31.2, -29.8, -28.4, -27, -25.6, -23.6, -21.5, -19.5, -17.4, -15.4,
    -13.3, -11.3, -9.2, -7.4, -5.6, -3.8, -2, -0.2, 1.5};
const std::vector<float> __jackbox_walk = {
    -49.8, -49.8, -49.2, -48.6, -47.9, -44.3, -40.7, -39.7, -38.7, -38.7,
    -37.8, -36.8, -35.8, -34, -32.1, -32.1, -32.1, -30.5, -28.9};
const std::vector<float> __balloon_walk = {
    -9.8, -8.3, -6.8, -5.4, -3.9, -2.6, -1.4, -0.1, 1, 2, 3.2,
    4.3, 5.4, 7.3, 9.2, 11.1, 13, 13, 13.1, 13.1, 13.1, 14.8,
    16.5, 18.2, 20, 21.7, 23.4, 25.1, 26.8, 27.7, 28.5, 29.3, 30.1,
    32.6, 35.1, 37.5, 40, 40, 40.1, 40.1, 40.1};
const std::vector<float> __digger_walk = {
    -59.8, -58.1, -56.4, -54.6, -52.9, -50, -47.1, -44.2, -41.3, -39.8,
    -38.3, -36.7, -35.2, -32.6, -30, -27.5, -24.9, -21.8, -18.8, -15.7,
    -12.7, -10.7, -8.8, -6.8, -4.9, -3.8, -2.8, -1.7, -0.7, 0.9,
    2.6, 4.4, 6.1, 7.8, 9.4, 11, 12.7};
const std::vector<float> __pogo_walk = {
    -9.8, -8.4, -7, -5.6, -4.1, -2.7, -1.3, 0, 1.4, 2.8, 4.2, 5.7,
    7.1, 7.9, 8.8, 9.7, 10.5, 10.6, 10.8, 10.9, 11, 11, 11, 11,
    11, 13.4, 15.8, 18.1, 20.5, 22.8, 25.2, 27.6, 29.9, 31.1, 32.3, 33.5,
    34.6, 35.9, 37, 38.2, 39.4, 39.5, 39.6, 39.7, 39.8, 39.9, 40};
const std::vector<float> __yeti_walk = {
    -103.8, -97.5, -91.2, -86.5, -81.9, -77.2, -72.6, -69.1, -65.6,
    -62.2, -58.8, -55.2, -51.6, -48, -44.4, -40.8, -37.2, -35.8,
    -34.4, -32.5, -30.6, -28.7, -26.8, -26.8, -26.8, -22.5, -18.2,
    -13.9, -9.6, -5.3, -1, 3.1, 7.5, 11.8};
const std::vector<float> __ladder_walk = {
    -39.8, -39, -38.1, -37.3, -36.5, -33.6, -30.7, -27.9, -25, -21.1,
    -17.2, -13.2, -9.3, -5.4, -4.6, -3.9, -3.1, -2.4, -1.6, -1.6,
    -1.6, -1.6, -1.6, -0.6, 0.2, 1.2, 2.1, 3, 5.2, 7.3,
    9.6, 11.7, 13.9, 15.9, 18, 20.1, 22.2, 24.2, 24.7, 25.2,
    25.6, 26.1, 26.1, 26.1, 26.1, 26.1, 26.1};
const std::vector<float> __gargantuar_walk = {
    -79.8, -75.3, -70.8, -66.3, -61.9, -57.4, -54.5, -51.5, -48.5, -45.6,
    -42.6, -38.2, -33.8, -29.4, -25, -24.6, -24.1, -23.7, -23.2, -22.8,
    -21.1, -19.4, -17.7, -16, -14.3, -11.9, -9.5, -7.1, -4.7, -2.3,
    3, 8.4, 13.8, 19.2, 24.6, 29.1, 33.5, 38, 42.5, 42.5,
    42.5, 42.5, 42.5, 42.5, 42.9, 43.3, 43.5, 43.9, 44.3};
const std::vector<float> __imp_walk = {
    -59.8, -56.5, -53.2, -49.9, -47.8, -45.7, -43.5, -40.3, -37.1, -33.9, -33,
    -32.2, -31.3, -30.4, -28.7, -26.9, -25.2, -21.3, -17.4, -13.5, -12.3, -11.2,
    -10, -7.8, -5.5, -3.3, -2.2, -1.1, -0.1, 0.9, 1.6, 2.4, 3.1};
// 返回[index]号僵尸保持当前运动模型（未再次受到减速、冰冻、定身等限制移动效果、进入啃食状态、鸭子僵尸入水、撑杆僵尸遇到植物跳跃等）移动时，未来[timeLength]厘秒内的横坐标向量，如果该僵尸是无法预测的类型或处于无法预测的状态，则返回空向量
// 注意，本函数可能需要读取僵尸的以下内存数据：
// 类型，
// 相对速度，
// 动画循环率，
// 动画起始帧，
// 横坐标，
// 冰冻状态倒计时，
// 减速状态倒计时。
// 示例：输出10号僵尸未来300厘秒内每厘秒的横坐标
// ALogger<AConsole> consoleLogger;
// std::vector<float> abscissas = {};
// abscissas = AGetZombieAbscissas(10, 300);
// if (!abscissas.empty()) {
//     for (int time = 0; time < abscissas.size(); ++time) {
//          consoleLogger.Info("#号僵尸在#厘秒后的横坐标为：#", 10, time,
//          abscissas[time]);
//     }
// } else {
//     consoleLogger.Info("#号僵尸是无法预测的类型或处于无法预测的状态", 10);
// }
inline std::vector<float> AGetZombieAbscissas(int index, int timeLength)
{
    auto zombies = AGetMainObject()->ZombieArray();
    int zombie_type = zombies[index].Type();
    float zombie_speed = zombies[index].Speed();
    float zombie_abscissa = zombies[index].Abscissa();
    int zombie_freeze_countdown = zombies[index].FreezeCountdown();
    int zombie_slow_countdown = zombies[index].SlowCountdown();
    auto animations = AGetPvzBase()->AnimationMain()->AnimationOffset()->AnimationArray();
    int index_animation = zombies[index].MRef<uint16_t>(0x118);
    int begin_frame = animations[index_animation].MRef<int>(0x18);
    std::vector<float> zombie_walk;
    std::vector<float> result;
    result.emplace_back(zombie_abscissa);
    float present_frame_movement = 0;
    bool is_uniform = false;
    std::vector<float> null;
    switch (zombie_type) {
    case AZOMBIE:
    case AFLAG_ZOMBIE:
    case ACONEHEAD_ZOMBIE:
    case ABUCKETHEAD_ZOMBIE:
    case ASCREEN_DOOR_ZOMBIE:
        switch (begin_frame) {
        case 44: // 行走，两手在前
            zombie_walk = __zombie_walk_a;
            break;
        case 91: // 行走，两手前后摆
            zombie_walk = __zombie_walk_b;
            break;
        case 250: // 水中
            zombie_walk = __zombie_swim;
        case 454: // 智慧树秘笈dance开启
            zombie_walk = __zombie_dance;
            break;
        default:
            return null;
            break;
        }
        break;
    case APOLE_VAULTING_ZOMBIE:
        switch (begin_frame) {
        case 13: // 跳跃前
            zombie_walk = __polevaulter_before_jump;
            break;
        case 93: // 跳跃后
            zombie_walk = __polevaulter_after_jump;
            break;
        default:
            return null;
            break;
        }
        break;
    case ANEWSPAPER_ZOMBIE:
        switch (begin_frame) {
        case 29: // 读报行走
        case 31: // 愤怒行走
            zombie_walk = __newspaper_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    case AFOOTBALL_ZOMBIE:
        switch (begin_frame) {
        case 21: // 行走
            zombie_walk = __football_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    case ADANCING_ZOMBIE:
        return null;
        break;
    case ABACKUP_DANCER:
        return null;
        break;
    case ASNORKEL_ZOMBIE:
        return null;
        break;
    case AZOMBONI:
        // 行走
        is_uniform = true;
        break;
    case ADOLPHIN_RIDER_ZOMBIE:
        return null;
        break;
    case AJACK_IN_THE_BOX_ZOMBIE:
        switch (begin_frame) {
        case 30: // 行走
            zombie_walk = __jackbox_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    case ABALLOON_ZOMBIE:
        switch (begin_frame) {
        case 0: // 飞行
            is_uniform = true;
            break;
        case 84: // 行走
            zombie_walk = __balloon_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    case ADIGGER_ZOMBIE:
        switch (begin_frame) {
        case 128: // 钻地
            is_uniform = true;
            break;
        case 18: // 行走
            zombie_walk = __digger_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    case APOGO_ZOMBIE:
        switch (begin_frame) {
        case 155: // 弹跳
            is_uniform = true;
            break;
        case 29: // 行走
            zombie_walk = __pogo_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    case AZOMBIE_YETI:
        switch (begin_frame) {
        case 15: // 行走
            zombie_walk = __yeti_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    case ALADDER_ZOMBIE:
        switch (begin_frame) {
        case 25:  // 有梯行走
        case 132: // 无梯行走
            zombie_walk = __ladder_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    case ACATAPULT_ZOMBIE:
        switch (begin_frame) {
        case 0: // 行走
            is_uniform = true;
            break;
        default:
            return null;
            break;
        }
        break;
    case AGARGANTUAR:
    case AGIGA_GARGANTUAR:
        switch (begin_frame) {
        case 22: // 行走
            zombie_walk = __gargantuar_walk;
            break;
        default:
            return null;
            break;
        }
        break;
    default:
        std::vector<float> null = {};
        return null;
        break;
    }
    if (is_uniform) {
        for (int time = 0; time < timeLength; ++time) {
            if (zombie_type == AZOMBONI) {
                if ((int)zombie_abscissa >= 700) {
                    present_frame_movement = 0.25;
                } else if ((int)zombie_abscissa >= 400) {
                    present_frame_movement = 0.25 - 0.0005 * (700 - (int)zombie_abscissa);
                } else {
                    present_frame_movement = 0.1;
                }
            } else {
                if (zombie_freeze_countdown > 0) {
                    present_frame_movement = 0;
                } else {
                    present_frame_movement = zombie_speed;
                    if (zombie_slow_countdown > 0) {
                        present_frame_movement = present_frame_movement * 0.4;
                    }
                }
            }
            zombie_abscissa -= present_frame_movement;
            result.emplace_back(zombie_abscissa);
            if (zombie_freeze_countdown > 0) {
                zombie_freeze_countdown--;
            }
            if (zombie_slow_countdown > 0) {
                zombie_slow_countdown--;
            }
        }
    } else {
        float total_movement = zombie_walk.back() - zombie_walk.front();
        int frame_number = zombie_walk.size();
        float frame_speed = zombie_speed * 47 * frame_number * 0.01 / total_movement;
        float circulation_rate = animations[index_animation].CirculationRate();
        float circulation_rate_change = zombie_speed * 47 * 0.01 / total_movement;
        int index_present_frame = 0;
        for (int time = 0; time < timeLength; ++time) {
            index_present_frame = (int)(circulation_rate * (frame_number - 1) + 1);
            if (zombie_freeze_countdown > 0) {
                present_frame_movement = 0;
            } else {
                present_frame_movement = frame_speed * (zombie_walk[index_present_frame] - zombie_walk[index_present_frame - 1]);
                if (zombie_slow_countdown > 0) {
                    present_frame_movement = present_frame_movement * 0.5;
                }
            }
            zombie_abscissa -= present_frame_movement;
            result.emplace_back(zombie_abscissa);
            if (zombie_freeze_countdown > 0) {
                circulation_rate += 0;
            } else if (zombie_slow_countdown > 0) {
                circulation_rate += circulation_rate_change * 0.5;
            } else {
                circulation_rate += circulation_rate_change;
            }
            circulation_rate = circulation_rate > 1 ? circulation_rate - 1 : circulation_rate;
            if (zombie_freeze_countdown > 0) {
                zombie_freeze_countdown--;
            }
            if (zombie_slow_countdown > 0) {
                zombie_slow_countdown--;
            }
        }
    }
    return result;
}
}; // namespace _qrmd

using _qrmd::AGetZombieAbscissas;
#endif