#pragma once

#include <QObject>
#include <QString>
#include <QSerialPortInfo>

class DeviceRegistry : public QObject
{
    Q_OBJECT

public:
    explicit DeviceRegistry(QObject* parent = nullptr);

    // 设备识别 key 生成（优先级：USB VID/PID/序列号 > 描述/厂商/序列号 > 端口名）
    QString deviceKeyForPort(const QString& portName) const;

    // 设备摘要文本（用于界面显示）
    QString deviceSummaryForPort(const QString& portName) const;

    // 别名管理（读写 QSettings）
    QString aliasForKey(const QString& key) const;
    void saveAlias(const QString& key, const QString& alias);

    // 根据设备 key 查找当前可用端口名（一键复连用）
    QString findPortForKey(const QString& key) const;
};
