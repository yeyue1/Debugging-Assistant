#pragma once

#include "ProtocolParserBase.h"

class RawDataParser : public ProtocolParserBase
{
    Q_OBJECT

public:
    explicit RawDataParser(QObject* parent = nullptr);

    void reset() override;
    QString name() const override;

public slots:
    void inputBytes(const QByteArray& data) override;
};
