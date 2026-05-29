#pragma once

#include <QSerialPort>
#include <QString>
#include <QSettings>

struct SerialConfig {
    QString portName;
    qint32 baudRate = QSerialPort::Baud115200;
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;

    void save(QSettings& settings) const {
        settings.setValue("serial/portName", portName);
        settings.setValue("serial/baudRate", baudRate);
        settings.setValue("serial/dataBits", static_cast<int>(dataBits));
        settings.setValue("serial/parity", static_cast<int>(parity));
        settings.setValue("serial/stopBits", static_cast<int>(stopBits));
        settings.setValue("serial/flowControl", static_cast<int>(flowControl));
    }

    void load(QSettings& settings) {
        portName = settings.value("serial/portName").toString();
        baudRate = settings.value("serial/baudRate", QSerialPort::Baud115200).toInt();
        dataBits = static_cast<QSerialPort::DataBits>(
            settings.value("serial/dataBits", QSerialPort::Data8).toInt());
        parity = static_cast<QSerialPort::Parity>(
            settings.value("serial/parity", QSerialPort::NoParity).toInt());
        stopBits = static_cast<QSerialPort::StopBits>(
            settings.value("serial/stopBits", QSerialPort::OneStop).toInt());
        flowControl = static_cast<QSerialPort::FlowControl>(
            settings.value("serial/flowControl", QSerialPort::NoFlowControl).toInt());
    }
};
