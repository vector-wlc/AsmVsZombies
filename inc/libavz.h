/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2020-10-30 21:03:04
 * @Description: 
 */
#ifndef __LIBAVZ_H__
#define __LIBAVZ_H__

#define __AVZ_VERSION__ 211212

#include "avz_cannon.h"
#include "avz_card.h"
#include "avz_click.h"
#include "avz_debug.h"
#include "avz_memory.h"
#include "avz_tick.h"
#include "avz_time_operation.h"
#include "memory.h"
#include "pvzfunc.h"
#include "pvzstruct.h"

namespace AvZ {
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