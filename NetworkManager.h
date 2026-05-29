#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QList>

class QTcpServer;
class QTcpSocket;
class QUdpSocket;

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum class Mode {
        TcpClient = 0,
        TcpServer = 1,
        Udp = 2
    };

    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager() override;

    bool isOpen() const;
    Mode mode() const;

public slots:
    void openTcpClient(const QString& host, quint16 port, const QString& localAddress, quint16 localPort);
    void openTcpServer(const QString& localAddress, quint16 port);
    void openUdp(const QString& localAddress, quint16 port);
    void close();
    qint64 sendData(const QByteArray& data, const QString& remoteHost, quint16 remotePort, const QString& peer = QString());

signals:
    void opened();
    void closed();
    void dataReceived(const QByteArray& payload, const QString& peer);
    void errorOccurred(const QString& message);
    void peerListChanged(const QStringList& peers);

private slots:
    void handleTcpClientReadyRead();
    void handleTcpServerNewConnection();
    void handleServerClientReadyRead();
    void handleServerClientDisconnected();
    void handleUdpReadyRead();
    void handleSocketError();

private:
    static QString peerName(const QTcpSocket* socket);
    static QString socketErrorText(const QTcpSocket* socket);
    static QString addressOrAny(const QString& address);

    void resetSockets(bool notifyPeers = true);
    void emitPeerListChanged();

    QTcpSocket* m_tcpClient = nullptr;
    QTcpServer* m_tcpServer = nullptr;
    QUdpSocket* m_udpSocket = nullptr;
    QList<QTcpSocket*> m_serverClients;
    Mode m_mode = Mode::TcpClient;
    bool m_udpOpen = false;
};
