<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 16:07:38
 * @Description: 
-->
# 存冰类

接下来介绍一个简单而又重要的类：存冰类

在 AvZ 中，存冰和使用冰的操作被封装成了类提供给大家使用，熟悉 C++ 的大佬鼠肯定知道，如果想使用类中的函数（这里指非静态函数），不可避免的是使用类创建一个对象，然后使用这个对象调用成员函数，但是现在不必这么做，因为 AvZ 中提前为大家准备好了存冰类的对象 ice_filler，其声明在 libavz.h 头文件下
```C++
extern IceFiller ice_filler;
```

在这条语句中 AvZ::IceFiller 就是存冰类，ice_filler 即为其创建（实例化）的对象，接下来说明如何使用 ice_filler 这个对象完成存冰和使用冰的功能。

```C++
// 在一行二列，一行三列进行存冰
// 注意是优先存 一行二列 再存 一行三列
// 不要频繁调用此接口，性能消耗大
ice_filler.start({{1, 2}, {1, 3}});

// 使用存冰列表中的冰
// 注意 coffee 函数将会倒序使用存冰列表中的冰，例如如果使用示例 start 中的列表，那么 coffee 函数将会优先使用一行三列的存冰。
ice_filler.coffee();

// 停止存冰 
// 完全停止这个对象的运行，如果长时间不再使用此对象需要调用此接口
// 再次激活对象的运行需要调用 start 
ice_filler.stop();

// 暂停存冰
// 对象以一种低耗状态运行，如果短时间不使用此对象需要调用此接口
// 再次激活对象的运行需要调用 goOn
ice_filler.pause();

// 继续存冰
ice_filler.goOn();

// 重置存冰列表为三行三列，三行四列
ice_filler.resetFillList({{3, 3}, {3, 4}});

// 只使用原版冰
ice_filler.resetIceSeedList({ICE_SHROOM})

// 优先使用模仿冰，再使用原版冰
ice_filler.resetIceSeedList({M_ICE_SHROOM, ICE_SHROOM}) 
```

[上一篇 时间设定](./time_rule.md)

[目录](../catalogue.md)

[下一篇 冰三和铲除函数](./ice3_and_shovel.md)







