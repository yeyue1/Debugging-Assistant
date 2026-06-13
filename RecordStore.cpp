#include "RecordStore.h"

#include <QStringList>
#include <QRegularExpression>

QString RecordStore::directionText(SerialRecordDirection direction)
{
    switch (direction) {
    case SerialRecordDirection::Rx:     return QStringLiteral("接收");
    case SerialRecordDirection::Tx:     return QStringLiteral("发送");
    case SerialRecordDirection::System: return QStringLiteral("系统");
    }
    return QStringLiteral("系统");
}

QString RecordStore::bytesToHex(const QByteArray& data)
{
    return QString::fromLatin1(data.toHex(' ')).toUpper();
}

RecordStore::RecordStore(QObject* parent)
    : QObject(parent)
{
}

void RecordStore::addRecord(const SerialRecord& record)
{
    m_records.append(record);
    if (m_records.size() > 5000) {
        m_records.removeFirst();
    }

    if (record.direction == SerialRecordDirection::Rx) {
        ++m_rxRecordCount;
        ++m_frameCount;
    } else if (record.direction == SerialRecordDirection::Tx) {
        ++m_txRecordCount;
    }

    if (!record.error.isEmpty()) {
        ++m_errorCount;
    }

    // 总是发出 recordAdded（用于统计和解析视图）
    emit recordAdded(record);

    // 判断过滤
    const bool matched = matchesFilter(record);

    if (m_filterHide && !matched) {
        // 隐藏未匹配的记录，不发出 recordReady
        return;
    }

    emit recordReady(record, matched);
}

void RecordStore::clear()
{
    m_records.clear();
    m_rxRecordCount = 0;
    m_txRecordCount = 0;
    m_frameCount = 0;
    m_errorCount = 0;
    emit cleared();
}

QString RecordStore::recordSearchText(const SerialRecord& record)
{
    const QString compactHex = QString::fromLatin1(record.payload.toHex()).toUpper();
    return QStringList{
        directionText(record.direction),
        record.protocol,
        record.text,
        bytesToHex(record.payload),
        compactHex,
        record.info,
        record.error
    }.join(QStringLiteral(" "));
}

bool RecordStore::matchesFilter(const SerialRecord& record) const
{
    if (m_filterPattern.trimmed().isEmpty()) {
        return true;
    }
    return matchesPattern(m_filterPattern, recordSearchText(record), m_filterRegex);
}

bool RecordStore::shouldDisplayRecord(const SerialRecord& record) const
{
    return !m_filterHide || matchesFilter(record);
}

bool RecordStore::matchesPattern(const QString& pattern, const QString& text, bool regexMode) const
{
    const QString trimmed = pattern.trimmed();
    if (trimmed.isEmpty()) {
        return true;
    }

    if (!regexMode) {
        return text.contains(trimmed, Qt::CaseInsensitive);
    }

    const QRegularExpression regex(trimmed, QRegularExpression::CaseInsensitiveOption);
    if (!regex.isValid()) {
        return false;
    }
    return regex.match(text).hasMatch();
}
