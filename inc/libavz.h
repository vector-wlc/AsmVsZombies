#ifndef __LIBAVZ_H__
#define __LIBAVZ_H__

#define __AVZ_VERSION__ 201101

#include "memory.h"
#include "pvzfunc.h"
#include "pvzstruct.h"
#include "avz_time_operation.h"
#include "avz_memory.h"
#include "avz_debug.h"
#include "avz_click.h"
#include "avz_card.h"
#include "avz_tick.h"
#include "avz_cannon.h"

// 阻塞运行直到达到目标时间点
// 使用方法与 SetTime 相同
#define SafeWaitUntil(time, wave)        \
    do                                   \
    {                                    \
        if (!AvZ::WaitUntil(time, wave)) \
        {                                \
            return;                      \
        }                                \
    } while (false)

namespace AvZ
{
    extern ItemCollector item_collector;
    extern IceFiller ice_filler;
    extern PlantFixer plant_fixer;
    extern PaoOperator pao_operator;
    extern KeyConnector key_connector;
} // namespace AvZ

#define KeyConnect AvZ::key_connector.add
#define StartMaidCheats AvZ::MaidCheats::dancing
#define StopMaidCheats AvZ::MaidCheats::stop

#endif