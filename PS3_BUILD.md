# PS3 / PSL1GHT 构建手册

本手册记录本项目在 Windows + PS3 SDK 4.75 环境中的已验证构建流程。它面向 FBNeo、MAME 2003-Plus 和统一版 RetroArch PS3。

## 1. 前置条件

- Windows 10/11、PowerShell、Git for Windows、GNU Make。
- PS3 SDK 4.75 安装在 `C:\PS3SDK\4.75\cell`。
- SDK 主机工具目录：`C:\PS3SDK\4.75\cell\host-win32`。
- Git Bash 的 `sh.exe`：通常为 `C:\Program Files\Git\bin\sh.exe`。

每个新 PowerShell 会话先设置：

```powershell
$env:CELL_SDK = 'C:/PS3SDK/4.75/cell'
$env:PS3DEV   = 'C:/PS3SDK/4.75/cell/host-win32'
$env:PATH     = 'C:/Program Files/Git/usr/bin;C:/PS3SDK/4.75/cell/host-win32/ppu/bin;C:/PS3SDK/4.75/cell/host-win32/bin;' + $env:PATH
```

必须能找到：

```text
ppu-lv2-gcc.exe
ppu-lv2-g++.exe
ppu-lv2-ar.exe
make_fself.exe
make_fself_npdrm.exe
```

## 2. 目录约定

```text
D:\ps3-work\FBNeo-libretro
D:\ps3-work\mame2003-plus-libretro
D:\ps3-work\RetroArch_PS3_Unified
```

统一前端的 PS3 核心位置：

```text
/dev_hdd0/game/ARCD00001/USRDIR/cores/fbneo_libretro_ps3.SELF
/dev_hdd0/game/ARCD00001/USRDIR/cores/mame2003_plus_libretro_ps3.SELF
```

## 3. FBNeo Libretro

源码目录：`FBNeo-libretro\src\burner\libretro`

```powershell
make -j4 platform=ps3 COMMONLV=lv2- SHELL='C:/Program Files/Git/bin/sh.exe'
```

PS3 关键配置应保留：

- `-D__PS3__ -DWORDS_BIGENDIAN`
- `-DFRONTEND_SUPPORTS_RGB565=1`
- `-DFBNEO_DEFAULT_AUDIO_RATE=44100`
- `-O3 -maltivec -mabi=altivec -fstrict-aliasing -fomit-frame-pointer -ffast-math`

### Windows 的 ar 参数过长问题

完整 `ar rcs` 可能报 `make (e=87)`。如果所有对象已编译，只增量更新变动对象：

```powershell
& 'C:\PS3SDK\4.75\cell\host-win32\ppu\bin\ppu-lv2-ar.exe' r fbneo_libretro_ps3.a '..\..\burner\libretro\libretro.o'
```

需要时对其他改变的 `.o` 重复 `ar r`。不要把 `.a` 上传到 Git。

### 已验证的 PS3 特性

- RGB565 输出，避免 PSGL 大端 XRGB8888 上传异常并降低带宽。
- 默认 44.1 kHz 音频。
- 64 MB、128 字节对齐内存池；大型 CPS3/PGM/CV1000 板卡绕过小块池，改用直接对齐分配。
- `cheat.dat`/`*.ini` 只加载一次；每个模拟帧调用 `CheatApply()`。
- 不启用 Libretro CHT 接口，当前使用 FBNeo 原生作弊文件。

## 4. MAME 2003-Plus Libretro

源码目录：`mame2003-plus-libretro`。

使用仓库根目录的 PS3 专用 Makefile/参数进行构建；如项目提供的命令不同，优先使用仓库的 `PS3_BUILD.md`。

产物为 PS3 静态库或核心 ELF。不要提交：

```text
*.o  *.d  *.a  *.elf  *.self  *.SELF
```

MAME 的低内存修改依赖运行时按游戏尺寸分配视频/音频缓冲区；不要恢复成全局超大静态数组。

## 5. 统一版 RetroArch PS3

进入：`RetroArch_PS3_Unified`。

先把已编译的 FBNeo/MAME 静态库复制为统一工程期望的库名，例如：

```text
RetroArch_PS3_Unified\libretro_ps3.a
```

然后编译：

```powershell
make -B -f Makefile.ps3 -j4
```

链接成功会生成：

```text
retroarch_ps3.elf
retroarch_ps3.self
```

按 Makefile 的目标或 SDK 工具转换为可部署的 SELF：

```powershell
make_fself.exe retroarch_ps3.elf retroarch_ps3.self
```

核心 SELF 与前端 SELF 不同：核心应放在 `USRDIR/cores/`，前端 EBOOT/SELF 放在应用 `USRDIR/`。

## 6. Salamander 与历史记录

- `retroarch-salamander.cfg` 的 `libretro_path` 必须是 `/cores/*.SELF`，不能保存成 ROM 或 EBOOT 路径。
- 历史列表条目的 `core_path` 必须指向对应核心 SELF。
- FBNeo 列表名 `FBNEO`/`FBNeo` 与缩略图数据库名必须匹配；已做兼容映射时不要回退。

## 7. 部署和实机日志

调试日志一般读取：

```text
/dev_hdd0/game/<TITLEID>/USRDIR/retroarch-log.txt
```

通过 FTP 覆盖核心时，只替换目标核心 SELF；不要用核心 SELF 覆盖前端 EBOOT。

每次实机测试记录：核心文件名、SHA256、ROM 名、症状、完整日志尾部和是否从 History/Load Content 启动。

## 8. Git 规则

源码仓库只提交可复现的内容。必须忽略：

```gitignore
*.o
*.d
*.a
*.elf
*.self
*.SELF
objs/
```

GitHub 普通仓库拒绝超过 100 MB 的单文件；静态库、ELF、SELF 应作为 Release 附件或本地归档，而不是 Git 提交。

## 9. 推荐顺序

```text
1. 配置 PS3 SDK 环境变量。
2. 编译 FBNeo 或 MAME 核心。
3. 验证静态库/ELF 已生成。
4. 复制核心库到统一前端工程所需位置。
5. 编译统一版 RetroArch。
6. 生成前端 SELF 与核心 SELF。
7. FTP 覆盖 PS3 的正确文件。
8. 用日志确认启动、加载游戏、Close Content、History 重载。
```

