#include "DeviceRegistry.h"

#include <QSerialPortInfo>
#include <QSettings>

DeviceRegistry::DeviceRegistry(QObject* parent)
    : QObject(parent)
{
}

QString DeviceRegistry::deviceKeyForPort(const QString& portName) const
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const auto& info : infos) {
        if (info.portName() != portName) {
            continue;
        }

        if (info.hasVendorIdentifier() && info.hasProductIdentifier()) {
            return QStringLiteral("usb:%1:%2:%3")
                .arg(info.vendorIdentifier(), 4, 16, QLatin1Char('0'))
                .arg(info.productIdentifier(), 4, 16, QLatin1Char('0'))
                .arg(info.serialNumber());
        }

        const QString descriptor = QStringList{
            info.description(),
            info.manufacturer(),
            info.serialNumber()
        }.join(QStringLiteral("|"));

        if (!descriptor.trimmed().isEmpty()) {
            return QStringLiteral("desc:%1").arg(descriptor);
        }

        return QStringLiteral("port:%1").arg(portName);
    }

    return QString();
}

QString DeviceRegistry::deviceSummaryForPort(const QString& portName) const
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const auto& info : infos) {
        if (info.portName() != portName) {
            continue;
        }

        QStringList parts;
        parts << info.portName();
        if (info.hasVendorIdentifier() && info.hasProductIdentifier()) {
            parts << QStringLiteral("VID:%1 PID:%2")
                         .arg(info.vendorIdentifier(), 4, 16, QLatin1Char('0'))
                         .arg(info.productIdentifier(), 4, 16, QLatin1Char('0'))
                         .toUpper();
        }
        if (!info.description().isEmpty()) {
            parts << info.description();
        }
        if (!info.manufacturer().isEmpty()) {
            parts << info.manufacturer();
        }
        if (!info.serialNumber().isEmpty()) {
            parts << info.serialNumber();
        }
        return parts.join(QStringLiteral(" | "));
    }

    return portName;
}

QString DeviceRegistry::aliasForKey(const QString& key) const
{
    if (key.isEmpty()) {
        return QString();
    }

    QSettings settings(QStringLiteral("yeyue"), QStringLiteral("serial_prot"));
    settings.beginGroup(QStringLiteral("deviceAliases"));
    const QString alias = settings.value(key).toString();
    settings.endGroup();
    return alias;
}

void DeviceRegistry::saveAlias(const QString& key, const QString& alias)
{
    if (key.isEmpty()) {
        return;
    }

    QSettings settings(QStringLiteral("yeyue"), QStringLiteral("serial_prot"));
    settings.beginGroup(QStringLiteral("deviceAliases"));
    settings.setValue(key, alias);
    settings.endGroup();
}

QString DeviceRegistry::findPortForKey(const QString& key) const
{
    if (key.isEmpty()) {
        return QString();
    }

    const auto infos = QSerialPortInfo::availablePorts();
    for (const auto& info : infos) {
        if (deviceKeyForPort(info.portName()) == key) {
            return info.portName();
        }
    }

    return QString();
}
