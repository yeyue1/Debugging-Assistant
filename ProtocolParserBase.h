#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

#include "ParsedFrame.h"

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
    void parsedFrameReady(const ParsedFrame& frame);
    void rawDataReady(const QByteArray& data);
    void frameError(const QString& message);
};
