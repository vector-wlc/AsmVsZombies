<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 15:20:21
 * @Description: 
-->

# 名称空间

为了避免与其他库的标识符冲突，AvZ 所有的标识符都放在了名称空间 **`AvZ`** 里面，**请注意不要写错大小写**，因此如果想使用 AvZ 的 API (函数或类等)，你可以使用以下三种方式：

``` C++
// 例1. 使用 AvZ::[标识符]
AvZ::Shovel(3, 4); // 正确
AvZ::Card(XRK_1, 3, 4); // 正确

// 例2. 使用 using AvZ::[标识符]; 语句后再直接使用 [标识符]
using AvZ::Shovel;
Shovel(3, 4); // 正确
Card(XRK_1, 3, 4); // 错误，因为 Card 并没有被 using

// 例3. 使用 using namespace AvZ; 语句后再直接使用 [标识符]
using namespace AvZ;
Shovel(3, 4); // 正确
Card(XRK_1, 3, 4); // 正确，因为 using namespace AvZ; 直接“废除”了 名称空间 AvZ 的作用
```
以上三种方式中可以根据个人喜好任选一种使用，**但是如果使用的第三方库较多，尽量不要使用第三种方式**。

##  在此声明，由于作者精力原因，此教程所有的示例代码没有严格到每次都注意使用 AvZ:: , 但请使用者使用时注意此名称空间的作用

[上一篇 起步](./start.md)

[目录](../catalogue.md)

[下一篇 多次生效](./multiple_effective.md)