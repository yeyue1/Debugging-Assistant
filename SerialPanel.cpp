#include "SerialPanel.h"
#include "ui_serialpanel.h"
#include "SearchBarHelper.h"

#include <QAction>
#include <QCheckBox>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QTextEdit>
#include <QToolButton>
#include <QTimer>
#include <QSpinBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QScrollBar>
#include <QListWidget>
#include <QSignalBlocker>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QSplitter>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QRegularExpression>

#include "AppConstants.h"
#include "SerialManager.h"
#include "SerialConfig.h"
#include "DeviceRegistry.h"
#include "RecordStore.h"
#include "AutomationRuleEngine.h"
#include "TemplateManager.h"
#include "ThemeColors.h"
#include "ParserController.h"
#include "SendQueue.h"
#include "PanelHelper.h"
#include "EncodingCodec.h"
#include "NewLineHelper.h"

namespace {

void selectComboData(QComboBox* combo, int value)
{
    const int index = combo->findData(value);
    if (index >= 0) {
        combo->setCurrentIndex(index);
    }
}

int s_serialPanelCount = 0;

} // anonymous namespace

SerialPanel::SerialPanel(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::SerialPanel),
      m_serial(new SerialManager(this)),
      m_parserController(new ParserController(this)),
      m_deviceRegistry(new DeviceRegistry(this)),
      m_recordStore(new RecordStore(this)),
      m_autoReplyEngine(new AutomationRuleEngine()),
      m_templateManager(new TemplateManager(this)),
      m_sendQueue(new SendQueue(this)),
      m_portScanTimer(new QTimer(this)),
      m_timerSendTimer(new QTimer(this)),
      m_instanceId(s_serialPanelCount++)
{
    ui->setupUi(this);

    setupUiFromForm();
    setupConnections();
    setupSendPreview();
    setupSendQueueControls();

    // 搜索栏（复用 SearchBarHelper）
    m_searchHelper = new SearchBarHelper(ui->rxEdit, this, this);

    refreshPortList();
    m_portScanTimer->setInterval(AppConstants::kPortScanIntervalMs);
    m_portScanTimer->start();

    restoreSplitterState();
}

SerialPanel::~SerialPanel()
{
    QSettings settings(QStringLiteral("yeyue"), QStringLiteral("serial_prot"));
    settings.setValue(splitterSettingsKey(), saveSplitterState());
    delete m_autoReplyEngine;
    emit panelDestroyed(portName());
    delete ui;
}

QString SerialPanel::portName() const
{
    return ui->portCombo->currentText();
}

bool SerialPanel::isConnected() const
{
    return m_serial->isOpen();
}

SerialConfig SerialPanel::serialConfig() const
{
    SerialConfig cfg;
    cfg.portName = ui->portCombo->currentText();
    cfg.baudRate = ui->baudCombo->currentData().toInt();
    cfg.dataBits = static_cast<QSerialPort::DataBits>(ui->dataBitsCombo->currentData().toInt());
    cfg.parity = static_cast<QSerialPort::Parity>(ui->parityCombo->currentData().toInt());
    cfg.stopBits = static_cast<QSerialPort::StopBits>(ui->stopBitsCombo->currentData().toInt());
    return cfg;
}

void SerialPanel::setSerialConfig(const SerialConfig& config, bool includePortName)
{
    if (includePortName) {
        if (!config.portName.isEmpty() && ui->portCombo->findText(config.portName) < 0) {
            ui->portCombo->addItem(config.portName);
        }

        const int portIndex = ui->portCombo->findText(config.portName);
        if (portIndex >= 0) {
            ui->portCombo->setCurrentIndex(portIndex);
        }
    }

    selectComboData(ui->baudCombo, config.baudRate);
    selectComboData(ui->dataBitsCombo, static_cast<int>(config.dataBits));
    selectComboData(ui->parityCombo, static_cast<int>(config.parity));
    selectComboData(ui->stopBitsCombo, static_cast<int>(config.stopBits));

    SerialConfig applied = config;
    if (!includePortName) {
        applied.portName = ui->portCombo->currentText();
    }
    m_serial->setConfig(applied);
}

QJsonObject SerialPanel::connectionTemplate() const
{
    QJsonObject object;
    object.insert(QStringLiteral("version"), 1);
    object.insert(QStringLiteral("serial"), TemplateManager::serialConfigToJson(serialConfig()));
    object.insert(QStringLiteral("dtr"), ui->dtrCheck->isChecked());
    object.insert(QStringLiteral("rts"), ui->rtsCheck->isChecked());
    object.insert(QStringLiteral("protocolIndex"), ui->protocolCombo->currentIndex());
    object.insert(QStringLiteral("newLineMode"), m_newLineMode);
    object.insert(QStringLiteral("encoding"), m_encoding);
    object.insert(QStringLiteral("alias"), m_deviceAliasEdit ? m_deviceAliasEdit->text().trimmed() : QString());
    return object;
}

bool SerialPanel::applyConnectionTemplate(const QJsonObject& object, bool includePortName)
{
    if (!TemplateManager::isValidTemplate(object)) {
        return false;
    }

    applyTemplateObject(object, includePortName);
    return true;
}

// ── Setup ──────────────────────────────────────────────────────────────────

void SerialPanel::setupUiFromForm()
{
    // Populate baud rate combo
    ui->baudCombo->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudCombo->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudCombo->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudCombo->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    ui->baudCombo->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudCombo->addItem(QStringLiteral("230400"), 230400);
    ui->baudCombo->addItem(QStringLiteral("460800"), 460800);
    ui->baudCombo->addItem(QStringLiteral("921600"), 921600);
    ui->baudCombo->setCurrentIndex(AppConstants::kDefaultBaudIndex);

    // Populate data bits combo
    ui->dataBitsCombo->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsCombo->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsCombo->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsCombo->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsCombo->setCurrentIndex(AppConstants::kDefaultDataBitsIndex);

    // Populate parity combo
    ui->parityCombo->addItem(tr("无"), QSerialPort::NoParity);
    ui->parityCombo->addItem(tr("偶校验"), QSerialPort::EvenParity);
    ui->parityCombo->addItem(tr("奇校验"), QSerialPort::OddParity);
    ui->parityCombo->addItem(tr("标记"), QSerialPort::MarkParity);
    ui->parityCombo->addItem(tr("空格"), QSerialPort::SpaceParity);

    // Populate stop bits combo
    ui->stopBitsCombo->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    ui->stopBitsCombo->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
    ui->stopBitsCombo->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    // Populate protocol combo
    ui->protocolCombo->addItem(tr("原始数据"));
    ui->protocolCombo->addItem(tr("文本"));
    ui->protocolCombo->addItem(tr("自定义协议"));

    // Populate newline combo
    ui->newLineCombo->addItem(tr("无"));
    ui->newLineCombo->addItem(tr("\\n"));
    ui->newLineCombo->addItem(tr("\\r\\n"));
    ui->newLineCombo->addItem(tr("\\r"));

    // Populate encoding combo
    ui->encodingCombo->addItem(tr("UTF-8"));
    ui->encodingCombo->addItem(tr("GBK"));
    ui->encodingCombo->addItem(tr("ASCII"));

    // Status indicator initial state
    ui->statusIndicator->setStyleSheet(QStringLiteral("background-color: %1; border-radius: 8px;")
                                           .arg(ThemeColors::Current::statusDisconnected()));
    bindFeatureUi();
    setConnectionControls(false);
}

void SerialPanel::bindFeatureUi()
{
    m_deviceAliasEdit = ui->deviceAliasEdit;
    m_saveAliasButton = ui->saveAliasButton;
    m_reconnectButton = ui->reconnectButton;
    m_exportTemplateButton = ui->exportTemplateButton;
    m_importTemplateButton = ui->importTemplateButton;
    m_deviceInfoLabel = ui->deviceInfoLabel;
    m_healthLabel = ui->healthLabel;
    m_statsLabel = ui->statsLabel;
    m_filterEdit = ui->filterEdit;
    m_filterRegexCheck = ui->filterRegexCheck;
    m_filterHideCheck = ui->filterHideCheck;
    m_frameTable = ui->frameTable;
    m_jsonPreviewEdit = ui->jsonPreviewEdit;

    m_frameTable->setHorizontalHeaderLabels({tr("字段"), tr("值"), tr("十六进制"), tr("信息")});
    m_frameTable->horizontalHeader()->setStretchLastSection(true);
    m_frameTable->verticalHeader()->setVisible(false);
    m_frameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    updateDeviceInfo();
    updateStatistics();
}

void SerialPanel::setupConnections()
{
    // ── UI 按钮 ──
    connect(ui->openButton, &QPushButton::clicked, this, &SerialPanel::onOpenClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &SerialPanel::onCloseClicked);
    connect(ui->sendButton, &QPushButton::clicked, this, &SerialPanel::onSendClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &SerialPanel::onClearClicked);
    connect(ui->hexCheck, &QCheckBox::toggled, this, &SerialPanel::onHexToggled);
    connect(ui->autoScrollCheck, &QCheckBox::toggled, this, &SerialPanel::onAutoScrollToggled);
    connect(ui->timestampCheck, &QCheckBox::toggled, this, &SerialPanel::onTimestampToggled);
    connect(ui->sendFileButton, &QPushButton::clicked, this, &SerialPanel::onSendFileClicked);
    connect(ui->saveDataButton, &QPushButton::clicked, this, &SerialPanel::onSaveDataClicked);
    connect(ui->timerSendCheck, &QCheckBox::toggled, this, &SerialPanel::onTimerSendToggled);
    connect(ui->timerSendInterval, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SerialPanel::onTimerSendIntervalChanged);
    connect(ui->protocolCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialPanel::onProtocolChanged);
    connect(ui->newLineCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialPanel::onNewLineChanged);
    connect(ui->encodingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialPanel::onEncodingChanged);
    connect(ui->portCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialPanel::onPortSelectionChanged);
    connect(ui->dtrCheck, &QCheckBox::toggled, this, &SerialPanel::onDtrToggled);
    connect(ui->rtsCheck, &QCheckBox::toggled, this, &SerialPanel::onRtsToggled);
    connect(ui->clearHistoryButton, &QPushButton::clicked, this, &SerialPanel::onClearHistoryClicked);
    connect(m_saveAliasButton, &QPushButton::clicked, this, &SerialPanel::onSaveDeviceAliasClicked);
    connect(m_reconnectButton, &QPushButton::clicked, this, &SerialPanel::onReconnectClicked);
    connect(m_exportTemplateButton, &QPushButton::clicked, this, &SerialPanel::onExportConfigTemplateClicked);
    connect(m_importTemplateButton, &QPushButton::clicked, this, &SerialPanel::onImportConfigTemplateClicked);

    // ── 过滤 UI → RecordStore ──
    connect(m_filterEdit, &QLineEdit::textChanged, this, &SerialPanel::onFilterChanged);
    connect(m_filterRegexCheck, &QCheckBox::toggled, this, &SerialPanel::onFilterChanged);
    connect(m_filterHideCheck, &QCheckBox::toggled, this, &SerialPanel::onFilterChanged);

    // ── 历史双击 ──
    connect(ui->historyList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item) {
            ui->txEdit->setText(item->text());
        }
    });

    // ── 定时器 ──
    connect(m_portScanTimer, &QTimer::timeout, this, &SerialPanel::refreshPortList);
    connect(m_timerSendTimer, &QTimer::timeout, this, &SerialPanel::onSendClicked);

    // ── SerialManager 信号 ──
    connect(m_serial, &SerialManager::opened, this, &SerialPanel::onOpened);
    connect(m_serial, &SerialManager::closed, this, &SerialPanel::onClosed);
    connect(m_serial, &SerialManager::errorOccurred, this, &SerialPanel::onSerialError);

    // SerialManager → ParserController（只有一条连接，切换解析器无需重连）
    connect(m_serial, &SerialManager::dataReceived, m_parserController, &ParserController::inputBytes);

    // ParserController → SerialPanel
    connect(m_parserController, &ParserController::frameReady, this, &SerialPanel::onFrameReady);
    connect(m_parserController, &ParserController::parsedFrameReady, this, &SerialPanel::onParsedFrameReady);
    connect(m_parserController, &ParserController::rawDataReady, this, &SerialPanel::onRawDataReady);
    connect(m_parserController, &ParserController::frameError, this, &SerialPanel::onFrameError);

    // SerialManager → 字节统计
    connect(m_serial, &SerialManager::dataReceived, this, &SerialPanel::onDataReceived);

    // RecordStore → SerialPanel
    connect(m_recordStore, &RecordStore::recordAdded, this, &SerialPanel::addRecord);
    connect(m_recordStore, &RecordStore::recordReady, this, &SerialPanel::appendRecordToReceive);
    connect(m_recordStore, &RecordStore::cleared, this, [this]() {
        ui->rxEdit->clear();
        if (m_frameTable) m_frameTable->setRowCount(0);
        if (m_jsonPreviewEdit) m_jsonPreviewEdit->clear();
    });

    setupActionMenus();
}

void SerialPanel::setupActionMenus()
{
    ui->dtrCheck->hide();
    ui->rtsCheck->hide();
    ui->hexCheck->hide();
    ui->autoScrollCheck->hide();
    ui->timestampCheck->hide();
    ui->timerSendCheck->hide();
    m_saveAliasButton->hide();
    m_reconnectButton->hide();
    m_exportTemplateButton->hide();
    m_importTemplateButton->hide();
    ui->clearButton->hide();
    ui->sendFileButton->hide();
    ui->saveDataButton->hide();
    ui->clearHistoryButton->hide();
    ui->timerSendInterval->setPrefix(tr("间隔 "));

    auto* connectionButton = PanelHelper::createMenuButton(this, tr("串口操作"));
    auto* connectionMenu = new QMenu(connectionButton);
    PanelHelper::addCheckAction(connectionMenu, tr("DTR"), ui->dtrCheck);
    PanelHelper::addCheckAction(connectionMenu, tr("RTS"), ui->rtsCheck);
    connectionMenu->addSeparator();
    connectionMenu->addAction(tr("保存别名"), this, &SerialPanel::onSaveDeviceAliasClicked);
    connectionMenu->addAction(tr("一键复连"), this, &SerialPanel::onReconnectClicked);
    connectionMenu->addSeparator();
    connectionMenu->addAction(tr("导出模板"), this, &SerialPanel::onExportConfigTemplateClicked);
    connectionMenu->addAction(tr("导入模板"), this, &SerialPanel::onImportConfigTemplateClicked);
    connectionMenu->addSeparator();
    connectionMenu->addAction(tr("清空接收区"), this, &SerialPanel::onClearClicked);
    connectionButton->setMenu(connectionMenu);
    PanelHelper::insertAfter(ui->stateRowLayout, ui->closeButton, connectionButton);

    auto* displayButton = PanelHelper::createMenuButton(this, tr("显示设置"));
    auto* displayMenu = new QMenu(displayButton);
    PanelHelper::addCheckAction(displayMenu, tr("十六进制显示"), ui->hexCheck);
    PanelHelper::addCheckAction(displayMenu, tr("自动滚动"), ui->autoScrollCheck);
    PanelHelper::addCheckAction(displayMenu, tr("时间戳"), ui->timestampCheck);
    displayButton->setMenu(displayMenu);
    ui->receiveOptionsLayout->insertWidget(0, displayButton);

    auto* sendButton = PanelHelper::createMenuButton(this, tr("发送操作"));
    auto* sendMenu = new QMenu(sendButton);
    PanelHelper::addCheckAction(sendMenu, tr("定时发送"), ui->timerSendCheck);
    sendMenu->addSeparator();
    sendMenu->addAction(tr("发送文件"), this, &SerialPanel::onSendFileClicked);
    sendMenu->addAction(tr("保存数据"), this, &SerialPanel::onSaveDataClicked);
    sendButton->setMenu(sendMenu);
    PanelHelper::insertAfter(ui->sendButtonsLayout, ui->sendButton, sendButton);

    auto* historyButton = PanelHelper::createMenuButton(this, tr("历史操作"));
    auto* historyMenu = new QMenu(historyButton);
    historyMenu->addAction(tr("清空历史"), this, &SerialPanel::onClearHistoryClicked);
    historyButton->setMenu(historyMenu);
    ui->historyLayout->addWidget(historyButton);
}

void SerialPanel::setupSendPreview()
{
    m_sendPreviewLabel = ui->sendPreviewLabel;
    m_sendPreviewLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    connect(ui->txEdit, &QTextEdit::textChanged, this, &SerialPanel::updateSendPreview);
    updateSendPreview();
}

void SerialPanel::setupSendQueueControls()
{
    m_queueRepeatSpin = ui->queueRepeatSpin;
    m_queueIntervalSpin = ui->queueIntervalSpin;
    m_queueLoopCheck = ui->queueLoopCheck;
    m_queueStatusLabel = ui->queueStatusLabel;
    m_queueIntervalSpin->setValue(ui->timerSendInterval->value());
    m_queueStatusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_queueTable = ui->queueTable;
    m_queueTable->setColumnCount(7);
    m_queueTable->setHorizontalHeaderLabels(
        {tr("序号"), tr("剩余"), tr("次数"), tr("间隔(ms)"), tr("字节"), tr("十六进制"), tr("内容")});
    m_queueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_queueTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_queueTable->setEditTriggers(QAbstractItemView::DoubleClicked
                                   | QAbstractItemView::SelectedClicked
                                   | QAbstractItemView::EditKeyPressed
                                   | QAbstractItemView::AnyKeyPressed);
    m_queueTable->verticalHeader()->setVisible(false);
    m_queueTable->horizontalHeader()->setStretchLastSection(true);
    m_queueTable->horizontalHeader()->setSectionsMovable(true);

    m_addQueueButton = ui->addQueueButton;
    m_insertQueueButton = ui->insertQueueButton;
    m_removeQueueButton = ui->removeQueueButton;
    m_clearQueueButton = ui->clearQueueButton;
    m_startQueueButton = ui->startQueueButton;
    m_stopQueueButton = ui->stopQueueButton;

    connect(m_addQueueButton, &QPushButton::clicked, this, &SerialPanel::onAddQueueClicked);
    connect(m_insertQueueButton, &QPushButton::clicked, this, &SerialPanel::onInsertQueueClicked);
    connect(m_removeQueueButton, &QPushButton::clicked, this, &SerialPanel::onRemoveQueueClicked);
    connect(m_clearQueueButton, &QPushButton::clicked, this, &SerialPanel::onClearQueueClicked);
    connect(m_startQueueButton, &QPushButton::clicked, this, &SerialPanel::onStartQueueClicked);
    connect(m_stopQueueButton, &QPushButton::clicked, this, &SerialPanel::onStopQueueClicked);
    connect(m_queueTable, &QTableWidget::itemChanged, this, &SerialPanel::onQueueTableItemChanged);
    connect(m_queueLoopCheck, &QCheckBox::toggled, m_sendQueue, &SendQueue::setLoopEnabled);
    connect(m_sendQueue, &SendQueue::sendRequested, this, &SerialPanel::onQueueSendRequested);
    connect(m_sendQueue, &SendQueue::queueChanged, this, &SerialPanel::updateSendQueueStatus);

    m_sendQueue->setIntervalMs(m_queueIntervalSpin->value());
    m_sendQueue->setLoopEnabled(m_queueLoopCheck->isChecked());
    updateSendQueueStatus(m_sendQueue->pendingItemCount(), m_sendQueue->pendingSendCount(), m_sendQueue->isRunning());
}


int SerialPanel::selectedQueueRow() const
{
    if (!m_queueTable || m_queueTable->selectionModel()->selectedRows().isEmpty()) {
        return -1;
    }
    return m_queueTable->selectionModel()->selectedRows().first().row();
}

void SerialPanel::refreshSendQueueTable()
{
    if (!m_queueTable) {
        return;
    }

    const int selectedRow = selectedQueueRow();
    m_updatingQueueTable = true;
    const QSignalBlocker blocker(m_queueTable);
    m_queueTable->setRowCount(m_sendQueue->pendingItemCount());
    for (int row = 0; row < m_sendQueue->pendingItemCount(); ++row) {
        const SendQueue::QueueItem item = m_sendQueue->itemAt(row);
        const QString displayText = PanelHelper::visibleQueueText(item.displayText);

        m_queueTable->setItem(row, 0, PanelHelper::readOnlyTableItem(QString::number(row + 1)));
        m_queueTable->setItem(row, 1, PanelHelper::editableTableItem(QString::number(item.remaining)));
        m_queueTable->setItem(row, 2, PanelHelper::editableTableItem(QString::number(item.repeatCount)));
        m_queueTable->setItem(row, 3, PanelHelper::editableTableItem(QString::number(item.intervalMs)));
        m_queueTable->setItem(row, 4, PanelHelper::readOnlyTableItem(QString::number(item.payload.size())));
        m_queueTable->setItem(row, 5, PanelHelper::readOnlyTableItem(QString::fromLatin1(item.payload.toHex(' ')).toUpper()));
        m_queueTable->setItem(row, 6, PanelHelper::editableTableItem(displayText));
    }

    m_queueTable->resizeColumnsToContents();
    m_queueTable->horizontalHeader()->setStretchLastSection(true);
    if (selectedRow >= 0 && selectedRow < m_queueTable->rowCount()) {
        m_queueTable->selectRow(selectedRow);
    }
    m_updatingQueueTable = false;
}

// ── 端口扫描 ──────────────────────────────────────────────────────────────

void SerialPanel::refreshPortList()
{
    if (m_serial->isOpen()) {
        return;
    }

    QStringList ports;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const auto& info : infos) {
        ports << info.portName();
    }

    if (ports != m_cachedPorts) {
        updatePortCombo(ports);
        m_cachedPorts = ports;
    }
}

void SerialPanel::updatePortCombo(const QStringList& ports)
{
    const QString current = ui->portCombo->currentText();
    ui->portCombo->clear();
    ui->portCombo->addItems(ports);

    const int idx = ui->portCombo->findText(current);
    if (idx >= 0) {
        ui->portCombo->setCurrentIndex(idx);
    }
    updateDeviceInfo();
}

// ── 连接控制 ──────────────────────────────────────────────────────────────

void SerialPanel::applyConfigFromUi()
{
    m_serial->setConfig(serialConfig());
}

void SerialPanel::onOpenClicked()
{
    applyConfigFromUi();
    m_serial->open();
}

void SerialPanel::onCloseClicked()
{
    m_serial->close();
}

void SerialPanel::onOpened()
{
    // 串口重新打开时重置所有自动回复规则的触发计数
    if (m_autoReplyEngine) {
        m_autoReplyEngine->resetTriggerState();
    }
    AutomationRuleEngine::resetAllRules();

    setConnectionControls(true);
    updateStatusIndicator(true);
    updateSendQueueStatus(m_sendQueue->pendingItemCount(), m_sendQueue->pendingSendCount(), m_sendQueue->isRunning());
    m_lastDeviceKey = currentDeviceKey();
    if (!m_lastDeviceKey.isEmpty() && m_deviceAliasEdit && !m_deviceAliasEdit->text().trimmed().isEmpty()) {
        onSaveDeviceAliasClicked();
    }
    onDtrToggled(ui->dtrCheck->isChecked());
    onRtsToggled(ui->rtsCheck->isChecked());
    emit connectionStateChanged(true);
}

void SerialPanel::onClosed()
{
    m_sendQueue->stop();
    ++m_disconnectCount;
    setConnectionControls(false);
    updateStatusIndicator(false);
    refreshPortList();
    updateStatistics();
    emit connectionStateChanged(false);
}

void SerialPanel::setConnectionControls(bool connected)
{
    ui->openButton->setEnabled(!connected);
    ui->closeButton->setEnabled(connected);
    setConfigurationControlsEnabled(!connected);
}

void SerialPanel::setConfigurationControlsEnabled(bool enabled)
{
    ui->portCombo->setEnabled(enabled);
    ui->baudCombo->setEnabled(enabled);
    ui->dataBitsCombo->setEnabled(enabled);
    ui->parityCombo->setEnabled(enabled);
    ui->stopBitsCombo->setEnabled(enabled);
}

// ── 收发数据 ──────────────────────────────────────────────────────────────

void SerialPanel::onSendClicked()
{
    QString text = ui->txEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }

    NewLineHelper::appendNewLine(text, m_newLineMode);

    sendPayload(encodeText(text), ui->txEdit->toPlainText(), true);
}

void SerialPanel::onSendFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), QString(), tr("所有文件 (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            sendPayload(data, QFileInfo(fileName).fileName(), false);
        }
    }
}

qint64 SerialPanel::sendPayload(const QByteArray& data, const QString& displayText, bool addHistory)
{
    const qint64 bytesWritten = m_serial->sendData(data);
    if (bytesWritten <= 0) {
        return 0;
    }

    m_txByteCount += static_cast<uint64_t>(bytesWritten);

    SerialRecord record;
    record.timestamp = QDateTime::currentDateTime();
    record.direction = SerialRecordDirection::Tx;
    record.payload = data.left(static_cast<int>(bytesWritten));
    record.text = displayText.isEmpty() ? displayTextForPayload(record.payload) : displayText;
    record.protocol = QStringLiteral("发送");
    addRecord(record);

    if (addHistory) {
        addToHistory(displayText);
    }

    return bytesWritten;
}

void SerialPanel::onDataReceived(const QByteArray& data)
{
    m_rxByteCount += static_cast<uint64_t>(data.size());
    updateStatistics();
}

// ── 解析器信号处理 ────────────────────────────────────────────────────────

void SerialPanel::onFrameReady(const QByteArray& payload, const QString& info)
{
    SerialRecord record;
    record.timestamp = QDateTime::currentDateTime();
    record.direction = SerialRecordDirection::Rx;
    record.payload = payload;
    record.text = displayTextForPayload(payload);
    record.protocol = m_parserController->currentName();
    record.info = info;
    m_recordStore->addRecord(record);
    handleAutoReply(record);
}

void SerialPanel::onParsedFrameReady(const ParsedFrame& frame)
{
    m_lastParsedFrame = frame;
    m_hasParsedFrame = true;
}

void SerialPanel::onRawDataReady(const QByteArray& data)
{
    SerialRecord record;
    record.timestamp = QDateTime::currentDateTime();
    record.direction = SerialRecordDirection::Rx;
    record.payload = data;
    record.text = displayTextForPayload(data);
    record.protocol = m_parserController->currentName();
    m_recordStore->addRecord(record);
    handleAutoReply(record);
}

void SerialPanel::onFrameError(const QString& message)
{
    SerialRecord record;
    record.timestamp = QDateTime::currentDateTime();
    record.direction = SerialRecordDirection::System;
    record.protocol = m_parserController->currentName();
    record.text = QStringLiteral("[帧错误] %1").arg(message);
    record.error = message;
    m_recordStore->addRecord(record);
}

// ── 错误处理 ──────────────────────────────────────────────────────────────

void SerialPanel::onSerialError(QSerialPort::SerialPortError code, const QString& message)
{
    if (code == QSerialPort::NoError) {
        return;
    }

    if (code == QSerialPort::ResourceError && m_serial->isOpen()) {
        m_serial->close();
    }

    m_lastDisconnectReason = message;
    SerialRecord record;
    record.timestamp = QDateTime::currentDateTime();
    record.direction = SerialRecordDirection::System;
    record.text = tr("串口错误: %1").arg(message);
    record.error = message;
    m_recordStore->addRecord(record);

    setConnectionControls(m_serial->isOpen());
    ui->statusIndicator->setStyleSheet(
        QStringLiteral("background-color: %1; border-radius: 8px; border: 2px solid %2;")
            .arg(ThemeColors::Current::statusDisconnected(), ThemeColors::Current::statusBorder()));
    ui->statusLabel->setText(tr("错误: %1").arg(message));
    ui->statusLabel->setObjectName("statusDisconnected");
    ui->statusLabel->style()->unpolish(ui->statusLabel);
    ui->statusLabel->style()->polish(ui->statusLabel);
}

// ── 记录管理 ──────────────────────────────────────────────────────────────

void SerialPanel::addRecord(const SerialRecord& record)
{
    refreshParsedFrameView(record);
    updateStatistics();
}

void SerialPanel::refreshRecordView()
{
    ui->rxEdit->clear();
    for (const auto& record : m_recordStore->records()) {
        const bool matchedFilter = m_recordStore->matchesFilter(record);
        if (!m_recordStore->shouldDisplayRecord(record)) {
            continue;
        }
        appendRecordToReceive(record, matchedFilter);
    }
}

void SerialPanel::refreshParsedFrameView(const SerialRecord& record)
{
    if (!m_frameTable || !m_jsonPreviewEdit || record.direction != SerialRecordDirection::Rx) {
        return;
    }

    m_frameTable->setRowCount(0);
    const auto addRow = [this](const QString& field, const QString& value, const QString& hex, const QString& info) {
        const int row = m_frameTable->rowCount();
        m_frameTable->insertRow(row);
        m_frameTable->setItem(row, 0, new QTableWidgetItem(field));
        m_frameTable->setItem(row, 1, new QTableWidgetItem(value));
        m_frameTable->setItem(row, 2, new QTableWidgetItem(hex));
        m_frameTable->setItem(row, 3, new QTableWidgetItem(info));
    };

    if (m_hasParsedFrame && !m_lastParsedFrame.fields.isEmpty()) {
        // 使用结构化帧数据填充表格
        addRow(QStringLiteral("协议"), m_lastParsedFrame.protocolName, QString(),
               m_lastParsedFrame.checksumOk ? QStringLiteral("校验通过") : m_lastParsedFrame.error);

        for (const ParsedField& field : m_lastParsedFrame.fields) {
            addRow(field.name, field.value,
                   QString::fromLatin1(field.raw.toHex(' ')).toUpper(),
                   field.note);
        }
    } else {
        addRow(QStringLiteral("协议"), record.protocol, QString(), record.info);
        addRow(QStringLiteral("长度"), QString::number(record.payload.size()), QString(), QString());
        addRow(QStringLiteral("负载"), record.text, RecordStore::bytesToHex(record.payload), QString());
    }

    // 构建 JSON 预览
    QJsonObject object = TemplateManager::recordToJson(record);
    object.insert(QStringLiteral("长度"), record.payload.size());

    if (m_hasParsedFrame && !m_lastParsedFrame.fields.isEmpty()) {
        QJsonArray fieldsArray;
        for (const ParsedField& field : m_lastParsedFrame.fields) {
            QJsonObject fieldObj;
            fieldObj.insert(QStringLiteral("name"), field.name);
            fieldObj.insert(QStringLiteral("value"), field.value);
            fieldObj.insert(QStringLiteral("raw"), QString::fromLatin1(field.raw.toHex()));
            fieldObj.insert(QStringLiteral("note"), field.note);
            fieldsArray.append(fieldObj);
        }
        object.insert(QStringLiteral("fields"), fieldsArray);
        object.insert(QStringLiteral("checksumOk"), m_lastParsedFrame.checksumOk);
        if (!m_lastParsedFrame.error.isEmpty()) {
            object.insert(QStringLiteral("error"), m_lastParsedFrame.error);
        }
    }

    m_jsonPreviewEdit->setPlainText(QString::fromUtf8(QJsonDocument(object).toJson(QJsonDocument::Indented)));

    // 重置标记，下一次如果没有 ParsedFrame 就走通用路径
    m_hasParsedFrame = false;
}

// ── 接收显示 ──────────────────────────────────────────────────────────────

void SerialPanel::appendRecordToReceive(const SerialRecord& record, bool matchedFilter)
{
    QString line;
    if (m_showTimestamp) {
        line += QStringLiteral("[%1] ").arg(record.timestamp.toString(QStringLiteral("hh:mm:ss.zzz")));
    }
    line += QStringLiteral("%1 ").arg(RecordStore::directionText(record.direction));

    if (!record.protocol.isEmpty()) {
        line += QStringLiteral("(%1) ").arg(record.protocol);
    }

    if (!record.error.isEmpty()) {
        line += QStringLiteral("错误: %1").arg(record.error);
    } else {
        line += record.text;
        if (!record.info.isEmpty()) {
            line += QStringLiteral("  [%1]").arg(record.info);
        }
    }

    // 颜色
    QString color = ThemeColors::Current::rxDefault();
    if (record.direction == SerialRecordDirection::Tx) {
        color = ThemeColors::Current::txDisplay();
    } else if (!record.error.isEmpty()) {
        color = ThemeColors::Current::errorDisplay();
    } else if (matchedFilter && m_filterEdit && !m_filterEdit->text().trimmed().isEmpty()) {
        color = ThemeColors::Current::filterMatch();
    } else if (record.direction == SerialRecordDirection::System) {
        color = ThemeColors::Current::systemDisplay();
    }

    ui->rxEdit->append(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                           .arg(color, line.toHtmlEscaped()));

    if (m_autoScroll) {
        QScrollBar* scrollBar = ui->rxEdit->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

// ── 自动回复 ──────────────────────────────────────────────────────────────

void SerialPanel::handleAutoReply(const SerialRecord& record)
{
    if (!m_autoReplyEngine) return;

    const QString reply = m_autoReplyEngine->checkAutoReply(record);
    if (reply.isEmpty()) return;

    QString text = reply;
    NewLineHelper::appendNewLine(text, m_newLineMode);

    sendPayload(encodeText(text), reply, false);
}

// ── 日志导出 ──────────────────────────────────────────────────────────────

void SerialPanel::saveRecordsAsText(const QString& fileName, bool filteredOnly) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    const auto& records = m_recordStore->records();
    for (const auto& record : records) {
        if (filteredOnly && !m_recordStore->shouldDisplayRecord(record)) {
            continue;
        }

        QString line;
        line += QStringLiteral("[%1] ").arg(record.timestamp.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz")));
        line += QStringLiteral("%1 ").arg(RecordStore::directionText(record.direction));

        if (!record.protocol.isEmpty()) {
            line += QStringLiteral("(%1) ").arg(record.protocol);
        }

        if (!record.error.isEmpty()) {
            line += QStringLiteral("错误: %1").arg(record.error);
        } else {
            line += record.text;
            if (!record.info.isEmpty()) {
                line += QStringLiteral("  [%1]").arg(record.info);
            }
        }

        out << line << '\n';
    }
}

void SerialPanel::saveRecordsAsCsv(const QString& fileName, bool filteredOnly) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << "时间,方向,协议,文本,十六进制,信息,错误\n";
    const auto& records = m_recordStore->records();
    for (const auto& record : records) {
        if (filteredOnly && !m_recordStore->shouldDisplayRecord(record)) {
            continue;
        }
        out << TemplateManager::recordToCsvLine(record);
    }
}

void SerialPanel::saveRecordsAsJson(const QString& fileName, bool filteredOnly) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QList<SerialRecord> exportRecords;
    const auto& records = m_recordStore->records();
    for (const auto& record : records) {
        if (filteredOnly && !m_recordStore->shouldDisplayRecord(record)) {
            continue;
        }
        exportRecords.append(record);
    }

    QJsonObject root;
    root.insert(QStringLiteral("version"), 1);
    root.insert(QStringLiteral("记录"), TemplateManager::recordsToJson(exportRecords));
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

// ── 配置模板 ──────────────────────────────────────────────────────────────

void SerialPanel::applyTemplateObject(const QJsonObject& object, bool includePortName)
{
    const SerialConfig config = TemplateManager::serialConfigFromJson(
        object.value(QStringLiteral("serial")).toObject());
    setSerialConfig(config, includePortName);

    ui->dtrCheck->setChecked(object.value(QStringLiteral("dtr")).toBool(ui->dtrCheck->isChecked()));
    ui->rtsCheck->setChecked(object.value(QStringLiteral("rts")).toBool(ui->rtsCheck->isChecked()));

    const int protocolIndex = object.value(QStringLiteral("protocolIndex")).toInt(ui->protocolCombo->currentIndex());
    if (protocolIndex >= 0 && protocolIndex < ui->protocolCombo->count()) {
        ui->protocolCombo->setCurrentIndex(protocolIndex);
    }

    const int newLineMode = object.value(QStringLiteral("newLineMode")).toInt(m_newLineMode);
    if (newLineMode >= 0 && newLineMode < ui->newLineCombo->count()) {
        ui->newLineCombo->setCurrentIndex(newLineMode);
    }

    const QString encoding = object.value(QStringLiteral("encoding")).toString(m_encoding);
    const int encodingIndex = ui->encodingCombo->findText(encoding);
    if (encodingIndex >= 0) {
        ui->encodingCombo->setCurrentIndex(encodingIndex);
    }

    const QString alias = object.value(QStringLiteral("alias")).toString();
    if (includePortName && m_deviceAliasEdit && !alias.isEmpty()) {
        m_deviceAliasEdit->setText(alias);
        onSaveDeviceAliasClicked();
    }

    updateDeviceInfo();
}

// ── 定时发送 ──────────────────────────────────────────────────────────────

void SerialPanel::onTimerSendToggled(bool enabled)
{
    ui->timerSendInterval->setEnabled(enabled);
    if (enabled) {
        m_timerSendTimer->start(ui->timerSendInterval->value());
    } else {
        m_timerSendTimer->stop();
    }
}

void SerialPanel::onTimerSendIntervalChanged(int value)
{
    if (ui->timerSendCheck->isChecked()) {
        m_timerSendTimer->start(value);
    }
}

void SerialPanel::onAddQueueClicked()
{
    const QString displayText = ui->txEdit->toPlainText();
    if (displayText.isEmpty()) {
        return;
    }

    QString text = displayText;
    NewLineHelper::appendNewLine(text, m_newLineMode);

    const QByteArray payload = encodeText(text);
    if (payload.isEmpty()) {
        return;
    }

    m_sendQueue->enqueue(payload,
                         displayText,
                         m_queueRepeatSpin->value(),
                         m_queueIntervalSpin->value());
    if (m_queueTable && m_sendQueue->pendingItemCount() > 0) {
        m_queueTable->selectRow(m_sendQueue->pendingItemCount() - 1);
    }
}

void SerialPanel::onInsertQueueClicked()
{
    const QString displayText = ui->txEdit->toPlainText();
    if (displayText.isEmpty()) {
        return;
    }

    QString text = displayText;
    NewLineHelper::appendNewLine(text, m_newLineMode);

    const QByteArray payload = encodeText(text);
    if (payload.isEmpty()) {
        return;
    }

    const int row = selectedQueueRow();
    const int insertRow = row < 0 ? m_sendQueue->pendingItemCount() : row;
    m_sendQueue->insert(insertRow,
                        payload,
                        displayText,
                        m_queueRepeatSpin->value(),
                        m_queueIntervalSpin->value());
    if (m_queueTable && insertRow < m_sendQueue->pendingItemCount()) {
        m_queueTable->selectRow(insertRow);
    }
}

void SerialPanel::onRemoveQueueClicked()
{
    m_sendQueue->removeAt(selectedQueueRow());
}

void SerialPanel::onClearQueueClicked()
{
    m_sendQueue->clear();
}

void SerialPanel::onStartQueueClicked()
{
    if (!m_serial->isOpen()) {
        return;
    }

    m_sendQueue->start();
}

void SerialPanel::onStopQueueClicked()
{
    m_sendQueue->stop();
}

void SerialPanel::onQueueTableItemChanged(QTableWidgetItem* tableItem)
{
    if (!tableItem || m_updatingQueueTable) {
        return;
    }

    const int row = tableItem->row();
    SendQueue::QueueItem item = m_sendQueue->itemAt(row);
    if (item.payload.isEmpty()) {
        refreshSendQueueTable();
        return;
    }

    switch (tableItem->column()) {
    case 1:
        item.remaining = PanelHelper::positiveCellValue(tableItem->text(), item.remaining, 1, AppConstants::kMaxRepeatCount);
        break;
    case 2:
        item.repeatCount = PanelHelper::positiveCellValue(tableItem->text(), item.repeatCount, 1, AppConstants::kMaxRepeatCount);
        item.remaining = item.repeatCount;
        break;
    case 3:
        item.intervalMs = PanelHelper::positiveCellValue(tableItem->text(), item.intervalMs, AppConstants::kMinIntervalMs, AppConstants::kMaxIntervalMs);
        break;
    case 6: {
        const QString displayText = PanelHelper::queueTextFromVisible(tableItem->text());
        if (displayText.isEmpty()) {
            refreshSendQueueTable();
            return;
        }

        QString text = displayText;
        NewLineHelper::appendNewLine(text, m_newLineMode);

        const QByteArray payload = encodeText(text);
        if (payload.isEmpty()) {
            refreshSendQueueTable();
            return;
        }

        item.displayText = displayText;
        item.payload = payload;
        break;
    }
    default:
        return;
    }

    m_sendQueue->updateItem(row, item);
}

void SerialPanel::onQueueSendRequested(const QByteArray& payload,
                                       const QString& displayText,
                                       const QString& remoteHost,
                                       quint16 remotePort,
                                       const QString& peer)
{
    Q_UNUSED(remoteHost)
    Q_UNUSED(remotePort)
    Q_UNUSED(peer)

    if (sendPayload(payload, displayText, true) <= 0) {
        m_sendQueue->stop();
    }
}

void SerialPanel::updateSendQueueStatus(int pendingItems, int pendingSends, bool running)
{
    refreshSendQueueTable();

    if (m_queueStatusLabel) {
        QStringList states;
        if (m_sendQueue->loopEnabled()) {
            states << tr("循环");
        }
        if (running) {
            states << tr("运行中");
        }

        m_queueStatusLabel->setText(tr("队列: %1 项 / %2 次%3")
                                        .arg(pendingItems)
                                        .arg(pendingSends)
                                        .arg(states.isEmpty()
                                                 ? QString()
                                                 : tr("（%1）").arg(states.join(tr("，")))));
    }
    if (m_startQueueButton) {
        m_startQueueButton->setEnabled(!running && pendingItems > 0 && m_serial->isOpen());
    }
    if (m_removeQueueButton) {
        m_removeQueueButton->setEnabled(pendingItems > 0);
    }
    if (m_clearQueueButton) {
        m_clearQueueButton->setEnabled(pendingItems > 0);
    }
    if (m_stopQueueButton) {
        m_stopQueueButton->setEnabled(running);
    }
}

// ── 端口选择 / 设备信息 ──────────────────────────────────────────────────

void SerialPanel::onPortSelectionChanged()
{
    updateDeviceInfo();
}

void SerialPanel::updateDeviceInfo()
{
    const QString key = currentDeviceKey();
    const QString alias = m_deviceRegistry->aliasForKey(key);
    if (m_deviceAliasEdit) {
        const QSignalBlocker blocker(m_deviceAliasEdit);
        m_deviceAliasEdit->setText(alias);
    }

    if (m_deviceInfoLabel) {
        QString summary = m_deviceRegistry->deviceSummaryForPort(ui->portCombo->currentText());
        if (!alias.isEmpty()) {
            summary = QStringLiteral("%1 - %2").arg(alias, summary);
        }
        m_deviceInfoLabel->setText(tr("设备: %1").arg(summary.isEmpty() ? QStringLiteral("-") : summary));
    }
}

QString SerialPanel::currentDeviceKey() const
{
    return m_deviceRegistry->deviceKeyForPort(ui->portCombo->currentText());
}

// ── 设备别名 & 一键复连 ──────────────────────────────────────────────────

void SerialPanel::onSaveDeviceAliasClicked()
{
    const QString key = currentDeviceKey();
    if (key.isEmpty()) {
        return;
    }

    m_deviceRegistry->saveAlias(key, m_deviceAliasEdit->text().trimmed());
    updateDeviceInfo();
}

void SerialPanel::onReconnectClicked()
{
    const QString key = !m_lastDeviceKey.isEmpty() ? m_lastDeviceKey : currentDeviceKey();
    if (key.isEmpty()) {
        return;
    }

    if (m_serial->isOpen()) {
        m_serial->close();
    }

    const QString portName = m_deviceRegistry->findPortForKey(key);
    if (!portName.isEmpty()) {
        const int index = ui->portCombo->findText(portName);
        if (index >= 0) {
            ui->portCombo->setCurrentIndex(index);
        } else {
            ui->portCombo->addItem(portName);
            ui->portCombo->setCurrentText(portName);
        }

        ++m_reconnectCount;
        applyConfigFromUi();
        m_serial->open();
        updateStatistics();
    } else {
        QMessageBox::warning(this, tr("一键复连"), tr("未找到该设备。"));
    }
}

// ── 配置模板导入/导出 ────────────────────────────────────────────────────

void SerialPanel::onExportConfigTemplateClicked()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this, tr("导出模板"), QString(), tr("串口模板 (*.json)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    file.write(QJsonDocument(connectionTemplate()).toJson(QJsonDocument::Indented));
}

void SerialPanel::onImportConfigTemplateClicked()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this, tr("导入模板"), QString(), tr("串口模板 (*.json);;所有文件 (*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject() || !applyConnectionTemplate(document.object(), true)) {
        QMessageBox::warning(this, tr("导入模板"), tr("串口模板格式无效。"));
    }
}

// ── 过滤 ──────────────────────────────────────────────────────────────────

void SerialPanel::onFilterChanged()
{
    m_recordStore->setFilterPattern(m_filterEdit ? m_filterEdit->text() : QString());
    m_recordStore->setFilterRegex(m_filterRegexCheck && m_filterRegexCheck->isChecked());
    m_recordStore->setFilterHide(m_filterHideCheck && m_filterHideCheck->isChecked());
    refreshRecordView();
}

// ── UI 开关 ───────────────────────────────────────────────────────────────

void SerialPanel::onClearClicked()
{
    ui->rxEdit->clear();
    m_recordStore->clear();
    m_rxByteCount = 0;
    m_txByteCount = 0;
    updateStatistics();
}

void SerialPanel::onHexToggled(bool enabled)
{
    m_hexDisplay = enabled;
}

void SerialPanel::onAutoScrollToggled(bool enabled)
{
    m_autoScroll = enabled;
}

void SerialPanel::onTimestampToggled(bool enabled)
{
    m_showTimestamp = enabled;
}

void SerialPanel::onProtocolChanged(int index)
{
    m_parserController->switchTo(static_cast<ParserController::ParserType>(index));
}

void SerialPanel::onDtrToggled(bool enabled)
{
    if (!m_serial->isOpen()) {
        return;
    }

    if (!m_serial->setDataTerminalReady(enabled)) {
        const QSignalBlocker blocker(ui->dtrCheck);
        ui->dtrCheck->setChecked(!enabled);
    }
}

void SerialPanel::onRtsToggled(bool enabled)
{
    if (!m_serial->isOpen()) {
        return;
    }

    if (!m_serial->setRequestToSend(enabled)) {
        const QSignalBlocker blocker(ui->rtsCheck);
        ui->rtsCheck->setChecked(!enabled);
    }
}

void SerialPanel::onNewLineChanged(int index)
{
    m_newLineMode = index;
    updateSendPreview();
}

void SerialPanel::onEncodingChanged(int index)
{
    QStringList encodings = {"UTF-8", "GBK", "ASCII"};
    if (index >= 0 && index < encodings.size()) {
        m_encoding = encodings[index];
    }
    updateSendPreview();
}

void SerialPanel::onSaveDataClicked()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("保存数据"),
        QString(),
        tr("JSON 文件 (*.json);;CSV 文件 (*.csv);;文本文件 (*.txt);;所有文件 (*)"));
    if (fileName.isEmpty()) {
        return;
    }

    // 导出选项对话框
    QMessageBox exportBox(this);
    exportBox.setWindowTitle(tr("导出范围"));
    exportBox.setText(tr("选择导出范围:"));
    QPushButton* allButton = exportBox.addButton(tr("全部记录"), QMessageBox::AcceptRole);
    exportBox.addButton(tr("当前过滤结果"), QMessageBox::RejectRole);
    exportBox.exec();

    const bool filteredOnly = exportBox.clickedButton() != allButton;

    const QString suffix = QFileInfo(fileName).suffix().toLower();
    if (suffix == QStringLiteral("json")) {
        saveRecordsAsJson(fileName, filteredOnly);
    } else if (suffix == QStringLiteral("csv")) {
        saveRecordsAsCsv(fileName, filteredOnly);
    } else {
        saveRecordsAsText(fileName, filteredOnly);
    }
}

void SerialPanel::onClearHistoryClicked()
{
    ui->historyList->clear();
    m_sendHistory.clear();
}

// ── 统计 ──────────────────────────────────────────────────────────────────

void SerialPanel::updateStatistics()
{
    ui->rxCountLabel->setText(tr("接收: %1").arg(m_rxByteCount));
    ui->txCountLabel->setText(tr("发送: %1").arg(m_txByteCount));
    if (m_statsLabel) {
        const uint64_t outcomes = m_recordStore->frameCount() + m_recordStore->errorCount();
        const double errorRate = outcomes == 0
            ? 0.0
            : static_cast<double>(m_recordStore->errorCount()) * 100.0 / static_cast<double>(outcomes);
        m_statsLabel->setText(tr("统计: 记录 %1/%2，帧 %3，错误率 %4%")
                                  .arg(m_recordStore->rxRecordCount())
                                  .arg(m_recordStore->txRecordCount())
                                  .arg(m_recordStore->frameCount())
                                  .arg(errorRate, 0, 'f', 1));
    }
    if (m_healthLabel) {
        const QString lastReason = m_lastDisconnectReason.isEmpty()
            ? QStringLiteral("-")
            : m_lastDisconnectReason;
        m_healthLabel->setText(tr("健康: 重连 %1，断开 %2，错误 %3，最近 %4")
                                   .arg(m_reconnectCount)
                                   .arg(m_disconnectCount)
                                   .arg(m_recordStore->errorCount())
                                   .arg(lastReason));
    }
}

void SerialPanel::updateSendPreview()
{
    if (!m_sendPreviewLabel) {
        return;
    }

    QString text = ui->txEdit->toPlainText();
    NewLineHelper::appendNewLine(text, m_newLineMode);

    m_sendPreviewLabel->setText(tr("发送预估: %1 B").arg(encodeText(text).size()));
}

// ── 辅助 ──────────────────────────────────────────────────────────────────

void SerialPanel::appendToReceive(const QString& text)
{
    if (m_showTimestamp) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        ui->rxEdit->append(QStringLiteral("[%1] %2").arg(timestamp, text));
    } else {
        ui->rxEdit->append(text);
    }

    if (m_autoScroll) {
        QScrollBar* scrollBar = ui->rxEdit->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void SerialPanel::updateStatusIndicator(bool connected)
{
    if (connected) {
        ui->statusIndicator->setStyleSheet(
            QStringLiteral("background-color: %1; border-radius: 8px; border: 2px solid %2;")
                .arg(ThemeColors::Current::statusConnected(), ThemeColors::Current::statusBorder()));
        ui->statusLabel->setText(tr("已连接"));
        ui->statusLabel->setObjectName("statusConnected");
    } else {
        ui->statusIndicator->setStyleSheet(
            QStringLiteral("background-color: %1; border-radius: 8px; border: 2px solid %2;")
                .arg(ThemeColors::Current::statusDisconnected(), ThemeColors::Current::statusBorder()));
        ui->statusLabel->setText(tr("未连接"));
        ui->statusLabel->setObjectName("statusDisconnected");
    }
    ui->statusLabel->style()->unpolish(ui->statusLabel);
    ui->statusLabel->style()->polish(ui->statusLabel);
}

void SerialPanel::addToHistory(const QString& text)
{
    if (text.isEmpty()) {
        return;
    }

    int idx = m_sendHistory.indexOf(text);
    if (idx >= 0) {
        m_sendHistory.removeAt(idx);
        delete ui->historyList->takeItem(idx);
    }

    m_sendHistory.prepend(text);
    ui->historyList->insertItem(0, text);

    while (m_sendHistory.size() > AppConstants::kMaxHistoryItems) {
        m_sendHistory.removeLast();
        delete ui->historyList->takeItem(ui->historyList->count() - 1);
    }
}

QString SerialPanel::decodeBytes(const QByteArray& data) const
{
    return EncodingCodec::decodeBytes(data, m_encoding);
}

QByteArray SerialPanel::encodeText(const QString& text) const
{
    return EncodingCodec::encodeText(text, m_encoding);
}

QString SerialPanel::displayTextForPayload(const QByteArray& data) const
{
    if (m_hexDisplay) {
        return RecordStore::bytesToHex(data);
    }
    return decodeBytes(data);
}

// ── QSplitter 布局持久化 ───────────────────────────────────────────────────

QByteArray SerialPanel::saveSplitterState() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << ui->rootSplitter->saveState();
    stream << ui->connectionSplitter->saveState();
    stream << ui->mainSplitter->saveState();
    stream << ui->leftSplitter->saveState();
    stream << ui->sendContentSplitter->saveState();
    stream << ui->rightSplitter->saveState();
    return data;
}

void SerialPanel::restoreSplitterState(const QByteArray& state)
{
    if (state.isEmpty()) {
        return;
    }

    QByteArray rootState, connState, mainState, leftState, sendState, rightState;
    QDataStream stream(state);
    stream >> rootState >> connState >> mainState >> leftState >> sendState >> rightState;

    if (!rootState.isEmpty()) ui->rootSplitter->restoreState(rootState);
    if (!connState.isEmpty()) ui->connectionSplitter->restoreState(connState);
    if (!mainState.isEmpty()) ui->mainSplitter->restoreState(mainState);
    if (!leftState.isEmpty()) ui->leftSplitter->restoreState(leftState);
    if (!sendState.isEmpty()) ui->sendContentSplitter->restoreState(sendState);
    if (!rightState.isEmpty()) ui->rightSplitter->restoreState(rightState);
}

QString SerialPanel::splitterSettingsKey() const
{
    return QStringLiteral("serialPanel/splitterState_%1").arg(m_instanceId);
}

void SerialPanel::restoreSplitterState()
{
    const QSettings settings(QStringLiteral("yeyue"), QStringLiteral("serial_prot"));
    const QByteArray state = settings.value(splitterSettingsKey()).toByteArray();
    restoreSplitterState(state);
}
