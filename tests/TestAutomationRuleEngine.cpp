// TestAutomationRuleEngine.cpp
// 测试多规则自动回复引擎：验证规则匹配、防循环保护、全局速率限制等功能

#include <QtTest>
#include <QThread>
#include "AutomationRuleEngine.h"

class TestAutomationRuleEngine : public QObject
{
    Q_OBJECT

private:
    AutomationRuleEngine* engine;

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

    static AutoReplyRule makeRule(const QString& pattern, const QString& reply,
                                   bool regex = false, int cooldown = 500, int maxTriggers = 10)
    {
        AutoReplyRule rule;
        rule.enabled = true;
        rule.pattern = pattern;
        rule.replyText = reply;
        rule.useRegex = regex;
        rule.cooldownMs = cooldown;
        rule.maxTriggerCount = maxTriggers;
        return rule;
    }

private slots:
    void init()
    {
        engine = new AutomationRuleEngine();
        AutomationRuleEngine::resetAllRules();
        AutomationRuleEngine::setGlobalMaxPerMinute(20);
    }

    void cleanup()
    {
        delete engine;
        engine = nullptr;
    }

    // ── 基本匹配 ────────────────────────────────────────────────────────────

    void testBasicTextMatch()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("Hello", "Hi there!"));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Hello World");
        QCOMPARE(engine->checkAutoReply(record), QStringLiteral("Hi there!"));
    }

    void testNoMatch()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("Hello", "Hi"));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Goodbye");
        QVERIFY(engine->checkAutoReply(record).isEmpty());
    }

    void testRegexMatch()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("\\d+\\.\\d+", "Got number", true));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Value: 3.14");
        QCOMPARE(engine->checkAutoReply(record), QStringLiteral("Got number"));
    }

    void testDisabledEngine()
    {
        engine->setEnabled(false);
        engine->addRule(makeRule("Hello", "Hi"));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Hello");
        QVERIFY(engine->checkAutoReply(record).isEmpty());
    }

    void testOnlyTriggersOnRx()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("Data", "OK"));

        SerialRecord txRecord = createTxRecord(QByteArray::fromHex("01"), "Data Sent");
        QVERIFY(engine->checkAutoReply(txRecord).isEmpty());

        SerialRecord rxRecord = createRxRecord(QByteArray::fromHex("01"), "Data Received");
        QCOMPARE(engine->checkAutoReply(rxRecord), QStringLiteral("OK"));
    }

    void testEmptyPattern()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("", "Reply"));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Any");
        QVERIFY(engine->checkAutoReply(record).isEmpty());
    }

    void testCaseInsensitive()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("hello", "Hi"));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "HELLO World");
        QCOMPARE(engine->checkAutoReply(record), QStringLiteral("Hi"));
    }

    void testInvalidRegex()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("[invalid", "Reply", true));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Text");
        QVERIFY(engine->checkAutoReply(record).isEmpty());
    }

    // ── 多规则 ──────────────────────────────────────────────────────────────

    void testMultipleRulesFirstMatchWins()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("Hello", "Reply A"));
        engine->addRule(makeRule("Hello", "Reply B"));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Hello");
        QCOMPARE(engine->checkAutoReply(record), QStringLiteral("Reply A"));
    }

    void testMultipleRulesSecondMatches()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("Foo", "Reply A"));
        engine->addRule(makeRule("Bar", "Reply B"));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Bar");
        QCOMPARE(engine->checkAutoReply(record), QStringLiteral("Reply B"));
    }

    void testDisabledRuleSkipped()
    {
        engine->setEnabled(true);
        AutoReplyRule rule = makeRule("Hello", "Disabled Reply");
        rule.enabled = false;
        engine->addRule(rule);
        engine->addRule(makeRule("Hello", "Enabled Reply"));

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Hello");
        QCOMPARE(engine->checkAutoReply(record), QStringLiteral("Enabled Reply"));
    }

    void testAddRemoveRules()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("A", "Reply A"));
        engine->addRule(makeRule("B", "Reply B"));
        QCOMPARE(engine->rules().size(), 2);

        engine->removeRule(0);
        QCOMPARE(engine->rules().size(), 1);

        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "B");
        QCOMPARE(engine->checkAutoReply(record), QStringLiteral("Reply B"));
    }

    // ── 最大触发次数 ────────────────────────────────────────────────────────

    void testMaxTriggerCount()
    {
        engine->setEnabled(true);
        AutoReplyRule rule = makeRule("Ping", "Pong");
        rule.maxTriggerCount = 3;
        rule.cooldownMs = 0; // 无冷却，快速测试
        engine->addRule(rule);

        // 触发 3 次
        for (int i = 0; i < 3; ++i) {
            QThread::msleep(10);
            SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Ping");
            QCOMPARE(engine->checkAutoReply(record), QStringLiteral("Pong"));
        }

        // 第 4 次不应触发
        SerialRecord record = createRxRecord(QByteArray::fromHex("01"), "Ping");
        QVERIFY(engine->checkAutoReply(record).isEmpty());
    }

    void testOnceOnly()
    {
        engine->setEnabled(true);
        AutoReplyRule rule = makeRule("Once", "Done");
        rule.onceOnly = true;
        rule.cooldownMs = 0;
        engine->addRule(rule);

        SerialRecord r1 = createRxRecord(QByteArray::fromHex("01"), "Once");
        QCOMPARE(engine->checkAutoReply(r1), QStringLiteral("Done"));

        SerialRecord r2 = createRxRecord(QByteArray::fromHex("01"), "Once");
        QVERIFY(engine->checkAutoReply(r2).isEmpty());
    }

    // ── 冷却时间 ────────────────────────────────────────────────────────────

    void testCooldownPreventsRetrigger()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("Trigger", "Fired", false, 500));

        SerialRecord r1 = createRxRecord(QByteArray::fromHex("01"), "Trigger");
        QCOMPARE(engine->checkAutoReply(r1), QStringLiteral("Fired"));

        // 立即再次触发 — 应被冷却阻止
        SerialRecord r2 = createRxRecord(QByteArray::fromHex("01"), "Trigger");
        QVERIFY(engine->checkAutoReply(r2).isEmpty());
    }

    // ── 重置 ────────────────────────────────────────────────────────────────

    void testResetTriggerState()
    {
        engine->setEnabled(true);
        AutoReplyRule rule = makeRule("Test", "Reply");
        rule.maxTriggerCount = 1;
        rule.cooldownMs = 0;
        engine->addRule(rule);

        SerialRecord r1 = createRxRecord(QByteArray::fromHex("01"), "Test");
        QCOMPARE(engine->checkAutoReply(r1), QStringLiteral("Reply"));

        // 已达上限
        SerialRecord r2 = createRxRecord(QByteArray::fromHex("01"), "Test");
        QVERIFY(engine->checkAutoReply(r2).isEmpty());

        // 重置后可以再次触发
        engine->resetTriggerState();
        SerialRecord r3 = createRxRecord(QByteArray::fromHex("01"), "Test");
        QCOMPARE(engine->checkAutoReply(r3), QStringLiteral("Reply"));
    }

    void testResetAllRules()
    {
        engine->setEnabled(true);
        engine->addRule(makeRule("A", "Reply A", false, 0));
        AutomationRuleEngine::setGlobalMaxPerMinute(2);

        // 触发 2 次达到全局限制
        SerialRecord r1 = createRxRecord(QByteArray::fromHex("01"), "A");
        QCOMPARE(engine->checkAutoReply(r1), QStringLiteral("Reply A"));
        SerialRecord r2 = createRxRecord(QByteArray::fromHex("02"), "A");
        QCOMPARE(engine->checkAutoReply(r2), QStringLiteral("Reply A"));

        // 全局限制
        SerialRecord r3 = createRxRecord(QByteArray::fromHex("03"), "A");
        QVERIFY(engine->checkAutoReply(r3).isEmpty());

        // 重置全局
        AutomationRuleEngine::resetAllRules();
        SerialRecord r4 = createRxRecord(QByteArray::fromHex("04"), "A");
        QCOMPARE(engine->checkAutoReply(r4), QStringLiteral("Reply A"));
    }

    // ── 信号 ────────────────────────────────────────────────────────────────

    void testRulesChangedSignal()
    {
        QSignalSpy spy(engine, &AutomationRuleEngine::rulesChanged);

        engine->addRule(makeRule("A", "Reply"));
        QCOMPARE(spy.count(), 1);

        engine->removeRule(0);
        QCOMPARE(spy.count(), 2);
    }
};

QTEST_MAIN(TestAutomationRuleEngine)
#include "TestAutomationRuleEngine.moc"
