#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

class ProtocolParserBase : public QObject
{
    Q_OBJECT

public:
    explicit ProtocolParserBase(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ProtocolParserBase() = default;

    virtual void reset() = 0;
    virtual QString name() const = 0;

public slots:
    virtual void inputBytes(const QByteArray& data) = 0;

signals:
    void frameReady(const QByteArray& payload, const QString& info);
    void rawDataReady(const QByteArray& data);
    void frameError(const QString& message);
};
