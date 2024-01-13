## 更新日志

AvZ 2.6.0 2024_01_13

* 修复了锁波长的依然刷新问题
* 修复了 AShovel 函数的一些问题
* ASetZombies 函数支持自然出怪
* 对状态钩的实现进行了一些优化
* 选卡函数支持极速选卡，瞬间到战斗界面
* 增加帧运行的新运行模式，可在注入之后一直运行
* 修复开启多次载入模式时，在选卡界面退出时出现无法再次载入的 BUG

AvZ 2.5.1 2023_09_26 (AReplay)

* 新增 `ACobManager::GetRecoverList` 得到炮的恢复列表
* `ACard` 新增两种重载形式，可用于多张卡片的使用，具体查看头文件中的注释
* 新增状态钩 `_AfterInject` 用于注入之后立即完成的操作，需要注意在此状态钩内使用 AvZ 的功能游戏可能崩溃
* 新增 `AEnterGame` 和 `ABackToMain` 完成快速进入和退出游戏的功能
* 修复了 `APainter` 的一些 BUG 
* 注入支持多开，通过右键双击指定要注入的 PvZ 窗口
* 增加回放功能: `AReplay`，可用于跳帧的死亡回放或者其他形式的游戏录制以及手动 TAS
* 帧运行新增优先级机制，整体运行顺序: 时间连接>全局帧运行>战斗界面帧运行，帧运行内部运行顺序由指定的 priority 值确定，范围为 [-20, 20]，priority 越小越先运行
* 修复了场景识别的 BUG，以及增添了一些关于场地的信息: `aFieldInfo` 

AvZ 2.4.4 2023_06_15

* 修复鼠标位置获取错误问题
* `ACobManager` 当炮消失，不再报错而是直接跳过
* 修复 `AIsSeedUsable` 的 bug
* 新增无条件删除植物函数 `ARemovePlant`，建议使用此函数代替 `AShovel`	
* 修复 `AShovel` 的铲除失败问题
* 修复协程内存访问错误问题，并新增 `ACoLaunch` 启动协程	
* 修复波数限制错误

AvZ 2.3.3 2023_05_01

* 新增内存读取和协程阻塞教程
* 修复了几个汇编函数的 bug
* 增加对坐标转换的范围检查
* 新增入口函数 `ACoroutine ACoScript()`, 此入口函数内可以使用协程阻塞 co_await
* 增加了 C++20 协程, 现在阻塞更加实用, 并弃用 `AWaitUntil`
* `ALogger<AConsole>` 现在退出战斗界面不会消失, 重新注入会消失

AvZ 2.2.6 2023_04_05

* 开放 `ACobManager::GetRoofFlyTime` 得到屋顶炮的飞行时间
* 新增 `ACobManager::GetUsablePtr` 和 `ACobManager::GetRoofUsablePtr` 分别得到目前的可用的炮的内存指针
* 新增 `ACobManager::GetRecoverPtr` 和 `ACobManager::GetRoofRecoverPtr` 分别得到恢复时间最短的炮的内存指针
* 新增 `AConnect` 调用形式: AConnect(ATime, bool Func), 意思为当时间到达之后不断运行 func, 直到 func 返回 false
* 新增 `AGetCobRecoverTime` 得到炮的恢复时间
* 修复了 `ATickRunner` 控制错误的问题
* 修正了 `IsStopped` `IsPaused` 的拼写错误问题, 并且之前的标识符被标记为 `Deprecated`
* 修复了 `ALogger` 状态未被重置的问题
* 修复了 `ALogger<AFile>` 的乱码问题
* 新增 `AGetAnimationArray` 直接获取动画数组
* 当阳光不够时，不再种植植物，并且新增 `AIsSeedUsable` 函数获取卡片是否可用

AvZ 2.1.0 2023_01_19

* 新增 PvZ 内置函数：删除、放置植物、僵尸函数，
* 修复了铲除函数错误铲除行数的问题
* 新增零度崩溃调试助手，提供更加丰富的崩溃信息
* 新增 `ATerminate` 使得 AvZ 立即停止运行
* 新增 `AGetInternalLogger` 获取 AvZ 内部的日志对象指针
* 修复关于使用 AException 无法让脚本正常退出的问题
* 修复 APainter 获取颜色错误的问题
* 修复了 ATime 的错误连接问题
* ARelOp 未被连接时会有警告信息

AvZ 2.0.0 2022_12_08

* 完善了 AStateHook 的设计
* 修复了一堆 BUG
* 阻塞函数不再有一帧延迟
* 选卡函数会阻塞到战斗界面
* 新增 `AWaitForFight`, 可直接用于阻塞到战斗界面
* `ASelectCards` 函数内部会自动调用 `AWaitForFight`

AvZ 2022_11_24 2.0.0 preview (breaking change)

* 所有的标识符都带有一个 `A`(类，函数，常量) 或者 `a`(变量、对象) 的前缀
* 新增核心机制 AConnect 
* 新增功能相较完善的 ALogger 日志类
* 新增假设波长功能 `AAssumeWavelength`，此功能在不内存的情况下实现 <-200 时间的设定
* 新增绘制类 `APainter`
* 移除操作队列的所有接口
* 操作函数不再分为 In Queue 和 Not In Queue，所有操作函数都是立即执行

AvZ 2022_10_01

* 修复使用 `WaitUntil` 之后脚本不再多次生效的 BUG (2022/10/12)
* 不再提供版本对应的脚本文件
* 完善了 AvZ 的异常机制
* 新增 `SetAdvancedPause`，可用于代码设置高级暂停
* 弃用 `SafePtr`， 直接使用原始指针即可
* 修复了对象迭代器类的 `*` 运算符返回类型错误的问题
* 修复了 roofPao 错误寻找最短 cd 炮的 BUG
* 优化自动存冰类、自动修植物类逻辑
* 优化坐标转换逻辑
* `class PaoOperator` 的 protected 成员更改为 protected 成员
* 修复继承 `GlobalVar` 的局部对象析构时无法移除 this 指针的问题
* 修复了偶尔存在卡片 CD 无法正常恢复的问题
* 修复了当鼠标位置不动且场地上存在收集物导致的 `MouseCol` `MouseRow` 读取内存值错误的问题

AvZ 2022_06_30

* 新增 `class GlobalVar` 以方便对全局变量/对象的状态进行管理，具体请参考进阶教程中的变量内存管理部分
* 新增对象迭代器类，可以更方便的遍历对象，具体请参考进阶教程中的对象过滤迭代器部分
* AvZ 开局不再重置游戏速度为原始速度，但是会在脚本运行结束后恢复运行 AvZ 之前的游戏速度，但是仅仅限于变速原理为修改每帧占用时长的修改器
* `SetWaveZombies` 函数不再在旗帜波自动生成蹦极
* 将 `SetWavelength` 的设置范围调整为 [601, 2510]
* Zombie 类 isExist 改为读取 isDisappeared 的内存地址
* 修复跳帧舞王时钟停滞更新的 BUG
* 修复 IceFiller::resetSeedList 的寒冰菇卡片识别不正确的 BUG

AvZ 2022_02_13

* 优化操作队列运行逻辑，将所有队列进行时间换算折合成一条队列运行，AvZ 更快了
* 修复倒计时时间识别错误问题
* 修复 `SetNowTime` 错误识别的 BUG
* 修改 AvZ 脚本底层运行方式，运行更加安全
* 按键绑定功能在选卡界面依然有效
* 改进底层注入方式，此后无尽生存游戏开始点为 (-599, 1)
* 新增跳帧功能 `SkipTick`
* 新增高级暂停功能 `SetAdvancedPauseKey`
* 删除条件操作，因为其可替代性很强，没有存在的价值
* 修改了停止 `OpenMultipleEffective` 的实现，现在按下停止键时不再弹出窗口提示，并且会立即停止脚本的工作，即脚本无法重启，除非重新注入。
* 修复了锁炮的问题
* 新增日志功能，插入操作和执行操作时显示内容，使用 `SetErrorMode(CONSOLE)` 开启此功能。
* 发布方式由 MinSizeRel 更改为 Release，理论上运行的更快了
* 削弱了 vector-wlc 的头发

AvZ 2021_12_12

* AvZ VSCode 插件支持断点调试，可更容易的查看 AvZ 脚本的执行流程
* 可能修复了多线程中数据冲突导致的游戏崩溃问题
* 使用异常实现了阻塞函数，不用再检测 waitUntil 的返回值，使用更方便了
* pvzstruct.h 中的 `enum PlantType` 增加了模仿者 `IMITATOR` (实际上是之前忘了写了)
* 增加了选卡判断, 即检查一次选卡中是否选择了两个相同的植物和是否选择了两个模仿者植物
* 新增判断某个位置能否种植物函数 : `Asm::GetPlantRejectType`, 请在文件 `pvzfunc.h` 中查看其用法
* 限制 `SetPlantActiveTime` 函数不允许修改的生效时间超过 3cs
* 自动收集类优先收集阳光

AvZ 2021_08_20

* 修复 InsertOperation 的开局不插入操作的问题(10-12)
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
