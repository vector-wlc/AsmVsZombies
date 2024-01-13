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

void AScript()
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
}