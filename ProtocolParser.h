#pragma once

#include "ProtocolParserBase.h"

class ProtocolParser : public ProtocolParserBase
{
    Q_OBJECT

public:
    explicit ProtocolParser(QObject* parent = nullptr);

    void reset() override;
    QString name() const override;

public slots:
    void inputBytes(const QByteArray& data) override;

private:
    enum class State {
        WaitHeader1,
        WaitHeader2,
        WaitLength,
        WaitPayload,
        WaitCrc1,
        WaitCrc2
    };

    void processByte(quint8 byte);
    void finalizeFrame();

    static quint16 crc16(const QByteArray& data);

    State m_state = State::WaitHeader1;
    quint8 m_length = 0;
    QByteArray m_payload;
    QByteArray m_frameRaw;
    quint8 m_crcBytes[2] = {0, 0};
};
