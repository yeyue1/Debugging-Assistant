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
            m_state = State::WaitHeader2;
        }
        break;
    case State::WaitHeader2:
        if (byte == 0xAA) {
            m_state = State::WaitLength;
        } else if (byte == 0x55) {
            m_state = State::WaitHeader2;
        } else {
            m_state = State::WaitHeader1;
        }
        break;
    case State::WaitLength:
        m_length = byte;
        m_payload.clear();
        if (m_length == 0) {
            m_state = State::WaitCrc1;
        } else {
            m_state = State::WaitPayload;
        }
        break;
    case State::WaitPayload:
        m_payload.append(static_cast<char>(byte));
        if (m_payload.size() >= m_length) {
            m_state = State::WaitCrc1;
        }
        break;
    case State::WaitCrc1:
        m_crcBytes[0] = byte;
        m_state = State::WaitCrc2;
        break;
    case State::WaitCrc2:
        m_crcBytes[1] = byte;
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
