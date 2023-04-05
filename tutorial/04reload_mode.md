<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 15:19:18
 * @Description: 
-->

# 载入脚本模式

我们知道很多框架在录制视频时有一个比较烦人的问题，就是一旦录制失败了，得重新启动脚本再次进行录制，但是本框架提供了一个函数 `AReloadMode`，即设置载入脚本模式函数，来解决这个问题，使用示例如下：
```C++

// 脚本运行一次之后依然会在游戏在主界面或者战斗界面选卡界面重新载入
// 注意这条语句经常用于挂机
// 注意这条语句经常用于挂机
// 注意这条语句经常用于挂机
ASetReloadMode(AReloadMode::MAIN_UI_OR_FIGHT_UI);

// 脚本运行一次之后依然会在游戏在主界面重新载入
ASetReloadMode(AReloadMode::MAIN_UI)

// 脚本运行一次之后不会再重新载入
ASetReloadMode(AReloadMode::NONE) 

```
[目录](./0catalogue.md)

