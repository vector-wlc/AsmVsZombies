<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-12-05 19:05:08
 * @Description:
-->
# 状态钩

本框架给出的入口函数是 `void AScript()` 和协程入口函数 `ACoroutine ACoScript()`，
诚然，将所有的代码写在这两个函数里面实际上已经满足绝大部分的需求了，但是总有那么一些需求是例外的。
比如以下需求：

* 有些用户想在刚注入的时候就做一些事情，但是上面两个函数就无法完成这样的功能，因为代码刚注入时游戏是必定不在战斗界面或者选卡界面的。
* 有些时候需要在关卡刚一开始或者结束的时候做一些工作，虽然这样的代码写在框架提供的入口函数中也可以，但是会比较麻烦，你需要做一些控制。
* 再比如，开启多次生效的时候，全局对象的状态重置有的时候也会是一个大问题，比如这个对象是给其他人使用的

以上种种需求都在迫使着`状态钩`的诞生，为了能够使本文能够让大家各有所需，本文分为两节来说明，
如果你是一般用户，你只需要看懂`基础使用`的内容即可，但是如果你是插件开发者或者想要搞懂状态钩的原理，
那么你需要看懂`进阶`。


## 基础使用（普通用户）

状态钩顾名思义，就是当游戏或者脚本达到某个状态时就调用的函数，举个例子说明一下
```C++
#include <avz.h>

AOnAfterInject(AEnterGame());

void AScript()
{
}
```
上面的代码中的 `AOnAfterInject(AEnterGame());` 这条语句就是使用了状态钩，大家需要注意的是：状态钩的代码是写在 `void AScript()` 函数外面的，
至于为啥这里不说明，有兴趣可以看`进阶`中的内容介绍。AOnAfterInject 的意思即使当注入之后，AEnterGame 在之前的教程已经说明了，所以不再详细介绍。
所以这条代码可以翻译成：当注入之后进入游戏，所以你粘贴复制运行一下，运行效果就是游戏没有经过任何操作就直接进入了选卡或者战斗界面，
但是接下来脚本就什么都不做了（除了有可能开始自动收集外），因为咱们没有在 `void AScript()` 里面写任何东西，所以状态钩的功能再解释就是：
当游戏处于XXX状态时，就做XXX事，咱们再举一个例子，比如说我想在游戏刚一进入战斗界面的时候，就输出一句话：“游戏开始了”，那么就是下面的这条代码：
```C++
#include <avz.h>

ALogger<AConsole> logger;

AOnEnterFight(logger.Info("游戏开始了"));

void AScript()
{
}
```

注意，这次使用的状态钩的名字是 `AOnEnterFight`，翻译一下就是当进入战斗的时候，所以通过上面两个例子，你应该就明白了状态钩是怎么一回事了，
我相信你应该觉得使用起来很方便吧，那么本框架共有多少个状态钩呢？现在的版本共有以下七种状态钩，每种状态钩的触发状态以及使用注意事项都写在注释里面了，
使用之前一定要仔细查看。
```C++
// 此函数会在 本框架 基本内存信息初始化完成后且调用 void AScript() 之前运行
AOnBeforeScript

// 此函数会在 本框架 调用 void AScript() 之后运行
AOnAfterScript

// 此函数会在游戏进入战斗界面后立即运行
AOnEnterFight

// 此函数会在游戏退出战斗界面后立即运行
// 特别注意: 如果用户从主界面进入选卡界面但是又立即退回主界面，此函数依然会运行
AOnExitFight

// 此函数会在每次注入之后运行
// 注意此函数非常危险，此函数内无法使用很多 AvZ 的功能，至于无法使用哪些，
// 用户可以自行踩雷，因为实在是太多了，不想一一枚举
// 因为 AvZ 的初始化发生在进入战斗界面或者选卡界面的时候
AOnAfterInject

// 此函数会在每帧运行 AvZ 主体代码之前运行
// 注意此函数非常危险，最好判断一下当前 PvZ 的状态再使用 AvZ 的内置函数
AOnBeforeTick

// 此函数会在每帧运行 AvZ 主体代码之后运行
// 注意此函数非常危险，最好判断一下当前 PvZ 的状态再使用 AvZ 的内置函数
AOnAfterTick

// 此函数在 AvZ 因异常退出或者卸载前运行
AOnBeforeExit
```
了解了所有的状态钩之后，咱们再举一个比较复杂点的例子，比如我想知道，AvZ 的性能如何？
就是每一帧 AvZ 运行耗时是多少？要想实现这个功能，是需要用到三个状态钩的：`AOnBeforeTick`、`AOnAfterTick`和`AOnExitFight`。

```C++
#include <avz.h>

AOnAfterInject(AEnterGame());
using namespace std::chrono;

uint64_t avzTime = 0;
uint64_t pvzTime = 0;

ALogger<AMsgBox> logger;

time_point<high_resolution_clock> t;
AOnBeforeTick({
    // 当游戏不在战斗界面的时候直接退出
    if (AGetPvzBase()->GameUi() != 3) {
        return;
    }
    auto tmp = high_resolution_clock::now();
    auto cnt = duration_cast<microseconds>(tmp - t).count();
    // 游戏的间隔是不可能大于 1s 的，所以只有当此值小于 1e6 时才有意义
    if (cnt < 1e6) {
        pvzTime += cnt;
    }
    t = tmp;
});

AOnAfterTick({
    // 当游戏不在战斗界面的时候直接退出
    if (AGetPvzBase()->GameUi() != 3) {
        return;
    }
    auto tmp = high_resolution_clock::now();
    auto cnt = duration_cast<microseconds>(tmp - t).count();
    if (cnt < 1e6) {
        avzTime += cnt;
    }
    t = tmp;
});

AOnExitFight({ // 显示统计结果
    logger.Info("AvZ 一局总耗时: #微秒, PvZ 一局总耗时: #微秒, 占比: #%", avzTime, pvzTime, double(avzTime) / pvzTime * 100);
});
void AScript()
{
    // 代码...
}

```

上面的代码就不进行解释了，其中 `AGetPvzBase()->GameUi()` 部分超纲了，但是下一个教程就会马上说到。
还有关于 std::chrono，这是C++标准库里面的计时库，这里也不详细说明了，就是一堆 API 的使用，没啥好说的。

普通用户看到这里就可以散了，除非你是插件开发者或者对状态钩的实现感兴趣，不然不推荐往下再读，因为比较劝退。

## 进阶

这节将介绍状态钩更深层次的使用和一些实现原理。

**首先说明一下，下面教程需要有一定的 C++ 基础才能读懂，C++ 基础比较薄弱的童鞋建议补一下语法再来看，不然可能读的怀疑人生**

你需要了解的语法知识要有：

* 类的构造函数是啥
* 变量的声明周期
* 堆内存申请
* 智能指针 (这个可能理解起来最难)
* 模板


在写脚本的过程中，我们可能需要一些脚本的初始化和收尾工作，比如我们写了一个类，这个类运行之前是需要一些初始化工作，
假如这个初始化工作就是获取是否选择了南瓜卡片，并把这个结果储存在一个变量里面，
然后此类的成员函数在调用的时候会检测一下这个变量，如果选择了南瓜卡片就执行操作 A， 如果未选择就执行操作 B。
对于 C++ 老鸟来说，很容易就把这个初始化的代码写在了类的构造函数里面，例如下面的代码

```C++
#include <avz.h>

class Demo {
public:
    Demo()
    {
        for (auto&& seed : aAliveSeedFilter) {
            if (seed.Type() == AM_NGT_30 || seed.Type() == ANGT_30) {
                _isChoosePumpkin = true;
            }
        }
    }

protected:
    bool _isChoosePumpkin = false;
};

void AScript()
{
}
```

这样写看似没什么毛病，并且标准 C++ 就是要把初始化工作放在类的构造函数里面，但是这样用在本框架里面是有巨大的问题的，问题在于：
这个类的构造函数啥时候才能被调用呢？实际上就是这个类创建的对象动态初始化的时候，这貌似说了句废话，咱们还是具体看下面的几种情况。

```C++
// 使用全局对象

// 创建一个全局对象，此时这个对象的构造函数会在注入完成后的一瞬间调用
// 这样做毫无疑问会导致游戏崩溃或者 demo 这个对象无法工作
// 因为注入的时候是在游戏的主界面的，此时游戏战斗界面的内存根本就没有被分配
// 而 aAliveSeedFilter 会访问这些内存，这就导致了访问野指针的行为，游戏大概率崩溃
Demo demo;

void AScript()
{
}
```

```C++

// 使用局部对象 + lambda = 捕获
void AScript()
{
    // 等待游戏进入战斗界面， 因为判断是否选用南瓜这个操作必须进了战斗界面才能确定
    // 选卡界面是正在选用卡片，所以选卡界面调用 aAliveSeedFilter 还是访问不了正确的内存
    AWaitForFight();

    // 创建一个局部对象，此时这个对象的构造函数调用时机正确
    // 但是随着 AScript() 函数调用结束，这个对象就会死亡，内存会被回收
    Demo demo;

    AConnect(ATime(1, 0), [=]{ // 这个对象被 lambda = 捕获，也就是被拷贝了一次

        // 这个 demo 对象和外面的那个 demo 并不是一个对象，而是拷贝
        // 因此这种写法也不符合要求
        demo.xxx();
    });
}
```

```C++

// 使用局部对象 + lambda & 捕获
void AScript()
{
    // 等待游戏进入战斗界面， 因为判断是否选用南瓜这个操作必须进了战斗界面才能确定
    // 选卡界面是正在选用卡片，所以选卡界面调用 aAliveSeedFilter 还是访问不了正确的内存
    AWaitForFight();

    // 创建一个局部对象，此时这个对象的构造函数调用时机正确
    // 但是随着 AScript() 函数调用结束，这个对象就会死亡，内存会被回收
    Demo demo;

    AConnect(ATime(1, 0), [&]{ // 这个对象被 lambda & 捕获，也就是直接使用了对象本身

        // 这个 demo 对象和外面的那个 demo 是一个对象，因为是引用
        // 但是这种写法犯下的错误更加严重，因为游戏时间到达 ATime(1, 0)，
        // 此时 demo 这个对象的内存早已不再，所以这样做还是访问了野指针
        // 轻则调用失效，重则游戏崩溃
        demo.xxx();
    });
}
```

```C++

// 使用局部静态对象
void AScript()
{
    // 等待游戏进入战斗界面， 因为判断是否选用南瓜这个操作必须进了战斗界面才能确定
    // 选卡界面是正在选用卡片，所以选卡界面调用 aAliveSeedFilter 还是访问不了正确的内存
    AWaitForFight();

    // 创建一个局部静态对象，此时这个对象的构造函数调用时机正确
    // 局部静态对象内存存在时间和 libavz.dll 一致
    // 但是存在一个问题，就是当脚本使用了重新载入脚本函数之后
    // 由于 demo 对象只会初始化一次，因此第二次重新载入脚本之后
    // 该对象的状态就不对了
    static Demo demo;

    AConnect(ATime(1, 0), []{ // 静态对象不需要捕获
        // 如果没用重新载入脚本函数，没有问题
        demo.xxx();
    });
}
```

```C++

// 使用堆内存
void AScript()
{
    // 等待游戏进入战斗界面， 因为判断是否选用南瓜这个操作必须进了战斗界面才能确定
    // 选卡界面是正在选用卡片，所以选卡界面调用 aAliveSeedFilter 还是访问不了正确的内存
    AWaitForFight();

    // 创建一个局部对象，此时这个对象的构造函数调用时机正确
    // 由于使用了堆内存的管理方式，这个对象的内存并不会被自动回收
    // 但是如果不手动 delete 会导致内存泄漏
    // 关于 C++ 内存泄漏，如果不懂，请利用好你的浏览器
    auto demo = new Demo;

    AConnect(ATime(1, 0), [=]{ // 这个对象指针被 lambda = 捕获

        // 这个 demo 对象指针和外面那个对象指针指向同一块内存
        // 所以调用的函数肯定没有任何问题，但是新的问题就来了
        // 啥时候析构这个对象呢？也就是啥时候 delete demo ?
        // 这是一个非常重要的问题，因为如果这个脚本是为了长期挂机的
        // 那么有内存泄漏必定运行时间无法长久
        // 所以这个方式也不是很好
        demo->xxx();
    });
}
```

```C++

// 使用智能指针
void AScript()
{
    // 等待游戏进入战斗界面， 因为判断是否选用南瓜这个操作必须进了战斗界面才能确定
    // 选卡界面是正在选用卡片，所以选卡界面调用 aAliveSeedFilter 还是访问不了正确的内存
    AWaitForFight();

    // 创建一个局部对象，此时这个对象的构造函数调用时机正确
    // 由于使用了智能指针管理内存方式，这个对象的内存会被智能指针管理
    // 当这个对象的内存不再有引用时，智能指针会自动回收对象的内存
    auto demo = std::make_shared<Demo>();

    AConnect(ATime(1, 0), [=]{ // 这个智能指针被 lambda = 捕获

        // 这个 demo 智能指针和外面那个智能指针指向同一块内存
        // 所以调用的函数肯定没有任何问题，而且没有任何内存泄漏的问题
        // 所以这是不是一个完美解？看似是，但是这里我不给予任何评价
        // 只能说这么做是没有犯任何错误的
        demo->xxx();
    });
}
```

总之，你最终发现上面的各种写法（除了最后一个）都有自己一些问题，而且此时你可能有这个想法：
既然 AvZ 中构造函数这么难用，我就写一个普通成员来让用户去主动调用不就行了？
似乎这个想法确实可行，然后你就写出了这样代码：

```C++
#include <avz.h>

class Demo {
public:
    void Init()
    {
        for (auto&& seed : aAliveSeedFilter) {
            if (seed.Type() == AM_NGT_30 || seed.Type() == ANGT_30) {
                _isChoosePumpkin = true;
            }
        }
    }

protected:
    bool _isChoosePumpkin = false;
};

Demo demo; // 构造函数是默认的，几乎不会做出任何导致游戏崩溃的行为，没有任何问题

void AScript()
{
    AWaitForFight();
    demo.Init();  // 进行初始化工作

    AConnect(ATime(1, 0), []{ // 全局对象不需要被捕获
        // 没有任何问题，全局对象的生命周期和 libavz.dll 一致
        demo.xxx();
    });
}
```
但是问题又来了，如果你写的这个类并不是简简单单的只给自己用，你还想分享给其他人用，也就是写成插件的形式，
那么此时别人只是这个类的使用者，他们是不是可能忘写 AWaitForFight();  demo.Init(); 这个语句呢？
那么如果忘写了这两条语句，带来的后果是什么，也就是 demo 这个全局对象没有被正确初始化，那么后面一系列的使用实际上都可能导致游戏崩溃或者失效。

那么看到这里，你可能实在忍不住了，这也不行，那也不行，那你说，咋样才行？？？

你先别急，没看到咱们还没介绍标题那三个字吗？状态钩啊！说实话，我上面写了这么多内容都有可能导致你忘了标题了。
咱们现在就开始介绍状态钩是如何解决上述问题的。

首先说一下状态钩是啥，实际上就是当游戏或者脚本运行到某一状态开始的时候，本框架承诺会调用的函数，具体看下面的例子

```C++
#include <avz.h>

class Demo : public AStateHook {
protected:
    bool _isChoosePumpkin = false;
    virtual void _EnterFight() override // 此函数 AvZ 承诺当游戏一进入战斗界面就会立即调用
    {
        for (auto&& seed : aAliveSeedFilter) {
            if (seed.Type() == ANGT_30) {
                _isChoosePumpkin = true;
            }
        }
    }
};

Demo demo;

void AScript()
{
    // 不再需要用户使用  AWaitForFight();  demo.Init();
    // 因为状态钩完成了初始化工作

    AConnect(ATime(1, 0), []{ // 全局对象不需要被捕获
        // 没有任何问题，全局对象的生命周期和本次注入生效时间一致
        demo.xxx();
    });
}
```

咱们分析一下上面的这个代码，首先咱们自定义的类 Demo 公有继承了一个类 AStateHook，
这个 AStateHook 咱们待会再说，现在接着往下看，其实就是把之前 Init 函数里面的内容搬进了一个特殊的函数
`virtual void _EnterFight() override` 里面，emmm，乍一看这个函数的样子貌似有点劝退，咱们先看咱们认识的内容，
首先 void 指的是这个函数的返回值类型为空，然后这个函数的名字叫做 _EnterFight， () 代表这个函数的参数列表为空，
那么接下来介绍咱们可能不认识的东西，首先就是这个 virtual， 这个的意思就是此函数为虚函数，如果你感兴趣虚函数是什么，
需要自行百度，这里我就不细说了，咱们只需要知道这个函数本框架承诺会在游戏一进入战斗界面就会立即调用，
至于理解虚函数是什么对于使用 AStateHook 不是很重要，然后最后说一下 override，
实际上这个东西不写也没事，但是最好写上，他的意思是 _EnterFight 这个函数重载了父类中的函数，如果编译器发现父类中没有这个函数，
会进行报错，这里我们由于必须重载这个虚函数才能让代码起作用，所以使用 override 实际上就是为了避免犯低级错误，比如有人错写了这个函数的名字，
比如写成了 `virtual void _EnteFight()`， 注意函数名这里是 _EnteFight，少写了一个 r， 这种错误实际上是很难发现的，因为他编译不会报错，
但是一旦加上了 override，编译器就会报错，咱们就能很明显的知道是咱们写错了函数名，防止错误遗留到运行期。
可见，用了 AStateHook 之后，发现 Demo 类的用户并不用担心这个类的初始化问题了，
对于用户来说更加的友好，然后也不存在使用构造函数那种方式出现的一系列问题。

明白了 AStateHook 的作用之后，咱们还需要进一步了解这个类的接口，这个类乍一看比较复杂，
我们先看看库中如何定义这个玩意的，咱们按照递归的形式一层一层往上扒源码。

```C++

// 1
using AStateHook = AOrderedStateHook<0>;

// 2
template <int hookOrder>
class AOrderedStateHook : protected __APublicStateHook {
public:
    AOrderedStateHook()
        : __APublicStateHook(hookOrder)
    {
    }
    static constexpr int HOOK_ORDER = hookOrder;
};

// 3
using __APublicStateHook = __APublicStateHookT<
    __APublicBeforeScriptHook,
    __APublicAfterScriptHook,
    __APublicEnterFightHook,
    __APublicExitFightHook,
    __APublicBeforeTickHook,
    __APublicAfterTickHook,
    __APublicAfterInjectHook,
    __APublicBeforeExitHook>;

// 4
template <typename... Types>
class __APublicStateHookT : public Types... {
public:
    __APublicStateHookT(int hookOrder)
        : Types(hookOrder)...
    {
    }
};
```
我相信你看到这可能已经麻了，但是不要放弃，咱们先看第 4 步的模板是干啥的，它实际上是一个专门继承别的类的工具模板类，
大白话就是他就是偷懒用的，也就是为了少写代码和少出错。即使你没有理解这些代码，咱们还是可以直接看第三步，
你可以理解成 `__APublicStateHook` 这个类继承了七个类:

* __APublicBeforeScriptHook
* __APublicAfterScriptHook
* __APublicEnterFightHook
* __APublicExitFightHook
* __APublicBeforeTickHook
* __APublicAfterTickHook
* __APublicAfterInjectHook

这七个类是不是有亿点点眼熟？没错，他们七个就是状态钩的核心实现。但是呢咱们先不看他们七个的实现，咱们还是先看之前第二步代码，
你又发现了一个模板，这个模板实际上也是工具模板，这里需要大家明白的是，这个类 **protected** 继承了 __APublicStateHook，
这点十分关键，大家先记下来，后续会填这个坑，至于这个模板参数是干啥的，咱们先不研究。

好了，说了上面一大堆东西，咱们需要介绍那七个类的定义是啥了。如果你翻翻源代码，就会发现下面的东西，


```C++
// 此函数会在 本框架 基本内存信息初始化完成后且调用 void AScript() 之前运行
__ADefineHookClass(BeforeScript);

// 此函数会在 本框架 调用 void AScript() 之后运行
__ADefineHookClass(AfterScript);

// 此函数会在游戏进入战斗界面后立即运行
__ADefineHookClass(EnterFight);

// 此函数会在游戏退出战斗界面后立即运行
// 特别注意: 如果用户从主界面进入选卡界面但是又立即退回主界面，此函数依然会运行
__ADefineHookClass(ExitFight);

// 此函数会在每次注入之后运行
// 注意此函数非常危险，此函数内无法使用很多 AvZ 的功能，至于无法使用哪些，
// 用户可以自行踩雷，因为实在是太多了，不想一一枚举
// 因为 AvZ 的初始化发生在进入战斗界面或者选卡界面的时候
__ADefineHookClass(AfterInject);

// 此函数会在每帧运行 AvZ 主体代码之前运行
// 注意此函数非常危险，最好判断一下当前 PvZ 的状态再使用 AvZ 的内置函数
__ADefineHookClass(BeforeTick);

// 此函数会在每帧运行 AvZ 主体代码之后运行
// 注意此函数非常危险，最好判断一下当前 PvZ 的状态再使用 AvZ 的内置函数
__ADefineHookClass(AfterTick);
```
现在一切的秘密都藏在了 `__ADefineHookClass` 这个东西里面，这个东西实际上是一个宏，这个宏的定义如下

```C++
#define __ADefineHookClass(HookName)                                                    \
    class __APublic##HookName##Hook {                                                   \
    public:                                                                             \
        using HookContainer = std::multimap<int, __APublic##HookName##Hook*>;           \
        __APublic##HookName##Hook(int runOrder);                                        \
        virtual ~__APublic##HookName##Hook();                                           \
        void Run##HookName();                                                           \
        static void Reset();                                                            \
        static void RunAll();                                                           \
                                                                                        \
    protected:                                                                          \
        static HookContainer& _GetHookContainer();                                      \
        virtual void _##HookName() {};                                                  \
        HookContainer::iterator _iter;                                                  \
        bool _isRun = false;                                                            \
        inline static bool _isRunAll = false;                                           \
    };                                                                                  \
    template <int hookOrder>                                                            \
    class AOrdered##HookName##Hook : protected __APublic##HookName##Hook {              \
    public:                                                                             \
        AOrdered##HookName##Hook()                                                      \
            : __APublic##HookName##Hook(hookOrder)                                      \
        {                                                                               \
        }                                                                               \
        static constexpr int HOOK_ORDER = hookOrder;                                    \
    };                                                                                  \
    using A##HookName##Hook = AOrdered##HookName##Hook<0>;                              \
    template <int hookOrder>                                                            \
    __APublic##HookName##Hook& AToPublicHook(AOrdered##HookName##Hook<hookOrder>& hook) \
    {                                                                                   \
        return *((__APublic##HookName##Hook*)(&hook));                                  \
    }
```

首先说明一下，宏中##的意思就是把字符串连接起来，注意这个字符串是相对编译器来说的，而不是编译之后的程序来说的，
例如
```C++
#define A(arg) test##arg##test

// 这个宏使得下面两行代码是一致的
int A(hello);

int testhellotest;
```

明白了这一点，咱们再看`__ADefineHookClass`，是不是就清晰一些了，它实际上就是一个代码生成器，用来批量生成状态钩类的定义代码。
这里就有关键的成员函数 `virtual void _##HookName() {}; ` 的原型，那么状态钩类的原理到底是什么，
**首先各个状态钩类的构造函数会将自己的 this 指针放到一个有序表里面**，
**然后框架就会在游戏到达指定状态的时候访问相应的有序表，从中取出 this 指针运行相应的虚函数**，
这两句话就是状态钩的实现核心，记住这个思想，去翻看相应的 .cpp 文件的函数定义源代码，就会一目了然，

现在看了一遍源代码的你，可能会有以下疑问？

* 模板类 AOrderedStateHook 的 hookOrder 有啥用
* 各个状态钩里面的 Run##HookName、Reset 和 RunAll 成员函数有啥用

咱们先解释第一条。首先咱们结合最一开始的例子就知道，状态钩的最初目的就是为了做一些初始化、状态重置或者回收工作的，
也可能有其他作用，既然有初始化和状态重置这个作用，那么就会引入新的问题，顺序问题，就是哪个对象能先初始化呢？
例如 AvZ 内的一些重要内置对象，帧运行管理对象，操作队列管理对象他们最应先被初始化，因为他们是后续一系列操作的基础，
所以 AOrderedStateHook 中的模板参数就是为了确定这个运行顺序的，

```C++
// 帧运行管理类
class __ATickManager : public AOrderedBeforeScriptHook<-32768>
{
    // ...
}

// 操作队列管理类
class __AOpQueueManager : public AOrderedBeforeScriptHook<-32768>, //
                          public AOrderedEnterFightHook<-32768>
{
    // ...
}
```

你会发现这两个类的模板参数都是 -32768，这就保证了这两个类的状态钩有非常高的运行优先级。
所以当你需要为自己状态钩设置运行顺序的时候，一种方法就是指定模板参数，指定模板参数是有两种方式的：

```C++
// 绝对顺序指定
// 将顺序指定为 1
class TestA : public AOrderedStateHook<1> {};

// 相对顺序指定

// 将顺序指定在 TestA 之后，实际上就是 2
class TestB : public AOrderedStateHook<AAfterHook<TestA>> {};

// 将顺序指定在 TestA 之前，实际上就是 0
class TestC : public AOrderedStateHook<ABeforeHook<TestA>> {};
```

使用哪种指定顺序根据使用情况来定，但是推荐使用相对顺序，因为这样的代码兼容性更好一些。

下面开始介绍第二条，各个状态钩里面的 Run##HookName、Reset 和 RunAll 成员函数有啥用，这就涉及到了第二个问题，
既然虚函数重写好了，咋调用呢，直接调用吗？直接调用虚函数乍一看是没有任何问题的，但是实际上还是有问题，
就是同一帧可能运行两次，这是因为状态钩提供了第二种确定运行顺序的方式，比如 TestB 某个对象的状态钩需要在 TestA 某个对象的状态钩之后运行，
除了像以上代码显式指定顺序外，还可以内部直接调用啊，语法就是这样的，

```C++
class TestA : public AStateHook {
    virtual void _EnterFight() override
    {
        // ...
    }
};

TestA testA;
class TestB : public AStateHook {
    virtual void _EnterFight() override
    {
        AToPublicHook(testA).RunEnterFight();
        // ...
    }
};
```

代码中的 `AToPublicHook` 你可能会有疑问是啥玩意，这个待会再说，上面代码就是状态钩的手动调用形式，
这个手动调用形式是为了增加状态钩调用的灵活性的，就是既可以让框架到一个时刻被动调用，也可以用户自己手动调用，
因为有了这个手动调用，就可能导致一个问题，就是一个状态后在一个时刻可能会被调用多次，就是因为这点，
那些重载的 _EnterFight 虚函数是不应该直接调用的，而应该调用 RunEnterFight 这种函数，RunEnterFight 里面带了运行次数检测，
他保证了状态钩在一帧内只运行一次，所以手动调用状态钩时，应该用 AToPublicHook(...).RunXXX() 形式，说完了这些，还剩最后一个问题，
为啥要用 AToPublicHook 这个函数，这个纯纯是因为我觉得状态钩的这些手动调用接口是不应该暴露给普通用户的，
普通用户是基本不可能有这种需求的，而且直接将他们设置成公开的接口，普通用户的编辑器代码补全列表内还会多很多东西，
也会降低用户的使用体验，因此就有了 AToPublicHook 这个玩意，好了状态钩的所有内容就是这么多。

至于有亿些实现没说明，也就不说了，只要看懂了这高阶的内容，看懂那些代码就是小菜一碟。

等等，还有一个问题，就是最一开始说的那些 AOnBeforeScript 啥啥啥之类的咋在高阶内容中没有呢？

回答：去看源码吧，就是个宏定义而已...

[目录](./00_catalogue.md)