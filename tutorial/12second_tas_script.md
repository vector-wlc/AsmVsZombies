<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-19 13:22:51
 * @Description: 
-->
# 第二个键控脚本

这里使用经典四炮作为第二个键控脚本，经典四炮可以将我们刚才学习的那几个接口进行运用，这里不再提供阻塞版本

**请强烈注意，我不是键控技术党，我是键控工具党，所以示例脚本的目的是教会大家如何使用本框架的接口，** 
**脚本中的一些数值并不是科学的数值，科学的数值请询问当前植吧顶尖技术党或者查看相关的帖子和B站动态等，但由于本人早已不是键控技术党，**
**而且没有精力和兴趣去学相关的键控技术知识。**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具** 

## 连接版本 

```C++


#include <avz.h>

void AScript()
{
    ASetGameSpeed(10);
    // 这种调用方式用于挂机，但是此脚本实际上不能挂机，还是需要一些手动操作
    // 注意由于第二次进入游戏会导致核无法正常种下
    // 所以 AAssumeWavelength 会报 N 个错误
    ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);
    ASetZombies({APJ_0, ATT_4, AWW_8, AQS_11, ABC_12, AXC_15, AKG_17, AHT_14, AFT_21, ABY_23, AHY_32,ABJ_20});
    ASelectCards({AICE_SHROOM, AM_ICE_SHROOM, ACOFFEE_BEAN, ADOOM_SHROOM, ALILY_PAD, ASQUASH, ACHERRY_BOMB, ABLOVER, APUMPKIN, APUFF_SHROOM});
    AAssumeWavelength({
        ATime(1, 601),
        ATime(2, 1800),
        ATime(3, 1800),
        ATime(4, 1150),
        ATime(5, 601),
        ATime(6, 1800),
        ATime(7, 1800),
        ATime(8, 1150),
        ATime(10, 601),
        ATime(11, 1800),
        ATime(12, 1800),
        ATime(13, 1150),
        ATime(14, 601),
        ATime(15, 1800),
        ATime(16, 1800),
        ATime(17, 1150),
        ATime(18, 601),
    });

    AConnect(ATime(1, -599), [=] {
        aCobManager.SetList({{3, 1}, {4, 1}, {3, 3}, {4, 3}});
        aIceFiller.Start({{3, 5}, {1, 4}, {6, 4}, {1, 5}, {6, 5}});

        // 自动补南瓜
        aPlantFixer.Start(ANGT_30, {{3, 5}, {3, 6}, {4, 5}, {4, 6}, {1, 4}, {6, 4}});
        aPlantFixer.SetHp(4000 / 3 * 2);
    });

    for (auto wave : {1, 5, 9, 14, 18}) {
        AConnect(ATime(wave, 341 - 373), [=] {
            aCobManager.Fire({{2, 9}, {5, 9}});
        });
        if (wave == 9) {
            AConnect(ATime(wave, 341 - 373), [=] {
                aCobManager.RecoverFire({{2, 8.5}, {5, 8.5}});
            });
        } else {
            // 这就是 AAssumeWavelength 带来的好处，可以让时间点的书写小于 -200
            AConnect(ATime(wave + 1, -298 + 1) /* 1 是为了兼容栈位带来的 1cs 偏差*/, [=] {
                aIceFiller.Coffee();
            });
        }
    }

    for (auto wave : {2, 6, 11, 15, 19}) {
        AConnect(ATime(wave, 1800 - 373 - 200), [=] {
            aCobManager.Fire({{2, 8.5}, {5, 8.5}});
        });

        if (wave == 19) {
            AConnect(ATime(wave, 1800 - 373 - 200), [=] {
                aCobManager.RecoverFire({{2, 8.5}, {5, 8.5}});
            });

            // 停止自动存冰线程
            AConnect(ATime(wave, 1800 - 373 - 200 + 1000), [=] {
                aIceFiller.Pause();
            });

        } else {
            // Ice3
            AConnect(ATime(wave, 1800 - 298 - 200 + 211), [=] {
                aIceFiller.Coffee();
                ASetPlantActiveTime(AICE_SHROOM, 298);
            });
        }
    }

    for (auto wave : {3, 7, 12, 16}) {
        AConnect(ATime(wave, 1800 - 373 - 200), [=] {
            aCobManager.Fire({{2, 8.5}, {5, 8.5}});
        });
        // Ice3
        AConnect(ATime(wave, 1800 - 298 - 200 + 211), [=] {
            aIceFiller.Coffee();
            ASetPlantActiveTime(AICE_SHROOM, 298);
        });
    }

    for (auto wave : {4, 8, 13, 17}) {
        // 使用核
        // 注意这个核不一定会稳定刷新
        AConnect(ATime(wave, 1150 - 200 - 298), [=] {
            if (ARangeIn(wave, {4, 17})) {
                ACard({{ALILY_PAD, 3, 8}, {ADOOM_SHROOM, 3, 8}, {ACOFFEE_BEAN, 3, 8}});
            } else if (wave == 8) {
                ACard({{ALILY_PAD, 3, 9},
                    {ADOOM_SHROOM, 3, 9},
                    {ACOFFEE_BEAN, 3, 9},
                    {APUMPKIN, 3, 9}});
            } else {
                ACard({{ALILY_PAD, 4, 9},
                    {ADOOM_SHROOM, 4, 9},
                    {ACOFFEE_BEAN, 4, 9},
                    {APUMPKIN, 4, 9}});
            }

            // 这里因为栈位问题会带来 1cs 的偏差
            // 由于使用了假定波长函数，这里的波长必须和假定的波长严格保持一致
            // 不然会报错，因此需要使用植物精准生效函数
            ASetPlantActiveTime(ADOOM_SHROOM, 298);
        });
    }

    // wave 10
    AConnect(ATime(10, 341 - 373), [] {
        aCobManager.Fire({{2, 9}, {5, 9}});
    });

    // 种植樱桃消除延迟
    AConnect(ATime(10, 341 - 100), [] {
        ACard(ACHERRY_BOMB, 2, 9);
    });

    AConnect(ATime(10 + 1, -298 + 1) /* 1 是为了兼容栈位带来的 1cs 偏差*/, [] {
        aIceFiller.Coffee();
    });

    // wave 20
    AConnect(ATime(20, 394 + 1 - 298) /* 1 是为了兼容栈位带来的 1cs 偏差*/, [] {
        aIceFiller.Coffee(); // 冰杀小偷
    });

    AConnect(ATime(20, 1000), [] {
        aCobManager.RecoverFire({{2, 9}, {5, 9}, {2, 8.5}, {5, 8.5}});
    });
}
```

[目录](./0catalogue.md)