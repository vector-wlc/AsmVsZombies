<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 18:34:08
 * @Description: 
-->

# 炮操作类——铲种

AvZ 中没有高级封装的 `fixPao` 函数，取而代之的是铲除函数 `Shovel` ，种炮函数 `plantPao`，这两个函数的有机结合会比 `fixPao` 更加灵活，下面举例说明

首先介绍这两个函数的使用

```C++
// 原地铲种位于二行三列的炮
Shovel(2, 3);
pao_operator.plantPao(2, 3);
```

看起来是要比 `fixPao` 复杂了，因为 `fixPao` 只需要一条语句，而这需要两条语句，但是在 "炮操作类——初步" 中提到过，铲种炮不仅仅原地铲种一种形式，还有位移铲种等多种形式，如果使用 `fixPao` 来实现全部的铲种形式时，就会显得力不从心，但是这两个函数可以很容易的实现，例如：


```C++
// 位移铲种
Shovel(2, 4);
pao_operator.plantPao(2, 3);

// 超时空铲种（先种后铲）
SetTime(-200, 2);
pao_operator.plantPao(2, 3);
SetTime(-100, 2);
Shovel(1, 4);
```

这些都是 `fixPao` 很难以实现的，因此这种方式比 `fixPao` 灵活的多。

除 `Shovel` 和 `plantPao` 之外，PaoOperator 中还存在着一个铲种函数 `fixLatestPao`，即为修补上一枚发射的炮，使用示例如下：
```C++
pao_operator.pao(2, 9);
pao_operator.fixLatestPao();
```

`fixLatestPao` 将会修补 `pao` 刚刚发射出去的炮，此函数不用填写任何参数，直接调用即可，**但是其只支持原地铲种**，原因后续再进行介绍。

[上一篇 炮操作类——多炮列表](./pao_operator_2.md)

[目录](../catalogue.md)

[下一篇 炮操作类——炮序模式](./pao_operator_4.md)