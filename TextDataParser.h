#pragma once

#include "ProtocolParserBase.h"

class TextDataParser : public ProtocolParserBase
{
    Q_OBJECT

public:
    explicit TextDataParser(QObject* parent = nullptr);

    void reset() override;
    QString name() const override;

public slots:
    void inputBytes(const QByteArray& data) override;
};
