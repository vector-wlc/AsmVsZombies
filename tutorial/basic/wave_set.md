<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 19:05:19
 * @Description: 
-->

# 波长设定与非定态

在脚本编写过程中，我们时常困扰于僵尸的提前刷新、延迟刷新问题，这些问题一旦发生，视频基本要推倒重来，为了解决这一问题，AvZ 提供了强制控制僵尸刷新函数 `SetWavelength`。其使用方法如下：
```C++
SetWaveLength({
    {1, 601},
    {2, 601},
    {3, 601},
    {4, 1800},
    {5, 601},
    {6, 601},
    {7, 601},
    {8, 1800},
    {10, 601},
    {11, 601},
    {12, 601},
    {13, 1800},
    {14, 601},
    {15, 601},
    {16, 601},
    {17, 1800},
    {18, 601},
});
```

看完这个参数的填写相必大家也就明白这个函数参数的意义是啥了，对没错，上述语句的意思是将第一波的波长设置为601，将第二波的波长设置为601……，当然我们不必将每一波的波长都设置一遍，例如下面这种调用方法也是可以的
```C++
SetWaveLength({
    {1, 601},
    {4, 1800},
    {6, 601},
});
```

**注意，此函数不能设置第 9 19 20 波的波长，并且设置波长的范围只能为 [601, 2500]，因为此函数不允许游戏最基本的规则被破坏。**

那么设置好相应波的波长后能带给我们什么便利呢？

**`SetTime` 的时间参数可以填写小于 -200 的值**

如果我们设定了第一波的波长，那么第二波的 `SetTime` 就可以这么填写 `SetTime`(-400, 2)，因为我们在游戏运行前告诉了 AvZ 这个框架僵尸的刷新时刻点。

其实有很多人疑惑为什么普通波的时刻点不能小于 -200，原因其实很简单，是由 PvZ 这个游戏的规则决定的，由于僵尸刷新倒计时会发生从某个值到 200cs 的突变，所以通常情况下时刻点的设定是不能小于 -200 的。

**给非定态解的视频录制带来巨大的便利**

我们知道，非定态解在录制视频时是很伤肝的，因为每一种刷新情况的出现是有几率的，但是如果我们使用了设定波长函数，那么僵尸的刷新就会强制和我们设定的波长一样，这样录制视频就会方便非常多。

总结：设定波长函数是一个通过改写游戏的内存来达到强制控制僵尸刷新的函数，当使用了这个函数，会扩充 `SetTime` 的时间参数填写范围，并且由于强制控制了僵尸的刷新情况，非定态解可以使用穷举法拆分成多个定态解去处理。

[上一篇 炮操作类——炮序排布](./pao_operator_5.md)

[目录](../catalogue.md)

[下一篇 按键绑定](./key_connect.md)