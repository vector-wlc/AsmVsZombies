<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-06-30 11:48:59
 * @Description: 
-->
# 变量内存管理


大多数用户是使用 AvZ 库的，但是有些高水平的用户不止步于此，他们常常想自己开发一些实用的但是 AvZ 库中没有的功能，
但是这会遇到一个问题，由于 AvZ 架构的特殊性，一些功能可能无法运行，接下来我们将举例说明。

## 局部变量
在 AvZ 中使用局部变量需要特别小心，例如下面的代码
```C++
class MyClass {
public:
    void Func();
};

void Script()
{
    MyClass my_class;
    AvZ::InsertTimeOperation(10, 1, [&my_class]() {
        my_class.Func();
    });
}
```
如果你是想在 Script 执行的时候创建一个对象，然后在 (10, 1) 这个时间点运行 Func() 函数，那么上述代码是完全错误的，
第一 Script 这个函数如果其中没有调用 WaitUntil 函数，他在注入完成之后的一瞬间就会被执行完成，
也就是说在 (10, 1) 这个时间点， my_class 早就被析构了，因此这就会造成野指针的问题，但是聪明的你会说，
我使用 new 申请内存不就行了，类似这样
```C++
class MyClass {
public:
    void Func();
};

void Script()
{
    MyClass* my_class = new MyClass;
    AvZ::InsertTimeOperation(10, 1, [my_class]() {
        my_class->Func();
    });
}
```
这样写确实不会造成野指针的问题，但是这样做明显会造成内存泄漏，如果你写的脚本是需要长期挂机的，这样写早晚会被系统杀死或者卡死系统，
因为内存会被申请的越来越少，直至枯竭，所以为了解决这个问题，推荐使用 C++ 标准库中的智能指针，shared_ptr
```C++
#include <memory>
class MyClass {
public:
    void Func();
};

void Script()
{
    auto my_class = std::make_shared<MyClass>();
    AvZ::InsertTimeOperation(10, 1, [my_class]() {
        my_class->Func();
    });
}
```
请注意，一定要 `#include <memory>`, 因为智能指针在这个头文件中，使用了智能指针就可以比较完美的解决访问野指针和内存泄漏的问题了。


## 全局变量
**需要 AvZ 版本 >= 220630**

对于全局变量，就没有局部变量那些内存泄漏和访问野指针的问题了，但是又会带来新的问题，
就是状态初始化和状态收尾比较难以处理。

比如有这样一个需求，我想写一个自动垫垫材类，那么有些初始化的操作就必须是读取游戏选卡的内存信息，
那么你很有可能这样写。

```C++
class DianCai {
public:
    DianCai()
    {
        // 获取选卡内存信息的代码
    }

    // 其他函数
    void setDianCai();
};

DianCai diancai;
void Script()
{
    diancai.setDianCai();
}
```

这样写看起来没啥问题，对于正常程序来说确实没啥问题，但是对于 AvZ 是有很大的问题的，
因为全局变量构造函数的调用是比 AvZ 一些内存初始化操作还要靠前的，也就是说这样写出来的代码读取内存的时候，
比如调用了 GetMainObject(), 但是此时这个函数并不会提供给你正确的内存地址，这是因为 AvZ 的内存初始化的代码还没有运行。
这只是全局变量中的一个小问题，其实还有很多的问题，比如 AvZ 内置的炮操作类，在每次 AvZ 内存初始化完成后会立即将自己的状态也进行初始化，
例如对锁炮记录进行清零等等，但是这样的代码是不能写在构造函数中的，还是之前那个问题，为了解决全局变量的这个问题，AvZ 新增了 GlobalVar。

```C++
class DianCai : AvZ::GlobalVar {

public:
    void virtual beforeScript() override
    {
        // 代码会在 AvZ 基本内存信息初始化完成后且调用 void Script() 之前运行
    }

    void virtual afterScript() override
    {
        // 代码会在 AvZ 调用 void Script() 之后运行
    }

    void virtual enterFight() override
    {
        // 代码会在游戏进入战斗界面后立即运行
        // 在这里写上读取游戏选卡的内存信息就可以
    }

    void virtual exitFight() override
    {
        // 代码会在游戏退出战斗界面后立即运行
    }

    // 其他函数
    void setDianCai();
};
DianCai diancai;
void Script()
{
    diancai.setDianCai();
}
```

**通过这样实现自动垫垫材类，AvZ就会自动调用那四个特殊的函数，不需要用户手动调用，此外，这四个函数并不需要都实现，类的设计者按需实现即可。**
还要注意的就是，virtual 和 override 这两个单词是必不可少的，理论上 virtual 可以不用写，因为编译器会为你自动补上，
override 也不用写，这个是程序猿告诉编译器这个函数必须是重载的，但是我还是强烈建议大家写的时候要写上这两个单词，因为这不仅提高了可读性，
而且有的时候会为你节省不少的时间，不要为了少打几个字母而调试的时候浪费更多的时间。

[上一篇 对象过滤迭代器](./iterator.md)

[目录](../catalogue.md)

[下一篇 插件](./extension.md)