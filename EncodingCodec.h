#pragma once

// EncodingCodec - 编码转换工具类
// 统一 SerialPanel 和 NetworkPanel 中重复的 GBK/ASCII/UTF-8 编解码逻辑

#include <QByteArray>
#include <QString>

namespace EncodingCodec {

// 将字节数据按指定编码解码为字符串
// 支持编码: "GBK"、"ASCII"、其他默认 UTF-8
inline QString decodeBytes(const QByteArray& data, const QString& encoding)
{
    if (encoding == QStringLiteral("GBK")) {
        return QString::fromLocal8Bit(data);
    }
    if (encoding == QStringLiteral("ASCII")) {
        return QString::fromLatin1(data);
    }
    return QString::fromUtf8(data);
}

// 将字符串按指定编码编码为字节数据
// 支持编码: "GBK"、"ASCII"、其他默认 UTF-8
inline QByteArray encodeText(const QString& text, const QString& encoding)
{
    if (encoding == QStringLiteral("GBK")) {
        return text.toLocal8Bit();
    }
    if (encoding == QStringLiteral("ASCII")) {
        return text.toLatin1();
    }
    return text.toUtf8();
}

} // namespace EncodingCodec
