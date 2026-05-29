#include "SendQueue.h"

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

void SendQueue::start()
{
    if (m_running || m_items.isEmpty()) {
        emitQueueChanged();
        return;
    }

    m_running = true;
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
        if (m_loopEnabled) {
            item.remaining = item.repeatCount;
            const QueueItem completedItem = item;
            m_items.removeFirst();
            m_items.append(completedItem);
        } else {
            m_items.removeFirst();
        }
    }

    if (m_items.isEmpty()) {
        m_running = false;
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
