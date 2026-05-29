# serial_prot 架构说明

## 概览

`serial_prot` 是一个基于 Qt Widgets 的通信调试工具，当前支持串口调试和网口调试。应用以 `MainWindow` 为外壳，每个标签页对应一个独立面板：串口标签页使用 `SerialPanel`，网口标签页使用 `NetworkPanel`。

当前实现偏向单进程桌面工具，没有单独的后台服务层。串口读写、网口读写、解析和界面更新都在 Qt 对象信号槽链路内完成。

## 目录结构

```text
serial_prot/
|-- CMakeLists.txt              构建配置
|-- main.cpp                    程序入口
|-- mainwindow.ui               主窗口界面和工具栏
|-- mainwindow.h/.cpp           主窗口逻辑
|-- serialpanel.ui              串口面板界面
|-- SerialPanel.h/.cpp          单个串口面板核心逻辑
|-- SerialManager.h/.cpp        QSerialPort 封装
|-- SerialConfig.h              串口配置结构和 QSettings 读写
|-- SerialRecord.h              收发记录数据结构
|-- SendQueue.h/.cpp            串口和网口共享的发送队列、单项间隔、循环和速率控制
|-- DeviceRegistry.h/.cpp       串口设备识别、别名、一键复连
|-- RecordStore.h/.cpp          串口记录存储、过滤、统计辅助
|-- AutomationRuleEngine.h/.cpp 自动回复规则匹配
|-- TemplateManager.h/.cpp      串口配置模板和记录导出辅助
|-- ParserController.h/.cpp     串口解析器切换和分发
|-- networkpanel.ui             网口面板界面
|-- NetworkPanel.h/.cpp         单个网口面板核心逻辑
|-- NetworkManager.h/.cpp       TCP/UDP 封装
|-- NetworkConfig.h             网口配置结构和 QSettings 读写
|-- ProtocolParserBase.h        协议解析器抽象基类
|-- RawDataParser.h/.cpp        原始数据解析器
|-- TextDataParser.h/.cpp       文本解析器
|-- ProtocolParser.h/.cpp       自定义协议解析器
|-- ThemeManager.h/.cpp         主题管理
`-- ARCHITECTURE.md            本文档
```

## 顶层模块关系

```text
MainWindow
  |
  |-- QTabWidget
        |
        |-- SerialPanel 1
        |     |-- SerialManager
        |     |-- ParserController
        |     |     |-- ProtocolParserBase
        |     |     |-- RawDataParser
        |     |     |-- TextDataParser
        |     |     `-- ProtocolParser
        |     |-- RecordStore
        |     |-- DeviceRegistry
        |     |-- AutomationRuleEngine
        |     |-- TemplateManager
        |     `-- SendQueue
        |
        |-- NetworkPanel 1
        |     |-- NetworkManager
        |     |     |-- QTcpSocket
        |     |     |-- QTcpServer
        |     |     `-- QUdpSocket
        |     |-- 发送定时器
        |     `-- SendQueue
        |
        `-- SerialPanel / NetworkPanel N
```

## MainWindow

`MainWindow` 负责应用外壳，不直接处理串口或网口数据。

主要职责：

- 创建、关闭和切换多个 `SerialPanel` / `NetworkPanel` 标签页。
- 提供菜单栏、工具栏、状态栏，相关控件定义在 `mainwindow.ui`。
- 管理暗色和亮色主题切换。
- 保存和恢复所有通信面板的基础配置。
- 将当前串口页的配置模板批量应用到其他串口页。

关键流程：

```text
启动
  -> setupUiFromForm()
  -> setupConnections()
  -> loadSettings()
  -> 若无历史配置则 addSerialPort()

新增串口
  -> addSerialPort()
  -> createSerialPortPanel()
  -> new SerialPanel
  -> tabWidget->addTab()

新增网口
  -> addNetworkPort()
  -> createNetworkPanel()
  -> new NetworkPanel
  -> tabWidget->addTab()

退出
  -> closeEvent()
  -> saveSettings()
```

持久化入口：

- `loadSettings()`
- `saveSettings()`

批量配置入口：

- `applyCurrentConfigToAll()`

## SerialPanel

`SerialPanel` 是当前项目的核心模块，负责单个串口标签页内的大部分业务逻辑。

主要职责：

- 从界面读取串口参数并打开或关闭串口。
- 维护 DTR、RTS、发送换行模式、编码模式等配置。
- 接收串口数据并分发给当前协议解析器。
- 将 RX、TX、系统错误统一记录为 `SerialRecord`。
- 展示接收区、发送历史、解析结果表格和 JSON 预览。
- 支持过滤、高亮、自动回复、设备别名、一键复连。
- 支持配置模板导入导出。
- 支持 TXT、CSV、JSON 格式保存数据。
- 支持定时发送、发送预估字节数、发送队列、重复次数、单项间隔、队列项编辑和循环队列控制。
- 通过运行期 `QToolButton + QMenu` 收纳低频操作。
- 通过 `.ui` 中的 `QSplitter` 让主区域、接收区、发送区、历史区、自动化区和解析区可调整大小。

### 界面组成

`serialpanel.ui` 提供串口面板的控件、主布局和分割器布局：

- 连接设置区
- 接收区
- 发送区
- 发送历史区
- 设备别名输入框
- 一键复连按钮
- 模板导入和导出按钮
- 设备信息、健康状态、统计信息
- 过滤输入框和过滤选项
- 自动回复规则
- 解析结果表格
- JSON 预览框

连接设置区拆成三行，避免端口、波特率、数据位、校验位、停止位、别名、模板按钮和状态信息全部挤在一行。接收区默认占更大比例，发送区限制默认高度，右侧辅助区限制默认宽度。`SerialPanel::bindFeatureUi()`、`setupSendPreview()` 和 `setupSendQueueControls()` 绑定 `.ui` 中已有控件；`setupActionMenus()` 仍负责运行期菜单。

```text
连接设置
  第 1 行：端口、波特率、数据位、校验位、停止位
  第 2 行：设备别名、隐藏的别名/模板按钮入口
  第 3 行：打开、关闭、串口操作菜单、状态灯、接收计数、发送计数

可调布局
  主体：左侧收发区 / 右侧辅助区，左右可拉动
  左侧：接收区 / 发送区，上下可拉动
  右侧：发送历史 / 自动化 / 解析结果，上下可拉动

运行期菜单
  串口操作：DTR、RTS、保存别名、一键复连、导入/导出模板、清空接收区
  显示设置：十六进制显示、自动滚动、时间戳
  发送操作：定时发送、发送文件、保存数据
  历史操作：清空历史

发送队列控件
  发送区：加入队列、插入选中前、删除选中、清空队列、开始队列、停止
  发送选项：入队次数、入队间隔、循环队列、队列状态
  队列表格：序号、剩余次数、总次数、间隔、字节数、十六进制、内容
  可编辑列：剩余次数、总次数、间隔、内容
```

## NetworkPanel

`NetworkPanel` 负责单个网口调试标签页。

主要职责：

- 支持 TCP 客户端、TCP 服务端和 UDP 三种模式。
- 从界面读取本地地址、本地端口、远端地址和远端端口。
- 通过 `NetworkManager` 打开、关闭和发送数据。
- 展示接收区、连接对象、状态统计、最近来源和最近错误。
- 支持十六进制显示、十六进制发送、时间戳、自动滚动。
- 支持 UTF-8、GBK、ASCII 编码选择。
- 支持定时发送、发送预估字节数、发送队列、重复次数、单项间隔、队列项编辑和循环队列控制。
- 支持发送文本、发送文件和保存接收数据。
- 通过运行期 `QToolButton + QMenu` 收纳低频操作。
- 通过 `.ui` 中的 `QSplitter` 让主区域、接收区、发送区、连接对象区和状态统计区可调整大小。

### 界面组成

`networkpanel.ui` 提供网口面板的控件、主布局和分割器布局：

- 网口连接区
- 接收区
- 发送区
- 连接对象区
- 状态统计区

```text
网口连接
  第 1 行：模式、本地地址、本地端口
  第 2 行：远端地址、远端端口、打开、关闭、网口操作菜单、状态灯

主体区域
  左侧：接收区、发送区
  右侧：连接对象、状态统计

可调布局
  主体：左侧收发区 / 右侧信息区，左右可拉动
  左侧：接收区 / 发送区，上下可拉动
  右侧：连接对象 / 状态统计，上下可拉动

运行期菜单
  网口操作：清空接收区
  显示设置：十六进制显示、时间戳、自动滚动
  发送操作：十六进制发送、定时发送、发送文件、保存数据

发送队列控件
  发送区：加入队列、插入选中前、删除选中、清空队列、开始队列、停止
  发送选项：入队次数、入队间隔、循环队列、队列状态
  队列表格：序号、剩余次数、总次数、间隔、字节数、目标、十六进制、内容
  可编辑列：剩余次数、总次数、间隔、内容
```

## NetworkManager

`NetworkManager` 是 TCP/UDP 的薄封装，负责网络 I/O，不处理界面展示。

主要职责：

- TCP 客户端：使用 `QTcpSocket` 连接远端。
- TCP 服务端：使用 `QTcpServer` 监听本地端口，并维护客户端列表。
- UDP：使用 `QUdpSocket` 绑定本地端口并发送数据报。
- 统一发出 `opened()`、`closed()`、`dataReceived()`、`errorOccurred()` 和 `peerListChanged()` 信号。

发送行为：

- TCP 客户端发送到当前连接。
- TCP 服务端可以向全部已连接客户端广播，也可以按界面选中的客户端发送。
- UDP 发送到面板中的远端地址和远端端口。
- 发送失败时返回 0，并发出错误信号。

## SerialManager

`SerialManager` 是 `QSerialPort` 的薄封装，负责串口 I/O，不处理界面展示。

主要职责：

- 保存和应用 `SerialConfig`。
- 打开和关闭串口。
- 发送数据。
- 设置 DTR 和 RTS。
- 缓冲接收数据，并按时间或大小批量发出。
- 转发串口错误。

接收节流参数：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `m_flushIntervalMs` | 20 ms | 接收缓冲刷新间隔 |
| `m_maxChunkSize` | 4096 字节 | 缓冲达到该大小时立即刷新 |

发送行为：

- `sendData()` 返回 `QSerialPort::write()` 实际接受的字节数。
- 未打开串口或写入失败时返回 0，并发出错误信号。
- UI 侧只按成功写入的字节数更新发送统计。

## 协议解析层

解析器使用简单策略模式。`SerialPanel` 持有 `ParserController`，协议下拉框切换时由 `ParserController::switchTo()` 销毁旧解析器并创建新解析器。`SerialManager::dataReceived` 只连接到 `ParserController::inputBytes()`，切换解析器时不需要重新连接串口信号。

```text
ProtocolParserBase
  |-- RawDataParser
  |-- TextDataParser
  `-- ProtocolParser
```

解析器输出信号：

- `rawDataReady(data)`：原始数据。
- `frameReady(payload, info)`：已解析帧。
- `frameError(message)`：解析错误。

当前解析器：

| 解析器 | 行为 |
| --- | --- |
| `RawDataParser` | 原样透传接收数据 |
| `TextDataParser` | 将接收数据作为一帧交给面板统一解码显示 |
| `ProtocolParser` | 解析 `0x55 0xAA + 长度 + 负载 + CRC16` 格式 |

自定义协议帧格式：

```text
0x55 0xAA Length Payload CRC16
 1B   1B    1B    0-255B  2B
```

CRC 使用 CRC-16/Modbus，初始值为 `0xFFFF`，CRC 低字节在前。

## 数据模型

### SerialConfig

`SerialConfig` 表示串口基础参数。

```cpp
struct SerialConfig {
    QString portName;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;

    void save(QSettings& settings) const;
    void load(QSettings& settings);
};
```

### NetworkConfig

`NetworkConfig` 表示网口基础参数。

```cpp
struct NetworkConfig {
    int mode;
    QString localAddress;
    quint16 localPort;
    QString remoteHost;
    quint16 remotePort;
    bool hexDisplay;
    bool hexSend;
    bool showTimestamp;
    bool autoScroll;
    QString encoding;

    void save(QSettings& settings) const;
    void load(QSettings& settings);
};
```

### SerialRecord

`SerialRecord` 是统一收发记录模型。过滤、保存、解析预览和统计都应该基于它扩展。

```cpp
enum class SerialRecordDirection {
    Rx,
    Tx,
    System
};

struct SerialRecord {
    QDateTime timestamp;
    SerialRecordDirection direction;
    QByteArray payload;
    QString text;
    QString protocol;
    QString info;
    QString error;
};
```

记录类型：

- `Rx`：接收到的数据或协议帧。
- `Tx`：发送成功进入串口写缓冲的数据。
- `System`：错误、状态或内部事件。

### SendQueue

`SendQueue` 是串口和网口共享的发送队列对象，只负责队列、重复次数、每项发送间隔、循环、插入、删除和队列项更新，不直接依赖串口或网口 I/O。

队列项包含：

- `payload`：已经按当前面板规则编码好的待发送字节。
- `displayText`：发送成功后用于历史或接收区展示的文本。
- `repeatCount`：该项每轮应发送的总次数。
- `remaining`：当前轮剩余发送次数。
- `intervalMs`：该项发送完成后到下一次发送之间的等待时间。
- `remoteHost` / `remotePort` / `peer`：网口队列项入队时冻结的发送目标。串口发送时忽略这些字段。

循环队列开启后，队列项当前轮发送完成时会重置 `remaining = repeatCount` 并移动到队尾；关闭循环时，当前轮发送完成的队列项会被移除。

核心信号：

- `sendRequested(payload, displayText, remoteHost, remotePort, peer)`：队列请求面板发送下一条。
- `queueChanged(pendingItems, pendingSends, running)`：更新队列状态和按钮状态。
- `finished()`：队列全部发送完成。

队列表格由 `SerialPanel` / `NetworkPanel` 根据 `SendQueue::itemAt()` 刷新。串口表格展示序号、剩余次数、总次数、间隔、字节数、十六进制和内容；网口表格额外展示发送目标。表格编辑会通过 `SendQueue::updateItem()` 回写队列项。

## 数据流

### 接收数据流

```text
串口设备
  -> QSerialPort::readyRead
  -> SerialManager::handleReadyRead()
  -> SerialManager::flushRxBuffer()
  -> SerialManager::dataReceived
       |-> SerialPanel::onDataReceived()
       |     更新 RX 字节统计
       |
       `-> ProtocolParserBase::inputBytes()
             |-> RawDataParser::rawDataReady
             |-> TextDataParser::frameReady
             `-> ProtocolParser::frameReady / frameError
                    -> SerialPanel::onRawDataReady()
                    -> SerialPanel::onFrameReady()
                    -> SerialPanel::onFrameError()
                    -> addRecord()
                    -> appendRecordToReceive()
                    -> refreshParsedFrameView()
                    -> handleAutoReply()
```

### 发送数据流

```text
用户输入或自动回复
  -> SerialPanel::updateSendPreview()
       根据换行模式和编码模式预估发送字节数
  -> SerialPanel::encodeText()
  -> SerialPanel::sendPayload()
  -> SerialManager::sendData()
  -> QSerialPort::write()
  -> addRecord(direction = Tx)
  -> addToHistory()
  -> updateStatistics()
```

定时发送不会走单独的数据路径。`m_timerSendTimer` 的 `timeout` 信号直接复用 `onSendClicked()`，因此编码、换行、发送记录和统计逻辑与手动发送保持一致。

### 串口队列发送数据流

```text
用户输入
  -> SerialPanel::onAddQueueClicked()
       按当前换行模式和编码模式生成 payload
  -> SendQueue::enqueue(payload, displayText, repeatCount, intervalMs)
  -> SendQueue::start()
  -> SendQueue::sendRequested
  -> SerialPanel::onQueueSendRequested()
  -> SerialPanel::sendPayload()
  -> SerialManager::sendData()
```

队列发送复用 `sendPayload()`，因此发送统计、`SerialRecord`、发送历史和过滤刷新逻辑与手动发送一致。

使用 `SerialPanel::onInsertQueueClicked()` 可以把当前输入插入到选中队列行之前；`onRemoveQueueClicked()` 删除选中行；`onClearQueueClicked()` 清空队列。

### 网口接收数据流

```text
TCP/UDP 设备
  -> QTcpSocket / QTcpServer / QUdpSocket
  -> NetworkManager
  -> NetworkManager::dataReceived(payload, peer)
  -> NetworkPanel::onDataReceived()
  -> appendLine()
  -> updateStatistics()
```

### 网口发送数据流

```text
用户输入或文件
  -> NetworkPanel::updateSendPreview()
       文本模式：按当前编码预估字节数
       十六进制模式：校验十六进制内容并预估字节数
  -> NetworkPanel::encodeSendText()
       文本模式：按 UTF-8 / GBK / ASCII 编码
       十六进制模式：解析十六进制文本为字节
  -> NetworkManager::sendData()
       |-> QTcpSocket::write()
       |-> 多个服务端客户端 QTcpSocket::write()
       `-> QUdpSocket::writeDatagram()
  -> appendLine()
  -> updateStatistics()
```

网口定时发送同样复用 `onSendClicked()`，因此 TCP 客户端、TCP 服务端广播/定向发送、UDP 远端发送、十六进制发送和编码选择都沿用同一条路径。

### 网口队列发送数据流

```text
用户输入
  -> NetworkPanel::onAddQueueClicked()
       文本模式：按 UTF-8 / GBK / ASCII 编码
       十六进制模式：校验并解析十六进制内容
       冻结当前远端地址、远端端口和发送目标
  -> SendQueue::enqueue(payload, displayText, repeatCount, intervalMs, remoteHost, remotePort, peer)
  -> SendQueue::start()
  -> SendQueue::sendRequested
  -> NetworkPanel::onQueueSendRequested()
  -> NetworkManager::sendData()
```

网口队列发送不会在执行时重新读取目标控件，避免队列运行过程中切换目标导致已入队内容被发送到新的地址或客户端。

使用 `NetworkPanel::onInsertQueueClicked()` 可以把当前输入和当前发送目标插入到选中队列行之前；删除和清空逻辑与串口一致。

## 持久化

使用 `QSettings("yeyue", "serial_prot")`。

| 键 | 内容 |
| --- | --- |
| `panels` | 当前标签页列表，按 `type=serial/network` 区分 |
| `serialPanels` | 旧版本串口页配置，仅用于兼容读取 |
| `currentTab` | 上次激活的标签页索引 |
| `deviceAliases/{key}` | 设备别名，按设备识别 key 保存 |
| `network/encoding` | 网口文本收发编码 |

设备识别 key 优先级：

1. USB VID、PID、序列号。
2. 描述、厂商、序列号组合。
3. 端口名。

## 配置模板

配置模板通过 JSON 文件导入导出，内容包含：

- 串口参数。
- DTR、RTS。
- 当前协议模式。
- 换行模式。
- 编码模式。
- 设备别名。

`MainWindow::applyCurrentConfigToAll()` 会读取当前页模板，并应用到其他页；应用时不会覆盖其他页的端口名。

## 主题系统

`ThemeManager` 通过全局样式表切换主题。

当前主题：

- 暗色主题：Catppuccin Mocha 风格。
- 亮色主题：Catppuccin Latte 风格。

需要注意：部分状态颜色依赖控件 `objectName`，例如 `statusConnected` 和 `statusDisconnected`。

## 当前架构问题

这些不是阻塞问题，但后续继续扩展时建议优先处理。

### SerialPanel 过大

`SerialPanel` 同时承担连接管理、收发、解析展示、过滤、自动回复、设备别名、模板和日志保存。继续增加协议编辑器、队列编辑器或远程 API 时，文件会继续膨胀。

项目已经拆出一部分辅助对象：

- `DeviceRegistry`：设备识别、别名、一键复连。
- `RecordStore`：`SerialRecord` 存储、过滤、导出。
- `AutomationRuleEngine`：接收匹配和自动回复。
- `TemplateManager`：配置模板导入导出。
- `ParserController`：解析器切换和多协议分发。
- `SendQueue`：发送队列、重复次数、单项间隔和循环发送。

后续建议继续拆分布局状态、日志导出策略、队列编辑器持久化和解析结果模型。

### 面板职责可以继续下沉

串口和网口界面主体、分割器、发送预估标签和发送队列控件均由 `.ui` 文件定义；面板类仍在运行期创建低频操作菜单。`SerialPanel` 和 `NetworkPanel` 仍然同时承担状态管理、编码转换、展示、导出和菜单增强逻辑。后续功能继续增加时，应优先把记录存储、导出、自动化规则、队列配置持久化和布局状态拆成独立对象。

### 布局状态尚未持久化

当前各区通过 `QSplitter` 可以手动调整大小，但比例没有保存。重启应用后会回到默认比例。

建议：

- 为每个面板保存顶层、左右、左侧、右侧 splitter 的 `saveState()`。
- 按面板类型和标签页顺序恢复 `restoreState()`。
- 若后续支持面板重命名，应将布局状态绑定到稳定面板 id。

### 解析结果模型偏薄

当前解析器只输出 `payload + info`，解析结果表格只能显示协议、长度、负载。要实现真正的字段表格、校验高亮和 JSON 结构化预览，需要引入更丰富的帧模型。

建议新增：

```cpp
struct ParsedField {
    QString name;
    QString value;
    QByteArray raw;
    QString note;
};

struct ParsedFrame {
    QString protocol;
    QByteArray raw;
    QList<ParsedField> fields;
    bool checksumOk;
    QString error;
};
```

### 自动回复缺少防循环保护

当前自动回复基于接收记录触发。若外部设备回显自动回复内容，可能形成持续触发。

建议：

- 增加冷却时间。
- 增加单条规则最大触发次数。
- 增加“只触发一次”选项。

### 日志导出和界面显示耦合

文本导出目前保存的是 `rxEdit` 当前显示内容，若开启隐藏未匹配记录，导出的文本会受过滤影响。CSV 和 JSON 使用完整 `SerialRecord`，行为更稳定。

建议：

- 文本导出也基于 `SerialRecord` 生成。
- 提供“导出全部记录”和“导出当前过滤结果”两个选项。

### 缺少测试

当前项目没有自动化测试。后续至少应给以下逻辑补测试：

- `ProtocolParser` 的有效帧和 CRC 错误帧。
- 设备 key 生成规则。
- 配置模板导入导出。
- 过滤规则和正则规则。
- 自动回复触发规则。
- TCP 客户端连接失败和断开重连。
- TCP 服务端多客户端收发。
- UDP 绑定端口和数据报发送。

## 后续演进顺序

建议按以下顺序继续扩展：

1. 保存和恢复 `QSplitter` 布局比例。
2. 增强发送队列：支持队列导入导出和分包发送。
3. 增加接收区搜索、高亮、暂停显示和“清空显示但保留记录”。
4. 把 `SerialRecord` 相关逻辑拆成独立记录仓库，并让网口也使用统一记录模型。
5. 引入结构化 `ParsedFrame`。
6. 做协议字段表格和 JSON 预览的真实字段展示。
7. 做日志回放，把日志重新喂给解析链路。
8. 做多协议自动识别分发。
9. 增加网口客户端备注名和 UDP 主机名解析。
10. 最后再考虑插件机制、HTTP/WebSocket API 和 CLI。

## 可继续增加的功能

短期适合增加、风险较低的功能：

- 布局比例保存/恢复。
- 接收区搜索、高亮和跳转。
- 暂停接收显示：后台继续统计和记录，界面暂停追加。
- 发送模板/常用命令：按编码和十六进制模式保存常用发送内容。
- 发送队列导入导出。
- 网口 TCP 客户端断线自动重连。
- TCP 服务端客户端备注名和定向发送增强。
- 导出当前过滤结果和导出全部记录两个入口。

中长期功能：

- 统一串口和网口记录模型。
- 分包发送和更细粒度速率限制。
- 协议编辑器和可视化帧字段。
- 日志回放和离线解析。
- 多协议自动识别。

## 构建

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt6
cmake --build build
```

CMake 关键配置：

- `CMAKE_AUTOUIC ON`：自动处理 `.ui` 文件。
- `CMAKE_AUTOMOC ON`：自动处理 `Q_OBJECT`。
- `CMAKE_AUTORCC ON`：自动处理资源文件。
- 链接模块：`Qt::Widgets`、`Qt::SerialPort`、`Qt::Network`。
