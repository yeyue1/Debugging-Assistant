// TestProtocolParser.cpp
// 测试协议解析器：验证帧解析、CRC校验、边界条件等功能

#include <QtTest>
#include <QSignalSpy>
#include "ProtocolParser.h"

class TestProtocolParser : public QObject
{
    Q_OBJECT

private:
    ProtocolParser* parser;

    // 计算 CRC16 (Modbus)，与 ProtocolParser 中的实现一致
    static quint16 crc16(const QByteArray& data)
    {
        quint16 crc = 0xFFFF;
        for (unsigned char byte : data) {
            crc ^= static_cast<quint16>(byte);
            for (int i = 0; i < 8; ++i) {
                if (crc & 0x0001) {
                    crc = (crc >> 1) ^ 0xA001;
                } else {
                    crc >>= 1;
                }
            }
        }
        return crc;
    }

    // 构造一个完整的有效帧: 0x55 0xAA + length + payload + CRC16(LE)
    static QByteArray buildValidFrame(const QByteArray& payload)
    {
        QByteArray frame;
        frame.append(static_cast<char>(0x55));  // 帧头1
        frame.append(static_cast<char>(0xAA));  // 帧头2
        frame.append(static_cast<char>(payload.size()));  // 长度
        frame.append(payload);  // 负载

        quint16 crc = crc16(payload);
        frame.append(static_cast<char>(crc & 0xFF));       // CRC低字节
        frame.append(static_cast<char>((crc >> 8) & 0xFF)); // CRC高字节

        return frame;
    }

private slots:
    void init()
    {
        parser = new ProtocolParser(this);
    }

    void cleanup()
    {
        delete parser;
        parser = nullptr;
    }

    // ── 测试有效帧解析 ──────────────────────────────────────────────────────

    void testValidFrameParsing()
    {
        // 测试目的：验证基本的有效帧能够被正确解析
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);
        QSignalSpy errorSpy(parser, &ProtocolParserBase::frameError);

        QByteArray payload = QByteArray::fromHex("01020304");
        QByteArray frame = buildValidFrame(payload);

        parser->inputBytes(frame);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(errorSpy.count(), 0);

        QList<QVariant> args = frameSpy.takeFirst();
        QCOMPARE(args.at(0).toByteArray(), payload);
        QVERIFY(args.at(1).toString().contains("CRC="));
    }

    void testMultipleFrames()
    {
        // 测试目的：验证连续多个帧能够被正确解析
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);

        QByteArray payload1 = QByteArray::fromHex("AABB");
        QByteArray payload2 = QByteArray::fromHex("CCDD");

        QByteArray data = buildValidFrame(payload1) + buildValidFrame(payload2);
        parser->inputBytes(data);

        QCOMPARE(frameSpy.count(), 2);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), payload1);
        QCOMPARE(frameSpy.at(1).at(0).toByteArray(), payload2);
    }

    void testFrameWithNoise()
    {
        // 测试目的：验证帧前有噪声数据时仍能正确解析
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);

        QByteArray payload = QByteArray::fromHex("FF");
        QByteArray noise = QByteArray::fromHex("00112233");
        QByteArray frame = noise + buildValidFrame(payload);

        parser->inputBytes(frame);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), payload);
    }

    void testByteByByteParsing()
    {
        // 测试目的：验证逐字节输入也能正确解析
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);

        QByteArray payload = QByteArray::fromHex("ABCD");
        QByteArray frame = buildValidFrame(payload);

        for (int i = 0; i < frame.size(); ++i) {
            parser->inputBytes(frame.mid(i, 1));
        }

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), payload);
    }

    // ── 测试 CRC 错误帧 ──────────────────────────────────────────────────────

    void testCrcErrorFrame()
    {
        // 测试目的：验证CRC错误的帧能被检测并报告错误
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);
        QSignalSpy errorSpy(parser, &ProtocolParserBase::frameError);

        QByteArray payload = QByteArray::fromHex("010203");
        QByteArray frame;
        frame.append(static_cast<char>(0x55));
        frame.append(static_cast<char>(0xAA));
        frame.append(static_cast<char>(payload.size()));
        frame.append(payload);
        frame.append(static_cast<char>(0x00));  // 错误的CRC
        frame.append(static_cast<char>(0x00));  // 错误的CRC

        parser->inputBytes(frame);

        QCOMPARE(frameSpy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
        QVERIFY(errorSpy.at(0).at(0).toString().contains("CRC"));
    }

    void testCrcErrorDoesNotAffectNextFrame()
    {
        // 测试目的：验证CRC错误后解析器能恢复正常解析下一帧
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);
        QSignalSpy errorSpy(parser, &ProtocolParserBase::frameError);

        // 先发送一个CRC错误的帧
        QByteArray payload1 = QByteArray::fromHex("01");
        QByteArray badFrame;
        badFrame.append(static_cast<char>(0x55));
        badFrame.append(static_cast<char>(0xAA));
        badFrame.append(static_cast<char>(1));
        badFrame.append(payload1);
        badFrame.append(static_cast<char>(0x00));
        badFrame.append(static_cast<char>(0x00));

        // 再发送一个正确的帧
        QByteArray payload2 = QByteArray::fromHex("AABB");
        QByteArray goodFrame = buildValidFrame(payload2);

        parser->inputBytes(badFrame + goodFrame);

        QCOMPARE(errorSpy.count(), 1);
        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), payload2);
    }

    // ── 测试空负载 ────────────────────────────────────────────────────────────

    void testEmptyPayload()
    {
        // 测试目的：验证空负载帧能被正确解析
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);

        QByteArray payload;  // 空负载
        QByteArray frame = buildValidFrame(payload);

        parser->inputBytes(frame);

        QCOMPARE(frameSpy.count(), 1);
        QVERIFY(frameSpy.at(0).at(0).toByteArray().isEmpty());
    }

    // ── 测试最大长度负载 ──────────────────────────────────────────────────────

    void testMaxPayloadLength()
    {
        // 测试目的：验证最大长度(255字节)负载能被正确解析
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);

        QByteArray payload(255, static_cast<char>(0xAB));  // 255字节负载
        QByteArray frame = buildValidFrame(payload);

        parser->inputBytes(frame);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), payload);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray().size(), 255);
    }

    void testPayloadLength254()
    {
        // 测试目的：验证254字节负载能被正确解析（边界测试）
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);

        QByteArray payload(254, static_cast<char>(0xCD));
        QByteArray frame = buildValidFrame(payload);

        parser->inputBytes(frame);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), payload);
    }

    // ── 测试解析器状态重置 ────────────────────────────────────────────────────

    void testReset()
    {
        // 测试目的：验证重置后解析器能从初始状态重新工作
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);

        // 先发送部分帧数据
        QByteArray partialFrame = QByteArray::fromHex("55AA030102");
        parser->inputBytes(partialFrame);

        // 重置解析器
        parser->reset();

        // 发送一个完整的帧
        QByteArray payload = QByteArray::fromHex("FF");
        QByteArray frame = buildValidFrame(payload);
        parser->inputBytes(frame);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), payload);
    }

    // ── 测试帧头重复 ──────────────────────────────────────────────────────────

    void testRepeatedHeaderByte()
    {
        // 测试目的：验证连续的0x55字节不会干扰解析
        QSignalSpy frameSpy(parser, &ProtocolParserBase::frameReady);

        QByteArray payload = QByteArray::fromHex("01");
        QByteArray frame;
        frame.append(static_cast<char>(0x55));
        frame.append(static_cast<char>(0x55));  // 重复的0x55
        frame.append(static_cast<char>(0x55));  // 重复的0x55
        frame.append(static_cast<char>(0xAA));
        frame.append(static_cast<char>(1));
        frame.append(payload);

        quint16 crc = crc16(payload);
        frame.append(static_cast<char>(crc & 0xFF));
        frame.append(static_cast<char>((crc >> 8) & 0xFF));

        parser->inputBytes(frame);

        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.at(0).at(0).toByteArray(), payload);
    }

    // ── 测试解析器名称 ────────────────────────────────────────────────────────

    void testName()
    {
        // 测试目的：验证解析器返回正确的名称
        QCOMPARE(parser->name(), QStringLiteral("自定义协议"));
    }
};

QTEST_MAIN(TestProtocolParser)
#include "TestProtocolParser.moc"
