#pragma once

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>

#include "SerialConfig.h"
#include "SerialRecord.h"

class TemplateManager : public QObject
{
    Q_OBJECT

public:
    explicit TemplateManager(QObject* parent = nullptr);

    // 序列化 / 反序列化
    static QJsonObject serialConfigToJson(const SerialConfig& config);
    static SerialConfig serialConfigFromJson(const QJsonObject& object);

    // 完整配置模板（含 DTR/RTS/协议/换行/编码/别名）
    static QString templateVersionKey() { return QStringLiteral("version"); }
    static QString templateSerialKey() { return QStringLiteral("serial"); }
    static QString templateDtrKey() { return QStringLiteral("dtr"); }
    static QString templateRtsKey() { return QStringLiteral("rts"); }
    static QString templateProtocolIndexKey() { return QStringLiteral("protocolIndex"); }
    static QString templateNewLineModeKey() { return QStringLiteral("newLineMode"); }
    static QString templateEncodingKey() { return QStringLiteral("encoding"); }
    static QString templateAliasKey() { return QStringLiteral("alias"); }

    // 记录导出为 JSON
    static QJsonObject recordToJson(const SerialRecord& record);
    static QJsonArray recordsToJson(const QList<SerialRecord>& records);

    // CSV 转义
    static QString csvEscape(const QString& value);
    static QString recordToCsvLine(const SerialRecord& record);

    // 校验模板格式
    static bool isValidTemplate(const QJsonObject& object);
};
