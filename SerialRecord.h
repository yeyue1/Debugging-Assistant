#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QString>

enum class SerialRecordDirection {
    Rx,
    Tx,
    System
};

struct SerialRecord {
    QDateTime timestamp;
    SerialRecordDirection direction = SerialRecordDirection::Rx;
    QByteArray payload;
    QString text;
    QString protocol;
    QString info;
    QString error;
};
