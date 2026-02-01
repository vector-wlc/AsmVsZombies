<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-19 21:55:15
 * @Description: 
-->
# 帧运行

在键控中，尤其是无炮键控中，我们常常需要某些操作时刻运行，那么这个时候我们就需要使用帧运行 `ATickRunner` 了

我们以自动三叶草程序为例，来介绍 ATickRunner 是如何使用的

```C++
#include <avz.h>

void UseBlover()
{
    for (auto&& zombie : aAliveZombieFilter) {
        if (zombie.Type() == AQQ_16 && zombie.Abscissa() <= 50) { // 如果有气球僵尸的横坐标小于50 (快飞到家了)
            // 这句话是获得三叶草卡片的内存指针
            // AGetCardIndex 函数就是从卡片名称得到卡片在卡槽位置的函数
            auto blover = AGetMainObject()->SeedArray() + AGetCardIndex(ASYC_27);
            if (blover->IsUsable()) { // 三叶草可用时才能吹气球
                ACard(ASYC_27, 1, 1);
                return; // 吹一次就结束了，因为再循环遍历也没用了
            }
        }
    }
}

ATickRunner tickRunner;

void AScript()
{
    tickRunner.Start(UseBlover);
}
```

相信这个程序很简单吧，大家应该都能看懂，但是实际上帧运行的 Start 函数还有一个参数，就是以何种方式运行，这是什么意思呢，看代码

```C++
tickRunner.Start(UseBlover);                            // 默认值只在战斗界面运行
tickRunner.Start(UseBlover, ATickRunner::GLOBAL);       // 在选卡和战斗界面运行
tickRunner.Start(UseBlover, ATickRunner::AFTER_INJECT); // 在注入之后的每帧都运行
```

实际上大多数情况下， 默认在战斗界面运行是足够用的，但是极少数情况下是需要再全局方式下运行，你可能会问，是什么情况下呢，
例如你觉得本框架自带的选卡函数不好的时候，就可以选择在全局方式下运行，对没错，只有在全局方式下运行，你才能选卡，是不是？
当然，还可能有其他作用，但是目前还不知道，反正他有一个这样的功能，你知道就行。

[目录](./00_catalogue.md)