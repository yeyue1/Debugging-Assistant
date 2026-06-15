#pragma once

#include <QWidget>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <cstdint>

#include "NetworkConfig.h"
#include "SendQueue.h"
#include "SerialRecord.h"

namespace Ui { class NetworkPanel; }

class NetworkManager;
class RecordStore;
class AutomationRuleEngine;
class SearchBarHelper;
class QColor;
class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QSpinBox;
class QTableWidget;
class QTableWidgetItem;
class QTimer;

class NetworkPanel : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkPanel(QWidget* parent = nullptr);
    ~NetworkPanel() override;

    bool isConnected() const;
    QString connectionName() const;
    AutomationRuleEngine* autoReplyEngine() const { return m_autoReplyEngine; }
    NetworkConfig networkConfig() const;
    void setNetworkConfig(const NetworkConfig& config);

    QByteArray saveSplitterState() const;
    void restoreSplitterState(const QByteArray& state);
    void restoreSplitterState();
    QString splitterSettingsKey() const;

signals:
    void connectionStateChanged(bool connected);

private slots:
    void onModeChanged(int index);
    void onOpenClicked();
    void onCloseClicked();
    void onSendClicked();
    void onSendFileClicked();
    void onSaveDataClicked();
    void onTimerSendToggled(bool enabled);
    void onTimerSendIntervalChanged(int value);
    void onClearClicked();
    void onOpened();
    void onClosed();
    void onDataReceived(const QByteArray& payload, const QString& peer);
    void onNetworkError(const QString& message);
    void onPeerListChanged(const QStringList& peers);
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
    void onFilterChanged();
    void handleAutoReply(const SerialRecord& record);

private:
    void setupUiFromForm();
    void setupConnections();
    void setupActionMenus();
    void setupSendPreview();
    void setupSendQueueControls();
    int selectedQueueRow() const;
    void refreshSendQueueTable();
    QString queueTargetText(const SendQueue::QueueItem& item) const;
    void updateModeUi();
    void updateStatusIndicator(bool connected);
    void updateStatistics();
    void updateSendPreview();
    void appendLine(const QString& text, const QColor& color);
    void setConfigurationControlsEnabled(bool enabled);
    QByteArray encodeSendText(const QString& text, bool* ok) const;
    QString currentEncoding() const;
    QByteArray encodeText(const QString& text) const;
    QString decodeBytes(const QByteArray& data) const;
    QString displayTextForBytes(const QByteArray& data) const;
    QString modeText() const;
    QString endpointText() const;
    void appendRecordToReceive(const SerialRecord& record, bool matchedFilter);
    void refreshRecordView();
    void saveRecordsAsText(const QString& fileName, bool filteredOnly) const;
    void saveRecordsAsCsv(const QString& fileName, bool filteredOnly) const;
    void saveRecordsAsJson(const QString& fileName, bool filteredOnly) const;

    int m_instanceId = 0;

    Ui::NetworkPanel* ui = nullptr;
    NetworkManager* m_network = nullptr;
    RecordStore* m_recordStore = nullptr;
    AutomationRuleEngine* m_autoReplyEngine = nullptr;
    SearchBarHelper* m_searchHelper = nullptr;
    SendQueue* m_sendQueue = nullptr;
    QTimer* m_timerSendTimer = nullptr;
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
    uint64_t m_rxByteCount = 0;
    uint64_t m_txByteCount = 0;
    bool m_updatingQueueTable = false;
};
