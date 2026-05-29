#include "NetworkPanel.h"
#include "ui_networkpanel.h"

#include <QAbstractItemView>
#include <QAction>
#include <QColor>
#include <QCheckBox>
#include <QBoxLayout>
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStringList>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QTextStream>
#include <QToolButton>
#include <QTimer>
#include <QtGlobal>

#include "NetworkManager.h"
#include "SendQueue.h"

namespace {

QString bytesToHex(const QByteArray& data)
{
    return QString::fromLatin1(data.toHex(' ')).toUpper();
}

QString normalizeHex(QString text)
{
    text.remove(QChar::Space);
    text.remove(QLatin1Char('\t'));
    text.remove(QLatin1Char('\r'));
    text.remove(QLatin1Char('\n'));
    return text;
}

void insertAfter(QBoxLayout* layout, QWidget* anchor, QWidget* widget)
{
    const int index = layout ? layout->indexOf(anchor) : -1;
    if (index >= 0) {
        layout->insertWidget(index + 1, widget);
    } else if (layout) {
        layout->addWidget(widget);
    }
}

QToolButton* createMenuButton(QWidget* parent, const QString& text)
{
    auto* button = new QToolButton(parent);
    button->setText(text);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    return button;
}

QTableWidgetItem* readOnlyTableItem(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

QTableWidgetItem* editableTableItem(const QString& text)
{
    return new QTableWidgetItem(text);
}

QString visibleQueueText(QString text)
{
    text.replace(QLatin1Char('\r'), QStringLiteral("\\r"));
    text.replace(QLatin1Char('\n'), QStringLiteral("\\n"));
    return text;
}

QString queueTextFromVisible(QString text)
{
    text.replace(QStringLiteral("\\r"), QStringLiteral("\r"));
    text.replace(QStringLiteral("\\n"), QStringLiteral("\n"));
    return text;
}

int positiveCellValue(const QString& text, int fallback, int minimum, int maximum)
{
    bool ok = false;
    const int value = text.trimmed().toInt(&ok);
    return ok ? qBound(minimum, value, maximum) : fallback;
}

QAction* addCheckAction(QMenu* menu, const QString& text, QCheckBox* checkBox)
{
    QAction* action = menu->addAction(text);
    action->setCheckable(true);
    action->setChecked(checkBox->isChecked());
    QObject::connect(action, &QAction::toggled, checkBox, &QCheckBox::setChecked);
    QObject::connect(checkBox, &QCheckBox::toggled, action, &QAction::setChecked);
    return action;
}

}

NetworkPanel::NetworkPanel(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::NetworkPanel),
      m_network(new NetworkManager(this)),
      m_sendQueue(new SendQueue(this)),
      m_timerSendTimer(new QTimer(this))
{
    ui->setupUi(this);
    setupUiFromForm();
    setupConnections();
    setupSendPreview();
    setupSendQueueControls();
}

NetworkPanel::~NetworkPanel()
{
    if (m_network) {
        disconnect(m_network, nullptr, this, nullptr);
        m_network->close();
    }
    delete ui;
}

bool NetworkPanel::isConnected() const
{
    return m_network->isOpen();
}

QString NetworkPanel::connectionName() const
{
    return endpointText();
}

NetworkConfig NetworkPanel::networkConfig() const
{
    NetworkConfig config;
    config.mode = ui->modeCombo->currentIndex();
    config.localAddress = ui->localAddressEdit->text().trimmed();
    config.localPort = static_cast<quint16>(ui->localPortSpin->value());
    config.remoteHost = ui->remoteHostEdit->text().trimmed();
    config.remotePort = static_cast<quint16>(ui->remotePortSpin->value());
    config.hexDisplay = ui->hexDisplayCheck->isChecked();
    config.hexSend = ui->hexSendCheck->isChecked();
    config.showTimestamp = ui->timestampCheck->isChecked();
    config.autoScroll = ui->autoScrollCheck->isChecked();
    config.encoding = ui->encodingCombo->currentText();
    return config;
}

void NetworkPanel::setNetworkConfig(const NetworkConfig& config)
{
    ui->modeCombo->setCurrentIndex(qBound(0, config.mode, ui->modeCombo->count() - 1));
    ui->localAddressEdit->setText(config.localAddress);
    ui->localPortSpin->setValue(config.localPort);
    ui->remoteHostEdit->setText(config.remoteHost);
    ui->remotePortSpin->setValue(config.remotePort);
    ui->hexDisplayCheck->setChecked(config.hexDisplay);
    ui->hexSendCheck->setChecked(config.hexSend);
    ui->timestampCheck->setChecked(config.showTimestamp);
    ui->autoScrollCheck->setChecked(config.autoScroll);
    const int encodingIndex = ui->encodingCombo->findText(config.encoding);
    if (encodingIndex >= 0) {
        ui->encodingCombo->setCurrentIndex(encodingIndex);
    }
    updateModeUi();
}

void NetworkPanel::setupUiFromForm()
{
    updateStatusIndicator(false);
    updateModeUi();
    updateStatistics();
}

void NetworkPanel::setupConnections()
{
    connect(ui->modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &NetworkPanel::onModeChanged);
    connect(ui->openButton, &QPushButton::clicked, this, &NetworkPanel::onOpenClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &NetworkPanel::onCloseClicked);
    connect(ui->sendButton, &QPushButton::clicked, this, &NetworkPanel::onSendClicked);
    connect(ui->sendFileButton, &QPushButton::clicked, this, &NetworkPanel::onSendFileClicked);
    connect(ui->saveDataButton, &QPushButton::clicked, this, &NetworkPanel::onSaveDataClicked);
    connect(ui->timerSendCheck, &QCheckBox::toggled, this, &NetworkPanel::onTimerSendToggled);
    connect(ui->timerSendInterval, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &NetworkPanel::onTimerSendIntervalChanged);
    connect(ui->clearButton, &QPushButton::clicked, this, &NetworkPanel::onClearClicked);
    connect(m_timerSendTimer, &QTimer::timeout, this, &NetworkPanel::onSendClicked);

    connect(m_network, &NetworkManager::opened, this, &NetworkPanel::onOpened);
    connect(m_network, &NetworkManager::closed, this, &NetworkPanel::onClosed);
    connect(m_network, &NetworkManager::dataReceived, this, &NetworkPanel::onDataReceived);
    connect(m_network, &NetworkManager::errorOccurred, this, &NetworkPanel::onNetworkError);
    connect(m_network, &NetworkManager::peerListChanged, this, &NetworkPanel::onPeerListChanged);

    setupActionMenus();
}

void NetworkPanel::setupActionMenus()
{
    ui->hexDisplayCheck->hide();
    ui->timestampCheck->hide();
    ui->autoScrollCheck->hide();
    ui->hexSendCheck->hide();
    ui->timerSendCheck->hide();
    ui->clearButton->hide();
    ui->sendFileButton->hide();
    ui->saveDataButton->hide();
    ui->timerSendInterval->setPrefix(tr("间隔 "));

    auto* connectionButton = createMenuButton(this, tr("网口操作"));
    auto* connectionMenu = new QMenu(connectionButton);
    connectionMenu->addAction(tr("清空接收区"), this, &NetworkPanel::onClearClicked);
    connectionButton->setMenu(connectionMenu);
    insertAfter(ui->remoteRowLayout, ui->closeButton, connectionButton);

    auto* displayButton = createMenuButton(this, tr("显示设置"));
    auto* displayMenu = new QMenu(displayButton);
    addCheckAction(displayMenu, tr("十六进制显示"), ui->hexDisplayCheck);
    addCheckAction(displayMenu, tr("时间戳"), ui->timestampCheck);
    addCheckAction(displayMenu, tr("自动滚动"), ui->autoScrollCheck);
    displayButton->setMenu(displayMenu);
    ui->receiveOptionsLayout->insertWidget(0, displayButton);

    auto* sendButton = createMenuButton(this, tr("发送操作"));
    auto* sendMenu = new QMenu(sendButton);
    addCheckAction(sendMenu, tr("十六进制发送"), ui->hexSendCheck);
    addCheckAction(sendMenu, tr("定时发送"), ui->timerSendCheck);
    sendMenu->addSeparator();
    sendMenu->addAction(tr("发送文件"), this, &NetworkPanel::onSendFileClicked);
    sendMenu->addAction(tr("保存数据"), this, &NetworkPanel::onSaveDataClicked);
    sendButton->setMenu(sendMenu);
    insertAfter(ui->sendButtonLayout, ui->sendButton, sendButton);
}

void NetworkPanel::setupSendPreview()
{
    m_sendPreviewLabel = ui->sendPreviewLabel;
    m_sendPreviewLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    connect(ui->txEdit, &QTextEdit::textChanged, this, &NetworkPanel::updateSendPreview);
    connect(ui->hexSendCheck, &QCheckBox::toggled, this, &NetworkPanel::updateSendPreview);
    connect(ui->encodingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &NetworkPanel::updateSendPreview);
    updateSendPreview();
}

void NetworkPanel::setupSendQueueControls()
{
    m_queueRepeatSpin = ui->queueRepeatSpin;
    m_queueIntervalSpin = ui->queueIntervalSpin;
    m_queueLoopCheck = ui->queueLoopCheck;
    m_queueStatusLabel = ui->queueStatusLabel;
    m_queueIntervalSpin->setValue(ui->timerSendInterval->value());
    m_queueStatusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_queueTable = ui->queueTable;
    m_queueTable->setColumnCount(8);
    m_queueTable->setHorizontalHeaderLabels(
        {tr("序号"), tr("剩余"), tr("次数"), tr("间隔(ms)"), tr("字节"), tr("目标"), tr("十六进制"), tr("内容")});
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

    connect(m_addQueueButton, &QPushButton::clicked, this, &NetworkPanel::onAddQueueClicked);
    connect(m_insertQueueButton, &QPushButton::clicked, this, &NetworkPanel::onInsertQueueClicked);
    connect(m_removeQueueButton, &QPushButton::clicked, this, &NetworkPanel::onRemoveQueueClicked);
    connect(m_clearQueueButton, &QPushButton::clicked, this, &NetworkPanel::onClearQueueClicked);
    connect(m_startQueueButton, &QPushButton::clicked, this, &NetworkPanel::onStartQueueClicked);
    connect(m_stopQueueButton, &QPushButton::clicked, this, &NetworkPanel::onStopQueueClicked);
    connect(m_queueTable, &QTableWidget::itemChanged, this, &NetworkPanel::onQueueTableItemChanged);
    connect(m_queueLoopCheck, &QCheckBox::toggled, m_sendQueue, &SendQueue::setLoopEnabled);
    connect(m_sendQueue, &SendQueue::sendRequested, this, &NetworkPanel::onQueueSendRequested);
    connect(m_sendQueue, &SendQueue::queueChanged, this, &NetworkPanel::updateSendQueueStatus);

    m_sendQueue->setIntervalMs(m_queueIntervalSpin->value());
    m_sendQueue->setLoopEnabled(m_queueLoopCheck->isChecked());
    updateSendQueueStatus(m_sendQueue->pendingItemCount(), m_sendQueue->pendingSendCount(), m_sendQueue->isRunning());
}

int NetworkPanel::selectedQueueRow() const
{
    if (!m_queueTable || m_queueTable->selectionModel()->selectedRows().isEmpty()) {
        return -1;
    }
    return m_queueTable->selectionModel()->selectedRows().first().row();
}

QString NetworkPanel::queueTargetText(const SendQueue::QueueItem& item) const
{
    if (!item.peer.isEmpty()) {
        return item.peer;
    }
    if (!item.remoteHost.isEmpty() && item.remotePort > 0) {
        return QStringLiteral("%1:%2").arg(item.remoteHost).arg(item.remotePort);
    }
    return tr("全部客户端");
}

void NetworkPanel::refreshSendQueueTable()
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
        const QString displayText = visibleQueueText(item.displayText);

        m_queueTable->setItem(row, 0, readOnlyTableItem(QString::number(row + 1)));
        m_queueTable->setItem(row, 1, editableTableItem(QString::number(item.remaining)));
        m_queueTable->setItem(row, 2, editableTableItem(QString::number(item.repeatCount)));
        m_queueTable->setItem(row, 3, editableTableItem(QString::number(item.intervalMs)));
        m_queueTable->setItem(row, 4, readOnlyTableItem(QString::number(item.payload.size())));
        m_queueTable->setItem(row, 5, readOnlyTableItem(queueTargetText(item)));
        m_queueTable->setItem(row, 6, readOnlyTableItem(QString::fromLatin1(item.payload.toHex(' ')).toUpper()));
        m_queueTable->setItem(row, 7, editableTableItem(displayText));
    }

    m_queueTable->resizeColumnsToContents();
    m_queueTable->horizontalHeader()->setStretchLastSection(true);
    if (selectedRow >= 0 && selectedRow < m_queueTable->rowCount()) {
        m_queueTable->selectRow(selectedRow);
    }
    m_updatingQueueTable = false;
}

void NetworkPanel::onModeChanged(int)
{
    updateModeUi();
}

void NetworkPanel::onOpenClicked()
{
    const int mode = ui->modeCombo->currentIndex();
    if (mode == static_cast<int>(NetworkManager::Mode::TcpClient)) {
        m_network->openTcpClient(
            ui->remoteHostEdit->text().trimmed(),
            static_cast<quint16>(ui->remotePortSpin->value()),
            QString(),
            0);
    } else if (mode == static_cast<int>(NetworkManager::Mode::TcpServer)) {
        m_network->openTcpServer(
            ui->localAddressEdit->text().trimmed(),
            static_cast<quint16>(ui->localPortSpin->value()));
    } else {
        m_network->openUdp(
            ui->localAddressEdit->text().trimmed(),
            static_cast<quint16>(ui->localPortSpin->value()));
    }
}

void NetworkPanel::onCloseClicked()
{
    m_network->close();
}

void NetworkPanel::onSendClicked()
{
    bool ok = false;
    const QByteArray payload = encodeSendText(ui->txEdit->toPlainText(), &ok);
    if (!ok || payload.isEmpty()) {
        return;
    }

    const qint64 written = m_network->sendData(
        payload,
        ui->remoteHostEdit->text().trimmed(),
        static_cast<quint16>(ui->remotePortSpin->value()),
        ui->sendTargetCombo->currentData().toString());
    if (written <= 0) {
        return;
    }

    m_txByteCount += static_cast<uint64_t>(written);
    appendLine(QStringLiteral("发送 %1 字节: %2")
                   .arg(written)
                   .arg(displayTextForBytes(payload.left(static_cast<int>(written)))),
               QColor(QStringLiteral("#89b4fa")));
    updateStatistics();
}

void NetworkPanel::onSendFileClicked()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("选择发送文件"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("发送文件"), tr("无法打开文件。"));
        return;
    }

    const QByteArray payload = file.readAll();
    const qint64 written = m_network->sendData(
        payload,
        ui->remoteHostEdit->text().trimmed(),
        static_cast<quint16>(ui->remotePortSpin->value()),
        ui->sendTargetCombo->currentData().toString());
    if (written <= 0) {
        return;
    }

    m_txByteCount += static_cast<uint64_t>(written);
    appendLine(QStringLiteral("发送文件 %1 字节: %2").arg(written).arg(fileName),
               QColor(QStringLiteral("#89b4fa")));
    updateStatistics();
}

void NetworkPanel::onSaveDataClicked()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this, tr("保存数据"), QString(), tr("文本文件 (*.txt);;所有文件 (*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("保存数据"), tr("无法写入文件。"));
        return;
    }

    QTextStream out(&file);
    out << ui->rxEdit->toPlainText();
}

void NetworkPanel::onTimerSendToggled(bool enabled)
{
    ui->timerSendInterval->setEnabled(enabled);
    if (enabled) {
        m_timerSendTimer->start(ui->timerSendInterval->value());
    } else {
        m_timerSendTimer->stop();
    }
}

void NetworkPanel::onTimerSendIntervalChanged(int value)
{
    if (ui->timerSendCheck->isChecked()) {
        m_timerSendTimer->start(value);
    }
}

void NetworkPanel::onAddQueueClicked()
{
    const QString displayText = ui->txEdit->toPlainText();
    bool ok = false;
    const QByteArray payload = encodeSendText(displayText, &ok);
    if (!ok || payload.isEmpty()) {
        return;
    }

    m_sendQueue->enqueue(payload,
                         displayText,
                         m_queueRepeatSpin->value(),
                         m_queueIntervalSpin->value(),
                         ui->remoteHostEdit->text().trimmed(),
                         static_cast<quint16>(ui->remotePortSpin->value()),
                         ui->sendTargetCombo->currentData().toString());
    if (m_queueTable && m_sendQueue->pendingItemCount() > 0) {
        m_queueTable->selectRow(m_sendQueue->pendingItemCount() - 1);
    }
}

void NetworkPanel::onInsertQueueClicked()
{
    const QString displayText = ui->txEdit->toPlainText();
    bool ok = false;
    const QByteArray payload = encodeSendText(displayText, &ok);
    if (!ok || payload.isEmpty()) {
        return;
    }

    const int row = selectedQueueRow();
    const int insertRow = row < 0 ? m_sendQueue->pendingItemCount() : row;
    m_sendQueue->insert(insertRow,
                        payload,
                        displayText,
                        m_queueRepeatSpin->value(),
                        m_queueIntervalSpin->value(),
                        ui->remoteHostEdit->text().trimmed(),
                        static_cast<quint16>(ui->remotePortSpin->value()),
                        ui->sendTargetCombo->currentData().toString());
    if (m_queueTable && insertRow < m_sendQueue->pendingItemCount()) {
        m_queueTable->selectRow(insertRow);
    }
}

void NetworkPanel::onRemoveQueueClicked()
{
    m_sendQueue->removeAt(selectedQueueRow());
}

void NetworkPanel::onClearQueueClicked()
{
    m_sendQueue->clear();
}

void NetworkPanel::onStartQueueClicked()
{
    if (!m_network->isOpen()) {
        return;
    }

    m_sendQueue->start();
}

void NetworkPanel::onStopQueueClicked()
{
    m_sendQueue->stop();
}

void NetworkPanel::onQueueTableItemChanged(QTableWidgetItem* tableItem)
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
        item.remaining = positiveCellValue(tableItem->text(), item.remaining, 1, 9999);
        break;
    case 2:
        item.repeatCount = positiveCellValue(tableItem->text(), item.repeatCount, 1, 9999);
        item.remaining = item.repeatCount;
        break;
    case 3:
        item.intervalMs = positiveCellValue(tableItem->text(), item.intervalMs, 10, 600000);
        break;
    case 7: {
        const QString displayText = queueTextFromVisible(tableItem->text());
        if (displayText.isEmpty()) {
            refreshSendQueueTable();
            return;
        }

        bool ok = false;
        const QByteArray payload = encodeSendText(displayText, &ok);
        if (!ok || payload.isEmpty()) {
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

void NetworkPanel::onQueueSendRequested(const QByteArray& payload,
                                        const QString& displayText,
                                        const QString& remoteHost,
                                        quint16 remotePort,
                                        const QString& peer)
{
    const qint64 written = m_network->sendData(payload, remoteHost, remotePort, peer);
    if (written <= 0) {
        m_sendQueue->stop();
        return;
    }

    m_txByteCount += static_cast<uint64_t>(written);
    appendLine(QStringLiteral("队列发送 %1 字节: %2")
                   .arg(written)
                   .arg(displayText.isEmpty()
                            ? displayTextForBytes(payload.left(static_cast<int>(written)))
                            : displayText),
               QColor(QStringLiteral("#89b4fa")));
    updateStatistics();
}

void NetworkPanel::updateSendQueueStatus(int pendingItems, int pendingSends, bool running)
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
        m_startQueueButton->setEnabled(!running && pendingItems > 0 && m_network->isOpen());
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

void NetworkPanel::onClearClicked()
{
    ui->rxEdit->clear();
    m_rxByteCount = 0;
    m_txByteCount = 0;
    ui->lastPeerLabel->setText(tr("最近来源: -"));
    ui->errorLabel->setText(tr("最近错误: -"));
    updateStatistics();
}

void NetworkPanel::onOpened()
{
    updateStatusIndicator(true);
    setConfigurationControlsEnabled(false);
    updateSendQueueStatus(m_sendQueue->pendingItemCount(), m_sendQueue->pendingSendCount(), m_sendQueue->isRunning());
    appendLine(QStringLiteral("已打开: %1").arg(endpointText()), QColor(QStringLiteral("#a6e3a1")));
    emit connectionStateChanged(true);
}

void NetworkPanel::onClosed()
{
    m_sendQueue->stop();
    updateStatusIndicator(false);
    setConfigurationControlsEnabled(true);
    appendLine(QStringLiteral("已关闭: %1").arg(modeText()), QColor(QStringLiteral("#a6adc8")));
    emit connectionStateChanged(false);
}

void NetworkPanel::onDataReceived(const QByteArray& payload, const QString& peer)
{
    m_rxByteCount += static_cast<uint64_t>(payload.size());
    ui->lastPeerLabel->setText(tr("最近来源: %1").arg(peer));
    appendLine(QStringLiteral("接收 [%1] %2").arg(peer, displayTextForBytes(payload)),
               QColor(QStringLiteral("#cdd6f4")));
    updateStatistics();
}

void NetworkPanel::onNetworkError(const QString& message)
{
    ui->errorLabel->setText(tr("最近错误: %1").arg(message));
    appendLine(QStringLiteral("错误: %1").arg(message), QColor(QStringLiteral("#f38ba8")));
}

void NetworkPanel::onPeerListChanged(const QStringList& peers)
{
    ui->peerListWidget->clear();
    ui->sendTargetCombo->clear();
    ui->sendTargetCombo->addItem(tr("全部客户端"), QString());

    for (const QString& peer : peers) {
        ui->peerListWidget->addItem(peer);
        ui->sendTargetCombo->addItem(peer, peer);
    }
}

void NetworkPanel::updateModeUi()
{
    const int mode = ui->modeCombo->currentIndex();
    const bool tcpClient = mode == static_cast<int>(NetworkManager::Mode::TcpClient);
    const bool tcpServer = mode == static_cast<int>(NetworkManager::Mode::TcpServer);
    const bool udp = mode == static_cast<int>(NetworkManager::Mode::Udp);
    const bool localEnabled = tcpServer || udp;

    ui->localAddressLabel->setEnabled(localEnabled);
    ui->localAddressEdit->setEnabled(localEnabled && !m_network->isOpen());
    ui->localPortLabel->setEnabled(localEnabled);
    ui->localPortSpin->setEnabled(localEnabled && !m_network->isOpen());
    ui->remoteHostLabel->setEnabled(tcpClient || udp);
    ui->remoteHostEdit->setEnabled((tcpClient || udp) && !m_network->isOpen());
    ui->remotePortLabel->setEnabled(tcpClient || udp);
    ui->remotePortSpin->setEnabled((tcpClient || udp) && !m_network->isOpen());
    ui->sendTargetLabel->setEnabled(tcpServer);
    ui->sendTargetCombo->setEnabled(tcpServer);
    ui->modeInfoLabel->setText(tr("模式: %1").arg(modeText()));
    ui->endpointInfoLabel->setText(tr("端点: %1").arg(endpointText()));
}

void NetworkPanel::updateStatusIndicator(bool connected)
{
    if (connected) {
        ui->statusIndicator->setStyleSheet(
            "background-color: #a6e3a1; border-radius: 8px; border: 2px solid #45475a;");
        ui->statusLabel->setText(tr("已打开"));
    } else {
        ui->statusIndicator->setStyleSheet(
            "background-color: #f38ba8; border-radius: 8px; border: 2px solid #45475a;");
        ui->statusLabel->setText(tr("未打开"));
    }

    ui->openButton->setEnabled(!connected);
    ui->closeButton->setEnabled(connected);
}

void NetworkPanel::updateStatistics()
{
    ui->rxCountLabel->setText(tr("接收: %1").arg(m_rxByteCount));
    ui->txCountLabel->setText(tr("发送: %1").arg(m_txByteCount));
    ui->endpointInfoLabel->setText(tr("端点: %1").arg(endpointText()));
}

void NetworkPanel::updateSendPreview()
{
    if (!m_sendPreviewLabel) {
        return;
    }

    if (!ui->hexSendCheck->isChecked()) {
        m_sendPreviewLabel->setText(tr("发送预估: %1 B").arg(encodeText(ui->txEdit->toPlainText()).size()));
        return;
    }

    const QString normalized = normalizeHex(ui->txEdit->toPlainText());
    if (normalized.isEmpty()) {
        m_sendPreviewLabel->setText(tr("发送预估: 0 B"));
        return;
    }

    if (normalized.size() % 2 != 0) {
        m_sendPreviewLabel->setText(tr("发送预估: 十六进制长度错误"));
        return;
    }

    const QByteArray hex = normalized.toLatin1();
    const QByteArray payload = QByteArray::fromHex(hex);
    if (payload.toHex() != hex.toLower()) {
        m_sendPreviewLabel->setText(tr("发送预估: 十六进制内容无效"));
        return;
    }

    m_sendPreviewLabel->setText(tr("发送预估: %1 B").arg(payload.size()));
}

void NetworkPanel::appendLine(const QString& text, const QColor& color)
{
    QString line = text;
    if (ui->timestampCheck->isChecked()) {
        line = QStringLiteral("[%1] %2")
                   .arg(QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss.zzz")), text);
    }

    ui->rxEdit->append(QStringLiteral("<span style=\"color:%1;\">%2</span>")
                           .arg(color.name(), line.toHtmlEscaped()));

    if (ui->autoScrollCheck->isChecked()) {
        QScrollBar* scrollBar = ui->rxEdit->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void NetworkPanel::setConfigurationControlsEnabled(bool enabled)
{
    ui->modeCombo->setEnabled(enabled);
    ui->localAddressEdit->setEnabled(enabled && ui->localAddressLabel->isEnabled());
    ui->localPortSpin->setEnabled(enabled && ui->localPortLabel->isEnabled());
    ui->remoteHostEdit->setEnabled(enabled && ui->remoteHostLabel->isEnabled());
    ui->remotePortSpin->setEnabled(enabled && ui->remotePortLabel->isEnabled());
}

QByteArray NetworkPanel::encodeSendText(const QString& text, bool* ok) const
{
    if (ok) {
        *ok = true;
    }

    if (!ui->hexSendCheck->isChecked()) {
        return encodeText(text);
    }

    const QString normalized = normalizeHex(text);
    if (normalized.isEmpty()) {
        return {};
    }

    if (normalized.size() % 2 != 0) {
        if (ok) {
            *ok = false;
        }
        QMessageBox::warning(const_cast<NetworkPanel*>(this), tr("十六进制发送"), tr("十六进制字符数量必须为偶数。"));
        return {};
    }

    const QByteArray hex = normalized.toLatin1();
    QByteArray payload = QByteArray::fromHex(hex);
    if (payload.toHex() != hex.toLower()) {
        if (ok) {
            *ok = false;
        }
        QMessageBox::warning(const_cast<NetworkPanel*>(this), tr("十六进制发送"), tr("十六进制内容无效。"));
        return {};
    }

    return payload;
}

QString NetworkPanel::currentEncoding() const
{
    return ui->encodingCombo->currentText();
}

QByteArray NetworkPanel::encodeText(const QString& text) const
{
    const QString encoding = currentEncoding();
    if (encoding == QStringLiteral("GBK")) {
        return text.toLocal8Bit();
    }
    if (encoding == QStringLiteral("ASCII")) {
        return text.toLatin1();
    }
    return text.toUtf8();
}

QString NetworkPanel::decodeBytes(const QByteArray& data) const
{
    const QString encoding = currentEncoding();
    if (encoding == QStringLiteral("GBK")) {
        return QString::fromLocal8Bit(data);
    }
    if (encoding == QStringLiteral("ASCII")) {
        return QString::fromLatin1(data);
    }
    return QString::fromUtf8(data);
}

QString NetworkPanel::displayTextForBytes(const QByteArray& data) const
{
    if (ui->hexDisplayCheck->isChecked()) {
        return bytesToHex(data);
    }
    return decodeBytes(data);
}

QString NetworkPanel::modeText() const
{
    return ui->modeCombo->currentText();
}

QString NetworkPanel::endpointText() const
{
    const int mode = ui->modeCombo->currentIndex();
    if (mode == static_cast<int>(NetworkManager::Mode::TcpClient)) {
        return QStringLiteral("%1:%2").arg(ui->remoteHostEdit->text().trimmed()).arg(ui->remotePortSpin->value());
    }

    if (mode == static_cast<int>(NetworkManager::Mode::TcpServer)) {
        return QStringLiteral("%1:%2").arg(ui->localAddressEdit->text().trimmed()).arg(ui->localPortSpin->value());
    }

    return QStringLiteral("本地 %1:%2 -> 远端 %3:%4")
        .arg(ui->localAddressEdit->text().trimmed())
        .arg(ui->localPortSpin->value())
        .arg(ui->remoteHostEdit->text().trimmed())
        .arg(ui->remotePortSpin->value());
}
