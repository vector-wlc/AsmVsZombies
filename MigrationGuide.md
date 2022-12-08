<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-19 18:36:24
 * @Description: 
-->
## 版本迁移文档

### 221124 2.0.0 preview -> 2.0.0 221208

1. AStateHook 的四个虚函数的名字发生了变化

```C++
BeforeScript() -> _BeforeScript()  
AfterScript() -> _AfterScript()  
EnterFight() -> _EnterFight()  
ExitFight() -> _ExitFight()  
```

### 221001 -> 221124 2.0.0 preview

1. 这是一个重大更新，使用 221124  2.0.0 preview 需要重新下载新的环境开发包

2. 会带来 VSCode 插件的不兼容性，需要修改编译命令为 `-m32 -static -std=c++20 -g -Wall "__FILE_NAME__" -I "__AVZ_DIR__/inc" -lavz -lgdi32 -L "__AVZ_DIR__/bin" -shared -o "./bin/libavz.dll"`，注意粘贴复制的时候千万不要有换行符， 然后这个设置需要打开扩展栏，找到 AvZ 扩展，然后右键 AvZ 右下角的齿轮找到扩展设置， 将 `Avz Compiler Cmd` 修改为上面的内容。

3. 其他更新请看新的图文教程

### 220630 -> 221001

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