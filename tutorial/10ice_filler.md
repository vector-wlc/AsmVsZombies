<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 16:07:38
 * @Description: 
-->
# 自动操作类

# 存冰类

接下来介绍一个简单而又重要的类：存冰类

在 本框架 中，存冰和使用冰的操作被封装成了类提供给大家使用，熟悉 C++ 的大佬鼠肯定知道，如果想使用类中的函数（这里指非静态函数），不可避免的是使用类创建一个对象，然后使用这个对象调用成员函数，但是现在不必这么做，因为 本框架 中提前为大家准备好了存冰类的对象 aIceFiller，其声明在 libavz.h 头文件下
```C++
extern AIceFiller aIceFiller;
```

在这条语句中 AIceFiller 就是存冰类，aIceFiller 即为其创建（实例化）的对象，接下来说明如何使用 aIceFiller 这个对象完成存冰和使用冰的功能。

```C++
// 在一行二列，一行三列进行存冰
// 注意是优先存 一行二列 再存 一行三列
// 不要频繁调用此接口，性能消耗大
aIceFiller.Start({{1, 2}, {1, 3}});

// 使用存冰列表中的冰
// 注意 coffee 函数将会倒序使用存冰列表中的冰，例如如果使用示例 Start 中的列表，那么 Coffee 函数将会优先使用一行三列的存冰。
aIceFiller.Coffee();

// 停止存冰 
// 完全停止这个对象的运行，如果长时间不再使用此对象需要调用此接口
// 再次激活对象的运行需要调用 Start 
aIceFiller.Stop();

// 暂停存冰
// 对象以一种低耗状态运行，如果短时间不使用此对象需要调用此接口
// 再次激活对象的运行需要调用 goOn
aIceFiller.Pause();

// 继续存冰
aIceFiller.GoOn();

// 重置存冰列表为三行三列，三行四列
aIceFiller.SetList({{3, 3}, {3, 4}});

// 只使用原版冰
aIceFiller.SetIceSeedList({ICE_SHROOM})

// 优先使用模仿冰，再使用原版冰
aIceFiller.SetIceSeedList({M_ICE_SHROOM, ICE_SHROOM}) 
```

# 修补植物类

有时候在游戏中我们需要修补植物，本框架将这一功能封装为了 `APlantFixer` 类供大家使用。

```C++
extern APlantFixer aPlantFixer;
```

在这条语句中，APlantFixer 是修补植物类，aPlantFixer 是其创建的对象，这条语句仍在头文件 libavz.h 中，接下来介绍修补植物类的接口函数。

## 接口简介

```C++
// 位置被重置为{2，3}，{3，4}
aPlantFixer.SetList({{2, 3}, {3, 4}});

// 自动得到修补的位置列表
aPlantFixer.AutoSetList();

// 修补全场的高坚果
aPlantFixer.Start(GJG_23)

// 修补位置为{1，3}，{2，3}位置的南瓜头
aPlantFixer.Start(NGT_30, {{1, 3}, {2, 3}})

// 修补位置为{1，3}，{2，3}位置的坚果，血量降至 300 开始修补
aPlantFixer.Start(JG_3, {{1, 3}, {2, 3}}, 300)

// 将修补触发血量改为200
aPlantFixer.SetHp(200)

// 使用咖啡豆
aPlantFixer.SetIsUseCoffee(true)

// 不使用咖啡豆
aPlantFixer.SetIsUseCoffee(false)

// Pause GoOn Stop  这三个接口与存冰类的含义相同，在此不再赘述

```

## 同时修补多种植物

在一局游戏中，我们可能需要修补多种植物，但是需要注意的是，一个修补植物对象只能修补一种植物，那如何解决这个问题呢，其实非常简单，我们只需要使用修补植物类创建多个修补植物对象即可。
```C++
APlantFixer nutFixer;
APlantFixer pumpkinFixer;

void AScript(){

}
```
注意，为了方便使用，需要将对象创建在 `AScript` 函数外部，像这样创建两个对象，用一个对象修补坚果，用另一个对象修补南瓜。

```C++
APlantFixer nutFixer;
APlantFixer pumpkinFixer;
void AScript(){
    nutFixer.Start(JG_3);
    nutFixer.SetHp(4000 / 3 * 2); // 这里注意一定要在 Start 函数之后再调用 SetHp, 不然效果会被 Start 函数的默认值覆盖
    pumpkinFixer.Start(NGT_30);
    pumpkinFixer.SetHp(4000 / 3 * 2);
}
```
这样我们就可以同时修补坚果和南瓜了，到这里大家可能有个疑惑，为什么一个对象不能同时开两个 `Start` 呢？原因很简单，因为如果这个对象再调用 `Stop` 函数时，他自己会不知道要停止哪个修补任务。

总结：我们可以使用 `Start` `Pause` `GoOn` `Stop` 控制语句来达到精准控制修补植物对象修补状态的目的，同时可以使用一系列的属性设置函数来控制修补植物对象的修补类型和修补位置等属性，当一局游戏中需要修补多种植物时，需要创建多个修补植物对象。

[目录](./0catalogue.md)








