#include "TextDataParser.h"

#include <QString>

TextDataParser::TextDataParser(QObject* parent)
    : ProtocolParserBase(parent)
{
}

void TextDataParser::reset()
{
}

QString TextDataParser::name() const
{
    return QStringLiteral("文本");
}

void TextDataParser::inputBytes(const QByteArray& data)
{
    if (!data.isEmpty()) {
        emit frameReady(data, QString());
    }
}
