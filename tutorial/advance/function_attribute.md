<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 20:12:46
 * @Description: 
-->
# 函数属性

AvZ 中的函数有一个非常重要的属性——是否进入操作队列属性，每个函数的注释其实都有写着自己的属性，如下
```C++
// In Queue
// 发炮函数
// 使用示例：
// pao(2,9)----------------炮击二行，九列
// pao({ {2,9},{5,9} })-----炮击二行，九列，五行，九列
void pao(int row, float col);

// Not In Queue
// 使用此函数可使脚本一次注入多次运行
// 适用于脚本完全无误后录制视频使用
// 注意：使用的关闭效果按键必须在战斗界面才会生效
// 使用示例
// OpenMultipleEffective() -------- 脚本多次生效，默认按下 C 键取消此效果
// OpenMultipleEffective('Q')-------  脚本多次生效，按下 Q 键取消此效果
// OpenMultipleEffective('Q', AvZ::MAIN_UI_OR_FIGHT_UI)-------  脚本多次生效，按下 Q 键取消此效果，多次生效效果在主界面和选卡界面都会生效
// OpenMultipleEffective('Q', AvZ::MAIN_UI)-------  脚本多次生效，按下 Q 键取消此效果，多次生效效果仅在主界面生效
void OpenMultipleEffective(char close_key = 'C', int _effective_mode = MAIN_UI);
```

在这两个函数的注释里面，注释的第一条就是该函数的属性，为什么写在第一行，因为这个属性十分重要，以前的教程我们说到过，AvZ 将操作根据时间点排序存到队列里，然后到设定的时间点在队列中取出操作运行，但这一切的一切是要求该函数的属性为 In Queue，如果这个函数的属性不是 In Queue 的话，排序存队列这些逻辑是根本不会发生的，属性为 Not In Queue 的函数会在 `Script` 函数中直接运行，例如下面的代码
```C++
SetTime(30, 1);
Card(XRK_1, 1, 1);
SetTime(600, 1);
OpenMultipleEffective();
```

在上述代码中 `Card` 函数的属性为 In Queue，所以 `SetTime` 设定的时间点将会对 `Card` 生效，因为 `Card` 操作会被录入队列中，即向日葵会在第一波刷新 30cs 后被种下，我们继续看下面的代码，`OpenMultipleEffective` 这个函数的属性是 Not In Queue，因此 `SetTime` 此时形同虚设，其设定的时间点毫无卵用，因为`OpenMultipleEffective` 并不会被录入操作队列，而是会被直接执行，到这里，我相信大家已经对这个属性的重要性有了一个比较清晰的了解，这里特意说明一下，有些函数的注释中并没有标明自己的属性是什么，这个时候默认为 Not In Queue !!!  例如一个大系列函数：内存读写函数基本都没有标明属性，例如
```C++

// 游戏基址
struct PvZ {
private:
    void operator=(PvZ&& __) { }
    PvZ(PvZ&& __) { }

public:
    PvZ() { }
    // 当前游戏信息和对象
    SafePtr<MainObject> mainObject()
    {
        return *(MainObject**)((uint8_t*)this + 0x768);
    }

    // ...

    // 每帧的时长
    int& tickMs()
    {
        return (int&)((uint8_t*)this)[0x454];
    }
};
```


这些函数甚至连注释都没有，这个时候他们的属性均为 Not In Queue，后续我将会和大家分享这些函数如何使用。

还有一个重要的细节是 `KeyConnect` 函数绑定的操作是否会受到函数是否进队列的影响？答案是不会，`KeyConnect` 函数内部智能处理了这一切，不论 In Queue 还是 Not In Queue，`KeyConnect` 都会在按下按键的时候立即执行这个函数的功能！

最后大家应该会有一个疑问，就是 `OpenMultipleEffective` 这个函数不会录入队列，那我们就想让他在固定的时间点运行该怎么办呢？很简单，使用上一篇教程中的 WaitUntil 阻塞函数就可以实现，**当然这种做法并不推荐，因为使用阻塞函数违背了 AvZ 想让使用者摆脱蛋疼的时间调试的设计初衷，** 因此下一篇教程将会再给大家介绍一个函数——动态插入队列函数 ：`InsertOperation`！


[上一篇 阻塞函数](./wait_until.md)

[目录](../catalogue.md)

[下一篇 动态操作插入](./insert_operation.md)
