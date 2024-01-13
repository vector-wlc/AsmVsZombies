// 这里使用经典十二炮作为第一个键控脚本，经典十二炮可以说是最简单的键控脚本了，没有之一

// **请强烈注意，我不是键控技术党，我是键控工具党，所以示例脚本的目的是教会大家如何使用本框架的接口，**
// **脚本中的一些数值可能并不是科学的数值，科学的数值请询问当前植吧顶尖技术党或者查看相关的帖子和B站动态等，但由于本人早已不是键控技术党，**
// **而且没有精力和兴趣去学相关的键控技术知识。**

// **所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

// **所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

// **所以不要把示例脚本视作完美无缺的，而只是把他视作学习本框架接口的工具**

#include <avz.h>

ACoroutine ACoScript()
{
    ASetZombies({
        ACG_3,  // 撑杆
        ATT_4,  // 铁桶
        ABC_12, // 冰车
        AXC_15, // 小丑
        AQQ_16, // 气球
        AFT_21, // 扶梯
        ATL_22, // 投篮
        ABY_23, // 白眼
        AHY_32, // 红眼
        ATT_18, // 跳跳
    });
    ASelectCards({
        AICE_SHROOM,   // 寒冰菇
        AM_ICE_SHROOM, // 模仿寒冰菇
        ACOFFEE_BEAN,  // 咖啡豆
        ADOOM_SHROOM,  // 毁灭菇
        ALILY_PAD,     // 荷叶
        ASQUASH,       // 倭瓜
        ACHERRY_BOMB,  // 樱桃炸弹
        ABLOVER,       // 三叶草
        APUMPKIN,      // 南瓜头
        APUFF_SHROOM,  // 小喷菇
    });

    co_await ATime(1, -599);
    aCobManager.AutoSetList();

    for (int wave = 1; wave < 21; ++wave) {
        if (wave == 10) {
            // wave 10 的附加操作
            // 樱桃消延迟
            co_await ATime(wave, 341 - 373);
            aCobManager.Fire({{2, 9}, {5, 9}});
            co_await ATime(wave, 341 - 100);
            ACard(ACHERRY_BOMB, 2, 9);
        } else if (wave == 20) {
            // wave 20 的附加操作
            // 咆哮珊瑚(炮消)
            co_await ATime(wave, 250 - 378);
            aCobManager.Fire(4, 7.625);
            co_await ATime(wave, 341 - 373);
            aCobManager.Fire({{2, 9}, {5, 9}});
            // wave 20 的附加操作
            // 收尾发四门炮
            co_await ATime(wave, 300);
            aCobManager.RecoverFire({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
        } else {
            // P6
            // 主体节奏
            co_await ATime(wave, 341 - 373);
            aCobManager.Fire({{2, 9}, {5, 9}});

            // wave 9 19 的附加操作
            // 收尾发四门炮
            if (wave == 19 || wave == 9) {
                co_await ATime(wave, 300);
                aCobManager.RecoverFire({{2, 9}, {5, 9}, {2, 9}, {5, 9}});
            }
        }
    }
}