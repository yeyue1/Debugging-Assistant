#pragma once

#include <QByteArray>
#include <QList>
#include <QString>

struct ParsedField
{
    QString name;
    QString value;
    QByteArray raw;
    QString note;
};

struct ParsedFrame
{
    QString protocolName;
    QByteArray rawBytes;
    QList<ParsedField> fields;
    bool checksumOk = false;
    QString error;
};
