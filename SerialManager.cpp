#include "SerialManager.h"

#include <QTimer>
#include <QSerialPort>
#include <QByteArray>

SerialManager::SerialManager(QObject* parent)
    : QObject(parent),
      m_port(new QSerialPort(this)),
      m_flushTimer(new QTimer(this))
{
    m_flushTimer->setSingleShot(true);
    m_flushTimer->setInterval(m_flushIntervalMs);

    connect(m_port, &QSerialPort::readyRead, this, &SerialManager::handleReadyRead);
    connect(m_port, &QSerialPort::errorOccurred, this, &SerialManager::handleError);
    connect(m_flushTimer, &QTimer::timeout, this, &SerialManager::flushRxBuffer);
}

SerialManager::~SerialManager() = default;

void SerialManager::setConfig(const SerialConfig& config)
{
    m_config = config;
    if (m_port->isOpen()) {
        applyConfig();
    }
}

SerialConfig SerialManager::config() const
{
    return m_config;
}

bool SerialManager::isOpen() const
{
    return m_port->isOpen();
}

int SerialManager::throttleIntervalMs() const
{
    return m_flushIntervalMs;
}

void SerialManager::setThrottleIntervalMs(int ms)
{
    if (ms <= 0) {
        return;
    }
    m_flushIntervalMs = ms;
    m_flushTimer->setInterval(ms);
}

int SerialManager::maxChunkSize() const
{
    return m_maxChunkSize;
}

void SerialManager::setMaxChunkSize(int bytes)
{
    if (bytes <= 0) {
        return;
    }
    m_maxChunkSize = bytes;
}

void SerialManager::open()
{
    if (m_port->isOpen()) {
        return;
    }

    applyConfig();
    if (!m_port->open(QIODevice::ReadWrite)) {
        emit errorOccurred(m_port->error(), m_port->errorString());
        return;
    }

    emit opened();
}

void SerialManager::close()
{
    if (!m_port->isOpen()) {
        return;
    }

    m_port->close();
    m_flushTimer->stop();
    m_rxBuffer.clear();

    emit closed();
}

qint64 SerialManager::sendData(const QByteArray& data)
{
    if (!m_port->isOpen()) {
        emit errorOccurred(QSerialPort::NotOpenError, QStringLiteral("串口未打开。"));
        return 0;
    }

    if (data.isEmpty()) {
        return 0;
    }

    const qint64 bytesWritten = m_port->write(data);
    if (bytesWritten < 0) {
        emit errorOccurred(m_port->error(), m_port->errorString());
        return 0;
    }

    return bytesWritten;
}

bool SerialManager::setDataTerminalReady(bool enabled)
{
    if (!m_port->isOpen()) {
        emit errorOccurred(QSerialPort::NotOpenError, QStringLiteral("串口未打开。"));
        return false;
    }

    if (!m_port->setDataTerminalReady(enabled)) {
        emit errorOccurred(m_port->error(), m_port->errorString());
        return false;
    }

    return true;
}

bool SerialManager::setRequestToSend(bool enabled)
{
    if (!m_port->isOpen()) {
        emit errorOccurred(QSerialPort::NotOpenError, QStringLiteral("串口未打开。"));
        return false;
    }

    if (!m_port->setRequestToSend(enabled)) {
        emit errorOccurred(m_port->error(), m_port->errorString());
        return false;
    }

    return true;
}

void SerialManager::handleReadyRead()
{
    m_rxBuffer.append(m_port->readAll());

    if (m_rxBuffer.size() >= m_maxChunkSize) {
        flushRxBuffer();
        return;
    }

    if (!m_flushTimer->isActive()) {
        m_flushTimer->start();
    }
}

void SerialManager::flushRxBuffer()
{
    if (m_rxBuffer.isEmpty()) {
        return;
    }

    QByteArray chunk;
    chunk.swap(m_rxBuffer);
    emit dataReceived(chunk);
}

void SerialManager::handleError(QSerialPort::SerialPortError code)
{
    if (code == QSerialPort::NoError) {
        return;
    }

    emit errorOccurred(code, m_port->errorString());
}

void SerialManager::applyConfig()
{
    m_port->setPortName(m_config.portName);
    m_port->setBaudRate(m_config.baudRate);
    m_port->setDataBits(m_config.dataBits);
    m_port->setParity(m_config.parity);
    m_port->setStopBits(m_config.stopBits);
    m_port->setFlowControl(m_config.flowControl);
}
