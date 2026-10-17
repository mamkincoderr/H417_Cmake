# H417_Cmake

> Reproducible **CMake + Ninja + RISC-V GCC 15** build for the dual-core **WCH CH32H417** (QingKe V3F + V5F),
> drop-in for **MounRiver Studio 1.92** — with build, flash and **simultaneous debugging of both cores**
> from the IDE.

[![visits](https://hits.sh/github.com/mamkincoderr/H417_Cmake.svg?view=today-total&label=visits)](https://hits.sh/github.com/mamkincoderr/H417_Cmake/)
[![stars](https://img.shields.io/github/stars/mamkincoderr/H417_Cmake?logo=github)](https://github.com/mamkincoderr/H417_Cmake/stargazers)
[![forks](https://img.shields.io/github/forks/mamkincoderr/H417_Cmake?logo=github)](https://github.com/mamkincoderr/H417_Cmake/network/members)
[![last commit](https://img.shields.io/github/last-commit/mamkincoderr/H417_Cmake)](https://github.com/mamkincoderr/H417_Cmake/commits/main)
[![code size](https://img.shields.io/github/languages/code-size/mamkincoderr/H417_Cmake)](https://github.com/mamkincoderr/H417_Cmake)
[![license](https://img.shields.io/badge/license-0BSD-blue)](LICENSE)
![MCU](https://img.shields.io/badge/MCU-CH32H417%20(V3F%20%2B%20V5F)-A2191F)
![build](https://img.shields.io/badge/build-CMake%20%2B%20Ninja%20%2B%20GCC%2015-064F8C)

**[Русский](#русский)** · **[English](#english)** · **[中文](#中文)**

---

```bat
build.bat            :: -> obj\H417_V3F.elf, obj\H417_V5F.elf, obj\H417_Merge.hex / .bin
build.bat flash      :: build + program both cores + verify + run (WCH-Link-E)
build.bat reset      :: clear stale breakpoints, reset and run
build.bat info       :: CHIPID, flash size, UID
```

MRS 1.92: **Build Targets** `all` · `flash` · `reset` · `erase` · `info` · `clean`; Debug **H417_Cmake** (V3F) +
**H417_V5F_dual** (V5F).

---

## Русский

Шаблон рабочего проекта для двухъядерного микроконтроллера **WCH CH32H417**: ядро QingKe **V3F** ведущее,
**V5F** ведомое. Проверен на оценочной плате **CH32H417QEU6-R0-1v1**.

**Назначение.** Из одного дерева исходников собираются два образа — для V3F и для V5F — и их склейка, которая
прошивается одним файлом. Основная среда работы инженера — **MounRiver Studio 1.92**: редактирование, сборка,
загрузка во flash и отладка. Компиляция выполняется не встроенным генератором makefile этой IDE, а воспроизводимой
цепочкой CMake / Ninja и **RISC-V GCC 15** из комплекта **MounRiver Studio 2**. Studio 1.92 вызывает её через
Build Targets (`build.bat`). Прошивка и отладка — OpenOCD из MounRiver Studio 2. В отладчике MRS 1.92 работают
оба ядра одновременно, каждое в своей сессии.

Библиотека периферии и стартапы — официальный SDK WCH [openwch/ch32h417](https://github.com/openwch/ch32h417),
EVT V1.6 (библиотека V1.0.6), без правок.

`#CH32H417` `#CH32H417QEU6` `#CH32H417QEU6-R0-1v1`

### Что делает прошивка

V3F стартует первым, настраивает тактирование и USART1 (printf 115200, PA9), будит V5F и ждёт от него сигнала
через аппаратный семафор HSEM. Затем V3F мигает LED1 (2 Гц), V5F — LED2 (5 Гц). Ожидание V5F сделано опросом
флага, без режима STOP: в STOP отладчик к чипу не подключается.

### Два ядра

| Ядро | Флеш | Код в RAM | Данные | Порт GDB |
|---|---|---|---|---|
| V3F | `0x00000`, 64 КБ | `0x20100000`, 64 КБ | `0x20110000`, 448 КБ | 3333 |
| V5F | `0x10000`, 128 КБ | ITCM `0x200A0000`, 128 КБ | DTCM `0x200C0000`, 256 КБ | 3334 |

Флаги ядра как в EVT: `-march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32`, FPU выключен. Код каждого ядра —
`User/V3F/` и `User/V5F/`, общий код — `Common/`. Оба ядра печатают в USART1: UART8 ядра V5F по умолчанию
выведен на PB4, а в корпусе QEU вывода PB4 нет.

### Системные требования

- **Windows**

- **MounRiver Studio 1.92** — основная среда разработки: исходники, Build, отладчик.

  <details>
  <summary>Где взять</summary>

  Дистрибутив — на сайте разработчика: [mounriver.com/download](http://www.mounriver.com/download). Установить
  Studio 1.9.x (ветка 1.92). Открыть в IDE файл `H417_Cmake.wvproj` этого репозитория (File → Open Project).

  Встроенный генератор makefile выключен: молоток и Build Targets вызывают `build.bat`. Первая сборка создаёт
  в `tools\` ссылки (junction) `gcc` и `openocd` на компилятор и OpenOCD из MounRiver Studio 2 — через них
  конфигурации отладки обходятся без абсолютных путей. Права администратора не нужны.
  </details>

- **MounRiver Studio 2** — компилятор GCC 15 (`riscv32-wch-elf-gcc`) и OpenOCD с конфигурацией
  `wch-dual-core.cfg` для двух ядер.

  <details>
  <summary>Где взять</summary>

  Отдельный установщик Studio 2 — там же: [mounriver.com/download](http://www.mounriver.com/download). Если после
  установки в каталоге тулчейна лежит архив, а не готовая папка

  `C:\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC15.zip`

  — распакуйте его **в тот же каталог** `Toolchain`, без лишнего уровня вложенности. Должен получиться компилятор:

  `C:\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC15\bin\riscv32-wch-elf-gcc.exe`

  Studio 2 установлена в другое место — задайте переменные окружения `WCH_TOOLCHAIN_DIR` (папка `bin` компилятора)
  и `WCH_OPENOCD_DIR` (папка с `openocd.exe`).
  </details>

- **CMake** версии 3.20 или новее.

  <details>
  <summary>Установка из PowerShell</summary>

  ```powershell
  winget install --id Kitware.CMake --exact
  ```

  Проверка: `cmake --version`. `build.bat` ищет `cmake.exe` в стандартных каталогах, перезапускать MRS не нужно.
  </details>

- **Ninja**.

  <details>
  <summary>Установка из PowerShell</summary>

  ```powershell
  winget install --id Ninja-build.Ninja --exact
  ```

  Проверка: `ninja --version`.
  </details>

- **WCH-LinkE** — программатор и отладчик.

  <details>
  <summary>Подключение</summary>

  Адаптер должен работать в режиме RISC-V: в диспетчере устройств Windows — **WCH-LinkRV**,
  `VID_1A86&PID_8010`. На плате — разъём P3: 1 = 3V3, 5 = PB9 (SWDIO), 7 = PB8 (SWCLK), остальные — GND.
  </details>

### Сборка и прошивка

В Project Explorer откройте **Build Targets** и щёлкните цель дважды:

| Цель | Что делает |
|---|---|
| `all` | собирает оба ядра и склейку; то же делает молоток |
| `flash` | собирает, записывает `obj\H417_Merge.hex` (оба ядра), проверяет и пускает чип |
| `reset` | снимает забытые аппаратные точки останова, сбрасывает и пускает чип |
| `erase` | стирает всю флеш |
| `info` | CHIPID, объём флеша, UID |
| `clean` | удаляет `obj` |

Из консоли то же самое: `build.bat <цель>`.

### Отладка двух ядер

Run → Debug Configurations → GDB OpenOCD MRS Debugging:

| Конфигурация | Ядро | Что делает |
|---|---|---|
| **H417_Cmake** | V3F | собирает, прошивает оба ядра, останавливает V3F на `main`; запускает OpenOCD |
| **H417_V5F_dual** | V5F | подключается к OpenOCD сессии H417_Cmake и доводит V5F до его `main` |
| **H417_V5F** | V5F | одна, без сессии V3F: останавливает работающий V5F; V3F продолжает работать |

Оба ядра сразу:

1. **Debug → H417_Cmake.** V3F встаёт на `main`.
2. **Resume (F8)** в сессии V3F. V3F будит V5F, тот встаёт на своём входе `0x10000`.
3. Не закрывая сессию V3F, **Debug → H417_V5F_dual.** V5F доходит до своего `main`.
4. В окне Debug выберите ядро: шаги, точки останова и переменные — у каждого свои. Остановка и пуск у ядер раздельные.
5. Конец — Terminate обеих сессий, затем Build Targets → `reset`.

<details>
<summary>Как это устроено и чего избегать</summary>

- `init` в OpenOCD останавливает оба ядра, а `reset init` останавливает и ещё не разбуженный V5F — такой V5F
  не слышит будильник ядра V3F. Поэтому сессия H417_Cmake ставит аппаратные точки останова на `main` V3F и на вход
  V5F `0x10000` (они переживают сброс), делает `reset run` и пускает одно V3F. Процедуры — `tools/h417.cfg`.
- V5F до пробуждения стоит на адресе 0. Пустить его в этот момент (Resume) нельзя: он выполнит код V3F, и чип
  перестанет отвечать до кнопки RESET. Конфигурации V5F такое подключение отклоняют с сообщением
  `V5F is not woken by V3F yet`.
- Пока V5F ждёт на входе, его код ещё не скопирован в ITCM: точки останова в коде V5F ставьте, когда он дошёл
  до `main`.
- Аппаратные точки останова переживают сброс чипа. Цели `flash` и `reset` снимают их на обоих ядрах.
- V3F ждёт сигнала V5F 2 с, затем пишет `no answer from V5F` и идёт дальше — при отладке это нормально.
- Если отлаживается только V3F, V5F ждёт на входе, LED2 не мигает. Чтобы V5F работал сам, в команде
  конфигурации H417_Cmake допишите аргумент 0: `eval "monitor h417_debug_start 0x%x 0", &main`.
- Примеры WCH уводят V3F в режим STOP, где отладчик не подключается. Чтобы прошить плату поверх такой прошивки,
  запускайте `flash` сразу после нажатия RESET.
</details>

### Плата CH32H417QEU6-R0

Схема и плата — в репозитории WCH:
[EVT/PUB/SCHPCB/CH32H417QEU6-R0](https://github.com/openwch/ch32h417/tree/main/EVT/PUB/SCHPCB/CH32H417QEU6-R0).

| Что | Подключение |
|---|---|
| WCH-LinkE | P3: 1 = 3V3, 5 = PB9 (SWDIO), 7 = PB8 (SWCLK), остальные — GND |
| printf | USART1 TX = PA9 (P5.32) → RX UART WCH-LinkE, 115200 |
| LED1 / LED2 | перемычки P8.27 (PE2) ↔ P8.29 и P8.28 (PE3) ↔ P8.30; светодиоды горят при 0 |
| KEY | P5.2, подтяжка к 3V3 |

### Проверка на железе

Сборка, прошивка и отладка выполнялись на плате CH32H417QEU6-R0-1v1 (CHIPID `0x4170052D` — CH32H417QEU,
ревизия 2, флеш 960 КБ), WCH-LinkE (прошивка 2.18), MounRiver Studio 1.92, OpenOCD и GCC 15.2 из MounRiver Studio 2.
Проверены: оба светодиода без отладчика, отладка V3F, отладка V5F и обоих ядер одновременно в MRS 1.92.

### Лицензия

Слой проекта (CMake, скрипты, `tools/`, `Common/`, `User/V3F/main.c`, `User/V5F/main.c`) — **[0BSD](LICENSE)**:
использование, копирование и распространение без ограничений и без требования указания авторства.

Файлы библиотеки периферии и EVT компании Nanjing Qinheng (`Core/`, `Peripheral/`, `Startup/`, `Ld/`,
`Debug/debug.*`, `User/*/system_ch32h417.*`, `User/*/ch32h417_conf.h`, `User/*/ch32h417_it.*`,
`svd/CH32H417xx.svd`) **не** покрываются 0BSD: действует условие WCH — только для микроконтроллеров этого
производителя.

Автор: [mamkincoderr](https://github.com/mamkincoderr) · [Telegram](https://t.me/oDeXteRo)

---

## English

A working-project template for the dual-core **WCH CH32H417** microcontroller: QingKe **V3F** is the master core,
**V5F** the slave. Verified on the **CH32H417QEU6-R0-1v1** evaluation board.

**Purpose.** One source tree builds two images — for V3F and for V5F — plus their merge, programmed as a single
file. The engineer's primary environment is **MounRiver Studio 1.92**: editing, building, flashing and debugging.
Compilation is done not by the IDE's built-in makefile generator but by a reproducible CMake / Ninja chain with
**RISC-V GCC 15** from **MounRiver Studio 2**; Studio 1.92 invokes it through Build Targets (`build.bat`). Flashing
and debugging use OpenOCD from MounRiver Studio 2. Both cores can be debugged in MRS 1.92 at the same time, each in
its own session.

The peripheral library and startups are the official WCH SDK
[openwch/ch32h417](https://github.com/openwch/ch32h417), EVT V1.6 (library V1.0.6), unmodified.

`#CH32H417` `#CH32H417QEU6` `#CH32H417QEU6-R0-1v1`

### What the firmware does

V3F starts first, sets up the clocks and USART1 (printf 115200, PA9), wakes V5F and waits for its signal through
the HSEM hardware semaphore. Then V3F blinks LED1 (2 Hz) and V5F blinks LED2 (5 Hz). The wait for V5F polls a flag
instead of entering STOP mode: in STOP the debugger cannot attach to the chip.

### Two cores

| Core | Flash | Code in RAM | Data | GDB port |
|---|---|---|---|---|
| V3F | `0x00000`, 64 KB | `0x20100000`, 64 KB | `0x20110000`, 448 KB | 3333 |
| V5F | `0x10000`, 128 KB | ITCM `0x200A0000`, 128 KB | DTCM `0x200C0000`, 256 KB | 3334 |

Core flags as in the EVT: `-march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32`, FPU off. Per-core code lives in
`User/V3F/` and `User/V5F/`, shared code in `Common/`. Both cores print to USART1: the V5F default UART8 sits on
PB4, which the QEU package does not have.

### System requirements

- **Windows**

- **MounRiver Studio 1.92** — the main development environment: sources, Build, debugger.

  <details>
  <summary>Where to get it</summary>

  The distribution is on the developer's site: [mounriver.com/download](http://www.mounriver.com/download). Install
  Studio 1.9.x (the 1.92 branch). Open this repository's `H417_Cmake.wvproj` in the IDE (File → Open Project).

  The built-in makefile generator is off: the hammer button and Build Targets call `build.bat`. The first build
  creates junctions `tools\gcc` and `tools\openocd` to the compiler and OpenOCD of MounRiver Studio 2, so the debug
  configurations need no absolute paths. No administrator rights are required.
  </details>

- **MounRiver Studio 2** — the GCC 15 compiler (`riscv32-wch-elf-gcc`) and OpenOCD with the dual-core
  `wch-dual-core.cfg` configuration.

  <details>
  <summary>Where to get it</summary>

  A separate Studio 2 installer is at the same place: [mounriver.com/download](http://www.mounriver.com/download).
  If the toolchain directory contains an archive instead of a ready folder

  `C:\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC15.zip`

  — extract it **into the same** `Toolchain` directory, without an extra nesting level. The result must be:

  `C:\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC15\bin\riscv32-wch-elf-gcc.exe`

  Studio 2 installed elsewhere — set the `WCH_TOOLCHAIN_DIR` (compiler `bin` folder) and `WCH_OPENOCD_DIR`
  (folder with `openocd.exe`) environment variables.
  </details>

- **CMake** version 3.20 or newer.

  <details>
  <summary>Install from PowerShell</summary>

  ```powershell
  winget install --id Kitware.CMake --exact
  ```

  Check: `cmake --version`. `build.bat` looks for `cmake.exe` in the standard directories, no MRS restart needed.
  </details>

- **Ninja**.

  <details>
  <summary>Install from PowerShell</summary>

  ```powershell
  winget install --id Ninja-build.Ninja --exact
  ```

  Check: `ninja --version`.
  </details>

- **WCH-LinkE** — programmer and debugger.

  <details>
  <summary>Connection</summary>

  The adapter must be in RISC-V mode: in Windows Device Manager — **WCH-LinkRV**, `VID_1A86&PID_8010`. On the
  board — connector P3: 1 = 3V3, 5 = PB9 (SWDIO), 7 = PB8 (SWCLK), the rest — GND.
  </details>

### Build and flash

In Project Explorer open **Build Targets** and double-click a target:

| Target | What it does |
|---|---|
| `all` | builds both cores and the merge; the hammer button does the same |
| `flash` | builds, programs `obj\H417_Merge.hex` (both cores), verifies and runs the chip |
| `reset` | clears stale hardware breakpoints, resets and runs the chip |
| `erase` | erases the whole flash |
| `info` | CHIPID, flash size, UID |
| `clean` | deletes `obj` |

The same from a console: `build.bat <target>`.

### Debugging both cores

Run → Debug Configurations → GDB OpenOCD MRS Debugging:

| Configuration | Core | What it does |
|---|---|---|
| **H417_Cmake** | V3F | builds, programs both cores, stops V3F at `main`; starts OpenOCD |
| **H417_V5F_dual** | V5F | attaches to the H417_Cmake session's OpenOCD and runs V5F to its `main` |
| **H417_V5F** | V5F | standalone, no V3F session: halts the running V5F; V3F keeps running |

Both cores at once:

1. **Debug → H417_Cmake.** V3F stops at `main`.
2. **Resume (F8)** in the V3F session. V3F wakes V5F, which stops at its entry `0x10000`.
3. With the V3F session still open, **Debug → H417_V5F_dual.** V5F runs to its `main`.
4. Pick a core in the Debug view: steps, breakpoints and variables are per core. Halting and resuming are per core.
5. When done — Terminate both sessions, then Build Targets → `reset`.

<details>
<summary>How it works and what to avoid</summary>

- OpenOCD `init` halts both cores, and `reset init` also halts the not-yet-woken V5F — such a V5F misses the V3F
  wake-up. So the H417_Cmake session sets hardware breakpoints on V3F `main` and on the V5F entry `0x10000` (they
  survive a reset), does `reset run` and resumes V3F only. The procedures are in `tools/h417.cfg`.
- Before it is woken, V5F sits at address 0. Resuming it then is forbidden: it would execute V3F code and the chip
  would stop answering until the RESET button. The V5F configurations refuse such an attach with
  `V5F is not woken by V3F yet`.
- While V5F waits at its entry its code is not yet copied into ITCM: set breakpoints in V5F code after it reaches
  `main`.
- Hardware breakpoints survive a chip reset. The `flash` and `reset` targets clear them on both cores.
- V3F waits 2 s for the V5F signal, then prints `no answer from V5F` and goes on — normal while debugging.
- When only V3F is debugged, V5F waits at its entry and LED2 stays off. To let V5F run on its own, add argument 0
  to the H417_Cmake command: `eval "monitor h417_debug_start 0x%x 0", &main`.
- WCH examples put V3F into STOP mode, where the debugger cannot attach. To flash over such firmware, start `flash`
  right after pressing RESET.
</details>

### CH32H417QEU6-R0 board

Schematic and PCB are in the WCH repository:
[EVT/PUB/SCHPCB/CH32H417QEU6-R0](https://github.com/openwch/ch32h417/tree/main/EVT/PUB/SCHPCB/CH32H417QEU6-R0).

| What | Connection |
|---|---|
| WCH-LinkE | P3: 1 = 3V3, 5 = PB9 (SWDIO), 7 = PB8 (SWCLK), the rest — GND |
| printf | USART1 TX = PA9 (P5.32) → WCH-LinkE UART RX, 115200 |
| LED1 / LED2 | jumpers P8.27 (PE2) ↔ P8.29 and P8.28 (PE3) ↔ P8.30; LEDs are on at 0 |
| KEY | P5.2, pulled up to 3V3 |

### Hardware verification

Building, flashing and debugging were run on a CH32H417QEU6-R0-1v1 board (CHIPID `0x4170052D` — CH32H417QEU,
revision 2, 960 KB flash), WCH-LinkE (firmware 2.18), MounRiver Studio 1.92, OpenOCD and GCC 15.2 from MounRiver
Studio 2. Verified: both LEDs without a debugger, V3F debugging, V5F debugging and both cores at once in MRS 1.92.

### License

The project layer (CMake, scripts, `tools/`, `Common/`, `User/V3F/main.c`, `User/V5F/main.c`) is
**[0BSD](LICENSE)**: use, copying and distribution without restriction and without an attribution requirement.

The peripheral-library and EVT files from Nanjing Qinheng (`Core/`, `Peripheral/`, `Startup/`, `Ld/`,
`Debug/debug.*`, `User/*/system_ch32h417.*`, `User/*/ch32h417_conf.h`, `User/*/ch32h417_it.*`,
`svd/CH32H417xx.svd`) are **not** covered by 0BSD: the WCH condition applies — for this manufacturer's
microcontrollers only.

Author: [mamkincoderr](https://github.com/mamkincoderr) · [Telegram](https://t.me/oDeXteRo)

---

## 中文

面向双核微控制器 **WCH CH32H417** 的工程模板：QingKe **V3F** 为主核，**V5F** 为从核。已在评估板
**CH32H417QEU6-R0-1v1** 上验证。

**用途。** 同一源码树构建两个镜像——V3F 和 V5F——以及二者的合并文件，一次烧录。工程师的主要工作环境是
**MounRiver Studio 1.92**：编辑、构建、烧录和调试。编译不由该 IDE 内置的 makefile 生成器完成，而是由可复现的
CMake / Ninja 链配合 **MounRiver Studio 2** 自带的 **RISC-V GCC 15** 完成；Studio 1.92 通过 Build Targets
（`build.bat`）调用它。烧录和调试使用 MounRiver Studio 2 的 OpenOCD。在 MRS 1.92 中可以同时调试两个内核，
每个内核一个会话。

外设库和启动文件为 WCH 官方 SDK [openwch/ch32h417](https://github.com/openwch/ch32h417)，EVT V1.6
（库 V1.0.6），未作修改。

`#CH32H417` `#CH32H417QEU6` `#CH32H417QEU6-R0-1v1`

### 固件功能

V3F 先启动，配置时钟和 USART1（printf 115200，PA9），唤醒 V5F，并通过硬件信号量 HSEM 等待其信号。之后 V3F
闪烁 LED1（2 Hz），V5F 闪烁 LED2（5 Hz）。等待 V5F 采用查询标志的方式，不进入 STOP 模式：在 STOP 模式下
调试器无法连接芯片。

### 两个内核

| 内核 | Flash | RAM 中的代码 | 数据 | GDB 端口 |
|---|---|---|---|---|
| V3F | `0x00000`，64 KB | `0x20100000`，64 KB | `0x20110000`，448 KB | 3333 |
| V5F | `0x10000`，128 KB | ITCM `0x200A0000`，128 KB | DTCM `0x200C0000`，256 KB | 3334 |

内核编译标志与 EVT 相同：`-march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32`，关闭 FPU。各内核代码位于
`User/V3F/` 和 `User/V5F/`，公共代码位于 `Common/`。两个内核都向 USART1 打印：V5F 默认的 UART8 在 PB4 上，
而 QEU 封装没有 PB4 引脚。

### 系统要求

- **Windows**

- **MounRiver Studio 1.92**——主要开发环境：源码、Build、调试器。

  <details>
  <summary>从哪里获取</summary>

  发行版在开发商网站：[mounriver.com/download](http://www.mounriver.com/download)。安装 Studio 1.9.x（1.92 分支）。
  在 IDE 中打开本仓库的 `H417_Cmake.wvproj`（File → Open Project）。

  内置 makefile 生成器已关闭：锤子按钮和 Build Targets 调用 `build.bat`。首次构建会在 `tools\` 中创建指向
  MounRiver Studio 2 编译器和 OpenOCD 的目录联接（junction）`gcc` 和 `openocd`，因此调试配置无需绝对路径。
  不需要管理员权限。
  </details>

- **MounRiver Studio 2**——GCC 15 编译器（`riscv32-wch-elf-gcc`）和带双核配置 `wch-dual-core.cfg` 的 OpenOCD。

  <details>
  <summary>从哪里获取</summary>

  Studio 2 的独立安装程序在同一处：[mounriver.com/download](http://www.mounriver.com/download)。若工具链目录中是
  压缩包而不是现成的文件夹

  `C:\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC15.zip`

  ——将其解压**到同一个** `Toolchain` 目录，不要多出一层嵌套。结果应当是：

  `C:\MounRiver\MounRiver_Studio2\resources\app\resources\win32\components\WCH\Toolchain\RISC-V Embedded GCC15\bin\riscv32-wch-elf-gcc.exe`

  Studio 2 安装在其他位置时，设置环境变量 `WCH_TOOLCHAIN_DIR`（编译器 `bin` 目录）和 `WCH_OPENOCD_DIR`
  （`openocd.exe` 所在目录）。
  </details>

- **CMake** 3.20 或更高版本。

  <details>
  <summary>从 PowerShell 安装</summary>

  ```powershell
  winget install --id Kitware.CMake --exact
  ```

  检查：`cmake --version`。`build.bat` 会在标准目录中查找 `cmake.exe`，无需重启 MRS。
  </details>

- **Ninja**。

  <details>
  <summary>从 PowerShell 安装</summary>

  ```powershell
  winget install --id Ninja-build.Ninja --exact
  ```

  检查：`ninja --version`。
  </details>

- **WCH-LinkE**——编程器和调试器。

  <details>
  <summary>连接</summary>

  适配器必须处于 RISC-V 模式：在 Windows 设备管理器中显示为 **WCH-LinkRV**，`VID_1A86&PID_8010`。板上为连接器
  P3：1 = 3V3，5 = PB9（SWDIO），7 = PB8（SWCLK），其余为 GND。
  </details>

### 构建与烧录

在 Project Explorer 中打开 **Build Targets**，双击目标：

| 目标 | 作用 |
|---|---|
| `all` | 构建两个内核及合并文件；锤子按钮相同 |
| `flash` | 构建、烧录 `obj\H417_Merge.hex`（两个内核）、校验并运行芯片 |
| `reset` | 清除遗留的硬件断点，复位并运行芯片 |
| `erase` | 擦除整个 Flash |
| `info` | CHIPID、Flash 容量、UID |
| `clean` | 删除 `obj` |

在控制台中相同：`build.bat <目标>`。

### 双核调试

Run → Debug Configurations → GDB OpenOCD MRS Debugging：

| 配置 | 内核 | 作用 |
|---|---|---|
| **H417_Cmake** | V3F | 构建、烧录两个内核，使 V3F 停在 `main`；启动 OpenOCD |
| **H417_V5F_dual** | V5F | 连接 H417_Cmake 会话的 OpenOCD，使 V5F 运行到其 `main` |
| **H417_V5F** | V5F | 单独使用，无 V3F 会话：暂停正在运行的 V5F；V3F 继续运行 |

同时调试两个内核：

1. **Debug → H417_Cmake。** V3F 停在 `main`。
2. 在 V3F 会话中 **Resume（F8）**。V3F 唤醒 V5F，V5F 停在其入口 `0x10000`。
3. 保持 V3F 会话不关闭，**Debug → H417_V5F_dual。** V5F 运行到其 `main`。
4. 在 Debug 视图中选择内核：单步、断点和变量各自独立。两个内核的暂停与运行互不影响。
5. 结束时——终止（Terminate）两个会话，然后 Build Targets → `reset`。

<details>
<summary>工作原理与注意事项</summary>

- OpenOCD 的 `init` 会暂停两个内核，而 `reset init` 也会暂停尚未被唤醒的 V5F——这样的 V5F 收不到 V3F 的唤醒。
  因此 H417_Cmake 会话在 V3F 的 `main` 和 V5F 的入口 `0x10000` 设置硬件断点（复位后仍保留），执行 `reset run`，
  只恢复运行 V3F。相关过程位于 `tools/h417.cfg`。
- V5F 被唤醒前停在地址 0。此时禁止恢复其运行（Resume）：它会执行 V3F 的代码，芯片将无法响应，直到按下 RESET。
  V5F 配置会拒绝这种连接并提示 `V5F is not woken by V3F yet`。
- V5F 在入口等待时，其代码尚未复制到 ITCM：请在 V5F 运行到 `main` 之后再在其代码中设置断点。
- 硬件断点在芯片复位后仍然保留。`flash` 和 `reset` 目标会清除两个内核上的断点。
- V3F 等待 V5F 信号 2 秒，然后打印 `no answer from V5F` 并继续运行——调试时属正常现象。
- 只调试 V3F 时，V5F 在入口等待，LED2 不亮。若要让 V5F 自行运行，在 H417_Cmake 的命令中加参数 0：
  `eval "monitor h417_debug_start 0x%x 0", &main`。
- WCH 例程会让 V3F 进入 STOP 模式，此时调试器无法连接。要在此类固件上烧录，请在按下 RESET 后立即执行 `flash`。
</details>

### CH32H417QEU6-R0 评估板

原理图和 PCB 位于 WCH 仓库：
[EVT/PUB/SCHPCB/CH32H417QEU6-R0](https://github.com/openwch/ch32h417/tree/main/EVT/PUB/SCHPCB/CH32H417QEU6-R0)。

| 项目 | 连接 |
|---|---|
| WCH-LinkE | P3：1 = 3V3，5 = PB9（SWDIO），7 = PB8（SWCLK），其余为 GND |
| printf | USART1 TX = PA9（P5.32）→ WCH-LinkE 串口 RX，115200 |
| LED1 / LED2 | 跳线 P8.27（PE2）↔ P8.29 与 P8.28（PE3）↔ P8.30；输出 0 时点亮 |
| KEY | P5.2，上拉至 3V3 |

### 硬件验证

构建、烧录和调试已在 CH32H417QEU6-R0-1v1 评估板（CHIPID `0x4170052D`——CH32H417QEU，版本 2，Flash 960 KB）上
运行，WCH-LinkE（固件 2.18），MounRiver Studio 1.92，以及 MounRiver Studio 2 的 OpenOCD 和 GCC 15.2。已验证：
无调试器时两个 LED 正常闪烁、V3F 调试、V5F 调试，以及在 MRS 1.92 中同时调试两个内核。

### 许可证

工程层（CMake、脚本、`tools/`、`Common/`、`User/V3F/main.c`、`User/V5F/main.c`）为 **[0BSD](LICENSE)**：
可无限制地使用、复制和分发，且无需署名。

南京沁恒（Nanjing Qinheng）的外设库和 EVT 文件（`Core/`、`Peripheral/`、`Startup/`、`Ld/`、`Debug/debug.*`、
`User/*/system_ch32h417.*`、`User/*/ch32h417_conf.h`、`User/*/ch32h417_it.*`、`svd/CH32H417xx.svd`）**不**在
0BSD 范围内：适用 WCH 的条件——仅限该厂商的微控制器。

作者：[mamkincoderr](https://github.com/mamkincoderr) · [Telegram](https://t.me/oDeXteRo)
