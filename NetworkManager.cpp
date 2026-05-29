#include "NetworkManager.h"

#include <QAbstractSocket>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

#include <utility>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
{
}

NetworkManager::~NetworkManager()
{
    resetSockets(false);
}

bool NetworkManager::isOpen() const
{
    if (m_mode == Mode::TcpClient) {
        return m_tcpClient && m_tcpClient->state() == QAbstractSocket::ConnectedState;
    }
    if (m_mode == Mode::TcpServer) {
        return m_tcpServer && m_tcpServer->isListening();
    }
    return m_udpOpen;
}

NetworkManager::Mode NetworkManager::mode() const
{
    return m_mode;
}

void NetworkManager::openTcpClient(const QString& host, quint16 port, const QString& localAddress, quint16 localPort)
{
    close();
    m_mode = Mode::TcpClient;
    m_tcpClient = new QTcpSocket(this);

    connect(m_tcpClient, &QTcpSocket::connected, this, &NetworkManager::opened);
    connect(m_tcpClient, &QTcpSocket::disconnected, this, &NetworkManager::closed);
    connect(m_tcpClient, &QTcpSocket::readyRead, this, &NetworkManager::handleTcpClientReadyRead);
    connect(m_tcpClient, &QAbstractSocket::errorOccurred, this, &NetworkManager::handleSocketError);

    if (localPort > 0 || !localAddress.trimmed().isEmpty()) {
        const QHostAddress bindAddress(addressOrAny(localAddress));
        if (!m_tcpClient->bind(bindAddress, localPort)) {
            emit errorOccurred(m_tcpClient->errorString());
            resetSockets();
            return;
        }
    }

    m_tcpClient->connectToHost(host, port);
}

void NetworkManager::openTcpServer(const QString& localAddress, quint16 port)
{
    close();
    m_mode = Mode::TcpServer;
    m_tcpServer = new QTcpServer(this);

    connect(m_tcpServer, &QTcpServer::newConnection, this, &NetworkManager::handleTcpServerNewConnection);

    if (!m_tcpServer->listen(QHostAddress(addressOrAny(localAddress)), port)) {
        emit errorOccurred(m_tcpServer->errorString());
        resetSockets();
        return;
    }

    emit opened();
    emitPeerListChanged();
}

void NetworkManager::openUdp(const QString& localAddress, quint16 port)
{
    close();
    m_mode = Mode::Udp;
    m_udpSocket = new QUdpSocket(this);

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::handleUdpReadyRead);
    connect(m_udpSocket, &QAbstractSocket::errorOccurred, this, &NetworkManager::handleSocketError);

    if (!m_udpSocket->bind(QHostAddress(addressOrAny(localAddress)), port)) {
        emit errorOccurred(m_udpSocket->errorString());
        resetSockets();
        return;
    }

    m_udpOpen = true;
    emit opened();
}

void NetworkManager::close()
{
    const bool wasOpen = isOpen();
    resetSockets();
    if (wasOpen) {
        emit closed();
    }
}

qint64 NetworkManager::sendData(const QByteArray& data, const QString& remoteHost, quint16 remotePort, const QString& peer)
{
    if (data.isEmpty()) {
        return 0;
    }

    if (m_mode == Mode::TcpClient) {
        if (!m_tcpClient || m_tcpClient->state() != QAbstractSocket::ConnectedState) {
            emit errorOccurred(QStringLiteral("TCP 客户端未连接。"));
            return 0;
        }
        const qint64 written = m_tcpClient->write(data);
        if (written < 0) {
            emit errorOccurred(m_tcpClient->errorString());
            return 0;
        }
        return written;
    }

    if (m_mode == Mode::TcpServer) {
        qint64 total = 0;
        for (QTcpSocket* client : std::as_const(m_serverClients)) {
            if (!client || client->state() != QAbstractSocket::ConnectedState) {
                continue;
            }
            if (!peer.isEmpty() && peerName(client) != peer) {
                continue;
            }
            const qint64 written = client->write(data);
            if (written > 0) {
                total += written;
            }
        }
        if (total <= 0) {
            emit errorOccurred(QStringLiteral("没有可发送的 TCP 客户端连接。"));
        }
        return total;
    }

    if (!m_udpSocket || !m_udpOpen) {
        emit errorOccurred(QStringLiteral("UDP 未打开。"));
        return 0;
    }

    const qint64 written = m_udpSocket->writeDatagram(data, QHostAddress(remoteHost), remotePort);
    if (written < 0) {
        emit errorOccurred(m_udpSocket->errorString());
        return 0;
    }
    return written;
}

void NetworkManager::handleTcpClientReadyRead()
{
    if (!m_tcpClient) {
        return;
    }
    emit dataReceived(m_tcpClient->readAll(), peerName(m_tcpClient));
}

void NetworkManager::handleTcpServerNewConnection()
{
    while (m_tcpServer && m_tcpServer->hasPendingConnections()) {
        QTcpSocket* client = m_tcpServer->nextPendingConnection();
        client->setParent(this);
        m_serverClients.append(client);

        connect(client, &QTcpSocket::readyRead, this, &NetworkManager::handleServerClientReadyRead);
        connect(client, &QTcpSocket::disconnected, this, &NetworkManager::handleServerClientDisconnected);
        connect(client, &QAbstractSocket::errorOccurred, this, &NetworkManager::handleSocketError);
    }
    emitPeerListChanged();
}

void NetworkManager::handleServerClientReadyRead()
{
    auto* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) {
        return;
    }
    emit dataReceived(client->readAll(), peerName(client));
}

void NetworkManager::handleServerClientDisconnected()
{
    auto* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) {
        return;
    }
    m_serverClients.removeAll(client);
    client->deleteLater();
    emitPeerListChanged();
}

void NetworkManager::handleUdpReadyRead()
{
    if (!m_udpSocket) {
        return;
    }

    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray payload;
        payload.resize(static_cast<int>(m_udpSocket->pendingDatagramSize()));

        QHostAddress address;
        quint16 port = 0;
        m_udpSocket->readDatagram(payload.data(), payload.size(), &address, &port);
        emit dataReceived(payload, QStringLiteral("%1:%2").arg(address.toString()).arg(port));
    }
}

void NetworkManager::handleSocketError()
{
    if (auto* tcpSocket = qobject_cast<QTcpSocket*>(sender())) {
        emit errorOccurred(socketErrorText(tcpSocket));
        return;
    }
    if (auto* udpSocket = qobject_cast<QUdpSocket*>(sender())) {
        emit errorOccurred(udpSocket->errorString());
    }
}

QString NetworkManager::peerName(const QTcpSocket* socket)
{
    if (!socket) {
        return QStringLiteral("-");
    }
    return QStringLiteral("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
}

QString NetworkManager::socketErrorText(const QTcpSocket* socket)
{
    return socket ? socket->errorString() : QStringLiteral("网络错误。");
}

QString NetworkManager::addressOrAny(const QString& address)
{
    const QString trimmed = address.trimmed();
    if (trimmed.isEmpty()) {
        return QStringLiteral("0.0.0.0");
    }
    return trimmed;
}

void NetworkManager::resetSockets(bool notifyPeers)
{
    for (QTcpSocket* client : std::as_const(m_serverClients)) {
        if (!client) {
            continue;
        }
        client->disconnect(this);
        client->close();
        client->deleteLater();
    }
    m_serverClients.clear();

    if (m_tcpClient) {
        m_tcpClient->disconnect(this);
        m_tcpClient->close();
        m_tcpClient->deleteLater();
        m_tcpClient = nullptr;
    }

    if (m_tcpServer) {
        m_tcpServer->disconnect(this);
        m_tcpServer->close();
        m_tcpServer->deleteLater();
        m_tcpServer = nullptr;
    }

    if (m_udpSocket) {
        m_udpSocket->disconnect(this);
        m_udpSocket->close();
        m_udpSocket->deleteLater();
        m_udpSocket = nullptr;
    }

    m_udpOpen = false;
    if (notifyPeers) {
        emitPeerListChanged();
    }
}

void NetworkManager::emitPeerListChanged()
{
    QStringList peers;
    for (const QTcpSocket* client : std::as_const(m_serverClients)) {
        if (client && client->state() == QAbstractSocket::ConnectedState) {
            peers << peerName(client);
        }
    }
    emit peerListChanged(peers);
}
