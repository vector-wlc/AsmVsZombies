<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-19 23:56:22
 * @Description: 
-->
# 内存读取

再读内存之前，咱们首先得会读内存指针表，首先咱们先看一下指针表长啥样

**注意：本教程中用的指针表好像已经比较旧了，但是对于演示指针表怎么用足够**

打开**植僵工具箱这个网站** : https://pvz.tools/ ，然后往下拉找到 **数据结构解析**，
然后找到 **指针表**，最后点击 **本站同步的镜像文件**，然后显示如下

```
┌─6A9EC0\\基址
├─C\\[std::string]公司名称
├─28\\[std::string]完整公司名称
├─44\\[std::string]产品名称
├─60\\[std::string]窗口标题
├─7C\\[std::string]注册表路径
├─98\\[std::string]游戏目录
├─D0\\[双浮点]音量
├─D8\\[双浮点]音效
├─┬─320\\鼠标和窗口
│ ├─28\\不断自增的值
│ ├─84\\[逻辑值]游戏窗口获得焦点则为true
│ ├─88\\[窗口]顶层窗口
│ ├─8C\\[窗口]鼠标按住的窗口
│ ├─90\\[窗口]鼠标所在的窗口
...
```

我们首先会看到这一大串的竖线和伸出的横线，乍一看挺劝退，但实际上挺简单，
比如咱们随便找一个咱们要读的东西，比如游戏刷新一帧需要多少时间，
在指针表中搜索 `每帧的时长`， 最后结果如下
```
├─354\\Invis窗口句柄
├─454\\每帧的时长(毫秒)
├─4AC\\画面是否刷新
```

咱们发现他的地址是 454，先记下来，然后咱们发现 454 左边是有一个竖线的，
说明它上面还有`爸爸`，咱们得找到他`爸爸的地址`才行，咱们就顺着这个竖线往上爬，
最后找到了的地址如下 
```
┌─6A9EC0\\基址
├─C\\[std::string]公司名称
```

注意这个 6A9EC0 ，这就是咱们要找的 `爸爸`，因为这条竖线上他已经到顶了，
所以现在咱们找到了两个地址，454 和 6A9EC0，那么咱们把这个内存都出来呢？
代码如下：
```C++
#include <avz.h>

ALogger<AConsole> console;

void AScript()
{
    int tickInterval = MPtr<APvzStruct>(0x6A9EC0)->MRef<int>(0x454);
    console.Info(std::to_string(tickInterval));
}
```

运行结果：

```
[1, -2147483648][INFO] 10
```

你会发现黑色的控制台打印了一个10，这是什么意思呢，意思就是 PvZ 一帧的时长是 10ms，也就是
1cs，芜湖，确实读对了！那么上面的代码是啥意思呢，咱们继续解释，想一下咱们之前找到的那两个地址，454 和 6A9EC0，
注意，我们之前说了 6A9EC0 是 454 的爸爸，所以要想得到 454 的值，就先得获取 6A9EC0，
因此 `MPtr<APvzStruct>(0x6A9EC0)` 就是获取 6A9EC0 是啥，
那么现在需要解释一下 `MPtr` 是啥，它实际上就是获取指针表中下一层偏移的意思，
**它适用于该地址下面还有孩子的情况**，0x6A9EC0 这个地址下面明显有大量的孩子，所以使用
MPtr 来读，然后 `<APvzStruct>` 是啥？首先 `<>` 在 C++ 中代表模板的实例化，也就是说，
咱们到底要让 MPtr 读什么类型，好了 `<>` 里面的内容就是咱们要读的类型，也就是 APvzStruct，
这里先给大家挖个坑，不给大家解释 APvzStruct 是啥，咱们看最后的 0x6A9EC0，你会发现咱们找到
的地址是 6A9EC0，前面没有 0x，怎么这里多了一个 0x？
实际上指针表中的所有数值都是**十六进制数**，在 C++ 语言中，表示一个十六进制数就得用前缀 0x，
所以就是 0x6A9EC0 了，介绍完了这一个，继续介绍 `MRef<int>(0x454)`是什么东西，首先 MRef 
这个接口的意思是读取数据的引用，它适用于**该地址下没有孩子的情况**， 0x454 是一个数据，他下面
没有孩子，那么为什么 `<>` 中是 int 呢？那么现在大家需要记住一个隐形规则，
**指针表中没有指定类型的数据，一律当作 int 来处理**，0x454 没有标明数据的类型，所以他就是 int,
好了，剩下的就没有什么可解释的了。那么什么是标明类型的数据呢？咱们找几个看看:
```
对于逻辑值，我们使用 <bool> 来读取
│ ├─54\\[逻辑值]true则显示画面
│ ├─58\\[逻辑值]鼠标按下则为true(暂停时为0并不再记录)
│ ├─59\\[逻辑值]鼠标在画面内则为true(暂停时为0并不再记录)
```
```
对于浮点值，我们使用 <float> 来读取
│ │ ├─2C\\[浮点]横坐标
│ │ ├─30\\[浮点]纵坐标
│ │ ├─34\\[浮点]僵尸横向相对速度
```
```
对于字节值，我们使用 <uintx_t> 来读取，
注意看到底是几个字节，比如 1 字节就是用 uint8_t， 2 字节就是 uint16_t
所以 x = 8 * 字节数值
│ │ ├─BE\\[1字节]上梯子时的影子跟随
│ │ ├─140\\[1字节]0和1之间变换
│ ├─54D4~54F4\\[1字节]出怪种类
45DEE0\\[2字节]非阳光植物攻击初始倒计时误差修正
```
还有要注意的一点是，你发现 45DEE0 这个地址前面没有竖线，这咋读？
很简单这么读: 
```C++
MRef<uint16_t>(0x45DEE0);
```
好了说了这么多，咱们读个现在关卡内的阳光数吧
搜索指针表找到 5560
```
│ ├─5560\\阳光值
```
然后沿着竖线找他的爸爸，结果是 768
```
├─┬─768\\[窗口]关卡界面窗口
│ ├─28\\\窗口刷新次数
```
然后再沿着 768 的竖线找到 0x6a9ec0

```
┌─6A9EC0\\基址
├─C\\[std::string]公司名称
```
所以咱们读取的代码如下

```C++
#include <avz.h>

ALogger<AConsole> console;

void AScript()
{
    int sunVal = MPtr<APvzStruct>(0x6A9EC0)->MPtr<APvzStruct>(0x768)->MRef<int>(0x5560);
    console.Info(std::to_string(sunVal));
}
```
运行结果:
```
[1, -2147483648][INFO] 8000
```
是不是特别简单，看完以上内容，我相信大部分数据打家都会读了，但是还有一类数据可能比较吃力，就是下面这种:
```
│ ├─┬─AC\\植物属性(+14C下一个)
│ │ ├─0\\[指针]=基址
│ │ ├─4\\[指针]=当前游戏信息和对象
│ │ ├─8\\植物横坐标
│ │ ├─C\\植物纵坐标
```
这种 +14C 下一个是啥意思，实际上这个玩意是个数组，咱们按照之间的读法试试看，比如就读取植物横坐标，
```C++
// 0x6a9ec0 -> 0x768 -> 0xac -> 0x8
#include <avz.h>

ALogger<AConsole> console;

void AScript()
{
    int x = MPtr<APvzStruct>(0x6A9EC0)
                ->MPtr<APvzStruct>(0x768)
                ->MPtr<APvzStruct>(0xAC)
                ->MRef<int>(0x8);
    console.Info(std::to_string(x));
}
```
结果是: 
```
[1, -2147483648][INFO] 40
```
也就是植物内存数组里面的第一个植物的横坐标是 40，那么咱们想读第二个植物的咋整？
```C++
// 0x6a9ec0 -> 0x768 -> 0xac+0x14C * i -> 0x8
#include <avz.h>

ALogger<AConsole> console;

void AScript()
{
    int x = MPtr<APvzStruct>(0x6A9EC0)
                ->MPtr<APvzStruct>(0x768)
                ->MPtr<APvzStruct>(0xAC + 0x14C * 1)
                ->MRef<int>(0x8);
    console.Info(std::to_string(x));
}
```
指针表里面都说了，+14C 下一个，所以咱们就应该这么读，看看都不对，发现一运行，很快啊，啪的一声游戏崩溃了，
你会疑惑这是为啥？ 原因很简单，因为咱们加错地方了，咱们换个加的地方不就行了。
```C++
// 0x6a9ec0 -> 0x768 -> 0xac -> 0x8 + 0x14C * i 
#include <avz.h>

ALogger<AConsole> console;

void AScript()
{
    int x = MPtr<APvzStruct>(0x6A9EC0)
                ->MPtr<APvzStruct>(0x768)
                ->MPtr<APvzStruct>(0xAC)
                ->MRef<int>(0x8 + 0x14C * 1);
    console.Info(std::to_string(x));
}
```
你会发现，这次咱们运行对了，所以下一个的意思就是让咱们**加到最后的数据层**，至于为啥就不解释了，
因为后续有更好的做法，那么现在你有没有一种大胆的想法，我想得到内存中所有植物的横坐标，
但是貌似咱们现在并不知道内存中有多少个植物，此时可能直接会想到，去游戏中数数有多少个植物不就行了，
答案是：**这种方法即低效而且是错误的**，正确的做法是直接去内存中找有多少个植物，我们找一下：
```
│ ├─B0\\最多时植物数
│ ├─BC\\当前植物数
```
咱们就发现了这两个关键内存地址信息，那么他们有啥区别？ **B0 是植物内存数组的大小，
BC 是目前场地上活着的植物的数目**，因此咱们如果想遍历所有的植物，应该用的就是 B0，
得到内存中所有植物(包括死了的植物)的横坐标代码如下:

```C++
#include <avz.h>

ALogger<AConsole> console;

void AScript()
{
    int plantArraySize = MPtr<APvzStruct>(0x6A9EC0)
                             ->MPtr<APvzStruct>(0x768)
                             ->MRef<int>(0xB0);
    for (int i = 0; i < plantArraySize; ++i) {
        int x = MPtr<APvzStruct>(0x6A9EC0)
                    ->MPtr<APvzStruct>(0x768)
                    ->MPtr<APvzStruct>(0xAC)
                    ->MRef<int>(0x8 + 0x14C * i);
        console.Info(std::to_string(x));
    }
}
```
结果是:
```
[1, -2147483648][INFO] 40
[1, -2147483648][INFO] 40
...
[1, -2147483648][INFO] 120
[1, -2147483648][INFO] 120
```
好了，到目前为止，PvZ 中所有的内存地址的读法咱们就都学会了。

但是还记得我之前说 0xAC 实际上是个数组吗，那么在 C/C++ 中如果想获取数组中第 x 个元素的语法是啥呢？
应该是 arr[x] 或 *(arr + x) 吧，大家实际上用的多的是第一种，而不是第二种，实际上第二种的用法就是
`MRef<int>(0x8 + 0x14C * i)`，但是你想说，不对啊，人家 C/C++ 里面直接加一个 x 就行，不用再乘个
0x14C 之类的东西啊，这是为啥呢？原因很简单，因为咱们在定义数组的时候是有类型这么个概念的，
比如咱们定义一个 int arr[]，那么在用 *(arr + x) 的时候，实际上地址偏移是 `+ 4 * x`，因为 int 是四字节的，
我相信这点大家应该都明白，因此咱们这里这个 APvzStruct 实际上是个空结构体，他的大小只有 1 字节，
因此咱们需要 `+ 0x14C * i`，那么为了实现类似 C/C++ 数组的写法，咱们得整一个大小为 0x14C 的结构体，
然后用这个结构体来访问内存不就方便多了，直接开整！
```C++
#include <avz.h>

ALogger<AConsole> console;

// 定义一个结构体，大小为 0x14C 个字节
struct Plant : public APvzStruct {
    uint8_t data[0x14C];
};

void AScript()
{
    int plantArraySize = MPtr<APvzStruct>(0x6A9EC0)
                             ->MPtr<APvzStruct>(0x768)
                             ->MRef<int>(0xB0);
    // 强转为 Plant 类型的指针
    // 这样咱们就可以用 C/C++ 数组的读取方式了
    auto plantArray = (Plant*)(MPtr<APvzStruct>(0x6A9EC0)
                                   ->MPtr<APvzStruct>(0x768)
                                   ->MPtr<APvzStruct>(0xAC));
    for (int i = 0; i < plantArraySize; ++i) {
        int x = (plantArray + i)->MRef<int>(0x8);
        console.Info(std::to_string(x));
    }
}
```
上述代码的执行结果和咱们的第一版执行结果是一样的，那么上述代码中的 `plantArray + x` 就是和 C 的语法一样了，
那么既然可以用这种语法，那么 [] 应该也是可以的吧，当然可以，代码如下：
```C++
#include <avz.h>

ALogger<AConsole> console;

struct Plant : public APvzStruct {
    uint8_t data[0x14C];
};

void AScript()
{
    int plantArraySize = MPtr<APvzStruct>(0x6A9EC0)
                             ->MPtr<APvzStruct>(0x768)
                             ->MRef<int>(0xB0);
    auto plantArray = (Plant*)(MPtr<APvzStruct>(0x6A9EC0)
                                   ->MPtr<APvzStruct>(0x768)
                                   ->MPtr<APvzStruct>(0xAC));
    for (int i = 0; i < plantArraySize; ++i) {
        int x = plantArray[i].MRef<int>(0x8);
        console.Info(std::to_string(x));
    }
}
```
这样一来代码可读性明显高了很多，那么咱们想啊，要知道人脑记这些乱七八糟的内存地址肯定记不住，
所以为了更适合人类阅读，咱们可以给 Plant 类加个成员函数来读取，就像下面的代码这样，
```C++
#include <avz.h>

ALogger<AConsole> console;

struct Plant : public APvzStruct {
    uint8_t data[0x14C];
    // 加一个成员
    // 实际上就是对内存地址的封装
    int& X()
    {
        return MRef<int>(0x8);
    }
};

void AScript()
{
    int plantArraySize = MPtr<APvzStruct>(0x6A9EC0)
                             ->MPtr<APvzStruct>(0x768)
                             ->MRef<int>(0xB0);
    auto plantArray = (Plant*)(MPtr<APvzStruct>(0x6A9EC0)
                                   ->MPtr<APvzStruct>(0x768)
                                   ->MPtr<APvzStruct>(0xAC));
    for (int i = 0; i < plantArraySize; ++i) {
        // 使用这个加的函数
        int x = plantArray[i].X();
        console.Info(std::to_string(x));
    }
}
```
你会发现可读性又高了一层楼，而且人脑好记多了不是！所以这就是咱们读取数组的最终形式，看到这里，
大家应该都会了，但是这里还是要提醒大家的一点是，MRef 这个函数读出来的可是个引用！引用意味着什么？
意味着可以读也可以改！因此，还记得咱们第一次读出来的游戏每帧时长的例子吗，如果咱们想让游戏十倍速
运行怎么做？

```C++
#include <avz.h>
void AScript()
{
    // 原来的值是 10，也就是游戏一帧的时长是 10ms， 现在改成 1，也就是游戏一帧的时长是 1ms
    // 因此实现了 10 倍速
    MPtr<APvzStruct>(0x6A9EC0)->MRef<int>(0x454) = 1;
}
```
好了这次没有意外，一运行游戏确实是 10 倍速了，函数 `ASetGameSpeed` 就是这么做的。

然后咱们要介绍一下 APvzStruct 是个什么东东，看源码:

```C++
class APvzStruct {
    __ADeleteCopyAndMove(APvzStruct);

public:
    template <typename T>
    __ANodiscard T& MRef(uintptr_t addr) noexcept
    {
        return (T&)((uint8_t*)this)[addr];
    }

    template <typename T>
    __ANodiscard T* MPtr(uintptr_t addr) noexcept
    {
        return *(T**)((uint8_t*)this + addr);
    }

    template <typename T>
    __ANodiscard T MVal(uintptr_t addr) noexcept
    {
        return (T)((uint8_t*)this + addr);
    }
};
```

emmm，__ADeleteCopyAndMove(APvzStruct); 这条代码是说，咱们访问的是 PvZ 的内存，所以这个对象不可复制和移动的，
因为这样做没有任何意义，如果理解不了也没什么关系，咱们关键是看下面这三个函数，你会发现前两个是老朋友了，
所以当时咱们为什么要模板实例化成 `<APvzStruct>` 呢？就是为了链式用这三个函数！至于这三个函数的源码大家看不懂
的话很正常，咱们也不浪费篇章解释了，因为懂了这些对于使用本框架来说没什么意义，那么 MVal 是干啥的？这个名字可能有点误解，
实质上它是用来读内置类型数组的，比如僵尸出怪列表之类的东西，这就不多说了，大多数情况下 MRef 和 MPtr 就够用了。
```
// 出怪列表
__ANodiscard uint32_t* ZombieList() noexcept
{
    return MVal<uint32_t*>(0x6b4);
}
```

最后咱们还需要介绍的是，为了方便大家编写脚本，本框架已经贴心的将很多内存地址封装了，
例如 0x6a9ec0 这个地址大家可以用 AGetPvzBase 这个函数获取，0x768 可以用 AGetMainObject 获取，
然后咱们读的阳光值已经封装好了，就是 AGetMainObject()->SunVal()，植物僵尸卡片内存数组也有，他们分别的是 
AGetMainObject()->PlantArray()， AGetMainObject()->ZombieArray()， AGetMainObject()->SeedArray()，
封装方式就是咱们之前说的那些，不信你去看源码，然后为了更更更方便大家去读这些内存数组的，本框架还设计了
对象过滤器，也就是 [对象过滤迭代器]，读到这里，是不是所有的知识都串起来了！

OK，这就是本节的全部内容了，实际上这节的内容只为了一些挂机或者键控无炮的玩家而写的，但是仅凭本节的内容，
可能还是不够，还需要大家有足够的 C++ 功底（至少要把 C 的那部分看完，变量，函数，指针，结构体，宏，枚举），
如果没有足够的内功，是很难写出复杂的无炮键控的，其次，还需要学习本框架中至少两个头文件中的内容，
他们分别是，`avz_memory.h 和 avz_pvz_struct.h`，它们分别对应着本框架封装的内存操作函数和内存指针表，
所以你至少要知道这两个头文件中的内容才能做好更深层次的内容，好了就是这样了。


[目录](./0catalogue.md) 