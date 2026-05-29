#include "ParserController.h"

#include "ProtocolParserBase.h"
#include "RawDataParser.h"
#include "TextDataParser.h"
#include "ProtocolParser.h"

ParserController::ParserController(QObject* parent)
    : QObject(parent)
{
    createParser(RawData);
}

ParserController::~ParserController()
{
}

ParserController::ParserType ParserController::switchTo(ParserType type)
{
    if (type == m_currentType && m_parser) {
        return m_currentType;
    }

    // 断开旧解析器的信号
    if (m_parser) {
        disconnect(m_parser, &ProtocolParserBase::frameReady, this, &ParserController::frameReady);
        disconnect(m_parser, &ProtocolParserBase::rawDataReady, this, &ParserController::rawDataReady);
        disconnect(m_parser, &ProtocolParserBase::frameError, this, &ParserController::frameError);
        m_parser->deleteLater();
        m_parser = nullptr;
    }

    createParser(type);
    return m_currentType;
}

void ParserController::createParser(ParserType type)
{
    m_currentType = type;

    switch (type) {
    case RawData:
        m_parser = new RawDataParser(this);
        break;
    case TextData:
        m_parser = new TextDataParser(this);
        break;
    case CustomProtocol:
        m_parser = new ProtocolParser(this);
        break;
    }

    // 将当前解析器的信号桥接到 ParserController 的信号
    if (m_parser) {
        connect(m_parser, &ProtocolParserBase::frameReady, this, &ParserController::frameReady);
        connect(m_parser, &ProtocolParserBase::rawDataReady, this, &ParserController::rawDataReady);
        connect(m_parser, &ProtocolParserBase::frameError, this, &ParserController::frameError);
    }
}

QString ParserController::currentName() const
{
    return m_parser ? m_parser->name() : QString();
}

void ParserController::inputBytes(const QByteArray& data)
{
    if (m_parser) {
        m_parser->inputBytes(data);
    }
}
