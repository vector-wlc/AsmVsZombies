<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 22:16:16
 * @Description: 
-->

# “智能”操作

“智能”操作，顾名思义就是脚本会根据场上的僵尸的情况做出是适当的决策，操作不是写死的，而是“智能”的。关于这方面文字很难将其表达清楚，因此笔者录制了相关的视频，视频将相应的 TAS 技巧展示了出来，视频链接：https://www.bilibili.com/video/BV1cw411R7mZ ，如果读者希望编写出“智能”脚本，甚至想键控无炮，请一定要仔细观看上面的视频，此外，示例脚本中的[传统四炮](https://gitee.com/vector-wlc/AsmVsZombies/blob/master/script/210820/PEChuanTong4.cpp)就是一个很好的例子，希望大家能够看懂其代码。

有关'智能'操作相关的代码

```C++
// 寻找游戏内的对象
// 寻找位于四行五列的加农炮
// 第一种方案，直接调用 AvZ 的接口
Plant* cannon = nullptr;
int cannon_idx = AvZ::GetPlantIndex(4, 5, YMJNP_47);
if (cannon_idx >= 0) {
    cannon = AvZ::GetMainObject()->plantArray() + cannon_idx;
}

// 第二种方案
auto plant_array = AvZ::GetMainObject()->plantArray();
auto plant_total = AvZ::GetMainObject()->plantTotal();
for (int idx = 0; idx < plant_total; ++idx) {
    if (plant_array[idx].isDisappeared() || plant_array->isCrushed()) {
        continue;
    }
    if (plant_array[idx].row() == 4 - 1 && plant_array[idx].col() == 5 - 1 && plant_array[idx].type() == YMJNP_47) {
        cannon = plant_array + idx;
        break;
    }
}

// 第三种方案
for (auto&& plant : AvZ::alive_plant_filter) {
    if (plant.row() == 4 - 1 && plant.col() == 5 - 1 && plant.type() == YMJNP_47) {
        cannon = &plant;
        break;
    }
}

// 注意绝对不能出现值拷贝，例如
auto first_plant = plant_array[0]; // ERROR !!!
// 因为 AvZ 中读取的就是对象本身的引用，
// 上方的代码会调用 Plant 的拷贝构造函数，
// 但是 AvZ 本身来说并不知道 Plant 的拷贝构造函数是什么
// 所以不能这样写

////////////////////////////////////////////
// 检查植物是否能种
// 这里可以写出类似上面的代码
// 检查这个格子是否有 植物、弹坑、冰道、冰车...
// 这样不是不可以就是比较麻烦
// 可以使用 PvZ 中的内置函数
// 查看 四行五列 是否能种玉米加农炮
if (Asm::getPlantRejectType(YMJNP_47, 4 - 1, 5 - 1) == Asm::NIL) {
    // some code
}
// 当然冰车碾压和巨人锤击等还需要额外检查

////////////////////////////////////////////
// 内存读取相关接口
// 在之前的老版本中，如果想要读取一个 AvZ 中没有封装的值
// 需要自己自行继承封装
// 例如读取阳光值
struct MainObjectMore : public MainObject {
    int& sunVal()
    {
        return (int&)((uint8_t*)this)[0x5560];
    }
};

int sun = ((MainObjectMore*)AvZ::GetMainObject())->sunVal();

// 现在只需要
constexpr int SUN_ADDR = 0x5560;
sun = AvZ::GetMainObject()->mRef<int>(SUN_ADDR);

// 读取下一层指针偏移
auto main_object = AvZ::GetPvzBase()->mPtr<MainObject>(0x768);

// 读取值
auto zombie_list = AvZ::GetMainObject()->mVal<uint32_t*>(0x6b4);
```


[上一篇 运行调试](./debug.md)

[目录](../catalogue.md)

[下一篇 对象过滤迭代器](./iterator.md)
