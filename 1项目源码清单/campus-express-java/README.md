# 校园快递取件信息管理系统（Java 版）

将原 C++ / MFC 项目的快递管理业务重新组织成 Java 控制台程序，练习面向对象、集合、业务校验和文件持久化。此版本未移植 MFC 界面，也没有数据库、Web、框架或第三方依赖。代码为辅助生成的学习起点，面试中应如实描述自己阅读、修改和验证过的部分。

## 技术栈与运行要求

- Java 17 语言和标准库；推荐使用 JDK 17 或 JDK 21。
- 控制台交互、类与对象、枚举、`ArrayList`、异常处理、`java.nio.file`。
- UTF-8 本地 TSV 存储，无 Maven、Gradle、JUnit 或其他外部依赖。
- 本次环境实际安装 JDK 26.0.2.1，使用 `javac --release 17` 编译，使用 JDK 26 运行。**未在真实 JDK 17 / 21 运行时及 IntelliJ 图形界面中执行验证**。`--release 17` 检查 Java 17 的语言和标准 API 兼容性，不等于实际运行了 JDK 17。

## 项目结构

```text
campus-express-java/
├─ src/main/java/campus/express/
│  ├─ Main.java                 入口：组装对象和确定数据路径
│  ├─ ExpressStatus.java        待取件、已取件两个状态
│  ├─ Express.java              实体及字段格式校验
│  ├─ ExpressService.java       增删改查、唯一性、取件、提交保存
│  ├─ ExpressRepository.java    UTF-8 TSV 读取和原子保存
│  └─ ConsoleUI.java            菜单、输入输出、删除确认
├─ src/test/java/campus/express/ExpressTests.java
├─ data/express.tsv            第一次成功修改数据时生成
├─ build/                      编译产物（自动生成）
├─ build.ps1 / run.ps1 / test.ps1
├─ README.md / DEMO.md / LEARNING_NOTES.md
└─ TEST_RESULTS.md             实际验证结果
```

## 功能与业务约定

| 菜单 | 功能 | 规则 |
| --- | --- | --- |
| 1 | 新增 | 单号和取件码分别唯一；初始待取件 |
| 2 | 全部记录 | 展示全部六个字段；空数据给出提示 |
| 3 | 查询 | 选择取件码或单号，精确匹配 |
| 4 | 修改 | 先查单号，重新录入四项；不能更改单号、状态 |
| 5 | 删除 | 展示记录，只有输入 Y/y 才执行 |
| 6 | 取件 | 通过取件码定位，禁止重复取件 |
| 0 | 退出 | 每次修改已立即保存，无退出时补保存流程 |

空值、空白字段、控制字符、无效菜单及不存在的记录均有反馈。所有字段去掉首尾空白，最长 100 个 UTF-16 字符单位；单号和取件码区分大小写。联系电话为 **7～15 位 ASCII 数字字符串**，保留前导零；这是本项目简化规则，不是完整国际电话校验，不接受 `+`、空格或连字符。已取件记录仍占用取件码；删除后其单号、取件码可以再次使用。修改已取件记录不会恢复待取件。

## Windows PowerShell 运行

1. 安装并选择 JDK 17 或 21，确认 `java -version` 和 `javac -version` 均可运行。已有可用 JDK 时不需要仅为本项目设置 `JAVA_HOME`。
2. 在本项目目录打开终端，执行：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\run.ps1
```

脚本会编译并运行，工作目录固定为项目目录；执行策略只对这次 PowerShell 进程生效。首次运行没有数据文件是正常现象。文件实际位置会在启动时打印。

仅编译或执行测试：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\test.ps1
```

也可以不使用脚本，在项目目录直接执行：

```powershell
New-Item -ItemType Directory -Force build/classes, build/test-classes
javac --release 17 -encoding UTF-8 -d build/classes src/main/java/campus/express/*.java
javac --release 17 -encoding UTF-8 -cp build/classes -d build/test-classes src/test/java/campus/express/*.java
java -cp build/classes campus.express.Main
java -cp "build/classes;build/test-classes" campus.express.ExpressTests
```

每条编译命令必须成功后再运行下一条。Windows 类路径使用分号。交互终端使用 Java Console 的 reader/writer；IDE 或重定向输入输出使用 UTF-8。测试输出也明确使用 UTF-8；终端若乱码，应将显示编码设为 UTF-8，例如先执行 `chcp 65001`。不要把终端显示乱码误判成数据文件损坏。

## IntelliJ IDEA 打开与运行

1. 使用 File → Open 打开 `campus-express-java` 文件夹，等待项目加载。
2. File → Project Structure → Project：选择 JDK 17 或 21，Language level 设为 17。若当前仅安装 JDK 26，也可先用它配合语言级别 17；本次命令行验证使用它。
3. 若 IDE 没有自动识别 Java 模块，在 Modules 中添加 Java 模块，Content root 指向项目目录，选择已有源码，不创建额外示例代码。
4. 右键 `src/main/java` → Mark Directory as → Sources Root；`src/test/java` → Test Sources Root。`build` 标为 Excluded。
5. Settings → Editor → File Encodings：项目和文件编码使用 UTF-8；若运行控制台可单独设置编码，也选 UTF-8。
6. 打开 `Main.java`，通过 `main` 左侧运行按钮启动。Run → Edit Configurations → Working directory 设为本项目根目录（可用 `$PROJECT_DIR$`）。
7. 测试也可运行 `ExpressTests.main`，但它的子进程测试需要 `build/classes`：先运行 `build.ps1`，测试工作目录也设为项目根目录。最简单的测试入口始终是 `test.ps1`。

## 核心业务流程

```text
启动 → Repository.load → 有效记录装入 ArrayList → 显示菜单
输入 → UI → Service → 校验字段/唯一性/状态
                  → 创建候选列表 → Repository.save
                  → 临时文件完整写入 → 原子替换
                  → 保存成功后替换内存列表 → UI 显示成功
```

保存失败：抛出异常，旧内存保持不变，界面提示本次操作未生效。正常环境恢复后可重新操作。实体没有 setter，修改通过新对象进行，防止候选数据修改到旧对象。

## 文件格式、异常与恢复

文件是 `data/express.tsv`，相对于**工作目录**，脚本已固定该目录。每行六列，无表头：单号、姓名、电话、取件码、公司、状态；列间为真实制表符，状态写 `PENDING` 或 `PICKED_UP`，编码 UTF-8（无 BOM）。

- 逗号、双引号属于普通内容，能原样往返。不是 CSV，不需要逗号转义。
- 字段拒绝制表符、CR/LF、其他 ISO 控制字符和 Unicode 行/段分隔符；不支持多行地址或备注。控制台每次输入本身也是一行，请勿批量粘贴多行到单个字段。
- 缺文件、零字节文件：正常空数据；空白行、错误列数、非法状态/电话、重复单号/取件码：跳过该行并提示行号，继续读取后面的有效行。
- 只要出现坏行或读错误，整个会话进入只读模式，允许查询，不允许增删改或取件，避免把被跳过的原始数据覆盖丢失。重复键保留先读到的有效记录。
- 恢复步骤：退出程序 → 复制原文件留存备份 → 用支持 UTF-8 的编辑器修复所提示行 → 重新启动确认无警告。不确定如何修复时保留原文件，不要直接清空。
- 保存先写同目录临时文件，再要求原子替换。文件系统不支持原子替换或权限不足等情况会明确失败，不采用不安全的覆盖写入回退。清理失败可能遗留 `express-*.tmp`，它不会被读取；退出后可检查再清理。

## 测试方式

运行 `test.ps1`，无需 `-ea`，失败会抛出 `AssertionError` 并返回非零状态。测试全部使用系统临时目录，不读取或覆盖项目内用户数据；目录路径会打印，故意保留输入、输出和异常数据供检查。29 组测试涵盖业务规则、菜单、文件异常与三个独立 JVM 的重启验证。真实结果见 [TEST_RESULTS.md](TEST_RESULTS.md)。

## 已知局限和后续优化

- 只支持单用户、单进程本地演示。没有文件锁，同时打开两个程序可能互相覆盖；运行中不要手工修改数据文件。
- 查询、单次唯一性判断是线性遍历 O(n)，每次保存重写整个文件；启动逐条查重最坏 O(n²)。未测试吞吐量、并发、用户规模，不能宣称生产级性能。
- 原子替换减少半写文件风险，但不承诺断电后的磁盘持久性；无事务日志、备份轮换、操作审计。
- 没有登录鉴权、加密、网络共享、数据库、Web 界面及自动修复；演示使用虚构姓名和电话。实际文件为明文。
- 没有自动化验证真实断电、磁盘满或所有权限/文件系统差异。损坏文件按只读保护处理，需人工修复。
- 可先补“按状态筛选”和有针对性的测试；理解现有边界后，再考虑索引、文件锁、日志，以及单独的 MySQL/JDBC 版本。上述均未实现。

## 阅读与学习

建议先运行 [DEMO.md](DEMO.md)，然后按 `ExpressStatus → Express → ExpressRepository → ExpressService → ConsoleUI → Main → ExpressTests` 阅读。[LEARNING_NOTES.md](LEARNING_NOTES.md) 解释每一部分如何协作。
