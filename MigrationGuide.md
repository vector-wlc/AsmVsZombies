<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-19 18:36:24
 * @Description:
-->

# 版本迁移文档

## Nightly

1. `AAliveFilter<APlant>` 不再包含已被蹦极抱走的植物和已生效的窝瓜；`AAliveFilter<AZombie>` 不再包含选卡界面的僵尸。
如果确实需要遍历这些对象，请使用 `ABasicFilter`

## 2.8.0 240713

1. `ATimeline` 的合并运算符改为 `&`，`ATimeline + ATimeline` 被弃用
2. `AAsm::RemoveZombie` 不再会产生死亡动画或掉落战利品；原本的函数改名为 `AAsm::KillZombie`
3. 在 DSL Shorthand 240713 中，窝瓜函数的名称改为 `W`

## 2.7.1 240510

1. 日志类内部实现更改为 `std::format`，因此 `{}` 在任何情况下都能作为格式化字符串使用，如果想显示出 `{}`，请使用 `{{}}`

## 2.3.3 230501

1. AWaitUntil 弃用，请使用 co_await 来代替此函数

## 2.0.0 221208

1. AStateHook 的四个虚函数的名字发生了变化

```C++
BeforeScript() -> _BeforeScript()
AfterScript() -> _AfterScript()
EnterFight() -> _EnterFight()
ExitFight() -> _ExitFight()
```

## 221124 2.0.0 preview

1. 这是一个重大更新，使用 221124 2.0.0 preview 需要重新下载新的环境开发包

2. 会带来 VSCode 插件的不兼容性，需要修改编译命令为 `-m32 -static -std=c++20 -g -Wall "__FILE_NAME__" -I "__AVZ_DIR__/inc" -lavz -lgdi32 -L "__AVZ_DIR__/bin" -shared -o "./bin/libavz.dll"`，注意粘贴复制的时候千万不要有换行符， 然后这个设置需要打开扩展栏，找到 AvZ 扩展，然后右键 AvZ 右下角的齿轮找到扩展设置， 将 `Avz Compiler Cmd` 修改为上面的内容。

3. 其他更新请看新的图文教程

## AvZ1 221001

1. 由于 SafePtr 已被弃用, 在脚本中使用 `SafePtr<Type>` 的用户直接使用原始指针 Type 即可,
**同时函数 toUnsafe 被删除**

```C++
// 220630
SafePtr<Zombie> zombie_array = GetMainObject()->zombieArray(); // 已弃用
zombie_array.toUnsafe(); // 没问题

// 221001
Zombie* zombie_array = GetMainObject()->zombieArray();
zombie_array.toUnsafe(); // 错误
```

注意 : 使用习惯为 `auto` 的用户可以无视

2. 由于一个设计失误, `Fliter` 的 `*` 运算符在之前的版本返回了错误的结果,
为了兼容性, 221001 版本依然支持之前的写法, 但是不推荐使用(已弃用).

```C++
// 220630
// 已弃用
AliveFilter<Zombie> zombie_filter;
for (auto&& zombie : zombie_filter) {
    if (zombie->type() == HY_32) {
        // some code
    }
}

// 221001
AliveFilter<Zombie> zombie_filter;
for (auto&& zombie : zombie_filter) {
    if (zombie.type() == HY_32) {
        // some code
    }
}
```
