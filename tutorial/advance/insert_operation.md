<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 20:28:06
 * @Description: 
-->

# 动态操作插入

操作插入函数可以说是 `WaitUntil` 高级代替函数，使用这个函数不用考虑时间点先后顺序。下面介绍这个函数的使用。
```C++
SetTime(40, 3);
InsertOperation([=]() {
    SetTime(40, 3);
    if (GetMainObject()->refreshCountdown() < 200) {
        pao_operator.pao(3, 4);
    } else {
        Card(SQUASH, 1, 1);
    }
});
```

上面语句的意思是将 `InsertOperation` 中的语句插入到队列中，这个时候判断僵尸是否刷新这个语句就会在第三波刷新 30cs 后运行，这就实现了我们本来想要的效果，当然，这个函数还可以嵌套使用，例如：
```C++
SetTime(40, 3);
InsertOperation([=]() {
    SetTime(60, 3);
    InsertOperation([=]() {
        if (GetMainObject()->seedArray()->isUsable()) {
            SetTime(60, 3);
            Card(1, 1, 1);
        }
    });
});
```

上述代码看起来好像晦涩难懂，其实现了这样的功能：

在（40，3）进行僵尸刷新的判断，如果刷新了就会在（60，3）这个时间点插入一个操作，这个操作是如果第一个卡片能用，就把第一张卡片种在一行一列 ，因此这就实现了动态操作插入，根据需要扩充操作队列中的操作，但是这明显不如 `WaitUntil` 来的直接，而且使用 `InsertOperation` 可能会大大降低代码的可读性，但是这个函数有一个优点是 `WaitUntil` 无法实现的，就是不用考虑时间点先后顺序，例如下面代码是完全正确的。
```C++
SetTime(40, 3);
InsertOperation([=]() {
    SetTime(60, 3);
    InsertOperation([=]() {
        if (GetMainObject()->seedArray()->isUsable()) {
            SetTime(60, 3);
            Card(1, 1, 1);
        }
    });
});

SetTime(-599, 1);
Card(PUFF_SHROOM, 1, 2);
```

`InsertOperation` 符合 AvZ 的设计目的，因此我在这里建议如果大家现在比较清楚操作队列的实现方式，那就多使用 `InsertOperation`，如果不清楚，建议大家还是使用 `WaitUntil`，因为阻塞函数比较简单粗暴，好理解，当然无炮脚本还是推荐使用 `WaitUntil`，**因为无炮使用 `InsertOperation` 逻辑应该会非常复杂，我们应该还是保护一下我们的头发。**

现在应该解释一下 In Queue 和 Not In Queue 到底是怎么回事了，查看某一个 In Queue 函数源码：
```C++
// In Queue
void pause()
{
    InsertOperation([=]() {
        is_paused = true;
    },
                    "pause");
}
```

其实很简单，In Queue 属性的函数只是内部调用了一下 `InsertOperation` 函数而已，就是这么简单，


而 Not In Queue 并没有在内部使用 `InsertOperation`，因此会立即执行，最后为了方便使用，还有一个 `InsertTimeOperation`，使用方法很简单，


其前两个参数就是操作插入的时间点，其他与 `InsertOperation` 相同。

[上一篇 函数属性](./function_attribute.md)

[目录](../catalogue.md)

[下一篇 属性转换](./transform_attribute.md)