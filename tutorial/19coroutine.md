<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 19:12:51
 * @Description: 
-->
# 阻塞再探

本页教程将说明 co_await 的所有用法，希望大家能够熟练的运用。

在正式介绍阻塞时，咱们首先得说明一下协程是什么，大家可以把协程理解成一个线程，
线程咱们知道能够并行运行，相应的，协程能够并发运行，注意并发和并行并不是一个东西，
至于有什么区别，这个和本框架使用协程没有任何关系，所以咱们就不详细的解释了，
咱们接下来写一个小示例。

```C++
#include <avz.h>

// 需要看教程 [日志功能]
ALogger<AConsole> console;

ACoroutine Func1()
{
    co_await ANowDelayTime(100);
    console.Info("Func1() 100");
    co_await ANowDelayTime(300);
    console.Info("Func1() 300");
}

ACoroutine Func2()
{
    co_await ANowDelayTime(200);
    console.Info("Func2() 200");
    co_await ANowDelayTime(400);
    console.Info("Func2() 400");
}

void AScript()
{
    AWaitForFight();
    // 协程需要使用 ACoLaunch 或者 AConnect 启动
    // ACoLaunch 是立即启动协程
    // AConnect 是按照设定的时间或者条件启动协程
    ACoLaunch(Func1);
    ACoLaunch(Func2);
}
```


```
[1, -2147483648][INFO]
=================================
脚本开始运行
=================================
[1, -499][INFO] Func1() 100
[1, -399][INFO] Func2() 200
[1, -199][INFO] Func1() 300
[1, 1][INFO] Func2() 400
```

我相信大家应该能看懂脚本都写的是啥意思，咱们主要看运行结果，
可以发现所有语句都在正确的时间点执行了，Func1() 和 Func2() 
是互不干扰的，这一点可以从日志的打印时间点看出来，因此，
**Func1() 和 Func2() 是并行执行的，他们的表现就像线程一样**，
理解这点十分重要，那么下面的代码，可以想一下运行结果是啥

```
#include <avz.h>

ALogger<AConsole> console;

ACoroutine Func1()
{
    co_await ATime(1, -399);
    console.Info("Func1() 1, -399");
    co_await ATime(1, -599);
    console.Info("Func1() 1, -599");
}

void AScript()
{
    AWaitForFight();
    ASetInternalLogger(console);
    ACoLaunch(Func1);
}
```

运行结果是：
```
[1, -2147483648][INFO]
=================================
脚本开始运行
=================================
[1, -599][INFO] 建立 时间-操作 [1, -399] 连接成功
[1, -399][INFO] 运行 时间-操作 [1, -399]
[1, -399][INFO] Func1() 1, -399
[1, -399][WARNING] 时间-操作 [1, -599] 但是现在时间已到 [1, -399]
[1, -399][INFO] 运行 时间-操作 [1, -599]
[1, -399][INFO] Func1() 1, -599
[1, -399][INFO] 协程退出

```

可以看到，协程函数内部的阻塞是要按照时间顺序书写的，时间顺序反了就没有办法运行，
这一点就是阻塞的缺点，之前其实早就提到了，好了，关于协程是怎么运行的咱们就介绍完了，
现在的关键问题是：既然他有这样的缺点，所以这玩意到底有什么用？咱们看下面的代码：

常年写自动挂机脚本的同志可能写出下面的代码

```C++
bool IsHasGiga()
{
    // aAliveZombieFilter 用法具体查看教程中的 [对象过滤迭代器]
    for (auto&& zombie : aAliveZombieFilter) {
        int zombieRow = zombie.Row();
        // 如果僵尸类型为红眼并且在上半场
        if (zombie.Type() == AGIGA_GARGANTUAR
            && (zombieRow == 0 || zombieRow == 1)) {
            return true;
        }
    }
    return false;
}

void AScript()
{
    // 按下 Q 进行收尾操作
    AConnect('Q', [] {
        // 如果没有红眼直接返回
        if (!IsHasGiga()) {
            return;
        }
        aCobManager.Fire(2, 9);
        // 延迟 373 看看一炮下去还有没有红眼僵尸存活
        AConnect(ANowDelayTime(373), [] {
            if (!IsHasGiga()) {
                return;
            }
            // 如果还有红眼，再来一炮
            aCobManager.Fire(2, 9);

            // 延迟 373 看看一炮下去还有没有红眼僵尸存活
            AConnect(ANowDelayTime(373), [] {
                if (!IsHasGiga()) {
                    return;
                }
                // 如果还有红眼，再来一炮
                aCobManager.Fire(2, 9);
            });
        });
    });
}
```

上述代码你会发现非常多的递归嵌套，如果再多个几层可能会直接要了脚本可读性的命，
那么协程阻塞就是为了解决这个事情的，看下面的代码

```C++
#include <avz.h>

bool IsHasGiga()
{
    // aAliveZombieFilter 用法具体查看教程中的 [对象过滤迭代器]
    for (auto&& zombie : aAliveZombieFilter) {
        int zombieRow = zombie.Row();
        // 如果僵尸类型为红眼并且在上半场
        if (zombie.Type() == AGIGA_GARGANTUAR
            && (zombieRow == 0 || zombieRow == 1)) {
            return false;
        }
    }
    return true;
}

void AScript()
{

    // 按下 Q 进行收尾操作
    AConnect('Q', []() -> ACoroutine {
        // 一共尝试三次
        for (int i = 0; i < 3; ++i) {
            if (!IsHasGiga()) {
                co_return;
            }
            aCobManager.Fire(2, 9);
            if (i != 2) { // 最后一次不需要再进行阻塞延迟了
                co_await ANowDelayTime(373);
            }
        }
    });
}
```
请注意这个 AConnect 函数右边可是 `[]() -> ACoroutine` 一定要写成这样，不然没法
使用 co_await，可以看到使用连接和阻塞的结合，再也没有了嵌套调用，
代码的可读性一下就变高了，上述 for 循环尝试了三次，如果这三次尝试不用阻塞，
纯用连接，那么就会有三次 AConnect 的嵌套调用，想一想就令人头皮发麻。

咱们再举一个例子，就是实现卡片好了立即使用卡片这个功能，如果不使用阻塞，就是接下来这种写法，
你需要注意很多的细节，比如为啥要用动态内存申请? (`std::make_shared<ATickRunner>()`)，
为啥这里不能用全局的 ATickRunner 对象? 为啥不能用局部的 ATickRunner 对象? 
这里就不解释了，因为涉及到的知识点比较多，而且解释这个东西不是本节的重点。

```C++
void RecoverCard(APlantType plant, int row, float col)
{
    // 卡片好了直接种
    if (AIsSeedUsable(plant)) {
        ACard(plant, row, col);
        return;
    }
    // 卡片没好，咱们得进行检测
    // 注意这里直接用 cd 的方式可能是不对的，因为还有阳光的限制，因此咱们得每帧检测
    // AIsSeedUsable 会检测卡片是否能种
    // 至于这个 std::make_shared 是啥，懂得自然懂，不懂的也不需要懂
    // 因为咱们不需要这种写法了
    auto tickRunner = std::make_shared<ATickRunner>();
    tickRunner->Start([=] {
        if (AIsSeedUsable(plant)) {
            ACard(plant, row, col);
            tickRunner->Stop();
        }
    });
}
```

咱们再看协程阻塞版本

```C++
void RecoverCard(APlantType plant, int row, float col)
{
    // 由于本框架实现的协程只能是无参形式，所以只能通过 Lambda 捕获的方式模拟有参协程
    ACoLaunch([=]() -> ACoroutine {
        // 卡片好了直接种
        if (AIsSeedUsable(plant)) {
            ACard(plant, row, col);
            co_return; // 注意这里是 co_return，协程里面不能用 return
        }
        // 等待卡片能用
        co_await [=] { return AIsSeedUsable(plant); };
        ACard(plant, row, col);
    });
}
```

脚本中的注意点我都写在注释里面了，大家可以看到使用了协程阻塞代码瞬间就变得清爽了起来，
直接就是一个符合人类阅读的等待卡片能用，然后直接种卡片就行了，非常舒服，
这里我需要解释一下 co_await
后面跟的是啥玩意，

co_await 是可以跟两个东西的
* ATime(x, y) : 意思为等到时间点到达 (x, y) 时释放阻塞
* bool Functor() : 意思是当这个 Functor 返回 true 的时候释放阻塞

很明显上面这个 `co_await [=] { return AIsSeedUsable(plant); };` 就是用了第二种形式
，那么说到这里，我相信你已经完全明白了 co_await 巨大优势，
它本质上可以理解为创建了一个小线程，这个小线程阻不阻塞和外面的世界没有任何关系。

最后再来一个例子吧

```C++
// 纯连接版本
// 按下 Q 后过 100cs 种下一张卡
AConnect('Q', [] {
    AConnect(ANowDelayTime(100), [] { ACard(1, 1, 1); });
});

// 协程阻塞版本
// 按下 Q 后过 100cs 种下一张卡
AConnect('Q', []()-> ACoroutine {
    co_await ANowDelayTime(100);
    ACard(1, 1, 1);
});
```

还是那个说法，协程阻塞拥有比纯连接更高的可读性。
**因此咱们外层使用 AConnect, AConnect 内部使用协程，也就是整体连接，局部阻塞**，
这一下就把两者的优缺点进行了强势互补，为什么咱们不喜欢用阻塞，
就是因为咱们得根据时间点调整咱们的代码书写顺序，但是现在咱们配合连接可以做到
**只是在一个小局部使用阻塞，这样阻塞的劣势将变得几乎没有，因为就一个小局部，时间点太好调了**。
而且也保证了脚本代码的可读性，这就是协程阻塞的真正威力。

[目录](./0catalogue.md)
