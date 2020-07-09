# AsmVsZombies

#####                                                                                                                                                                                                       --------windows

High-precision PvZ TAS Frameworks

高精度植物大战僵尸键控框架

## 简介

AvZ (Assembly vs. Zombies - windows )是一套使用 C++ 语言编写的高精度植物大战僵尸键控框架，理论由 yuchenxi0_0 提出，框架底层由 yuchenxi0_0 实现，和其他框架相似的接口由 vector-wlc 编写。

AvZ 操作精度为理论上的100%绝对精准，使用这套框架将再也不用担心精度的问题，可在一定程度上减少录制视频次数，有效的完成视频制作。

本项目使用 [VSCode](https://code.visualstudio.com/) + MinGW 进行代码编辑、编译和注入。 

~~本项目编译器有两个选择，分别是 MinGW 和 MSVC，使用者可根据自身情况选择使用其中一个。~~

## 原理

在游戏主循环函数前面注入键控代码，使得键控脚本在每一帧都被调用，从而实现真正意义上100%精确的键控。

## 对比

从原理可以明显看出此套框架在理论实现上与传统框架截然不同，传统框架使用一个程序向 PvZ 窗口发送点击消息，而此套框架使用代码注入，直接入侵到程序内部，让游戏程序运行我们编写的脚本！其优缺点大致如下：

> 缺点 
>
> * ~~软件体积更大 (MSVC--VS)~~
> * ~~复杂脚本编写工作量可能更大~~
> * 编写不慎可能会导致游戏崩溃

> 优点
>
> * 精度极高
> * 脚本出现错误时提示更加人性化
> * 对硬件配置 (CPU) 的要求低
> * 对操作时间顺序不做严格要求

## 使用

#### MinGW Visual Studio Code
在 [https://wwe.lanzous.com/b015az7nc](https://wwe.lanzous.com/b015az7nc) (a0a3) 下载压缩包，
解压并使用 [VSCode](https://code.visualstudio.com/) 打开，无需任何配置，便可直接使用。<br>
在 [https://wwe.lanzous.com/b015az8yj](https://wwe.lanzous.com/b015az8yj) (37zu) 下载相应版本包，
进行更新或退回，注意在 2020_07_10 之后的版本包将只支持新的 [VSCode](https://code.visualstudio.com/) 环境包

#### ~~MSVC Visual Studio~~

~~Visual Studio 使用方便，无需配置，简单粗暴，稳定可靠。但是其体积庞大，安装包大概 2-3 G。~~

~~若您的电脑硬盘容量足够，请优先使用 VS~~

**~~注意~~**  ~~部分 win7 可能无法正常安装 Visual Studio~~


## 问题（BUG）

Windows 7 仍存在使用 selectCards 函数崩溃的问题

## 致谢
[yuchenxi2000/AssemblyVsZombies](https://github.com/yuchenxi2000/AssemblyVsZombies)<br>
[lmintlcx/pvzscript](https://github.com/lmintlcx/pvzscripts)

## 更新日志

**AvZ 2020_07_10**
* 放弃对于 Visual Studio 的支持，提供新的无需配置的 AvZ [VSCode](https://code.visualstudio.com/) 环境包
* 新增 setWavelength 设定波长函数，使用此函数可以使用小于 -200 的时间参数
* 新增 setInsertOperation 设定插入队列属性函数，使用此函数可以临时改变 insertOperation 的属性
* 修改女仆秘籍实现方法，相关使用见 AvZ::MaidCheats 类
* 修改：openMultipleEffective 可使用热键关闭多次生效效果
* 修复 insertOperation 可能导致的崩溃问题
* 修复炮冷却计算问题
* 削弱了 vector-wlc 的头发

AvZ 2020_05_08

* 修复 recoverPao 导致游戏崩溃的问题 (06-06)
* 修复注入后再次进入战斗界面导致游戏崩溃的问题 (06-06)
* 操作队列大更新, 使用 20 条队列并行读取操作, 即不用考虑跨波操作时间先后顺序问题
* 新增调试函数 showQueue, 作用为显示操作对列中为未被执行的内容
* 删除 nowTimeWave, 新增 nowTime, 具体使用方法见使用说明
* PaoOperator : 删除反人类的炮的信息更新机制，手动发炮、铲种炮不再需要更新炮的信息, 如果在炮列表内使用了位移铲种, 需要手动更新炮列表
* 修正了 Card 名称中模仿寒冰射手 "vhbss" 的拼写错误 --> "Mhbss"
* 削弱了 vector-wlc 的头发

AvZ 2020_04_12

* PaoOperator 类大更新，删除 tryPao 系列函数，增加按照时间顺序使用炮，详情请看 bilibili 视频教程
* IceFiller 类增添成员函数 resetIceSeedList，允许对冰卡进行更加细致的控制
* 当 PvZ 窗口不是顶层窗口时，KeyConnect 将无效
* 削弱了 vector-wlc 的头发

AvZ 2020_03_16

* 增添了 selectCards，选卡函数，请注意其与 CvZ 的使用区别
* 增添了 setZombies setWaveZombies，设定刷怪函数，允许您对刷怪精确到以波为单位的控制
* 带来了极致舒爽的录制视频体验
* 增添了 plantPao (炮操作类成员)，种炮函数，自动更新炮的信息
* 将 fixLastPao 重命名为 fixLatestPao  并修复了 fixLatestPao 的 BUG
* 修复 updatePaoMessage 的 BUG （03-22）
* 修复 selectCards 选择模仿者卡片引起的游戏崩溃问题--win7 (03-23)
* 削弱了 vector-wlc 的头发

AvZ 2020_02_20

* 增添了 openMultipleEffective，一次注入多次生效
* 增加了 setTime insertOperation insertTimeOperation 等一系列方便对操作队列进行操作的函数
* 增加了阻塞函数 WaitUntil，使得编写非定态脚本更加人性化
* 优化了与操作对列有关的逻辑
* 削弱了 vector-wlc 的头发