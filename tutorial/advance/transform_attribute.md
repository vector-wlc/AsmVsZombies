<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 20:40:44
 * @Description: 
-->
# 属性转换

在之前的教程中我们提到过在 AvZ 中存在着非常重要的一对属性：In Queue / Not In Queue。在本文中，我们会了解到如何将函数在这两种属性之间进行转换。

## 将 Not In Queue 属性函数转为 In Queue
这个转换实际上比较简单，例如 `ClickGrid` 函数
```C++
// *** Not In Queue
// 点击格子
// *** 使用示例：
// ClickGrid(3, 4)---- 点击格子(3, 4)
// ClickGrid(3, 4, 10)---- 向下偏移10像素点击格子(3, 4)
void ClickGrid(int row, float col, int offset = 0);
```

然后下面的代码
```C++
InsertOperation([=]() {
    ClickGrid(3, 4);
});
```
就将 `ClickGrid` 这个函数由 Not In Queue 转换为了 In Queue，是的，你没有看错 `InsertOperation` 还可以有这个属性转换的作用！

## 将  In Queue 属性函数转为 Not In Queue
完成这个转换需要使用 `SetInsertOperation` 函数或者使用类 `InsertGuard`


例如炮操作类中的 pao 函数是 In Queue 属性函数，如果想将他转换为 Not In Queue 可以这么写：
```C++
SetInsertOperation(false);
pao_operator.pao(3, 4);
SetInsertOperation(true);
```

这样 `SetInsertOperation` 包夹的操作将不会录入操作队列，会被立即执行，即达到了属性转换的目的，当然可以包夹多条代码，例如：
```C++
SetInsertOperation(false);
pao_operator.pao(3, 4);
Card(HBG_14, 1, 4);
SetInsertOperation(true);
```

当然你也可以不用在结尾使用 `SetInsertOperation(true)`，如果不使用这条语句，就意味着接下来的所有操作都不会录入操作队列，都会被立即执行！

有时大家可能忘了使用 `SetInsertOperation(true)` 而导致脚本出现非常严重的 BUG，并且这样的 BUG 还是很难发现的，为了避免这个问题，笔者提供了 `InsertGuard` 类来避免此行为。
```C++
{
    // 进入该代码块的所有操作将被临时转换为 Not In Queue
    // 当改代码块生命周期结束时，InsertGuard 会自动将操作录入的状态恢复
    InsertGuard ig(false);
    pao_operator.pao(3, 4);
}
```
`InsertGuard` 类实现非常简单，即在构造函数首先保存当前的操作录入状态，然后调用 `SetInsertOperation` 来设置操作录入状态，最后在析构函数种恢复之前的操作录入状态。**因此使用此类转换状态更加安全。**

看到这里，相信聪明的你已经想到了 AvZ 的 KeyConnect 就内部调用了这个 `InsertGuard`，可以看一下它的部分源码

```C++
if ((GetAsyncKeyState(key_operation.first) & 0x8001) == 0x8001 && GetForegroundWindow() == __pvz_hwnd)
{ 
    // 检测 pvz 是否为顶层窗口
    InsertGuard insert_guard(false);
    key_operation.second();
    return;
}
```
所以 `KeyConnect` 函数将会把绑定的操作属性隐式转换为 Not In Queue ！！！ 因此在 `KeyConnect` 函数内部直接使用 `SetTime` 或 `Delay` 函数是无效的！！！那么如何在 `KeyConnect` 内使用 `SetTime` 或 `Delay` 呢？代码如下：

```C++
KeyConnect('Q', [=]() {
    InsertGuard ig(true);
    SetNowTime();
    pao_operator.pao(3, 4);
    Delay(100);
    pao_operator.pao(3, 4);
    // some code
});
```

实际上很简单，只要再使用 `InsertGuard` 转换为 In Queue 不就行了吗！不过这里需要注意的是 `SetNowTime` 这个函数的调用，这条代码十分重要，因为如果不调用此条代码，`InsertOperation` 将会把操作插入上个 `SetTime` 设定的时间点中，这样明显不符合我们的预期，因此 `SetNowTime` 这个函数就是将时间点设置为 KeyConnect 绑定的代码运行的时候。


[上一篇 动态操作插入](./insert_operation.md)

[目录](../catalogue.md)

[下一篇 运行调试](./debug.md)