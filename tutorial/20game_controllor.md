<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-02-12 11:05:57
 * @Description: 
-->

# 游戏控制函数

## 高级暂停

高级暂停功能由 `ASetAdvancedPause`  


```C++
// 设定高级暂停
// *** 注意开启高级暂停时不能使用跳帧
// *** 特别注意的是 `ASetAdvancedPause` 一旦使得程序进入高级暂停状态后，
//     除了模式为 GLOBAL 和 AFTER_INJECT 的帧运行以及 AfterTick BeforeTick 的状态钩，框架将不再执行其他代码，
// *** 使用示例
// ASetAdvancedPause(true) ------ 开启高级暂停
// ASetAdvancedPause(false) ------ 关闭高级暂停
void ASetAdvancedPause(bool isAdvancedPause);

// 一般来说，这个函数需要配合 AConnect 使用
// 按下 Q 进行高级暂停
AConnect('Q', []{
    ASetAdvancedPause(true);
});

// 按下 W 停止高级暂停
AConnect('W', []{
    ASetAdvancedPause(false);
});


// 你仍然可以这样做 ，自己整一个 static 变量
// 至于 static 变量是啥作用，你需要自行百度
// 简单理解就是这个变量离开作用域也不会消失，且只会初始化一次
// 那么下面的代码就是实现了这样的功能
// 如果游戏不是高级暂停，按下 Q 就高级暂停，如果是高级暂停，按下 Q 就不会高级暂停
AConnect('Q', []{
    static bool isPaused = false;
    isPaused = !isPaused;
    ASetAdvancedPause(isPaused);
});

```

## 跳帧

跳帧是节省脚本调试和游戏冲关时间的重要功能，需要使用 `ASkipTick` 来使用此功能。

### 跳到指定时间

```C++
// 跳到游戏时刻点 (2, 300)
ASkipTick(2, 300);
```
使用这条语句游戏将会跳到 (2, 300)，注意在此过程中，游戏画面是不会刷新的（游戏画面直接卡住），
游戏内部会以上几十甚至几百倍运行（由 CPU 性能决定），以 i5-7300HQ 为例，运行速度大概为 40 倍，
当游戏时间进行到 (2, 300) 时，游戏会回复正常运行状态。

### 跳到指定条件

```C++
// 直接跳过整个游戏
ASkipTick([=](){
    return true;
});
```
除了可以指定时间外，还可以指定条件，当返回 `true` 时，游戏帧将会被跳过去，当返回 `false` 时，跳帧将会停止，
由于上述代码一直返回 `true`，因此所有游戏帧都将会被跳过去。

```C++
// 使得游戏接近 50 倍速运行
// 原理解释：当游戏时钟对 50 取余值为 1 时，触发此连接的操作函数，
// 操作函数运行即进行跳帧，跳帧依然对 50 取余，当此值为 0 时跳帧结束
// 然后取余 0 的下一帧值又为 1，又触发了连接，便又开始跳帧，
// 也就是游戏每隔 50cs 启用一次跳帧，跳帧会跳过 49 帧，这便突破了 ASetGameSpeed 10 倍以上的游戏运行速度
AConnect([] { return AGetMainObject()->GameClock() % 50 == 1; }, 
         [] { ASkipTick([] { return AGetMainObject()->GameClock() % 50; }); });
```

```C++
// 检测位于 {1, 3}, {1, 5} 的玉米炮是否被破坏，如果被破坏，停止跳帧
auto condition = [=]() {
    std::vector<int> results;
    AGetPlantIndices({{1, 3}, {1, 5}}, AYMJNP_47, results);

    for (auto result : results) {
        if (result < 0) {
            return false;
        }
    }
    return true;
};

ASkipTick(condition);
```
上述代码是检测破阵的跳帧使用示例，假如位于 {1, 3}, {1, 5} 的玉米炮被破坏就意味着此解失败，
那么上述代码会使得游戏直接跳到玉米炮被损坏的时候。

```C++
auto condition = [=]() {
    std::vector<int> results;
    AGetPlantIndices({{1, 3}, {1, 5}}, AYMJNP_47, results);

    for (auto result : results) {
        if (result < 0) {
            return false;
        }
    }
    return true;
};

auto callback = [=]() {
    // 写春哥没了的提示代码，比如用一个 ALogger 显示信息
};

ASkipTick(condition, callback);
```

当然 `ASkipTick` 还可以使用回调函数，就是当停止跳帧时立马执行的函数，上述代码执行结果：
当位于 {1, 3}, {1, 5} 的玉米炮被破坏时报出错误："春哥无了，嘤嘤嘤"。



## 快速进入游戏
快速进入游戏由函数 `AEnterGame` 实现，此函数可以节省进入游戏的手动操作时间

```C++
// 快速进入游戏函数
// *** 使用示例
// AEnterGame() ------ 默认进入泳池无尽生存模式，默认会自动点掉继续对话框
// AEnterGame(AAsm::SURVIVAL_ENDLESS_STAGE_1) -------- 进入白天无尽生存模式，默认会自动点掉继续对话框
// AEnterGame(AAsm::SURVIVAL_ENDLESS_STAGE_1, false) -------- 进入白天无尽生存模式，不会自动点掉继续对话框
inline void AEnterGame(int gameMode = AAsm::SURVIVAL_ENDLESS_STAGE_3, bool hasContinueDialog = false);
```

## 快速回到游戏主界面
```C++
// 快速回到游戏主界面
// *** 注意：此函数仅在战斗界面生效
// *** 使用示例
// ABackToMain() ----- 直接回到主界面，默认会自动存档
// ABackToMain(false) ----- 直接回到主界面，不会自动存档
inline void ABackToMain(bool isSaveData = true);
```

以上两个功能可以合并一下实现一个非常懒狗的功能
```C++
// 按下 Q 键直接返回主界面，然后又从主界面立即进入游戏选卡或者战斗界面
// 所以当存档设置为只读的时候，此功能就非常懒狗
AConnect('Q', [] {
    ABackToMain();
    AEnterGame();
});
```

[目录](./0catalogue.md)
