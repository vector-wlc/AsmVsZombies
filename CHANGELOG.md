
## 更新日志

AvZ 2021_08_20
* 修复了 TickRunner pushFunc 函数调用后下一帧才生效的问题 (09-24)
* 提供了 VSCode AvZ 插件，并且不再维护 AvZTools
* 增加了得到出怪类型函数
* 对大量内存函数提供了使用说明
* 用卡函数、铲除植物函数、选卡函数使用 PvZ 内置函数，不再使用点击函数
* 修复 autoGetPaoList 带来的炮列表错乱问题
* 修复脚本退出后女仆秘籍仍有效果的 BUG
* 修复因未临时保护时间插入点而导致操作插入时间点错误的 BUG
* 优化注入程序运行逻辑，理论上减少 "复制 libavz.dll 失败" 报错的出现
* 削弱了 vector-wlc 的头发

AvZ 2021_04_19

* 修复 Card 函数对非十卡位置不支持的 BUG
* 修复了一些面向非无尽的脚本无响应的 BUG
* 修复了屋顶铲除南瓜错误的 BUG
* 修复了大波时间设定不能小于 -200 的 BUG
* 没有增加任何新功能
* 没有削弱 vector-wlc 的头发

AvZ 2021_02_11

* 开放得到卡片索引接口 GetCardIndex
* 新增设置脚本开始时间函数 SetScriptStartTime
* 脚本默认时间设定为进入战斗界面的时间
* Ice3 函数升级为 SetPlantActiveTime，并且 Ice3 函数以宏的形式提供
* 修正 GridToCoordinate 的坐标计算问题
* STRUCT Crood 更名为 Position

AvZ 2020_12_26

* 新增 SetDelayTime，可更方便的用于键盘绑定函数中
* SetErrorMode 中的参数由 AvZ::PVZ_TITLE 变为 AvZ::CONSOLE，即错误提示转换为控制台显示
* 新增 ScriptExitDeal 脚本退出处理函数，方便动态内存管理
* 没有削弱 vector-wlc 的头发

AvZ 2020_11_01

* AvZ 由 class -> namesapce
* 加入一系列安全检测，游戏崩溃概率变低
* 加入条件操作，操作队列非定态支持增强
* 修正单词拼写错误：indexs->indices
* PaoOperator 新增了自动填充炮列表函数 ： autoGetPaoList
* PaoOperator 使用 调用 PvZ 内置函数发炮 的方法实现发炮，理论上解决炮发不出去的问题
* SelectCards 使用 调用 PvZ 内置函数 的方法进入战斗界面，理论上解决选卡函数带来的崩溃问题 
* 修复了 clickGrid 天台场景有点击误差的 BUG

AvZ 2020_09_15

* AvZ::openMultipleEffective 添加了第二个参数，可以选择多次生效的模式，对挂机更加友好(09-19)
* 修复了 选卡函数，设置僵尸函数 对选卡界面外僵尸的不友好问题(09-19)
* 修复操作队列错误识别游戏时钟问题(09-19)
* 更新了专用于 AvZ 使用的工具软件 AvZTools，如果电脑无法使用 VSCode，可以尝试使用这款工具，下载地址： [https://wwe.lanzous.com/b015az7nc](https://wwe.lanzous.com/b015az7nc) (a0a3)
* 改善 SelectCards 函数实现方法，崩溃率可能更低
* 修复 阻塞函数 WaitUntil 的若干 BUG
* 将用卡函数的参数为拼音字符串改为英文枚举，命名与英文原版一致
* 用卡函数删除了根据卡槽位置用卡的多张使用形式
* 修复 showError 导致游戏崩溃的 BUG
* 修补植物类，自动存冰类在没有花盆荷叶的情况下不会种植植物
* 削弱了 vector-wlc 的头发

AvZ 2020_08_10 

* 新增 AvZ::setGameSpeed 函数，可以设定游戏的运行速度
* 修复部分 Windows 7 存在的选卡函数崩溃问题
* 修复 SetZombies 函数杀死战斗界面僵尸的问题
* 修复 FE 场景下脚本注入后无反应的问题
* 修复 roofPao 的飞行时间修正错误问题
* 将 popErrorWindow 函数名更改为 showError
* 削弱了 vector-wlc 的头发

AvZ 2020_07_27 (2020_07_10 的优化版，因此框架代码中的版本号没有改变)

* 提供更加人性化的 VSCode 安装包，安装和启动更加方便
* 新增错误提示方式，具体用法请查看 setErrorMode 函数
* 改进了 waitUntil 阻塞函数的实现方法，对非定态和无炮有了更加人性化的支持
* 修复了非第一波进入游戏的 BUG，调试脚本的时间占用将会变得更少
* 修复了不立即使用 fixLatestPao 会报错误的 BUG 
* 改进了注入程序的运行逻辑，减少不必要的注入错误
* 削弱了 vector-wlc 的头发

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