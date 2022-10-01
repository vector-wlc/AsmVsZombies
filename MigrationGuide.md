<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-19 18:36:24
 * @Description: 
-->
## 版本迁移文档

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