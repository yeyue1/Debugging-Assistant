#include "SendQueue.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

SendQueue::SendQueue(QObject* parent)
    : QObject(parent),
      m_timer(new QTimer(this))
{
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &SendQueue::dispatchNext);
}

int SendQueue::pendingItemCount() const
{
    return m_items.size();
}

int SendQueue::pendingSendCount() const
{
    int total = 0;
    for (const QueueItem& item : m_items) {
        total += item.remaining;
    }
    return total;
}

SendQueue::QueueItem SendQueue::itemAt(int index) const
{
    if (index < 0 || index >= m_items.size()) {
        return {};
    }
    return m_items.at(index);
}

void SendQueue::setIntervalMs(int intervalMs)
{
    if (intervalMs <= 0) {
        return;
    }

    m_defaultIntervalMs = intervalMs;
}

void SendQueue::setLoopEnabled(bool enabled)
{
    if (m_loopEnabled == enabled) {
        return;
    }

    m_loopEnabled = enabled;
    if (m_loopEnabled) {
        m_loopRemaining = m_loopRepeatCount;
    }
    emitQueueChanged();
}

void SendQueue::setLoopRepeatCount(int count)
{
    if (count < 0) {
        return;
    }

    m_loopRepeatCount = count;
    if (m_loopEnabled) {
        m_loopRemaining = m_loopRepeatCount;
    }
    emitQueueChanged();
}

void SendQueue::enqueue(const QByteArray& payload,
                        const QString& displayText,
                        int repeatCount,
                        int intervalMs,
                        const QString& remoteHost,
                        quint16 remotePort,
                        const QString& peer)
{
    insert(m_items.size(), payload, displayText, repeatCount, intervalMs, remoteHost, remotePort, peer);
}

void SendQueue::insert(int index,
                       const QByteArray& payload,
                       const QString& displayText,
                       int repeatCount,
                       int intervalMs,
                       const QString& remoteHost,
                       quint16 remotePort,
                       const QString& peer)
{
    if (payload.isEmpty() || repeatCount <= 0 || intervalMs <= 0) {
        return;
    }

    QueueItem item;
    item.payload = payload;
    item.displayText = displayText;
    item.remoteHost = remoteHost;
    item.remotePort = remotePort;
    item.peer = peer;
    item.repeatCount = repeatCount;
    item.remaining = repeatCount;
    item.intervalMs = intervalMs;

    const int boundedIndex = qBound(0, index, m_items.size());
    m_items.insert(boundedIndex, item);
    emitQueueChanged();
}

void SendQueue::updateItem(int index, const QueueItem& item)
{
    if (index < 0 || index >= m_items.size()
        || item.payload.isEmpty()
        || item.repeatCount <= 0
        || item.remaining <= 0
        || item.intervalMs <= 0) {
        emitQueueChanged();
        return;
    }

    m_items[index] = item;
    emitQueueChanged();
}

void SendQueue::removeAt(int index)
{
    if (index < 0 || index >= m_items.size()) {
        return;
    }

    m_items.removeAt(index);
    if (m_items.isEmpty()) {
        stop();
    } else {
        emitQueueChanged();
    }
}

void SendQueue::clear()
{
    m_items.clear();
    stop();
}

void SendQueue::clearAndStop()
{
    m_timer->stop();
    m_running = false;
    m_loopRemaining = 0;
    m_items.clear();
    emitQueueChanged();
}

void SendQueue::start()
{
    if (m_running || m_items.isEmpty()) {
        emitQueueChanged();
        return;
    }

    m_running = true;
    if (m_loopEnabled) {
        m_loopRemaining = m_loopRepeatCount;
        m_backupItems = m_items;
    }
    emitQueueChanged();
    dispatchNext();
}

void SendQueue::stop()
{
    if (!m_running && !m_timer->isActive()) {
        emitQueueChanged();
        return;
    }

    m_timer->stop();
    m_running = false;
    emitQueueChanged();
}

void SendQueue::dispatchNext()
{
    if (m_items.isEmpty()) {
        stop();
        emit finished();
        return;
    }

    QueueItem& item = m_items.first();
    const int intervalMs = item.intervalMs;
    emit sendRequested(item.payload, item.displayText, item.remoteHost, item.remotePort, item.peer);

    if (!m_running) {
        emitQueueChanged();
        return;
    }

    --item.remaining;
    if (item.remaining <= 0) {
        m_items.removeFirst();
    }

    if (m_items.isEmpty()) {
        if (m_running && m_loopEnabled) {
            if (m_loopRepeatCount == 0 || m_loopRemaining > 0) {
                if (m_loopRepeatCount != 0) {
                    --m_loopRemaining;
                }
                m_items = m_backupItems;
                for (QueueItem& qi : m_items) {
                    qi.remaining = qi.repeatCount;
                }
                emitQueueChanged();
                if (m_running) {
                    m_timer->start(intervalMs);
                }
                return;
            }
        }
        m_running = false;
        m_loopRemaining = 0;
        m_backupItems.clear();
        m_timer->stop();
        emitQueueChanged();
        emit finished();
        return;
    }

    emitQueueChanged();
    if (m_running) {
        m_timer->start(intervalMs);
    }
}

void SendQueue::emitQueueChanged()
{
    emit queueChanged(pendingItemCount(), pendingSendCount(), m_running);
}

bool SendQueue::exportToJson(const QString& filePath) const
{
    QJsonArray itemsArray;
    for (const QueueItem& item : m_items) {
        QJsonObject obj;
        obj[QLatin1String("payload")] = QString::fromLatin1(item.payload.toBase64());
        obj[QLatin1String("displayText")] = item.displayText;
        obj[QLatin1String("remoteHost")] = item.remoteHost;
        obj[QLatin1String("remotePort")] = item.remotePort;
        obj[QLatin1String("peer")] = item.peer;
        obj[QLatin1String("repeatCount")] = item.repeatCount;
        obj[QLatin1String("intervalMs")] = item.intervalMs;
        itemsArray.append(obj);
    }

    QJsonObject root;
    root[QLatin1String("items")] = itemsArray;
    root[QLatin1String("loopEnabled")] = m_loopEnabled;
    root[QLatin1String("loopRepeatCount")] = m_loopRepeatCount;
    root[QLatin1String("defaultIntervalMs")] = m_defaultIntervalMs;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(QJsonDocument(root).toJson());
    return true;
}

bool SendQueue::importFromJson(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }

    const QJsonObject root = doc.object();
    const QJsonArray itemsArray = root[QLatin1String("items")].toArray();

    m_items.clear();
    for (const QJsonValue& val : itemsArray) {
        const QJsonObject obj = val.toObject();
        QueueItem item;
        item.payload = QByteArray::fromBase64(obj[QLatin1String("payload")].toString().toLatin1());
        item.displayText = obj[QLatin1String("displayText")].toString();
        item.remoteHost = obj[QLatin1String("remoteHost")].toString();
        item.remotePort = static_cast<quint16>(obj[QLatin1String("remotePort")].toInt());
        item.peer = obj[QLatin1String("peer")].toString();
        item.repeatCount = obj[QLatin1String("repeatCount")].toInt(1);
        item.remaining = item.repeatCount;
        item.intervalMs = obj[QLatin1String("intervalMs")].toInt(m_defaultIntervalMs);

        if (!item.payload.isEmpty() && item.repeatCount > 0 && item.intervalMs > 0) {
            m_items.append(item);
        }
    }

    m_loopEnabled = root[QLatin1String("loopEnabled")].toBool(false);
    m_loopRepeatCount = root[QLatin1String("loopRepeatCount")].toInt(0);
    m_defaultIntervalMs = root[QLatin1String("defaultIntervalMs")].toInt(1000);

    emitQueueChanged();
    return true;
}
