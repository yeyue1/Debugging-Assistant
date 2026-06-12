#pragma once

#include <QString>

// 集中管理主题颜色常量
// 从 ThemeManager、SerialPanel、NetworkPanel 中提取的硬编码颜色
// 暗色主题使用 Catppuccin Mocha 调色板，亮色主题使用 Catppuccin Latte 调色板
namespace ThemeColors
{

// ── 状态指示器颜色 ──────────────────────────────────────────────────────
// 用于 statusIndicator 的背景色和 statusLabel 的文字色

namespace Dark
{
    // 状态指示器
    inline QString statusConnected()    { return QStringLiteral("#a6e3a1"); }
    inline QString statusDisconnected() { return QStringLiteral("#f38ba8"); }
    inline QString statusBorder()       { return QStringLiteral("#45475a"); }

    // RX / TX 标签
    inline QString rxLabel() { return QStringLiteral("#89b4fa"); }
    inline QString txLabel() { return QStringLiteral("#f9e2af"); }

    // 接收区显示颜色
    inline QString rxDefault()    { return QStringLiteral("#cdd6f4"); }  // 普通接收
    inline QString txDisplay()    { return QStringLiteral("#89b4fa"); }  // 发送数据
    inline QString errorDisplay() { return QStringLiteral("#f38ba8"); }  // 错误信息
    inline QString filterMatch()  { return QStringLiteral("#f9e2af"); }  // 过滤匹配
    inline QString systemDisplay(){ return QStringLiteral("#a6adc8"); }  // 系统消息
} // namespace Dark

namespace Light
{
    // 状态指示器
    inline QString statusConnected()    { return QStringLiteral("#40a02b"); }
    inline QString statusDisconnected() { return QStringLiteral("#d20f39"); }
    inline QString statusBorder()       { return QStringLiteral("#45475a"); }

    // RX / TX 标签
    inline QString rxLabel() { return QStringLiteral("#1e66f5"); }
    inline QString txLabel() { return QStringLiteral("#df8e1d"); }

    // 接收区显示颜色（亮色主题下使用深色文字）
    inline QString rxDefault()    { return QStringLiteral("#4c4f69"); }
    inline QString txDisplay()    { return QStringLiteral("#1e66f5"); }
    inline QString errorDisplay() { return QStringLiteral("#d20f39"); }
    inline QString filterMatch()  { return QStringLiteral("#df8e1d"); }
    inline QString systemDisplay(){ return QStringLiteral("#6c6f85"); }
} // namespace Light

} // namespace ThemeColors
