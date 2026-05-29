#pragma once

#include <QObject>
#include <QByteArray>

class ProtocolParserBase;

// 管理协议解析器的创建、切换和信号路由
// SerialManager::dataReceived 连接到本对象的 inputBytes() 槽即可，
// 切换解析器后无需重新连接。
class ParserController : public QObject
{
    Q_OBJECT

public:
    enum ParserType {
        RawData = 0,
        TextData = 1,
        CustomProtocol = 2,
    };

    explicit ParserController(QObject* parent = nullptr);
    ~ParserController() override;

    ProtocolParserBase* currentParser() const { return m_parser; }
    ParserType currentType() const { return m_currentType; }

    // 切换解析器（旧解析器自动 delete），返回新类型
    ParserType switchTo(ParserType type);

    // 获取当前解析器名称
    QString currentName() const;

public slots:
    // 接收数据并转发给当前解析器
    void inputBytes(const QByteArray& data);

signals:
    // 透传当前解析器的信号
    void frameReady(const QByteArray& payload, const QString& info);
    void rawDataReady(const QByteArray& data);
    void frameError(const QString& message);

private:
    void createParser(ParserType type);

    ProtocolParserBase* m_parser = nullptr;
    ParserType m_currentType = RawData;
};
