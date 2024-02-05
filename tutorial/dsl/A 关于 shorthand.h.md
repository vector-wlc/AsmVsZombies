# 关于 `shorthand.h`

与 AvZ 本体不同，`shorthand.h` **不保证向后兼容**。为保证更新之后已有的脚本仍然可以运行，AvZ 存储库会保留 `shorthand.h` 的所有版本，路径为 `inc/dsl/shorthand_xxxxxx.h`。`shorthand.h` 永远指向最新的版本。在对外发布脚本时，最好把 `#include <dsl/shorthand.h>` 改为 `#include <dsl/shorthand_版本名.h>`，方便使用者运行。

有些人可能不喜欢 `shorthand.h` 中的一些名称，或者是想改进其中的功能。由于 AvZ 的更新机制，**对 AvZ 提供的文件作出的修改会在下次更新时被覆盖**。如果对 `shorthand.h` 不满意，有以下几种解决方案：

## 1 不使用 `shorthand.h`

把脚本中的 `#include <dsl/shorthand.h>` 改为 `#include <dsl/main.h>` 即可。你可能还需要 `using namespace ALiterals;` 以使用 `_wave`、`_cs` 等。

不建议使用这种方法。

## 2 创建 `shorthand.h` 的副本

复制 `inc/dsl/shorthand_[最新版本].h`，并粘贴到同一目录下，新文件随便取一个名字（比如 `my_shorthand.h`）。在新文件里进行你想要的修改，使用时把 `#include <dsl/shorthand.h>` 改为 `#include <dsl/my_shorthand.h>` 即可。


[目录](../0catalogue.md)

