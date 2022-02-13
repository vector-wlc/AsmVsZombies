<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-02-12 10:44:56
 * @Description: 
-->
# 跳帧

跳帧是节省脚本调试和游戏冲关时间的重要功能，需要使用 `SkipTick` 来使用此功能。

## 跳到指定时间

```C++
// 跳到游戏时刻点 (300, 2)
SkipTick(300, 2);
```
使用这条语句游戏将会跳到 (300, 2)，注意在此过程中，游戏画面是不会刷新的（游戏画面直接卡住），
游戏内部会以上几十甚至几百倍运行（由 CPU 性能决定），以 i5-7300HQ 为例，运行速度大概为 40 倍，
当游戏时间进行到 (300, 2) 时，游戏会回复正常运行状态。

## 跳到指定条件

```C++
// 直接跳过整个游戏
SkipTick([=](){
    return true;
});
```
除了可以指定时间外，还可以指定条件，当返回 `true` 时，游戏帧将会被跳过去，当返回 `false` 时，跳帧将会停止，
由于上述代码一直返回 `true`，因此所有游戏帧都将会被跳过去。

```C++
// 检测位于 {1, 3}, {1, 5} 的玉米炮是否被破坏，如果被破坏，停止跳帧
auto condition = [=]() {
    std::vector<int> results;
    GetPlantIndices({{1, 3}, {1, 5}}, YMJNP_47, results);

    for (auto result : results) {
        if (result < 0) {
            return false;
        }
    }
    return true;
};

SkipTick(condition);
```
上述代码是检测破阵的跳帧使用示例，假如位于 {1, 3}, {1, 5} 的玉米炮被破坏就意味着此解失败，
那么上述代码会使得游戏直接跳到玉米炮被损坏的时候。

```C++
auto condition = [=]() {
    std::vector<int> results;
    GetPlantIndices({{1, 3}, {1, 5}}, YMJNP_47, results);

    for (auto result : results) {
        if (result < 0) {
            return false;
        }
    }
    return true;
};

auto callback = [=]() {
    ShowErrorNotInQueue("春哥无了，嘤嘤嘤");
};

SkipTick(condition, callback);
```

当然 `SkipTick` 还可以使用回调函数，就是当停止跳帧时立马执行的函数，上述代码执行结果：
当位于 {1, 3}, {1, 5} 的玉米炮被破坏时报出错误："春哥无了，嘤嘤嘤"。


[上一篇 高级暂停](./advance_pause.md)

[目录](../catalogue.md)

[下一篇 操作队列](../advance/time_operate.md)