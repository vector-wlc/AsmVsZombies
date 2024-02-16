<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-06-30 11:10:57
 * @Description:
-->
# 内存读取初步

有时候，我们希望脚本根据场上的情况“智能”地作出行动。为达到这个目的，需要读取游戏内存中的各种数据。本节会介绍如何读取 AvZ 中已经封装好的游戏数据。

## `AMainObject`

`AMainObject` 对应的是游戏战斗界面的内存，里面储存着几乎所有和战斗有关的信息。读取数据的代码长这样：

```C++
// 启用一个控制台日志对象
// 关于如何使用日志，请查看教程目录，日志功能
ALogger<AConsole> logger;
// 读取当前阳光数
int sun = AGetMainObject()->Sun();
// 设置阳光为 8000
AGetMainObject()->Sun() = 8000;

AConnect(ATime(20, 0), []{
    // 对于非阻塞式脚本，内存读取一般放在 AConnect 里
    // 变量 sun 的值不会随着游戏进行而更新，而是定格为战斗开始时的阳光值
    // 为了正确显示出 w20 时的阳光，需要在 AConnect 里调用读内存函数
    logger.Info("当前阳光: #", AGetMainObject()->Sun());
});

// 这样写是不行的！AMainObject（以及后面介绍的各种对象）都直接对应 PvZ 的内存，这也就意味着不能复制这些对象
// AMainObject mo = *AGetMainObject();
// 如果不理解 C++ 里什么时候会复制对象的话，只需要记住一个简单的原则：用指针变量存储这一类对象
AMainObject* mo = AGetMainObject();
std::vector<AZombie*> zombies;
```

## 对象过滤器

对象过滤器是对 PvZ 中的各种对象数组的封装，可以遍历场上的所有对象。对象过滤器支持的对象类型有：

- 植物（`APlant`）
- 僵尸（`AZombie`）
- 卡片（`ASeed`）
- 场地物品（`APlaceItem`）

```C++
int cobCnt = 0;
// 这个 for 循环遍历了场上所有存活的植物
for (auto& plant : aAlivePlantFilter) {
    // 注意：这里访问属性用的是 . 而不是 ->
    if (plant.Type() == ACOB_CANNON) {
        // plant 的类型是 APlant&，需要写 &plant 把引用转化成指针
        bool isAvailable = (AGetCobRecoverTime(&plant) == 0);
        cobcnt += isAvailable;
    }
}
AGetInternalLogger()->Info("当前有 # 门炮可用", cobCnt);

// 如果一个比较复杂的条件多次被使用，可以预先定义一个附带条件的对象过滤器
// 条件过滤器定义后可以随时使用，什么时候使用就基于什么时候的数据遍历，不像变量一样定义了之后就不会改了
AAliveFilter<AZombie> gigaWithImp([](AZombie* zombie) {
    return zombie->Type() == AGIGA_GARGANTUAR && zombie->Hp() > 3000;
});

for (auto& zombie : gigaWithImp) {
    // ...
}

// aAliveSeedFilter 只遍历当前可用的卡片。想遍历所有的卡片，需要使用 ABasicFilter
// ABasicFilter 会无条件遍历所有对象（其中可能有无效对象，虽然遍历卡片时是不会有的），需要自行处理
for (auto& seed : ABasicFilter<ASeed>()) {
    // ...
}

/*
场地物品大家可能不太熟悉，它包含以下几类对象：
1 - 墓碑
2 - 弹坑
3 - 梯子
4 - 传送门（圆）
5 - 传送门（方）
6 - 脑子（水族馆）
7 - 罐子
11 - 钉耙
12 - 脑子（我是僵尸）
这个列表也出现在了 APlaceItem::Type() 的文档中。左侧的数字代表对象 Type() 的值。
*/
// 吞掉场上的墓碑
for (auto& item : aAlivePlaceItemFilter) {
    if (item.Type() == 1) {
        ACard(AGRAVE_BUSTER, item.Row() + 1, item.Col() + 1);
        break;
    }
}
```

## 场地信息

AvZ 提供了 `aFieldInfo` 对象，可以方便地读取当前场地的信息。这种方式比使用 `AGetMainObject()->Scene()` 兼容性更好，也更方便。

```C++
enum class ARowType {
    NONE = 0, // 不能种植，不出僵尸
    LAND = 1, // 陆地
    POOL = 2, // 水池
    UNSODDED, // 不能种植，出僵尸
};

struct AFieldInfo {
    int nRows;           // 目前游戏中使用的行数
    int rowHeight;       // 一行有多高
    ARowType rowType[7]; // 每行的类型
    bool isNight;        // 是否是夜晚
    bool isRoof;         // 是否是屋顶
    bool hasGrave;       // 是否出墓碑
    bool hasPool;        // 是否有泳池
} aFieldInfo;

// 注意 rowType 的索引是 1~6，不是 0~5
```

[目录](./0catalogue.md)
