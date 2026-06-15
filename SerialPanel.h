#pragma once

#include <QWidget>
#include <QSerialPort>
#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <cstdint>

#include "SerialConfig.h"
#include "SerialRecord.h"
#include "ParsedFrame.h"

namespace Ui { class SerialPanel; }

class QTimer;
class QLabel;
class QListWidget;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QSpinBox;
class QTableWidget;
class QTableWidgetItem;
class QTextEdit;
class SerialManager;
class ProtocolParserBase;
class DeviceRegistry;
class RecordStore;
class SearchBarHelper;
class AutomationRuleEngine;
class TemplateManager;
class ParserController;
class SendQueue;

class SerialPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPanel(QWidget* parent = nullptr);
    ~SerialPanel() override;

    QString portName() const;
    bool isConnected() const;
    AutomationRuleEngine* autoReplyEngine() const { return m_autoReplyEngine; }
    SerialConfig serialConfig() const;
    void setSerialConfig(const SerialConfig& config, bool includePortName = true);
    QJsonObject connectionTemplate() const;
    bool applyConnectionTemplate(const QJsonObject& object, bool includePortName = true);

    QByteArray saveSplitterState() const;
    void restoreSplitterState(const QByteArray& state);
    void restoreSplitterState();
    QString splitterSettingsKey() const;

signals:
    void connectionStateChanged(bool connected);
    void panelDestroyed(const QString& portName);

private slots:
    void refreshPortList();
    void applyConfigFromUi();
    void onOpenClicked();
    void onCloseClicked();
    void onSendClicked();
    void onClearClicked();
    void onHexToggled(bool enabled);
    void onProtocolChanged(int index);
    void onAutoScrollToggled(bool enabled);
    void onTimestampToggled(bool enabled);
    void onSendFileClicked();
    void onTimerSendToggled(bool enabled);
    void onTimerSendIntervalChanged(int value);
    void onPortSelectionChanged();

    void onOpened();
    void onClosed();
    void onDataReceived(const QByteArray& data);
    void onFrameReady(const QByteArray& payload, const QString& info);
    void onParsedFrameReady(const ParsedFrame& frame);
    void onRawDataReady(const QByteArray& data);
    void onFrameError(const QString& message);
    void onSerialError(QSerialPort::SerialPortError code, const QString& message);

    void onSaveDataClicked();
    void onClearHistoryClicked();
    void onDtrToggled(bool enabled);
    void onRtsToggled(bool enabled);
    void onNewLineChanged(int index);
    void onEncodingChanged(int index);
    void onSaveDeviceAliasClicked();
    void onReconnectClicked();
    void onExportConfigTemplateClicked();
    void onImportConfigTemplateClicked();
    void onFilterChanged();
    void onAddQueueClicked();
    void onInsertQueueClicked();
    void onRemoveQueueClicked();
    void onClearQueueClicked();
    void onStartQueueClicked();
    void onStopQueueClicked();
    void onQueueTableItemChanged(QTableWidgetItem* item);
    void onQueueSendRequested(const QByteArray& payload,
                              const QString& displayText,
                              const QString& remoteHost,
                              quint16 remotePort,
                              const QString& peer);
    void updateSendQueueStatus(int pendingItems, int pendingSends, bool running);

private:
    void setupUiFromForm();
    void bindFeatureUi();
    void setupConnections();
    void setupActionMenus();
    void setupSendPreview();
    void setupSendQueueControls();
    int selectedQueueRow() const;
    void refreshSendQueueTable();
    void updatePortCombo(const QStringList& ports);
    void updateStatistics();
    void updateSendPreview();
    void appendToReceive(const QString& text);
    void appendRecordToReceive(const SerialRecord& record, bool matchedFilter);
    void refreshRecordView();
    void refreshParsedFrameView(const SerialRecord& record);
    void updateStatusIndicator(bool connected);
    void addToHistory(const QString& text);
    void updateDeviceInfo();
    QString currentDeviceKey() const;
    QString decodeBytes(const QByteArray& data) const;
    QByteArray encodeText(const QString& text) const;
    QString displayTextForPayload(const QByteArray& data) const;
    qint64 sendPayload(const QByteArray& data, const QString& displayText, bool addHistory);
    void setConnectionControls(bool connected);
    void setConfigurationControlsEnabled(bool enabled);
    void addRecord(const SerialRecord& record);
    void handleAutoReply(const SerialRecord& record);
    void saveRecordsAsText(const QString& fileName, bool filteredOnly) const;
    void saveRecordsAsCsv(const QString& fileName, bool filteredOnly) const;
    void saveRecordsAsJson(const QString& fileName, bool filteredOnly) const;
    void applyTemplateObject(const QJsonObject& object, bool includePortName);

    int m_instanceId = 0;

    Ui::SerialPanel* ui = nullptr;
    SerialManager* m_serial = nullptr;
    ParserController* m_parserController = nullptr;
    DeviceRegistry* m_deviceRegistry = nullptr;
    RecordStore* m_recordStore = nullptr;
    AutomationRuleEngine* m_autoReplyEngine = nullptr;
    TemplateManager* m_templateManager = nullptr;
    SendQueue* m_sendQueue = nullptr;
    QTimer* m_portScanTimer = nullptr;
    QTimer* m_timerSendTimer = nullptr;

    // Statistics
    uint64_t m_rxByteCount = 0;
    uint64_t m_txByteCount = 0;

    // Settings
    bool m_hexDisplay = false;
    bool m_autoScroll = true;
    bool m_showTimestamp = false;
    QStringList m_cachedPorts;

    int m_newLineMode = 0;
    QString m_encoding = "UTF-8";
    QStringList m_sendHistory;

    // Feature UI pointers (from serialpanel.ui)
    QLabel* m_deviceInfoLabel = nullptr;
    QLineEdit* m_deviceAliasEdit = nullptr;
    QPushButton* m_saveAliasButton = nullptr;
    QPushButton* m_reconnectButton = nullptr;
    QPushButton* m_exportTemplateButton = nullptr;
    QPushButton* m_importTemplateButton = nullptr;
    QLabel* m_healthLabel = nullptr;
    QLabel* m_statsLabel = nullptr;
    QLabel* m_sendPreviewLabel = nullptr;
    QLabel* m_queueStatusLabel = nullptr;
    QSpinBox* m_queueIntervalSpin = nullptr;
    QSpinBox* m_queueRepeatSpin = nullptr;
    QCheckBox* m_queueLoopCheck = nullptr;
    QPushButton* m_addQueueButton = nullptr;
    QPushButton* m_insertQueueButton = nullptr;
    QPushButton* m_removeQueueButton = nullptr;
    QPushButton* m_clearQueueButton = nullptr;
    QPushButton* m_startQueueButton = nullptr;
    QPushButton* m_stopQueueButton = nullptr;
    QTableWidget* m_queueTable = nullptr;
    QLineEdit* m_filterEdit = nullptr;
    QCheckBox* m_filterRegexCheck = nullptr;
    QCheckBox* m_filterHideCheck = nullptr;
    QTableWidget* m_frameTable = nullptr;
    QTextEdit* m_jsonPreviewEdit = nullptr;

    // Search bar
    SearchBarHelper* m_searchHelper = nullptr;

    // Parsed frame data
    ParsedFrame m_lastParsedFrame;
    bool m_hasParsedFrame = false;

    // Runtime state
    QString m_lastDeviceKey;
    QString m_lastDisconnectReason;
    uint64_t m_reconnectCount = 0;
    uint64_t m_disconnectCount = 0;
    bool m_updatingQueueTable = false;
};
