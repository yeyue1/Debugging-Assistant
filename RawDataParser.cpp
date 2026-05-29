#include "RawDataParser.h"

RawDataParser::RawDataParser(QObject* parent)
    : ProtocolParserBase(parent)
{
}

void RawDataParser::reset()
{
}

QString RawDataParser::name() const
{
    return QStringLiteral("原始数据");
}

void RawDataParser::inputBytes(const QByteArray& data)
{
    if (!data.isEmpty()) {
        emit rawDataReady(data);
    }
}
