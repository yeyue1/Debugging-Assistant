// TestTemplateManager.cpp
// 测试模板管理器：验证序列化/反序列化、CSV转义、模板校验等功能

#include <QtTest>
#include <QJsonObject>
#include <QJsonArray>
#include "TemplateManager.h"
#include "SerialConfig.h"
#include "SerialRecord.h"

class TestTemplateManager : public QObject
{
    Q_OBJECT

private:
    TemplateManager* manager;

    // 辅助方法：创建一个测试用SerialConfig
    static SerialConfig createTestConfig()
    {
        SerialConfig config;
        config.portName = QStringLiteral("COM3");
        config.baudRate = 9600;
        config.dataBits = QSerialPort::Data7;
        config.parity = QSerialPort::EvenParity;
        config.stopBits = QSerialPort::TwoStop;
        config.flowControl = static_cast<QSerialPort::FlowControl>(1); // HardwareFlowControl
        return config;
    }

    // 辅助方法：创建一个测试用SerialRecord
    static SerialRecord createTestRecord()
    {
        SerialRecord record;
        record.timestamp = QDateTime(QDate(2025, 1, 15), QTime(10, 30, 45, 123));
        record.direction = SerialRecordDirection::Rx;
        record.payload = QByteArray::fromHex("AABBCC");
        record.text = QStringLiteral("Hello World");
        record.protocol = QStringLiteral("TestProtocol");
        record.info = QStringLiteral("CRC=0x1234");
        record.error = QString();
        return record;
    }

private slots:
    void init()
    {
        manager = new TemplateManager(this);
    }

    void cleanup()
    {
        delete manager;
        manager = nullptr;
    }

    // ── 测试模板键名常量 ──────────────────────────────────────────────────────

    void testTemplateKeyConstants()
    {
        // 测试目的：验证模板键名常量正确
        QCOMPARE(TemplateManager::templateVersionKey(), QStringLiteral("version"));
        QCOMPARE(TemplateManager::templateSerialKey(), QStringLiteral("serial"));
        QCOMPARE(TemplateManager::templateDtrKey(), QStringLiteral("dtr"));
        QCOMPARE(TemplateManager::templateRtsKey(), QStringLiteral("rts"));
        QCOMPARE(TemplateManager::templateProtocolIndexKey(), QStringLiteral("protocolIndex"));
        QCOMPARE(TemplateManager::templateNewLineModeKey(), QStringLiteral("newLineMode"));
        QCOMPARE(TemplateManager::templateEncodingKey(), QStringLiteral("encoding"));
        QCOMPARE(TemplateManager::templateAliasKey(), QStringLiteral("alias"));
    }

    // ── 测试SerialConfig序列化 ────────────────────────────────────────────────

    void testSerialConfigToJson()
    {
        // 测试目的：验证SerialConfig能正确序列化为JSON
        SerialConfig config = createTestConfig();
        QJsonObject json = TemplateManager::serialConfigToJson(config);

        QCOMPARE(json.value(QStringLiteral("portName")).toString(), QStringLiteral("COM3"));
        QCOMPARE(json.value(QStringLiteral("baudRate")).toInt(), 9600);
        QCOMPARE(json.value(QStringLiteral("dataBits")).toInt(), static_cast<int>(QSerialPort::Data7));
        QCOMPARE(json.value(QStringLiteral("parity")).toInt(), static_cast<int>(QSerialPort::EvenParity));
        QCOMPARE(json.value(QStringLiteral("stopBits")).toInt(), static_cast<int>(QSerialPort::TwoStop));
        QCOMPARE(json.value(QStringLiteral("flowControl")).toInt(), static_cast<int>(static_cast<QSerialPort::FlowControl>(1))); // HardwareFlowControl
    }

    void testSerialConfigFromJson()
    {
        // 测试目的：验证JSON能正确反序列化为SerialConfig
        QJsonObject json;
        json.insert(QStringLiteral("portName"), QStringLiteral("COM5"));
        json.insert(QStringLiteral("baudRate"), 115200);
        json.insert(QStringLiteral("dataBits"), static_cast<int>(QSerialPort::Data8));
        json.insert(QStringLiteral("parity"), static_cast<int>(QSerialPort::NoParity));
        json.insert(QStringLiteral("stopBits"), static_cast<int>(QSerialPort::OneStop));
        json.insert(QStringLiteral("flowControl"), static_cast<int>(QSerialPort::NoFlowControl));

        SerialConfig config = TemplateManager::serialConfigFromJson(json);

        QCOMPARE(config.portName, QStringLiteral("COM5"));
        QCOMPARE(config.baudRate, 115200);
        QCOMPARE(config.dataBits, QSerialPort::Data8);
        QCOMPARE(config.parity, QSerialPort::NoParity);
        QCOMPARE(config.stopBits, QSerialPort::OneStop);
        QCOMPARE(config.flowControl, QSerialPort::NoFlowControl);
    }

    void testSerialConfigRoundTrip()
    {
        // 测试目的：验证序列化→反序列化往返保持数据一致
        SerialConfig original = createTestConfig();
        QJsonObject json = TemplateManager::serialConfigToJson(original);
        SerialConfig restored = TemplateManager::serialConfigFromJson(json);

        QCOMPARE(restored.portName, original.portName);
        QCOMPARE(restored.baudRate, original.baudRate);
        QCOMPARE(restored.dataBits, original.dataBits);
        QCOMPARE(restored.parity, original.parity);
        QCOMPARE(restored.stopBits, original.stopBits);
        QCOMPARE(restored.flowControl, original.flowControl);
    }

    void testSerialConfigFromEmptyJson()
    {
        // 测试目的：验证空JSON对象返回默认配置
        QJsonObject emptyJson;
        SerialConfig config = TemplateManager::serialConfigFromJson(emptyJson);

        QVERIFY(config.portName.isEmpty());
        QCOMPARE(config.baudRate, static_cast<qint32>(QSerialPort::Baud115200));
        QCOMPARE(config.dataBits, QSerialPort::Data8);
        QCOMPARE(config.parity, QSerialPort::NoParity);
        QCOMPARE(config.stopBits, QSerialPort::OneStop);
        QCOMPARE(config.flowControl, QSerialPort::NoFlowControl);
    }

    // ── 测试记录序列化 ────────────────────────────────────────────────────────

    void testRecordToJson()
    {
        // 测试目的：验证单条记录能正确序列化为JSON
        SerialRecord record = createTestRecord();
        QJsonObject json = TemplateManager::recordToJson(record);

        // 验证时间字段（ISO格式）
        QVERIFY(json.value(QStringLiteral("时间")).toString().contains(QStringLiteral("2025-01-15")));
        // 验证方向字段
        QCOMPARE(json.value(QStringLiteral("方向")).toString(), QStringLiteral("接收"));
        // 验证协议字段
        QCOMPARE(json.value(QStringLiteral("协议")).toString(), QStringLiteral("TestProtocol"));
        // 验证文本字段
        QCOMPARE(json.value(QStringLiteral("文本")).toString(), QStringLiteral("Hello World"));
        // 验证十六进制字段
        QCOMPARE(json.value(QStringLiteral("十六进制")).toString(), QStringLiteral("aabbcc"));
        // 验证信息字段
        QCOMPARE(json.value(QStringLiteral("信息")).toString(), QStringLiteral("CRC=0x1234"));
        // 验证错误字段
        QVERIFY(json.value(QStringLiteral("错误")).toString().isEmpty());
    }

    void testRecordToJsonTxDirection()
    {
        // 测试目的：验证Tx方向的记录序列化正确
        SerialRecord record;
        record.timestamp = QDateTime::currentDateTime();
        record.direction = SerialRecordDirection::Tx;
        record.payload = QByteArray::fromHex("0102");
        record.text = QStringLiteral("Sent");

        QJsonObject json = TemplateManager::recordToJson(record);
        QCOMPARE(json.value(QStringLiteral("方向")).toString(), QStringLiteral("发送"));
    }

    void testRecordToJsonSystemDirection()
    {
        // 测试目的：验证System方向的记录序列化正确
        SerialRecord record;
        record.timestamp = QDateTime::currentDateTime();
        record.direction = SerialRecordDirection::System;
        record.payload = QByteArray();
        record.text = QStringLiteral("System message");

        QJsonObject json = TemplateManager::recordToJson(record);
        QCOMPARE(json.value(QStringLiteral("方向")).toString(), QStringLiteral("系统"));
    }

    void testRecordsToJson()
    {
        // 测试目的：验证多条记录序列化为JSON数组
        QList<SerialRecord> records;

        SerialRecord r1;
        r1.timestamp = QDateTime::currentDateTime();
        r1.direction = SerialRecordDirection::Rx;
        r1.payload = QByteArray::fromHex("AA");
        r1.text = QStringLiteral("Record 1");
        records.append(r1);

        SerialRecord r2;
        r2.timestamp = QDateTime::currentDateTime();
        r2.direction = SerialRecordDirection::Tx;
        r2.payload = QByteArray::fromHex("BB");
        r2.text = QStringLiteral("Record 2");
        records.append(r2);

        QJsonArray array = TemplateManager::recordsToJson(records);

        QCOMPARE(array.size(), 2);
        QCOMPARE(array.at(0).toObject().value(QStringLiteral("文本")).toString(), QStringLiteral("Record 1"));
        QCOMPARE(array.at(1).toObject().value(QStringLiteral("文本")).toString(), QStringLiteral("Record 2"));
    }

    void testRecordsToJsonEmpty()
    {
        // 测试目的：验证空记录列表返回空数组
        QList<SerialRecord> empty;
        QJsonArray array = TemplateManager::recordsToJson(empty);
        QCOMPARE(array.size(), 0);
    }

    // ── 测试CSV转义 ──────────────────────────────────────────────────────────

    void testCsvEscapeQuotes()
    {
        // 测试目的：验证双引号被正确转义
        QString result = TemplateManager::csvEscape(QStringLiteral("He said \"hello\""));
        QCOMPARE(result, QStringLiteral("\"He said \"\"hello\"\"\""));
    }

    void testCsvEscapeNewlines()
    {
        // 测试目的：验证换行符被正确转义
        QString result = TemplateManager::csvEscape(QStringLiteral("line1\r\nline2"));
        QCOMPARE(result, QStringLiteral("\"line1\\r\\nline2\""));
    }

    void testCsvEscapeCarriageReturn()
    {
        // 测试目的：验证回车符被正确转义
        QString result = TemplateManager::csvEscape(QStringLiteral("text\rtext"));
        QCOMPARE(result, QStringLiteral("\"text\\rtext\""));
    }

    void testCsvEscapeLineFeed()
    {
        // 测试目的：验证换行符被正确转义
        QString result = TemplateManager::csvEscape(QStringLiteral("text\ntext"));
        QCOMPARE(result, QStringLiteral("\"text\\ntext\""));
    }

    void testCsvEscapePlainText()
    {
        // 测试目的：验证普通文本只被双引号包裹
        QString result = TemplateManager::csvEscape(QStringLiteral("Hello"));
        QCOMPARE(result, QStringLiteral("\"Hello\""));
    }

    void testCsvEscapeEmpty()
    {
        // 测试目的：验证空字符串返回双引号对
        QString result = TemplateManager::csvEscape(QString());
        QCOMPARE(result, QStringLiteral("\"\""));
    }

    // ── 测试模板校验 ──────────────────────────────────────────────────────────

    void testIsValidTemplate()
    {
        // 测试目的：验证包含serial对象的模板是有效的
        QJsonObject serialObj;
        serialObj.insert(QStringLiteral("portName"), QStringLiteral("COM3"));

        QJsonObject templ;
        templ.insert(QStringLiteral("serial"), serialObj);

        QVERIFY(TemplateManager::isValidTemplate(templ));
    }

    void testIsValidTemplateNoSerial()
    {
        // 测试目的：验证缺少serial字段的模板无效
        QJsonObject templ;
        templ.insert(QStringLiteral("version"), 1);

        QVERIFY(!TemplateManager::isValidTemplate(templ));
    }

    void testIsValidTemplateSerialNotObject()
    {
        // 测试目的：验证serial字段不是对象时模板无效
        QJsonObject templ;
        templ.insert(QStringLiteral("serial"), QStringLiteral("not an object"));

        QVERIFY(!TemplateManager::isValidTemplate(templ));
    }

    void testIsValidTemplateEmpty()
    {
        // 测试目的：验证空JSON对象无效
        QJsonObject empty;
        QVERIFY(!TemplateManager::isValidTemplate(empty));
    }

    // ── 测试CSV行生成 ────────────────────────────────────────────────────────

    void testRecordToCsvLine()
    {
        // 测试目的：验证记录能正确转换为CSV行
        SerialRecord record = createTestRecord();
        QString csvLine = TemplateManager::recordToCsvLine(record);

        // CSV行应以换行符结尾
        QVERIFY(csvLine.endsWith(QLatin1Char('\n')));
        // 应包含时间戳
        QVERIFY(csvLine.contains(QStringLiteral("2025-01-15")));
        // 应包含方向
        QVERIFY(csvLine.contains(QStringLiteral("接收")));
        // 应包含协议
        QVERIFY(csvLine.contains(QStringLiteral("TestProtocol")));
        // 应包含文本
        QVERIFY(csvLine.contains(QStringLiteral("Hello World")));
    }
};

QTEST_MAIN(TestTemplateManager)
#include "TestTemplateManager.moc"
