<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 19:12:51
 * @Description: 
-->
# 连接再探

本页教程将说明 AConnect 函数的所有用法，AConnect 是本框架的核心功能之一，希望大家能够熟练的运用。


## 连接按键和 bool Func()
```C++

// *** 使用示例:
ALogger<AMsgBox> logger;
AConnectHandle keyHandle;

void AScript()
{
    logger.SetLevel({ALogLevel::INFO});

    // 按下 0 键弹出一个窗口, 显示 hello, 注意 0 是单引号
    // 运行方式为 true 时, AConnect 创建的连接选卡界面和高级暂停时都生效, 反之不生效
    // *** 特别注意：如果连接创建失败, 连接控制器将被赋值为 nullptr.
    keyHandle = AConnect('0', [] { logger.Info("hello"); });

    // 按下 E 键控制 0 键的是否暂停
    // 如果 0 键此时暂停生效, 按下 E 键便会继续生效, 反之相反
    // 注意字母必须是大写
    AConnect('E', [] {
        if (keyHandle.isPaused()) {
            keyHandle.GoOn();
        } else {
            keyHandle.Pause();
        }
    });

    // 按下 Q 键控制 0 键行为, 即将 0 键的显示变为 world
    AConnect('Q', [] {
        keyHandle.Stop(); // 注意此时 keyHandle 已失效
        keyHandle = AConnect('0', [] { logger.Info("world"); }); // 此时 keyHandle 重新有效
    });

    // AConnect 第一个参数还可传入一个 bool Func(), 如果此函数返回 true, 则会执行后面的操作
    // 这个示例就是游戏每 10 秒钟显示一个 world 的窗口
    AConnect([] { return AGetMainObject()->GameClock() % 1000 == 0; }, [] { logger.Info("world"); });
}
```

看到这里，你可能疑惑是否能让 AConnect 绑定鼠标左键，答案是可以。

但是你需要查看 :  虚拟键盘表 https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes

比如虚拟键盘表中的鼠标左键为 `VK_LBUTTON`

那么如果你想绑定左键就可以这么写

```C++
AConnect(VK_LBUTTON, [] { /* some code */});
```


## 连接时间

**警告，如果你是本框架的初学者，并且暂时不想了解比较复杂的时间管理方式，建议先不要看下面的内容，因为下面这些语句的语法看起来可读性比较差**

AConnect 的 ATime 调用形式是有返回值的，
```C++
auto timeHandle = AConnect(ATime(1, 1), []{});
timeHandle.Stop();  // 使建立的连接的失效，但是没有 Pause 这些成员
```

除了最基础的 AConnect(ATime(1, 1), []{}) 这种调用方式，对于时间连接 AConnect 还有下面几种方式

```C++
#include <avz.h>

// 使用 ARelOp 可让操作从操作轴变为效果轴
ARelOp EffectFire(int row, float col)
{

    return ARelOp(-373, [=] { aCobManager.Fire(row, col); });
}

ARelOp EffectCard(APlantType type, int row, float col)
{
    // 这里的一帧变化，需要了解
    // https://www.bilibili.com/read/cv11389849
    return ARelOp(-100 + 1, [=] { ACard(type, row, col); });
}

ARelOp EffectIce3(APlantType type, int row, float col)
{
    return ARelOp(-298, [=] { ACard({{type, row, col}, {ACOFFEE_BEAN, row, col}});AIce3(298); });
}

void AScript()
{
    ASetZombies({ABY_23});
    AConnect(ATime(1, -599), [] {
        aCobManager.AutoGetList();
    });

    // 在 (1, 400) 这个时间点炮落地和樱桃炸弹同时生效
    AConnect(ATime(1, 400), EffectFire(2, 9) + EffectCard(AYTZD_2, 5, 9));

    // 然后可以冰三
    // 这里的一帧变化，需要了解
    // https://www.bilibili.com/read/cv11389849
    AConnect(ATime(1, 400 + 210 + 1), EffectIce3(AICE_SHROOM, 2, 9));
}
```

我相信上面的代码，第一看到你可能会很惊讶或者惊喜或者看不懂。不论你处于哪种状态，只要你理解了 ARelOp 如何运用，
他会极大程度提高程序的可读性，并且能让脚本十分漂亮。咱们再解释一下上面的代码，ARelOp 实际上做了一件事情，
它允许将一个相对时间点和操作进行捆绑封装合成一个新的操作，并且合成的操作可以进行相加结合，也就是上述代码中你看到的
那个可能令你疑惑的 +，当 AConnect 遇到 ARelOp 的时候，便会对他进行解封装，提取出相对时间，并与 AConnect 中的绝对时间
进行相加运算，于是，第一个用炮真正的操作时间是 (1, 400 - 373), 同理，樱桃炸弹的使用时间是 (1, 400 - 100), 冰三的使用时间为
(1, 400 + 211 - 298)，到这里，我相信你应该明白了 ARelOp 是个什么玩意了。

不仅如此，ARelOp 还支持递归定义，什么意思呢? 就是 ARelOp 允许内嵌 ARelOp

```C++
#include <avz.h>

// 使用 ARelOp 可让操作从操作轴变为效果轴
ARelOp EffectFire(int row, float col)
{
    return ARelOp(-373, [=] { aCobManager.Fire(row, col); });
}

ARelOp EffectCard(APlantType type, int row, float col)
{
    // 这里的一帧变化，需要了解
    // https://www.bilibili.com/read/cv11389849
    return ARelOp(-100 + 1, [=] { ACard(type, row, col); });
}

ARelOp EffectIce3(APlantType type, int row, float col)
{
    return ARelOp(-298, [=] { ACard({{type, row, col}, {ACOFFEE_BEAN, row, col}});AIce3(298); });
}

// ARelOp 可以相互组合
ARelOp EffectGroup()
{
    // 这里的一帧变化，需要了解
    // https://www.bilibili.com/read/cv11389849
    return ARelOp(400, EffectFire(2, 9) + EffectCard(AYTZD_2, 5, 9) + ARelOp(210 + 1, EffectIce3(AICE_SHROOM, 1, 6)));
}

void AScript()
{
    AConnect(ATime(1, 0), EffectGroup());
}
```

这段代码和上面那个实际上是一样的，但是其中内嵌了 ARelOp 的定义方式，这其中计算最为复杂的就是用冰了，咱们再看一下怎么算，
要想算出冰三真正的操作时间，咱们只需要从内向外一层层扒 ARelOp 就行，EffectIce3 第一层是 -298， 然后第二层 210 + 1, 然后第三层 400，第四层 ATime(1, 0), 然后递归结束，那么冰三真正的操作时间点是 ATime(1, 0 + 400 + 210 + 1 - 298)，可见，这里面封装了这么多层，我相信到时候这么写很可能会把人写麻了，但是这是不适当的使用， ARelOp 是把双刃剑，利用好了就可以极大程度提高程序的可读性，并且能让脚本十分漂亮，但是用的不好，太多的嵌套定义，我只能说，耗子尾汁。

[目录](./0catalogue.md)
