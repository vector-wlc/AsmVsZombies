<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 19:12:51
 * @Description: 
-->

# 按键绑定
在 AvZ 中我们可以使用 `KeyConnect` 函数将操作和键盘进行绑定，使用如下
```C++
KeyConnect('Q', [=](){
    Card(WG_17, 1, 1);
});
```

使用这条语句后，当按下 Q 键时，将会在一行一列种植一个倭瓜，但是这种写法不是很好，因为我们一局游戏很少情况只将一个卡片用在一个位置上，因此这个时候需要再介绍两个函数，`MouseRow` 和 `MouseCol` ，分别是获取当前鼠标所在行和所在列，我们将这两个函数与 `KeyConnect` 结合起来，例如:
```C++
KeyConnect('Q', [=](){
    Card(WG_17, MouseRow(), MouseCol());
});
```


这样的话，当我们按下 Q 键时，将会在鼠标位置处放置一个倭瓜，这样实用性就大大提升了，当然 `KeyConnect` 几乎可以与 AvZ 中所有函数进行绑定。

按下 Q 键在鼠标位置处发一门炮
```C++
KeyConnect('Q', [=](){
    pao_operator.pao(MouseRow(), MouseCol());
});
```

按下 Q 键开启多次生效模式
```C++
KeyConnect('Q', [=](){
    OpenMultipleEffective();
});
```


因此，`KeyConnect` 的使用特别灵活，但是需要注意的一点是，一个按键只能被绑定一次，例如下面的代码就是错误的
```C++
KeyConnect('Q', [=](){
    Card(WG_17, MouseRow(), MouseCol());
});

KeyConnect('Q', [=](){
    OpenMultipleEffective();
});
```

Q 键绑定了用卡和多次生效，这样做是不可以的，因为 AvZ 并不知道按下 Q 键时到底要执行哪个绑定的操作，会导致热键冲突，如果此时确实想按下 Q 键同时执行用卡和多次生效的操作，只需要将他们写在一个 `KeyConnect` 中即可。

```C++
KeyConnect('Q', [=](){
    Card(WG_17, MouseRow(), MouseCol());    
    OpenMultipleEffective();
});
```

[上一篇 波长设定与非定态](./wave_set.md)

[目录](../catalogue.md)

[下一篇 出怪设置与女仆秘籍](./set_zombie.md)
