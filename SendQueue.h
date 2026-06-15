#pragma once

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QString>
#include <QtGlobal>

class QTimer;

class SendQueue : public QObject
{
    Q_OBJECT

public:
    struct QueueItem {
        QByteArray payload;
        QString displayText;
        QString remoteHost;
        quint16 remotePort = 0;
        QString peer;
        int repeatCount = 1;
        int remaining = 1;
        int intervalMs = 1000;
    };

    explicit SendQueue(QObject* parent = nullptr);

    bool isRunning() const { return m_running; }
    bool loopEnabled() const { return m_loopEnabled; }
    int loopRepeatCount() const { return m_loopRepeatCount; }
    int intervalMs() const { return m_defaultIntervalMs; }
    int pendingItemCount() const;
    int pendingSendCount() const;
    QueueItem itemAt(int index) const;

    void setIntervalMs(int intervalMs);
    void setLoopEnabled(bool enabled);
    void setLoopRepeatCount(int count);
    void enqueue(const QByteArray& payload,
                 const QString& displayText,
                 int repeatCount,
                 int intervalMs = 1000,
                 const QString& remoteHost = QString(),
                 quint16 remotePort = 0,
                 const QString& peer = QString());
    void insert(int index,
                const QByteArray& payload,
                const QString& displayText,
                int repeatCount,
                int intervalMs = 1000,
                const QString& remoteHost = QString(),
                quint16 remotePort = 0,
                const QString& peer = QString());
    void updateItem(int index, const QueueItem& item);
    void removeAt(int index);
    void clear();
    void clearAndStop();

    bool exportToJson(const QString& filePath) const;
    bool importFromJson(const QString& filePath);

public slots:
    void start();
    void stop();

signals:
    void sendRequested(const QByteArray& payload,
                       const QString& displayText,
                       const QString& remoteHost,
                       quint16 remotePort,
                       const QString& peer);
    void queueChanged(int pendingItems, int pendingSends, bool running);
    void finished();

private slots:
    void dispatchNext();

private:
    void emitQueueChanged();

    QList<QueueItem> m_items;
    QList<QueueItem> m_backupItems;
    QTimer* m_timer = nullptr;
    int m_defaultIntervalMs = 1000;
    bool m_running = false;
    bool m_loopEnabled = false;
    int m_loopRepeatCount = 0;
    int m_loopRemaining = 0;
};
