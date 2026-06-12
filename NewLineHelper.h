#pragma once

// NewLineHelper - 换行模式处理工具类
// 将 SerialPanel 中重复出现的 switch(m_newLineMode) 逻辑集中管理
// 模式: 0=无, 1=\n, 2=\r\n, 3=\r

#include <QString>

namespace NewLineHelper {

// 根据换行模式在文本末尾追加对应的换行符
inline void appendNewLine(QString& text, int newLineMode)
{
    switch (newLineMode) {
    case 1: text += QStringLiteral("\n"); break;
    case 2: text += QStringLiteral("\r\n"); break;
    case 3: text += QStringLiteral("\r"); break;
    default: break;
    }
}

} // namespace NewLineHelper
