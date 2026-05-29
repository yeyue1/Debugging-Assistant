#pragma once

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QString>

#include "SerialConfig.h"

class QTimer;

class SerialManager : public QObject
{
    Q_OBJECT

public:
    explicit SerialManager(QObject* parent = nullptr);
    ~SerialManager() override;

    void setConfig(const SerialConfig& config);
    SerialConfig config() const;

    bool isOpen() const;

    int throttleIntervalMs() const;
    void setThrottleIntervalMs(int ms);

    int maxChunkSize() const;
    void setMaxChunkSize(int bytes);

public slots:
    void open();
    void close();
    qint64 sendData(const QByteArray& data);
    bool setDataTerminalReady(bool enabled);
    bool setRequestToSend(bool enabled);

signals:
    void opened();
    void closed();
    void dataReceived(const QByteArray& payload);
    void errorOccurred(QSerialPort::SerialPortError code, const QString& message);

private slots:
    void handleReadyRead();
    void flushRxBuffer();
    void handleError(QSerialPort::SerialPortError code);

private:
    void applyConfig();

    QSerialPort* m_port = nullptr;
    QTimer* m_flushTimer = nullptr;

    QByteArray m_rxBuffer;
    int m_flushIntervalMs = 20;
    int m_maxChunkSize = 4096;

    SerialConfig m_config;
};
