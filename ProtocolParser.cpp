#include "ProtocolParser.h"

#include <QString>

ProtocolParser::ProtocolParser(QObject* parent)
    : ProtocolParserBase(parent)
{
}

void ProtocolParser::reset()
{
    m_state = State::WaitHeader1;
    m_length = 0;
    m_payload.clear();
    m_frameRaw.clear();
    m_crcBytes[0] = 0;
    m_crcBytes[1] = 0;
}

QString ProtocolParser::name() const
{
    return QStringLiteral("自定义协议");
}

void ProtocolParser::inputBytes(const QByteArray& data)
{
    for (unsigned char byte : data) {
        processByte(static_cast<quint8>(byte));
    }
}

void ProtocolParser::processByte(quint8 byte)
{
    switch (m_state) {
    case State::WaitHeader1:
        if (byte == 0x55) {
            m_frameRaw.clear();
            m_frameRaw.append(static_cast<char>(byte));
            m_state = State::WaitHeader2;
        }
        break;
    case State::WaitHeader2:
        if (byte == 0xAA) {
            m_frameRaw.append(static_cast<char>(byte));
            m_state = State::WaitLength;
        } else if (byte == 0x55) {
            m_frameRaw.clear();
            m_frameRaw.append(static_cast<char>(byte));
            m_state = State::WaitHeader2;
        } else {
            m_state = State::WaitHeader1;
        }
        break;
    case State::WaitLength:
        m_length = byte;
        m_payload.clear();
        m_frameRaw.append(static_cast<char>(byte));
        if (m_length == 0) {
            m_state = State::WaitCrc1;
        } else {
            m_state = State::WaitPayload;
        }
        break;
    case State::WaitPayload:
        m_payload.append(static_cast<char>(byte));
        m_frameRaw.append(static_cast<char>(byte));
        if (m_payload.size() >= m_length) {
            m_state = State::WaitCrc1;
        }
        break;
    case State::WaitCrc1:
        m_crcBytes[0] = byte;
        m_frameRaw.append(static_cast<char>(byte));
        m_state = State::WaitCrc2;
        break;
    case State::WaitCrc2:
        m_crcBytes[1] = byte;
        m_frameRaw.append(static_cast<char>(byte));
        finalizeFrame();
        m_state = State::WaitHeader1;
        break;
    }
}

void ProtocolParser::finalizeFrame()
{
    const quint16 expected = crc16(m_payload);
    const quint16 received = static_cast<quint16>(m_crcBytes[0])
        | (static_cast<quint16>(m_crcBytes[1]) << 8);

    if (expected != received) {
        emit frameError(QStringLiteral("CRC 校验失败：期望 0x%1，实际 0x%2")
                            .arg(expected, 4, 16, QLatin1Char('0'))
                            .arg(received, 4, 16, QLatin1Char('0'))
                            .toUpper());
        return;
    }

    QString info = QStringLiteral("CRC=0x%1").arg(received, 4, 16, QLatin1Char('0')).toUpper();

    // 构建结构化帧数据（先于 frameReady 发出，以便 SerialPanel 可在 record 链路中使用）
    ParsedFrame parsedFrame;
    parsedFrame.protocolName = name();
    parsedFrame.rawBytes = m_frameRaw;
    parsedFrame.checksumOk = true;

    // 帧头字段
    ParsedField headerField;
    headerField.name = QStringLiteral("帧头");
    headerField.raw = m_frameRaw.left(2);
    headerField.value = QStringLiteral("0x55AA");
    headerField.note = QStringLiteral("帧头标识 (0x55AA)");
    parsedFrame.fields.append(headerField);

    // 长度字段
    ParsedField lengthField;
    lengthField.name = QStringLiteral("长度");
    lengthField.raw = QByteArray(1, static_cast<char>(m_length));
    lengthField.value = QString::number(m_length);
    lengthField.note = QStringLiteral("负载长度 (%1 字节)").arg(m_length);
    parsedFrame.fields.append(lengthField);

    // 负载字段
    ParsedField payloadField;
    payloadField.name = QStringLiteral("负载");
    payloadField.raw = m_payload;
    if (m_payload.isEmpty()) {
        payloadField.value = QStringLiteral("(空)");
    } else {
        payloadField.value = QString::fromLatin1(m_payload.toHex(' ')).toUpper();
    }
    payloadField.note = QStringLiteral("负载数据 (%1 字节)").arg(m_payload.size());
    parsedFrame.fields.append(payloadField);

    // CRC 字段
    ParsedField crcField;
    crcField.name = QStringLiteral("CRC");
    crcField.raw = QByteArray(1, static_cast<char>(m_crcBytes[0]))
        + QByteArray(1, static_cast<char>(m_crcBytes[1]));
    crcField.value = QStringLiteral("0x%1").arg(received, 4, 16, QLatin1Char('0')).toUpper();
    crcField.note = QStringLiteral("CRC16-MODBUS 校验 (通过)");
    parsedFrame.fields.append(crcField);

    emit parsedFrameReady(parsedFrame);
    emit frameReady(m_payload, info);
}

quint16 ProtocolParser::crc16(const QByteArray& data)
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
