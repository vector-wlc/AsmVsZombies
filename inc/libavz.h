/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-14 16:51:28
 * @Description:
 */
#ifndef __LIBAVZ_H__
#define __LIBAVZ_H__

#define __AVZ_VERSION__ 240113

#include "avz_asm.h"
#include "avz_card.h"
#include "avz_click.h"
#include "avz_exception.h"
#include "avz_game_controllor.h"
#include "avz_iterator.h"
#include "avz_memory.h"
#include "avz_script.h"
#include "avz_seh.h"
#include "avz_smart.h"
#include "avz_time_queue.h"
#include "avz_connector.h"
#include "avz_coroutine.h"
#include "avz_replay.h"

// #include "avz_tick_runner.h"
// #include "avz_global.h"
// #include "avz_logger.h"
// #include "avz_pvz_struct.h"
// #include "avz_types.h"
// #include "avz_painter.h"

inline AItemCollector aItemCollector;
extern AIceFiller aIceFiller;
extern APlantFixer aPlantFixer;
extern ACobManager aCobManager;
extern AAliveFilter<AZombie> aAliveZombieFilter;
extern AAliveFilter<APlant> aAlivePlantFilter;
extern AAliveFilter<ASeed> aAliveSeedFilter;
extern AAliveFilter<APlaceItem> aAlivePlaceItemFilter;
extern APainter aPainter;

#endif