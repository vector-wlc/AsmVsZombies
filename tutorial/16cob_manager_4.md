<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 18:39:42
 * @Description: 
-->
# 炮管理类——炮序模式

在 ACobManager 类中，有一个十分重要的成员我们还未介绍，那就是 `SetSequentialMode`，在 本框架 中，使用炮的顺序有两种，一个是根据时间顺序用炮（框架默认），另一个是根据空间顺序用炮，接下来，我们将介绍这两种模式如何设定以及有什么不同。

如果想设定炮序使用模式，需要使用 `SetSequentialMode` 函数，示例如下：
```C++
// 将使用模式设置为空间使用模式，本框架 默认开启此模式
aCobManager.SetSequentialMode(ACobManager::TIME);

// 时间使用模式
aCobManager.SetSequentialMode(ACobManager::SAPCE);
```


## 时间使用模式
时间使用模式比较智能，开启此模式时，炮操作对象会每次扫描其储存的炮列表中恢复时间最短的炮，如果同时有多门炮可用时，炮操作对象会根据 `SetList` 中书写的顺序发射炮。这种模式下会使得原地铲种不用调整炮序，但是同样会带来一个麻烦，就是我们可能无法知道某一时刻点炮操作对象到底发到了哪个位置的炮，此时如果我们需要进行铲种就会比较麻烦，此时，就需要调用前一个文章中的函数 `FixLatest` 了，虽然我们不知道，但是炮操作对象知道自己上一次发射的炮位于什么位置，因此如果解中只存在原地铲种且不关心铲种什么位置时，推荐使用此模式与 `FixLatest` 的组合。

## 空间使用模式
空间使用模式一旦开启，炮管理类对象将会完全按照 `SetList` 中写的顺序进行发射，这样带来的好处就是我们每时每刻都知道炮发到了哪个位置，但是有个明显的缺点就是当解中含有铲种炮时，会带来比较烦人的炮序调整。

在文章的最后，再给大家介绍几个函数 `Skip`, `SetNext`, `GetNextUsable`, `GetNextRoofUsable` 和 `GetRoofFlyTime`
```C++
// 跳过炮列表中的两门炮
aCobManager.Skip(2);

// 将炮列表中位于二行三列中的炮设置为下一门即将发射的炮
aCobManager.SetNext(2, 3);

// 将炮列表中的第一门炮设置为下一门即将发射的炮
aCobManager.SetNext(1);

// 以下几个函数不仅仅可以用于炮序排布
// 他们可以得到更加详细的炮的状态

// 得到可用的炮的内存指针
auto cobPtr = GetUsablePtr() 

// 得到发往九列可用的屋顶炮的内存指针
auto cobPtr = GetRoofUsablePtr(9) 

// 得到恢复时间最短的炮的内存指针
auto cobPtr = GetRecoverPtr() 

// 得到发往九列恢复时间最短的屋顶炮的内存指针
auto cobPtr = GetRoofRecoverPtr(9) 

// 得到 1 列屋顶炮发往 7 列飞行时间
auto flyTime = ACobManager::GetRoofFlyTime(1, 7);
```

**注意，虽然调整炮序函数和空间使用模式是绝配，但是并不意味着在时间使用模式下不能用；
同理，虽然 `FixLatest` 和时间使用模式是绝配，但是并不意味着在空间使用模式下不能用；**
具体使用后发生的现象笔者不在赘述，如果实在想象不出来可以在脚本中试一试，这很好理解。


[目录](./0catalogue.md)
