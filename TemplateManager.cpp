#include "TemplateManager.h"

#include "RecordStore.h"
#include "SerialRecord.h"

TemplateManager::TemplateManager(QObject* parent)
    : QObject(parent)
{
}

QJsonObject TemplateManager::serialConfigToJson(const SerialConfig& config)
{
    QJsonObject object;
    object.insert(QStringLiteral("portName"), config.portName);
    object.insert(QStringLiteral("baudRate"), config.baudRate);
    object.insert(QStringLiteral("dataBits"), static_cast<int>(config.dataBits));
    object.insert(QStringLiteral("parity"), static_cast<int>(config.parity));
    object.insert(QStringLiteral("stopBits"), static_cast<int>(config.stopBits));
    object.insert(QStringLiteral("flowControl"), static_cast<int>(config.flowControl));
    return object;
}

SerialConfig TemplateManager::serialConfigFromJson(const QJsonObject& object)
{
    SerialConfig config;
    config.portName = object.value(QStringLiteral("portName")).toString();
    config.baudRate = object.value(QStringLiteral("baudRate")).toInt(QSerialPort::Baud115200);
    config.dataBits = static_cast<QSerialPort::DataBits>(
        object.value(QStringLiteral("dataBits")).toInt(QSerialPort::Data8));
    config.parity = static_cast<QSerialPort::Parity>(
        object.value(QStringLiteral("parity")).toInt(QSerialPort::NoParity));
    config.stopBits = static_cast<QSerialPort::StopBits>(
        object.value(QStringLiteral("stopBits")).toInt(QSerialPort::OneStop));
    config.flowControl = static_cast<QSerialPort::FlowControl>(
        object.value(QStringLiteral("flowControl")).toInt(QSerialPort::NoFlowControl));
    return config;
}

QJsonObject TemplateManager::recordToJson(const SerialRecord& record)
{
    QJsonObject object;
    object.insert(QStringLiteral("时间"), record.timestamp.toString(Qt::ISODateWithMs));
    object.insert(QStringLiteral("方向"), RecordStore::directionText(record.direction));
    object.insert(QStringLiteral("协议"), record.protocol);
    object.insert(QStringLiteral("文本"), record.text);
    object.insert(QStringLiteral("十六进制"), QString::fromLatin1(record.payload.toHex()));
    object.insert(QStringLiteral("信息"), record.info);
    object.insert(QStringLiteral("错误"), record.error);
    return object;
}

QJsonArray TemplateManager::recordsToJson(const QList<SerialRecord>& records)
{
    QJsonArray array;
    for (const auto& record : records) {
        array.append(recordToJson(record));
    }
    return array;
}

QString TemplateManager::csvEscape(const QString& value)
{
    QString escaped = value;
    escaped.replace(QStringLiteral("\""), QStringLiteral("\"\""));
    escaped.replace(QStringLiteral("\r"), QStringLiteral("\\r"));
    escaped.replace(QStringLiteral("\n"), QStringLiteral("\\n"));
    return QStringLiteral("\"%1\"").arg(escaped);
}

QString TemplateManager::recordToCsvLine(const SerialRecord& record)
{
    return csvEscape(record.timestamp.toString(Qt::ISODateWithMs)) + ',' +
           csvEscape(RecordStore::directionText(record.direction)) + ',' +
           csvEscape(record.protocol) + ',' +
           csvEscape(record.text) + ',' +
           csvEscape(RecordStore::bytesToHex(record.payload)) + ',' +
           csvEscape(record.info) + ',' +
           csvEscape(record.error) + '\n';
}

bool TemplateManager::isValidTemplate(const QJsonObject& object)
{
    return object.contains(QStringLiteral("serial")) &&
           object.value(QStringLiteral("serial")).isObject();
}
