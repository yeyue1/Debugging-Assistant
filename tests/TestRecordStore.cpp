// TestRecordStore.cpp
// 测试记录存储：验证记录添加、过滤规则、隐藏模式、统计功能

#include <QtTest>
#include <QSignalSpy>
#include "RecordStore.h"

class TestRecordStore : public QObject
{
    Q_OBJECT

private:
    RecordStore* store;

    // 辅助方法：创建一个Rx记录
    static SerialRecord createRxRecord(const QByteArray& payload, const QString& text = QString())
    {
        SerialRecord record;
        record.timestamp = QDateTime::currentDateTime();
        record.direction = SerialRecordDirection::Rx;
        record.payload = payload;
        record.text = text.isEmpty() ? QString::fromLatin1(payload.toHex()) : text;
        record.protocol = "Test";
        return record;
    }

    // 辅助方法：创建一个Tx记录
    static SerialRecord createTxRecord(const QByteArray& payload, const QString& text = QString())
    {
        SerialRecord record;
        record.timestamp = QDateTime::currentDateTime();
        record.direction = SerialRecordDirection::Tx;
        record.payload = payload;
        record.text = text.isEmpty() ? QString::fromLatin1(payload.toHex()) : text;
        record.protocol = "Test";
        return record;
    }

    // 辅助方法：创建一个带错误的记录
    static SerialRecord createErrorRecord(const QByteArray& payload, const QString& error)
    {
        SerialRecord record;
        record.timestamp = QDateTime::currentDateTime();
        record.direction = SerialRecordDirection::Rx;
        record.payload = payload;
        record.text = QString::fromLatin1(payload.toHex());
        record.error = error;
        return record;
    }

private slots:
    void init()
    {
        store = new RecordStore(this);
    }

    void cleanup()
    {
        delete store;
        store = nullptr;
    }

    // ── 测试添加记录 ──────────────────────────────────────────────────────────

    void testAddRecord()
    {
        // 测试目的：验证记录能被正确添加到存储中
        QSignalSpy recordAddedSpy(store, &RecordStore::recordAdded);
        QSignalSpy recordReadySpy(store, &RecordStore::recordReady);

        SerialRecord record = createRxRecord(QByteArray::fromHex("AABB"), "Test Data");
        store->addRecord(record);

        QCOMPARE(store->records().size(), 1);
        QCOMPARE(store->records().first().payload, QByteArray::fromHex("AABB"));
        QCOMPARE(store->records().first().text, "Test Data");
        QCOMPARE(recordAddedSpy.count(), 1);
        QCOMPARE(recordReadySpy.count(), 1);
    }

    void testAddMultipleRecords()
    {
        // 测试目的：验证多条记录能被正确添加
        store->addRecord(createRxRecord(QByteArray::fromHex("01"), "Record 1"));
        store->addRecord(createTxRecord(QByteArray::fromHex("02"), "Record 2"));
        store->addRecord(createRxRecord(QByteArray::fromHex("03"), "Record 3"));

        QCOMPARE(store->records().size(), 3);
    }

    void testMaxRecordsLimit()
    {
        // 测试目的：验证记录数量超过5000时旧记录被移除
        for (int i = 0; i < 5005; ++i) {
            store->addRecord(createRxRecord(QByteArray::fromHex("01"), QString("Record %1").arg(i)));
        }

        QCOMPARE(store->records().size(), 5000);
        // 验证最旧的记录被移除了
        QVERIFY(store->records().first().text != "Record 0");
    }

    // ── 测试过滤规则（文本） ──────────────────────────────────────────────────

    void testTextFilterMatch()
    {
        // 测试目的：验证文本过滤能正确匹配包含关键字的记录
        store->setFilterPattern("Hello");
        store->setFilterRegex(false);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Hello World");
        QVERIFY(store->matchesFilter(record));
    }

    void testTextFilterNoMatch()
    {
        // 测试目的：验证文本过滤能正确排除不包含关键字的记录
        store->setFilterPattern("Hello");
        store->setFilterRegex(false);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Goodbye World");
        QVERIFY(!store->matchesFilter(record));
    }

    void testTextFilterCaseInsensitive()
    {
        // 测试目的：验证文本过滤不区分大小写
        store->setFilterPattern("hello");
        store->setFilterRegex(false);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "HELLO World");
        QVERIFY(store->matchesFilter(record));
    }

    void testEmptyFilterMatchesAll()
    {
        // 测试目的：验证空过滤模式匹配所有记录
        store->setFilterPattern("");

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Any Text");
        QVERIFY(store->matchesFilter(record));
    }

    // ── 测试过滤规则（正则） ──────────────────────────────────────────────────

    void testRegexFilterMatch()
    {
        // 测试目的：验证正则过滤能正确匹配
        store->setFilterPattern("\\d+\\.\\d+");
        store->setFilterRegex(true);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Value: 3.14");
        QVERIFY(store->matchesFilter(record));
    }

    void testRegexFilterNoMatch()
    {
        // 测试目的：验证正则过滤能正确排除不匹配的记录
        store->setFilterPattern("^ERROR");
        store->setFilterRegex(true);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "INFO: All good");
        QVERIFY(!store->matchesFilter(record));
    }

    void testInvalidRegex()
    {
        // 测试目的：验证无效正则表达式不会匹配任何记录
        store->setFilterPattern("[invalid");
        store->setFilterRegex(true);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Any Text");
        QVERIFY(!store->matchesFilter(record));
    }

    void testFilterMatchesHexPayload()
    {
        // 测试目的：验证过滤能匹配十六进制负载内容
        store->setFilterPattern("AABB");
        store->setFilterRegex(false);

        SerialRecord record = createRxRecord(QByteArray::fromHex("AABBCCDD"), "Data");
        QVERIFY(store->matchesFilter(record));
    }

    // ── 测试隐藏模式 ──────────────────────────────────────────────────────────

    void testHideModeFiltersUnmatched()
    {
        // 测试目的：验证隐藏模式下不匹配的记录不发出recordReady信号
        QSignalSpy recordReadySpy(store, &RecordStore::recordReady);
        QSignalSpy recordAddedSpy(store, &RecordStore::recordAdded);

        store->setFilterPattern("Hello");
        store->setFilterHide(true);

        // 添加不匹配的记录
        SerialRecord unmatched = createRxRecord(QByteArray::fromHex("01"), "Goodbye");
        store->addRecord(unmatched);

        QCOMPARE(recordAddedSpy.count(), 1);  // recordAdded 总是发出
        QCOMPARE(recordReadySpy.count(), 0);  // recordReady 不应发出
    }

    void testHideModeShowsMatched()
    {
        // 测试目的：验证隐藏模式下匹配的记录正常发出recordReady信号
        QSignalSpy recordReadySpy(store, &RecordStore::recordReady);

        store->setFilterPattern("Hello");
        store->setFilterHide(true);

        SerialRecord matched = createRxRecord(QByteArray::fromHex("01"), "Hello World");
        store->addRecord(matched);

        QCOMPARE(recordReadySpy.count(), 1);
        QCOMPARE(recordReadySpy.at(0).at(1).toBool(), true);  // matchedFilter = true
    }

    void testNonHideModeShowsAll()
    {
        // 测试目的：验证非隐藏模式下所有记录都发出recordReady信号
        QSignalSpy recordReadySpy(store, &RecordStore::recordReady);

        store->setFilterPattern("Hello");
        store->setFilterHide(false);

        store->addRecord(createRxRecord(QByteArray::fromHex("01"), "Hello"));
        store->addRecord(createRxRecord(QByteArray::fromHex("02"), "Goodbye"));

        QCOMPARE(recordReadySpy.count(), 2);
        // 第一条匹配
        QCOMPARE(recordReadySpy.at(0).at(1).toBool(), true);
        // 第二条不匹配
        QCOMPARE(recordReadySpy.at(1).at(1).toBool(), false);
    }

    // ── 测试记录统计 ──────────────────────────────────────────────────────────

    void testRxRecordCount()
    {
        // 测试目的：验证接收记录计数正确
        store->addRecord(createRxRecord(QByteArray::fromHex("01")));
        store->addRecord(createRxRecord(QByteArray::fromHex("02")));
        store->addRecord(createTxRecord(QByteArray::fromHex("03")));

        QCOMPARE(store->rxRecordCount(), 2ULL);
    }

    void testTxRecordCount()
    {
        // 测试目的：验证发送记录计数正确
        store->addRecord(createTxRecord(QByteArray::fromHex("01")));
        store->addRecord(createTxRecord(QByteArray::fromHex("02")));
        store->addRecord(createRxRecord(QByteArray::fromHex("03")));

        QCOMPARE(store->txRecordCount(), 2ULL);
    }

    void testFrameCount()
    {
        // 测试目的：验证帧计数只计算Rx记录
        store->addRecord(createRxRecord(QByteArray::fromHex("01")));
        store->addRecord(createTxRecord(QByteArray::fromHex("02")));
        store->addRecord(createRxRecord(QByteArray::fromHex("03")));

        QCOMPARE(store->frameCount(), 2ULL);
    }

    void testErrorCount()
    {
        // 测试目的：验证错误计数正确
        store->addRecord(createRxRecord(QByteArray::fromHex("01")));
        store->addRecord(createErrorRecord(QByteArray::fromHex("02"), "CRC Error"));
        store->addRecord(createErrorRecord(QByteArray::fromHex("03"), "Timeout"));

        QCOMPARE(store->errorCount(), 2ULL);
    }

    void testClearResetsStatistics()
    {
        // 测试目的：验证清空操作重置所有统计
        store->addRecord(createRxRecord(QByteArray::fromHex("01")));
        store->addRecord(createTxRecord(QByteArray::fromHex("02")));
        store->addRecord(createErrorRecord(QByteArray::fromHex("03"), "Error"));

        store->clear();

        QCOMPARE(store->records().size(), 0);
        QCOMPARE(store->rxRecordCount(), 0ULL);
        QCOMPARE(store->txRecordCount(), 0ULL);
        QCOMPARE(store->frameCount(), 0ULL);
        QCOMPARE(store->errorCount(), 0ULL);
    }

    // ── 测试工具方法 ──────────────────────────────────────────────────────────

    void testDirectionText()
    {
        // 测试目的：验证方向文本转换正确
        QCOMPARE(RecordStore::directionText(SerialRecordDirection::Rx), QStringLiteral("接收"));
        QCOMPARE(RecordStore::directionText(SerialRecordDirection::Tx), QStringLiteral("发送"));
        QCOMPARE(RecordStore::directionText(SerialRecordDirection::System), QStringLiteral("系统"));
    }

    void testBytesToHex()
    {
        // 测试目的：验证字节转十六进制字符串正确
        QCOMPARE(RecordStore::bytesToHex(QByteArray::fromHex("AABBCC")), QString("AA BB CC"));
        QCOMPARE(RecordStore::bytesToHex(QByteArray::fromHex("00FF")), QString("00 FF"));
        QCOMPARE(RecordStore::bytesToHex(QByteArray()), QString(""));
    }

    void testRecordSearchText()
    {
        // 测试目的：验证记录搜索文本包含所有关键信息
        SerialRecord record;
        record.direction = SerialRecordDirection::Rx;
        record.protocol = "TestProtocol";
        record.text = "Hello";
        record.payload = QByteArray::fromHex("AABB");
        record.info = "CRC=0x1234";
        record.error = "";

        QString searchText = RecordStore::recordSearchText(record);
        QVERIFY(searchText.contains("接收"));
        QVERIFY(searchText.contains("TestProtocol"));
        QVERIFY(searchText.contains("Hello"));
        QVERIFY(searchText.contains("AA BB"));
        QVERIFY(searchText.contains("CRC=0x1234"));
    }

    void testShouldDisplayRecord()
    {
        // 测试目的：验证shouldDisplayRecord在不同模式下的行为
        SerialRecord matched = createRxRecord(QByteArray::fromHex("01"), "Match");
        SerialRecord unmatched = createRxRecord(QByteArray::fromHex("02"), "NoMatch");

        // 无过滤时全部显示
        store->setFilterPattern("");
        QVERIFY(store->shouldDisplayRecord(matched));
        QVERIFY(store->shouldDisplayRecord(unmatched));

        // 隐藏模式 + 过滤
        store->setFilterPattern("Match");
        store->setFilterHide(true);
        QVERIFY(store->shouldDisplayRecord(matched));
        QVERIFY(!store->shouldDisplayRecord(unmatched));
    }

    // ── 测试信号 ──────────────────────────────────────────────────────────────

    void testClearedSignal()
    {
        // 测试目的：验证清空时发出cleared信号
        QSignalSpy clearedSpy(store, &RecordStore::cleared);

        store->addRecord(createRxRecord(QByteArray::fromHex("01")));
        store->clear();

        QCOMPARE(clearedSpy.count(), 1);
    }
};

QTEST_MAIN(TestRecordStore)
#include "TestRecordStore.moc"
