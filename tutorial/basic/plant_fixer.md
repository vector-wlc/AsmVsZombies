<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 16:43:22
 * @Description: 
-->
# 修补植物类

有时候在游戏中我们需要修补植物，AvZ 将这一功能封装为了 `PlantFixer` 类供大家使用。

```C++
extern AvZ::PlantFixer plant_fixer;
```

在这条语句中，AvZ::PlantFixer 是修补植物类，plant_fixer 是其创建的对象，这条语句仍在头文件 libavz.h 中，接下来介绍修补植物类的接口函数。

## 接口简介

```C++
// 位置被重置为{2，3}，{3，4}
plant_fixer.resetFixList({{2, 3}, {3, 4}});

// 自动得到修补的位置列表
plant_fixer.autoGetFixList();

// 修补全场的高坚果
plant_fixer.start(GJG_23)

// 修补位置为{1，3}，{2，3}位置的南瓜头
plant_fixer.start(NGT_30, {{1, 3}, {2, 3}})

// 修补位置为{1，3}，{2，3}位置的坚果，血量降至 300 开始修补
plant_fixer.start(JG_3, {{1, 3}, {2, 3}}, 300)

// 将修补触发血量改为200
plant_fixer.resetFixHp(200)

// 使用咖啡豆
plant_fixer.isUseCoffee(true)

// 不使用咖啡豆
plant_fixer.isUseCoffee(false)

// pause goOn stop  这三个接口与存冰类的含义相同，在此不再赘述

```

## 同时修补多种植物

在一局游戏中，我们可能需要修补多种植物，但是需要注意的是，一个修补植物对象只能修补一种植物，那如何解决这个问题呢，其实非常简单，我们只需要使用修补植物类创建多个修补植物对象即可。
```C++
AvZ::PlantFixer nut_fixer;
AvZ::PlantFixer pumpkin_fixer;

void Script(){

}
```
注意，为了方便使用，需要将对象创建在 `Script` 函数外部，像这样创建两个对象，用一个对象修补坚果，用另一个对象修补南瓜。

```C++
void Script(){
    nut_fixer.start(JG_3);
    nut_fixer.resetFixHp(4000 / 3 * 2); // 这里注意一定要在 start 函数之后再调用 resetFixHp, 不然效果会被 start 函数的默认值覆盖
    pumpkin_fixer.start(NGT_30);
    pumpkin_fixer.resetFixHp(4000 / 3 * 2);
}
```
这样我们就可以同时修补坚果和南瓜了，到这里大家可能有个疑惑，为什么一个对象不能同时开两个 `start` 呢？原因很简单，因为如果这个对象再调用 `stop` 函数时，他自己会不知道要停止哪个修补任务。

总结：我们可以使用 `start` `pause` `goOn` `stop` 控制语句来达到精准控制修补植物对象修补状态的目的，同时可以使用一系列的属性设置函数来控制修补植物对象的修补类型和修补位置等属性，当一局游戏中需要修补多种植物时，需要创建多个修补植物对象。

[上一篇 冰三和铲除函数](./ice3_and_shovel.md)

[目录](../catalogue.md)

[下一篇 炮操作类——初步](./pao_operator_1.md)