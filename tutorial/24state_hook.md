<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-12-05 19:05:08
 * @Description: 
-->
# 状态钩

**首先说明一下，本页教程需要有一定的 C++ 基础才能读懂，C++ 基础比较薄弱的童鞋建议补一下语法再来看，不然可能读的怀疑人生**

你需要了解的语法知识要有：

* 类的构造函数是啥
* 变量的声明周期
* 堆内存申请
* 智能指针 (这个可能理解起来最难)


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
        // 没有任何问题，全局对象的生命周期和 libavz.dll 一致
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

明白了 AStateHook 的作用之后，咱们还需要进一步了解这个类的接口，

```C++
class __APublicStateHook {
public:
    __APublicStateHook(int runOrder)
    {
        _iter = __AStateHookManager::GetHookContainer().emplace(runOrder, this);
    }

    void Init();
    void RunBeforeScript();
    void RunAfterScript();
    void RunEnterFight();
    void RunExitFight();

    virtual ~__APublicStateHook()
    {
        __AStateHookManager::GetHookContainer().erase(_iter);
    }

protected:
    __AStateHookManager::HookContainer::iterator _iter;
    bool _isRunBeforeScript = false;
    bool _isRunAfterScript = false;
    bool _isRunEnterFight = false;
    bool _isRunExitFight = false;

    // 此函数会在 本框架 基本内存信息初始化完成后且调用 void Script() 之前运行
    virtual void _BeforeScript() { }

    // 此函数会在 本框架 调用 void Script() 之后运行
    virtual void _AfterScript() { }

    // 此函数会在游戏进入战斗界面后立即运行
    virtual void _EnterFight() { }

    // 此函数会在游戏退出战斗界面后立即运行
    // 特别注意: 如果用户从主界面进入选卡界面但是又立即退回主界面，此函数依然会运行
    virtual void _ExitFight() { }
};

// hookOrder 默认为 0, 数值越小, AStateHook 越先运行
template <int hookOrder>
class AOrderedStateHook : protected __APublicStateHook {
public:
    AOrderedStateHook()
        : __APublicStateHook(hookOrder)
    {
    }
    static constexpr int HOOK_ORDER = hookOrder;
};

using AStateHook = AOrderedStateHook<0>;
```

首先咱们看最下面的代码 `using AStateHook = AOrderedStateHook<0>;`， 可以看到 AStateHook 是 AOrderedStateHook<0> 的一个别名，
所以咱们得先看 AOrderedStateHook 的定义，发现 AOrderedStateHook 是一个类模板， 而 AOrderedStateHook 本身什么普通成员函数都没有，
这个模板参数中的 runOrder 咱们先不分析是什么，反正咱们现在知道了 AOrderedStateHook 实际上是个壳子，
真正的实现在 __APublicStateHook 里面，所以咱们先看 __APublicStateHook 中的 protected 成员函数，
你会发现都是虚函数，而且你发现这个` virtual void _EnterFight() { }` 实际上就是 Demo 类中重载的那个函数，然后你会发现还有其他三个哥们和这个函数长得很一样，
他们分别是 `_BeforeScript _AfterScript _ExitFight`，至于这些函数的作用，和 _EnterFight 是一致的，只不过本框架调用这几个函数的时机是有差别的，
至于什么时候调用请看相应的注释。

介绍完这四个函数之后，咱们再介绍上面的 `RunBeforeScript RunAfterScript RunEnterFight RunExitFight` 是干什么用的，
比如咱们又来了一个 DemoA 类，然后 DemoA 类的 _EnterFight 的调用时机必须在 Demo 类的 _EnterFight 之后运行，
那么如何保证这一点？就是使用上面这四个 Run 系列的函数，看下面的代码

```C++
#include <avz.h>

class Demo : public AStateHook {
protected:
    bool _isChoosePumpkin = false;
    virtual void _EnterFight() override
    {
        for (auto&& seed : aAliveSeedFilter) {
            if (seed.Type() == AM_NGT_30 || seed.Type() == ANGT_30) {
                _isChoosePumpkin = true;
            }
        }
    }
};

Demo demo;

class DemoA : public AStateHook {
protected:
    virtual void _EnterFight() override
    {
        AToPublicHook(demo).RunEnterFight();
        // some code
    }
};

void AScript()
{
}
```

上述代码中的 `AToPublicHook` 函数实际上就是个转换，源码如下

```C++
template <int hookOrder>
__APublicStateHook& AToPublicHook(AOrderedStateHook<hookOrder>& hook)
{
    return *((__APublicStateHook*)(&hook));
}
```

通过整个转换之后，可以看到 DemoA 类的 _EnterFight 函数内调用了 demo 对象的 RunEnterFight 函数，这就保证了这一点，
那么此时你可能还是有以下疑问:

* 为什么不直接调用 demo 的 _EnterFight 函数，而是调用 RunEnterFight 函数？
* 为什么 AStateHook 要包一层 APublicStateHook 的皮，直接让 Demo 继承 APublicStateHook 不好吗？

下面咱们回答一下这两个问题，对于第一个问题，由于有这种对象之间的依赖关系，假如现在不止有 DemoA 类，
还有一个 DemoB 类需要调用一下 demo 对象的 _EnterFight，此时 DemoA DemoB 都有对应的对象，那么游戏到了战斗界面的时候，
是不是会调用 demo 对象的 _EnterFight 函数两次？那么此时问题就来了，这个函数能被调用两次吗，或者说他应该被调用两次吗？
很明显，这是不应该的，因为这个函数起到了类似初始化的作用，所以应当只被调用一次，那么如何保证，我们看一下 RunEnterFight 这个函数的源码就知道了，

```C++
void APublicStateHook::RunEnterFight()
{
    if (!_isRunEnterFight) {
        _isRunEnterFight = true;
        _EnterFight();
    }
}
```

可以看到这个函数首先检测一个变量 _isRunEnterFight 是不是为 false，如果是，那么就会把 _isRunEnterFight 设置为 true，再运行 _EnterFight，
那么下次再调用这个 RunEnterFight 时，就不会再调用 _EnterFight 了，这样就有了只运行一次保证。

对于第二个问题，为啥要套个壳子，那么咱们需要考虑，如果不套壳子会咋样？很明显，当用户使用 demo. 时， vscode 会有一个成员函数补全列表，
此时因为 `RunBeforeScript RunAfterScript RunEnterFight RunExitFight` 为 public 函数，所以这四个函数也会被用户看到，
那么请问这四个函数应该被用户看到吗，很明显，不应该，而且用户看到了这四个函数是不是还会疑问一下，这是什么奇怪的函数，更有好奇宝宝甚至会调用这四个函数，
很明显，这是我们万万不想的，本来就给使用者封装好了，根本不需要用户去调用，所以就有了 AStateHook 这个壳子。

好了到这里状态钩基本就解释完了，还差最后一个内容，那就是 AStateHook 构造函数中的 runOrder 是什么意思，
这个其实很容易想到，比如 DemoA DemoB 类都创建了相应的对象，那么这些对象的 _EnterFight 的运行总得有个运行顺序吧，
那么好了，顺序是什么？就是由这个 runOrder 来决定的，构造函数参数 runOrder 默认为 0， 数值越小， AStateHook 越先运行，在本框架中，有一些类的运行顺序是特别靠前的，
比如时间操作容器的初始化，帧运行操作容器的初始化，他们的状态钩要比其他对象的状态钩运行都要靠前才行，如果都像 DemoA 那样调用 RunEnterFight 的形式，未免太麻烦了，
因此就有了 runOrder 这个东西，咱们再看一下这个 runOrder 是咋用的吧，这里就直接上本框架的源码了

```C++

// 时间操作容器管理者
class __AOperationQueueManager : public AOrderedStateHook<INT_MIN> { // 运行顺序设置为最靠前

protected:
    // 这两个状态钩会最先运行
    // 这两兄弟主要管理时间操作初始化的工作
    virtual void _EnterFight() override;
    virtual void _BeforeScript() override;
};


// 帧运行操作容器管理者
class __ATickManager : public AOrderedStateHook<INT_MIN> { // 运行顺序设置为最靠前

protected:
    // 主要管理帧运行操作初始化的工作
    virtual void _BeforeScript() override;
};


// 炮管理类
class ACobManager : public AOrderedStateHook<-1> { // 运行顺序设置为 -1

protected:
    virtual void _BeforeScript() override;

    // 此函数会调用 AutoGetList 函数
    // 所以本框架不写 SetList 也是可以的
    virtual void _EnterFight() override;
};


class AItemCollector : public AOrderedStateHook<-1>,
                       public ATickRunnerWithNoStart { // 运行顺序设置为 -1

protected:
    // 此函数会调用 Start 函数
    // 这就是为什么自动收集会自动开启的原因
    virtual void _EnterFight() override;
};

```

实际上看到这里，你可能就想到了一个 DemoA Demo 的 _EnterFight 调用顺序的第二个解决方案，那就是下面的代码

```C++
class DemoA : public AOrderedStateHook<1> {
protected:
    virtual void _EnterFight() override
    {
        // some code
    }
};
```
我们把 DemoA _EnterFight 的运行顺序设置为 1，而 Demo 的运行顺序为 0，这个 Demo 的 _EnterFight 就必定在 DemoA 的之前运行了，
是的，这样做是没有任何错误的，但是本框架提供了一个更好的方法

```C++
class DemoA : public AOrderedStateHook<AAfterHook<Demo>> {
protected:
    virtual void _EnterFight() override
    {
        // some code
    }
};
```

可以和之前的代码相比较，发现就有一处不同，也就是 `AAfterHook<Demo>`， 这个代码为什么更好呢，可以看到可读性更好了，
意思就是 DemoA 的运行顺序就在 Demo 之后，而且我们不再需要知道 Demo 的运行顺序了，直接一个调用一个函数即可，
除了上述作用之外，AAfterHook 还有一个作用，就是支持多个类的先后顺序，例如 DemoA 这个类状态钩需要在 Demo，DemoB，
DemoC 的状态钩后面，那么只需要写成 `AAfterHook<Demo, DemoB, DemoC>` 即可。除了 AAfterHook，本框架还提供了 ABeforeHook，
这个函数的作用我相信大家单看名字就知道是啥意思，我这里就不解释了。

好了，到最后了你可能还是有疑问，如果两个类的 runOrder 一模一样咋办，我可以很明确的告诉你，这两者状态钩的运行顺序纯看编译器的脸，
但是这无所谓了，既然都一样了，那也就是不计较这两者的相对运行顺序了，比如 __AOperationQueueManager 和 __ATickManager，
这两个的状态钩谁先运行都行，反正两者互不干扰，好了这就是 AStateHook 的全部内容了，希望大家能够理解。

[目录](./0catalogue.md)