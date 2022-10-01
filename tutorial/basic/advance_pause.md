<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-02-12 11:05:57
 * @Description: 
-->

# 高级暂停

高级暂停功能由 `SetAdvancedPause` 和 `SetAdvancedPauseKey` 函数实现，
前者用于代码设定高级暂停，后者用于按键设定高级暂停。
需要特别注意的是 `SetAdvancedPause` 一旦使得程序进入高级暂停状态后，便再也不会执行 Script 中的代码，
此时必须使用 `SetAdvancedPauseKey` 才能解除高级暂停状态。


```C++

// *** Not In Queue
// 设定高级暂停
// *** 注意开启高级暂停时不能使用跳帧
// *** 特别注意的是 `SetAdvancedPause` 一旦使得程序进入高级暂停状态后，便再也不会执行 Script 中的代码，
// 此时必须使用 `SetAdvancedPauseKey` 才能解除高级暂停状态。
// *** 使用示例
// SetAdvancedPause(true) ------ 开启高级暂停
// SetAdvancedPause(false) ------ 关闭高级暂停
void SetAdvancedPause(bool is_advanced_pause);


// *** Not In Queue
// 设定高级暂停按键
// *** 注意开启高级暂停时不能使用跳帧
// *** 使用示例
// SetAdvancedPauseKey('Q') ------ 将 Q 键设定为高级暂停管理按键，即按下 Q
// 游戏暂停，再按 Q 游戏继续运行
void SetAdvancedPauseKey(char key);
```




[上一篇 出怪设置与女仆秘籍](./basic/set_zombie.md)

[目录](../catalogue.md)

[下一篇 跳帧](../basic/skip_tick.md)