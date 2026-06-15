// TestParserController.cpp
// 测试解析器控制器：验证解析器切换、数据分发、信号转发等功能

#include <QtTest>
#include <QSignalSpy>
#include "ParserController.h"
#include "ProtocolParserBase.h"

class TestParserController : public QObject
{
    Q_OBJECT

private:
    ParserController* controller;

private slots:
    void init()
    {
        controller = new ParserController(this);
    }

    void cleanup()
    {
        delete controller;
        controller = nullptr;
    }

    // ── 测试初始状态 ──────────────────────────────────────────────────────────

    void testDefaultParserIsRawData()
    {
        // 测试目的：验证默认解析器为RawData
        QCOMPARE(controller->currentType(), ParserController::RawData);
        QVERIFY(controller->currentParser() != nullptr);
    }

    void testDefaultName()
    {
        // 测试目的：验证默认解析器名称为"原始数据"
        QCOMPARE(controller->currentName(), QStringLiteral("原始数据"));
    }

    // ── 测试解析器切换 ────────────────────────────────────────────────────────

    void testSwitchToRawDataParser()
    {
        // 测试目的：验证切换到RawData解析器
        controller->switchTo(ParserController::TextData);
        QCOMPARE(controller->currentName(), QStringLiteral("文本"));

        ParserController::ParserType result = controller->switchTo(ParserController::RawData);
        QCOMPARE(result, ParserController::RawData);
        QCOMPARE(controller->currentType(), ParserController::RawData);
        QCOMPARE(controller->currentName(), QStringLiteral("原始数据"));
    }

    void testSwitchToTextDataParser()
    {
        // 测试目的：验证切换到TextData解析器
        ParserController::ParserType result = controller->switchTo(ParserController::TextData);
        QCOMPARE(result, ParserController::TextData);
        QCOMPARE(controller->currentType(), ParserController::TextData);
        QCOMPARE(controller->currentName(), QStringLiteral("文本"));
    }

    void testSwitchToProtocolParser()
    {
        // 测试目的：验证切换到CustomProtocol解析器
        ParserController::ParserType result = controller->switchTo(ParserController::CustomProtocol);
        QCOMPARE(result, ParserController::CustomProtocol);
        QCOMPARE(controller->currentType(), ParserController::CustomProtocol);
        QCOMPARE(controller->currentName(), QStringLiteral("自定义协议"));
    }

    void testSwitchToSameTypeNoOp()
    {
        // 测试目的：验证切换到相同类型时不做任何操作（返回当前类型）
        QCOMPARE(controller->currentType(), ParserController::RawData);

        // 切换到RawData（已经是当前类型）
        ParserController::ParserType result = controller->switchTo(ParserController::RawData);
        QCOMPARE(result, ParserController::RawData);
        QCOMPARE(controller->currentType(), ParserController::RawData);
    }

    void testSwitchPreservesParser()
    {
        // 测试目的：验证切换后当前解析器指针有效
        controller->switchTo(ParserController::TextData);
        QVERIFY(controller->currentParser() != nullptr);
        QCOMPARE(controller->currentParser()->name(), QStringLiteral("文本"));

        controller->switchTo(ParserController::CustomProtocol);
        QVERIFY(controller->currentParser() != nullptr);
        QCOMPARE(controller->currentParser()->name(), QStringLiteral("自定义协议"));
    }

    // ── 测试数据分发 ──────────────────────────────────────────────────────────

    void testDataDispatchThroughRawDataParser()
    {
        // 测试目的：验证数据通过RawDataParser分发
        QSignalSpy rawDataSpy(controller, &ParserController::rawDataReady);

        QByteArray data = QByteArray::fromHex("AABBCCDD");
        controller->inputBytes(data);

        QCOMPARE(rawDataSpy.count(), 1);
        QCOMPARE(rawDataSpy.at(0).at(0).toByteArray(), data);
    }

    void testDataDispatchThroughTextDataParser()
    {
        // 测试目的：验证数据通过TextDataParser分发
        controller->switchTo(ParserController::TextData);

        QSignalSpy frameSpy(controller, &ParserController::frameReady);

        QByteArray data = QByteArray::fromHex("48656C6C6F");  // "Hello"
        controller->inputBytes(data);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), data);
    }

    void testDataDispatchEmpty()
    {
        // 测试目的：验证空数据不触发信号
        QSignalSpy rawDataSpy(controller, &ParserController::rawDataReady);
        QSignalSpy frameSpy(controller, &ParserController::frameReady);

        controller->inputBytes(QByteArray());

        QCOMPARE(rawDataSpy.count(), 0);
        QCOMPARE(frameSpy.count(), 0);
    }

    // ── 测试信号转发 ──────────────────────────────────────────────────────────

    void testRawDataSignalForwarding()
    {
        // 测试目的：验证RawData解析器的rawDataReady信号通过控制器转发
        QSignalSpy rawDataSpy(controller, &ParserController::rawDataReady);

        QByteArray testData = QByteArray::fromHex("112233");
        controller->inputBytes(testData);

        QCOMPARE(rawDataSpy.count(), 1);
        QCOMPARE(rawDataSpy.at(0).at(0).toByteArray(), testData);
    }

    void testFrameReadySignalForwarding()
    {
        // 测试目的：验证TextData解析器的frameReady信号通过控制器转发
        controller->switchTo(ParserController::TextData);

        QSignalSpy frameSpy(controller, &ParserController::frameReady);

        QByteArray testData = QByteArray::fromHex("48454C4C4F");  // "HELLO"
        controller->inputBytes(testData);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), testData);
    }

    void testSwitchPreservesConnections()
    {
        // 测试目的：验证切换解析器后信号连接仍然有效
        // 连接到控制器的rawDataReady信号
        QSignalSpy rawDataSpy(controller, &ParserController::rawDataReady);

        // 第一次：RawData模式下发送数据
        QByteArray data1 = QByteArray::fromHex("AABB");
        controller->inputBytes(data1);
        QCOMPARE(rawDataSpy.count(), 1);

        // 切换到TextData（此时rawDataReady不应触发）
        controller->switchTo(ParserController::TextData);
        rawDataSpy.clear();

        // TextData模式下发送数据（应触发frameReady而非rawDataReady）
        QSignalSpy frameSpy(controller, &ParserController::frameReady);
        QByteArray data2 = QByteArray::fromHex("CCDD");
        controller->inputBytes(data2);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(rawDataSpy.count(), 0);  // rawDataReady不应触发

        // 切换回RawData
        controller->switchTo(ParserController::RawData);
        frameSpy.clear();

        // RawData模式下发送数据（应触发rawDataReady）
        QByteArray data3 = QByteArray::fromHex("EEFF");
        controller->inputBytes(data3);

        QCOMPARE(rawDataSpy.count(), 1);
        QCOMPARE(rawDataSpy.at(0).at(0).toByteArray(), data3);
        QCOMPARE(frameSpy.count(), 0);
    }

    // ── 测试多次切换 ──────────────────────────────────────────────────────────

    void testMultipleSwitches()
    {
        // 测试目的：验证多次切换解析器后仍能正常工作
        // RawData -> TextData -> CustomProtocol -> RawData
        controller->switchTo(ParserController::TextData);
        QCOMPARE(controller->currentType(), ParserController::TextData);

        controller->switchTo(ParserController::CustomProtocol);
        QCOMPARE(controller->currentType(), ParserController::CustomProtocol);

        controller->switchTo(ParserController::RawData);
        QCOMPARE(controller->currentType(), ParserController::RawData);
        QCOMPARE(controller->currentName(), QStringLiteral("原始数据"));

        // 验证最终状态下的数据分发
        QSignalSpy rawDataSpy(controller, &ParserController::rawDataReady);
        QByteArray data = QByteArray::fromHex("1234");
        controller->inputBytes(data);

        QCOMPARE(rawDataSpy.count(), 1);
        QCOMPARE(rawDataSpy.at(0).at(0).toByteArray(), data);
    }
};

QTEST_MAIN(TestParserController)
#include "TestParserController.moc"
