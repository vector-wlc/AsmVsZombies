/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-16 19:24:35
 * @Description:
 */
#include "libavz.h"

AItemCollector aItemCollector;
AIceFiller aIceFiller;
APlantFixer aPlantFixer;
ACobManager aCobManager;
AAliveFilter<AZombie> aAliveZombieFilter;
AAliveFilter<APlant> aAlivePlantFilter;
AAliveFilter<ASeed> aAliveSeedFilter;
AAliveFilter<APlaceItem> aAlivePlaceItemFilter;
APainter aPainter;