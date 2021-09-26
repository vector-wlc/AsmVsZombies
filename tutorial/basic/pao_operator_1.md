<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 17:15:45
 * @Description: 
-->

# 炮操作类——初步

众所周知，炮序排布是一块难啃的瓜，如果解中存在着原地铲种炮、位移铲种炮、闪现铲种炮、超时空铲种炮，并且场地为天台时，排炮序可能让我们怀疑人生。因此我们需要一个强大的工具使我们从复杂的工作中解放出来，炮操作类便应运而生。吹逼结束。

炮操作类是 AvZ 中最为复杂的一个类，但在我的角度来看，他也是最为强大的一个类。为了防止本文篇幅过长，这里只介绍其部分基础接口 。

首先，炮操作类与存冰类一样，也有一个预先创建的对象 pao_operator，其也声明在头文件 libavz.h 中，声明如下：
```C++
extern AvZ::PaoOperator pao_operator; 
```

其中 AvZ::PaoOperator 为炮操作类的名称，pao_operator 是炮操作类实例化的对象，如果使用者在脚本中只使用一个炮列表，pao_operator 是足够的，下面介绍最基本的接口。


首先介绍炮列表是什么，炮列表实际上一个发射炮的列表
```C++
pao_operator.resetPaoList({{1, 2}, {2, 3}});
```
对于上条语句的意思是将一行二列，二行三列的炮存到 pao_operator 对象中，此时 pao_operator 就知道了自己在发射炮的时候发哪里的炮了。

接下来是发炮函数 `pao`
```C++
pao_operator.pao(2, 9);
pao_operator.pao({{2, 9}, {5, 9}});
```

pao 函数有两种调用形式，即为单发形式和多发形式，针对于示例中的炮列表，炮函数将会先把位于一行二列的炮发往二行九列，然后再把二行三列的炮发往二行九列，最后把一行二列的炮发往五行九列，注意对应顺序，resetPaoList 告诉了 pao_operator 我能发射哪里的炮，pao 则根据 resetPaoList 中的位置书写顺序依次发射。

接下来是等待炮恢复立即发炮函数 `recoverPao`
```C++
pao_operator.recoverPao(2, 9);
pao_operator.recoverPao({{2, 9}, {5, 9}});
```

`recoverPao` 函数会一直等待炮可用后再将炮立即发出去，其他方面和 `pao` 函数相同。

接下来是屋顶飞行时间调整函数 `roofPao`
```C++
pao_operator.roofPao(2, 9);
pao_operator.roofPao({{2, 9}, {5, 9}});
```


我们知道，由于上届之风（游戏BUG）的影响，天台中位于不同列的炮其飞行时间也不同，这带来了非常复杂的时间调整问题，为了解决这一问题，`roofPao` 统一将炮的飞行时间修正为了 387cs ，注意，**这里的修正并没有改变游戏规则，而是 AvZ 根据炮所在的列数对其进行了适当的延后发射调整。**

接下是最高发射权限函数 `rawPao` /  `rawRoofPao`
```C++
pao_operator.rawPao(1, 1, 2, 9);
pao_operator.rawPao({{1, 1, 2, 9}, {1, 2, 5, 9}});
pao_operator.rawRoofPao(1, 1, 2, 9);
pao_operator.rawRoofPao({{1, 1, 2, 9}, {1, 2, 5, 9}});
```

对于第一个例子的解释是将位于一行一列的炮发往二行九列，其余的例子由上述解释很简单的推出是什么意思，这里要强调的是：rawPao 系列的函数是超级权限的函数，不论发射的炮是否位于 `resetPaoList` 录入的位置，`rawPao` 都能发射，只要场地上目标位置确实存在炮，rawPao 函数都可以发射。 

[上一篇 修补植物类](./plant_fixer.md)

[目录](../catalogue.md)

[下一篇 炮操作类——多炮列表](./pao_operator_2.md)