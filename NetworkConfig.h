#pragma once

#include <QSettings>
#include <QString>

struct NetworkConfig {
    int mode = 0;
    QString localAddress = QStringLiteral("0.0.0.0");
    quint16 localPort = 9000;
    QString remoteHost = QStringLiteral("127.0.0.1");
    quint16 remotePort = 9000;
    bool hexDisplay = false;
    bool hexSend = false;
    bool showTimestamp = false;
    bool autoScroll = true;
    QString encoding = QStringLiteral("UTF-8");

    void save(QSettings& settings) const {
        settings.setValue("network/mode", mode);
        settings.setValue("network/localAddress", localAddress);
        settings.setValue("network/localPort", localPort);
        settings.setValue("network/remoteHost", remoteHost);
        settings.setValue("network/remotePort", remotePort);
        settings.setValue("network/hexDisplay", hexDisplay);
        settings.setValue("network/hexSend", hexSend);
        settings.setValue("network/showTimestamp", showTimestamp);
        settings.setValue("network/autoScroll", autoScroll);
        settings.setValue("network/encoding", encoding);
    }

    void load(QSettings& settings) {
        mode = settings.value("network/mode", 0).toInt();
        localAddress = settings.value("network/localAddress", QStringLiteral("0.0.0.0")).toString();
        localPort = static_cast<quint16>(settings.value("network/localPort", 9000).toUInt());
        remoteHost = settings.value("network/remoteHost", QStringLiteral("127.0.0.1")).toString();
        remotePort = static_cast<quint16>(settings.value("network/remotePort", 9000).toUInt());
        hexDisplay = settings.value("network/hexDisplay", false).toBool();
        hexSend = settings.value("network/hexSend", false).toBool();
        showTimestamp = settings.value("network/showTimestamp", false).toBool();
        autoScroll = settings.value("network/autoScroll", true).toBool();
        encoding = settings.value("network/encoding", QStringLiteral("UTF-8")).toString();
    }
};
