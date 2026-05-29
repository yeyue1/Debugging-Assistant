#pragma once

#include <QObject>
#include <QList>

#include "SerialRecord.h"

class RecordStore : public QObject
{
    Q_OBJECT

public:
    explicit RecordStore(QObject* parent = nullptr);

    const QList<SerialRecord>& records() const { return m_records; }

    // 统计
    uint64_t rxRecordCount() const { return m_rxRecordCount; }
    uint64_t txRecordCount() const { return m_txRecordCount; }
    uint64_t frameCount() const { return m_frameCount; }
    uint64_t errorCount() const { return m_errorCount; }

    // 过滤设置
    void setFilterPattern(const QString& pattern) { m_filterPattern = pattern; }
    void setFilterRegex(bool enabled) { m_filterRegex = enabled; }
    void setFilterHide(bool enabled) { m_filterHide = enabled; }
    bool matchesFilter(const SerialRecord& record) const;
    bool shouldDisplayRecord(const SerialRecord& record) const;

    // 工具方法
    static QString recordSearchText(const SerialRecord& record);
    static QString directionText(SerialRecordDirection direction);
    static QString bytesToHex(const QByteArray& data);

signals:
    // 每条记录新增时发出（用于统计和解析视图更新）
    void recordAdded(const SerialRecord& record);
    // 每条记录新增时发出，附带是否匹配过滤条件（用于显示和过滤高亮）
    // 当 filterHide=true 且 matched=false 时不发出此信号
    void recordReady(const SerialRecord& record, bool matchedFilter);
    // 所有记录被清空
    void cleared();

public slots:
    void addRecord(const SerialRecord& record);
    void clear();

private:
    bool matchesPattern(const QString& pattern, const QString& text, bool regexMode) const;

    QList<SerialRecord> m_records;
    uint64_t m_rxRecordCount = 0;
    uint64_t m_txRecordCount = 0;
    uint64_t m_frameCount = 0;
    uint64_t m_errorCount = 0;

    QString m_filterPattern;
    bool m_filterRegex = false;
    bool m_filterHide = false;
};
