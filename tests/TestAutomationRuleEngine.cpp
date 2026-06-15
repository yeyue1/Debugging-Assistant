// TestAutomationRuleEngine.cpp
// 测试自动回复规则引擎：验证触发规则、冷却时间、最大触发次数、只触发一次、全局速率限制等功能

#include <QtTest>
#include <QThread>
#include "AutomationRuleEngine.h"

class TestAutomationRuleEngine : public QObject
{
    Q_OBJECT

private:
    AutomationRuleEngine* engine;

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

private slots:
    void init()
    {
        engine = new AutomationRuleEngine();
        // 每个测试前重置全局状态
        AutomationRuleEngine::resetAllRules();
        AutomationRuleEngine::setGlobalMaxPerMinute(20);
    }

    void cleanup()
    {
        delete engine;
        engine = nullptr;
    }

    // ── 测试基本触发规则 ──────────────────────────────────────────────────────

    void testBasicTextTrigger()
    {
        // 测试目的：验证基本文本匹配能触发自动回复
        engine->setEnabled(true);
        engine->setPattern("Hello");
        engine->setUseRegex(false);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Hello World");
        QVERIFY(engine->shouldAutoReply(record));
    }

    void testTextNoMatch()
    {
        // 测试目的：验证不匹配的文本不会触发自动回复
        engine->setEnabled(true);
        engine->setPattern("Hello");
        engine->setUseRegex(false);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Goodbye World");
        QVERIFY(!engine->shouldAutoReply(record));
    }

    void testRegexTrigger()
    {
        // 测试目的：验证正则表达式匹配能触发自动回复
        engine->setEnabled(true);
        engine->setPattern("\\d+\\.\\d+");
        engine->setUseRegex(true);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Value: 3.14");
        QVERIFY(engine->shouldAutoReply(record));
    }

    void testDisabledEngine()
    {
        // 测试目的：验证禁用状态下不触发自动回复
        engine->setEnabled(false);
        engine->setPattern("Hello");

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Hello World");
        QVERIFY(!engine->shouldAutoReply(record));
    }

    void testOnlyTriggersOnRx()
    {
        // 测试目的：验证只有Rx记录才能触发自动回复
        engine->setEnabled(true);
        engine->setPattern("Data");

        SerialRecord txRecord = createTxRecord(QByteArray::fromHex("01"), "Data Sent");
        QVERIFY(!engine->shouldAutoReply(txRecord));

        SerialRecord rxRecord = createRxRecord(QByteArray::fromHex("01"), "Data Received");
        QVERIFY(engine->shouldAutoReply(rxRecord));
    }

    void testEmptyPattern()
    {
        // 测试目的：验证空模式不会触发自动回复
        engine->setEnabled(true);
        engine->setPattern("");

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Any Text");
        QVERIFY(!engine->shouldAutoReply(record));
    }

    void testCaseInsensitiveMatch()
    {
        // 测试目的：验证匹配不区分大小写
        engine->setEnabled(true);
        engine->setPattern("hello");

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "HELLO World");
        QVERIFY(engine->shouldAutoReply(record));
    }

    void testInvalidRegex()
    {
        // 测试目的：验证无效正则表达式不会触发自动回复
        engine->setEnabled(true);
        engine->setPattern("[invalid");
        engine->setUseRegex(true);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Any Text");
        QVERIFY(!engine->shouldAutoReply(record));
    }

    // ── 测试冷却时间 ──────────────────────────────────────────────────────────

    void testCooldownPreventsImmediateRetrigger()
    {
        // 测试目的：验证冷却时间内不能重复触发
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setCooldownMs(1000);  // 1秒冷却

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 第一次触发成功
        QVERIFY(engine->shouldAutoReply(record));

        // 立即再次触发应失败（在冷却时间内）
        QVERIFY(!engine->shouldAutoReply(record));
    }

    void testCooldownExpiredAllowsRetrigger()
    {
        // 测试目的：验证冷却时间过后可以重新触发
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setCooldownMs(100);  // 100毫秒冷却

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 第一次触发成功
        QVERIFY(engine->shouldAutoReply(record));

        // 等待冷却时间过期
        QThread::msleep(150);

        // 冷却时间过后应能再次触发
        QVERIFY(engine->shouldAutoReply(record));
    }

    void testZeroCooldownAllowsImmediateRetrigger()
    {
        // 测试目的：验证冷却时间为0时允许立即重复触发
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setCooldownMs(0);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        QVERIFY(engine->shouldAutoReply(record));
        QVERIFY(engine->shouldAutoReply(record));
    }

    // ── 测试最大触发次数 ──────────────────────────────────────────────────────

    void testMaxTriggerCount()
    {
        // 测试目的：验证达到最大触发次数后不再触发
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setMaxTriggerCount(3);
        engine->setCooldownMs(0);  // 无冷却时间

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 前3次应该成功
        QVERIFY(engine->shouldAutoReply(record));
        QVERIFY(engine->shouldAutoReply(record));
        QVERIFY(engine->shouldAutoReply(record));

        // 第4次应该失败
        QVERIFY(!engine->shouldAutoReply(record));
    }

    void testMaxTriggerCountZeroUnlimited()
    {
        // 测试目的：验证最大触发次数为0时表示不限制
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setMaxTriggerCount(0);  // 不限制
        engine->setCooldownMs(0);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 应该能触发多次
        for (int i = 0; i < 10; ++i) {
            QVERIFY(engine->shouldAutoReply(record));
        }
    }

    void testResetTriggerStateResetsCount()
    {
        // 测试目的：验证重置触发状态后计数器归零
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setMaxTriggerCount(2);
        engine->setCooldownMs(0);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 触发2次
        engine->shouldAutoReply(record);
        engine->shouldAutoReply(record);

        // 第3次应该失败
        QVERIFY(!engine->shouldAutoReply(record));

        // 重置状态
        engine->resetTriggerState();

        // 重置后应该能再次触发
        QVERIFY(engine->shouldAutoReply(record));
    }

    // ── 测试只触发一次 ────────────────────────────────────────────────────────

    void testOnceOnly()
    {
        // 测试目的：验证onceOnly模式下只能触发一次
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setOnceOnly(true);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 第一次触发成功
        QVERIFY(engine->shouldAutoReply(record));

        // 第二次应该失败
        QVERIFY(!engine->shouldAutoReply(record));
    }

    void testOnceOnlyWithReset()
    {
        // 测试目的：验证onceOnly模式下重置后可以再次触发
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setOnceOnly(true);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 触发一次
        QVERIFY(engine->shouldAutoReply(record));
        QVERIFY(!engine->shouldAutoReply(record));

        // 重置后应能再次触发
        engine->resetTriggerState();
        QVERIFY(engine->shouldAutoReply(record));
        QVERIFY(!engine->shouldAutoReply(record));
    }

    // ── 测试组合场景 ──────────────────────────────────────────────────────────

    void testCooldownAndMaxCountCombined()
    {
        // 测试目的：验证冷却时间和最大触发次数同时生效
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setCooldownMs(100);
        engine->setMaxTriggerCount(2);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 第1次触发
        QVERIFY(engine->shouldAutoReply(record));

        // 冷却时间内不能触发
        QVERIFY(!engine->shouldAutoReply(record));

        // 等待冷却时间
        QThread::msleep(150);

        // 第2次触发
        QVERIFY(engine->shouldAutoReply(record));

        // 冷却时间内不能触发
        QVERIFY(!engine->shouldAutoReply(record));

        // 等待冷却时间
        QThread::msleep(150);

        // 达到最大次数，不能触发
        QVERIFY(!engine->shouldAutoReply(record));
    }

    void testMatchInHexPayload()
    {
        // 测试目的：验证能匹配十六进制负载内容
        engine->setEnabled(true);
        engine->setPattern("AABB");

        SerialRecord record = createRxRecord(QByteArray::fromHex("AABBCCDD"), "Some Data");
        QVERIFY(engine->shouldAutoReply(record));
    }

    // ── 测试默认值 ────────────────────────────────────────────────────────────

    void testDefaultMaxTriggerCountIsTen()
    {
        // 测试目的：验证默认最大触发次数为10
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setCooldownMs(0);
        // 不设置 maxTriggerCount，使用默认值

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 应该能触发10次
        for (int i = 0; i < 10; ++i) {
            QVERIFY(engine->shouldAutoReply(record));
        }

        // 第11次应该失败
        QVERIFY(!engine->shouldAutoReply(record));
    }

    void testDefaultCooldownIs500ms()
    {
        // 测试目的：验证默认冷却时间为500毫秒
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setMaxTriggerCount(0);  // 不限制触发次数
        // 不设置 cooldownMs，使用默认值500ms

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 第一次触发成功
        QVERIFY(engine->shouldAutoReply(record));

        // 立即再次触发应该失败（默认500ms冷却）
        QVERIFY(!engine->shouldAutoReply(record));

        // 等待超过默认冷却时间
        QThread::msleep(600);

        // 冷却过后应该能再次触发
        QVERIFY(engine->shouldAutoReply(record));
    }

    // ── 测试全局速率限制 ──────────────────────────────────────────────────────

    void testGlobalRateLimit()
    {
        // 测试目的：验证全局速率限制（每分钟最多N次触发）
        AutomationRuleEngine::setGlobalMaxPerMinute(5);

        AutomationRuleEngine engine1;
        engine1.setEnabled(true);
        engine1.setPattern("Hello");
        engine1.setCooldownMs(0);
        engine1.setMaxTriggerCount(0);  // 不限制单规则触发次数

        AutomationRuleEngine engine2;
        engine2.setEnabled(true);
        engine2.setPattern("World");
        engine2.setCooldownMs(0);
        engine2.setMaxTriggerCount(0);

        SerialRecord record1 = createRxRecord(QByteArray::fromHex("01"), "Hello");
        SerialRecord record2 = createRxRecord(QByteArray::fromHex("02"), "World");

        // 两个引擎交替触发，总计5次应该成功
        QVERIFY(engine1.shouldAutoReply(record1));
        QVERIFY(engine2.shouldAutoReply(record2));
        QVERIFY(engine1.shouldAutoReply(record1));
        QVERIFY(engine2.shouldAutoReply(record2));
        QVERIFY(engine1.shouldAutoReply(record1));

        // 第6次应该被全局速率限制阻止
        QVERIFY(!engine2.shouldAutoReply(record2));
    }

    void testResetAllRulesClearsGlobalRate()
    {
        // 测试目的：验证 resetAllRules() 清除全局速率限制
        AutomationRuleEngine::setGlobalMaxPerMinute(2);

        AutomationRuleEngine engine1;
        engine1.setEnabled(true);
        engine1.setPattern("Hello");
        engine1.setCooldownMs(0);
        engine1.setMaxTriggerCount(0);

        AutomationRuleEngine engine2;
        engine2.setEnabled(true);
        engine2.setPattern("World");
        engine2.setCooldownMs(0);
        engine2.setMaxTriggerCount(0);

        SerialRecord record1 = createRxRecord(QByteArray::fromHex("01"), "Hello");
        SerialRecord record2 = createRxRecord(QByteArray::fromHex("02"), "World");

        // 触发2次，达到全局限制
        QVERIFY(engine1.shouldAutoReply(record1));
        QVERIFY(engine2.shouldAutoReply(record2));
        QVERIFY(!engine1.shouldAutoReply(record1));

        // 重置所有规则
        AutomationRuleEngine::resetAllRules();

        // 重置后应该能再次触发
        QVERIFY(engine1.shouldAutoReply(record1));
        QVERIFY(engine2.shouldAutoReply(record2));
    }

    void testGlobalRateLimitWithDifferentCooldowns()
    {
        // 测试目的：验证全局速率限制独立于单规则冷却时间
        AutomationRuleEngine::setGlobalMaxPerMinute(3);

        AutomationRuleEngine engine1;
        engine1.setEnabled(true);
        engine1.setPattern("A");
        engine1.setCooldownMs(0);
        engine1.setMaxTriggerCount(0);

        AutomationRuleEngine engine2;
        engine2.setEnabled(true);
        engine2.setPattern("B");
        engine2.setCooldownMs(1000);  // 长冷却时间
        engine2.setMaxTriggerCount(0);

        SerialRecord record1 = createRxRecord(QByteArray::fromHex("01"), "A");
        SerialRecord record2 = createRxRecord(QByteArray::fromHex("02"), "B");

        // engine1 触发3次（无冷却），应该全部成功
        QVERIFY(engine1.shouldAutoReply(record1));
        QVERIFY(engine1.shouldAutoReply(record1));
        QVERIFY(engine1.shouldAutoReply(record1));

        // 第4次应该被全局速率限制阻止
        QVERIFY(!engine1.shouldAutoReply(record1));

        // engine2 因为全局限制也无法触发
        QVERIFY(!engine2.shouldAutoReply(record2));
    }

    // ── 测试信号发射 ──────────────────────────────────────────────────────────

    void testRuleDisabledSignal()
    {
        // 测试目的：验证达到最大触发次数时发射 ruleDisabled 信号
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setMaxTriggerCount(2);
        engine->setCooldownMs(0);

        int signalCount = 0;
        connect(engine, &AutomationRuleEngine::ruleDisabled, this,
                [&signalCount](int) { ++signalCount; });

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 前2次成功，不发射信号
        QVERIFY(engine->shouldAutoReply(record));
        QVERIFY(engine->shouldAutoReply(record));
        QCOMPARE(signalCount, 0);

        // 第3次失败，发射 ruleDisabled 信号
        QVERIFY(!engine->shouldAutoReply(record));
        QCOMPARE(signalCount, 1);
    }

    void testCooldownActiveSignal()
    {
        // 测试目的：验证冷却时间内触发时发射 cooldownActive 信号
        engine->setEnabled(true);
        engine->setPattern("Trigger");
        engine->setCooldownMs(1000);
        engine->setMaxTriggerCount(0);

        int signalCount = 0;
        connect(engine, &AutomationRuleEngine::cooldownActive, this,
                [&signalCount](int) { ++signalCount; });

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Trigger Now");

        // 第一次成功
        QVERIFY(engine->shouldAutoReply(record));
        QCOMPARE(signalCount, 0);

        // 冷却时间内触发，发射 cooldownActive 信号
        QVERIFY(!engine->shouldAutoReply(record));
        QCOMPARE(signalCount, 1);
    }
};

QTEST_MAIN(TestAutomationRuleEngine)
#include "TestAutomationRuleEngine.moc"
