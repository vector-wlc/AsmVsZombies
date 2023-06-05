<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-19 15:46:28
 * @Description: 
-->
# 第三个键控脚本

这里使用天台十炮作为第三个键控脚本，天台十炮可以让我们体会到多炮列表的作用

**请强烈注意，我不是键控技术党，我是键控工具党，所以示例脚本的目的是教会大家如何使用本框架的接口，** 
**脚本中的一些数值并不是科学的数值，科学的数值请询问当前植吧顶尖技术党或者查看相关的帖子和B站动态等，但由于本人早已不是键控技术党，**
**而且没有精力和兴趣去学相关的键控技术知识。**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

**所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具** 

## 连接版本 

```C++
#include <avz.h>

ACobManager windCob;
ACobManager groudCob;

void AScript()
{
    ASetZombies({
        AHY_32,
        ABY_23,
        AGL_7,
        AFT_21,
        AQQ_16,
        ABC_12,
        ATT_18,
        ATT_4,
        ALZ_2,
        APJ_0,
        ABJ_20,
    });
    ASelectCards({
        AICE_SHROOM,   // 寒冰菇
        AM_ICE_SHROOM, // 模仿寒冰菇
        ACOFFEE_BEAN,  // 咖啡豆
        ADOOM_SHROOM,  // 毁灭菇
        AFLOWER_POT,   // 花盆
        ASQUASH,       // 倭瓜
        ACHERRY_BOMB,  // 樱桃炸弹
        ABLOVER,       // 三叶草
        APUMPKIN,      // 南瓜头
        APUFF_SHROOM,  // 小喷菇
    });

    AConnect(ATime(1, -599), [] {
        windCob.SetList({{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}});
        groudCob.SetList({{1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}});
    });
    for (int wave = 1; wave < 21; ++wave) {
        AConnect(ATime(wave, 3475 / 5 - 200 - 387), [] {
            windCob.RoofFire(2, 8.8625);
            groudCob.RoofFire(4, 8.8625);
        });
    }

    AConnect(ATime(10, 3475 / 5 - 200 - 100), [] {
        ACard({{AFLOWER_POT, 2, 9}, {ACHERRY_BOMB, 2, 9}});
    });

    for (auto wave : {9, 19, 20}) {
        AConnect(ATime(wave, 3475 / 5 - 200 - 387), [] {
            windCob.RecoverFire(2, 9);
            groudCob.RecoverFire(5, 9);
            windCob.RecoverFire(2, 9);
            groudCob.RecoverFire(5, 9);
        });
    }

    AConnect(ATime(20, -298 - 1), [] {
        ACard({{AFLOWER_POT, 2, 9}, {AICE_SHROOM, 2, 9}, {ACOFFEE_BEAN, 2, 9}});
    });
}
```

[目录](./0catalogue.md)