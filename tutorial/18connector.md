<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 19:12:51
 * @Description: 
-->
# 连接再探

本页教程将说明 AConnect 函数的所有用法，希望大家能够熟练的运用。


## 连接按键和 bool Functor()
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

    // AConnect 第一个参数还可传入一个 bool Functor(), 如果此函数返回 true, 则会执行后面的操作
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

关于时间连接还有两个不得不介绍的函数，那就是 `ANowTime` 和 `ANowDelayTime` ,

```C++
// ANowTime 会返回当前的游戏时间点

// 无参调用形式返回值类型为 ATime
// 如果当前游戏不在战斗界面，他将会返回 (1, INT_MIN) 这么一个特殊的时间点，
// 如果游戏在战斗界面，他将会返回 (游戏目前波数，波数对应的时间点)
ANowTime();

// 返回第五波对应的时间点
// 有参调用形式返回值类型为 int
ANowTime(5);

// ANowDelayTime 会返回以当前时间点为基准，延迟 参数cs 之后的时间点

// 仅指定 delayTime 的返回值类型为 ATime
// 假如现在的时间点是 (1, 0)，那么此函数返回值为 ATime(1, 50)
ANowDelayTime(50);

// 指定波数和 delayTime 的返回值类型为 int
// 假如现在的时间点是 (5, -150)，那么此函数返回值为 -100
ANowDelayTime(5, 50);

```

```C++
// 按下 Q 后过 100cs 种下一张卡
AConnect('Q', [] {
    AConnect(ANowDelayTime(100), [] { ACard(1, 1, 1); });
});
```

[目录](./0catalogue.md) 
