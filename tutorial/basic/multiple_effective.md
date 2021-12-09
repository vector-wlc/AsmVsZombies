<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 15:19:18
 * @Description: 
-->

## 多次生效

我们知道很多框架在录制视频时有一个比较烦人的问题，就是一旦录制失败了，得重新启动脚本再次进行录制，但是 AvZ 提供了一个函数 `OpenMultipleEffective`，即一次注入多次生效函数，如果使用了这个函数，当录制视频失败时，但只要脚本的内容没有发生改变，就无需再次注入，只需要退出战斗界面，再次进入战斗界面，脚本会自动从头开始运行，这样就省去了很多调试占用的时间。使用示例如下：
```C++
// 注意：使用的关闭效果按键必须在战斗界面才会生效

// 脚本多次生效，默认按下 C 键取消此效果
OpenMultipleEffective() 

// 脚本多次生效，按下 Q 键取消此效果
OpenMultipleEffective('Q') 

// 脚本多次生效，按下 Q 键取消此效果，多次生效效果在主界面和选卡界面都会生效
// 脚本挂机可用这条语句
OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI)

// 脚本多次生效，按下 Q 键取消此效果，多次生效效果仅在主界面生效
OpenMultipleEffective('Q', AvZ::MAIN_UI)
```

[上一篇 名称空间](./namespace.md)

[目录](../catalogue.md)

[下一篇 卡片操作](./card.md)
