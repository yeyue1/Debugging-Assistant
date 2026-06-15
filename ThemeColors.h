#pragma once

#include <QString>
#include <QApplication>

// 集中管理主题颜色常量
// 暗色主题使用 Catppuccin Mocha 调色板，亮色主题使用 Catppuccin Latte 调色板
namespace ThemeColors
{

// ── 暗色主题 ──────────────────────────────────────────────────────────────

namespace Dark
{
    inline QString statusConnected()    { return QStringLiteral("#a6e3a1"); }
    inline QString statusDisconnected() { return QStringLiteral("#f38ba8"); }
    inline QString statusBorder()       { return QStringLiteral("#45475a"); }
    inline QString rxLabel() { return QStringLiteral("#89b4fa"); }
    inline QString txLabel() { return QStringLiteral("#f9e2af"); }
    inline QString rxDefault()    { return QStringLiteral("#cdd6f4"); }
    inline QString txDisplay()    { return QStringLiteral("#89b4fa"); }
    inline QString errorDisplay() { return QStringLiteral("#f38ba8"); }
    inline QString filterMatch()  { return QStringLiteral("#f9e2af"); }
    inline QString systemDisplay(){ return QStringLiteral("#a6adc8"); }
    inline QString sentDisplay()  { return QStringLiteral("#a6e3a1"); }
} // namespace Dark

// ── 亮色主题 ──────────────────────────────────────────────────────────────

namespace Light
{
    inline QString statusConnected()    { return QStringLiteral("#40a02b"); }
    inline QString statusDisconnected() { return QStringLiteral("#d20f39"); }
    inline QString statusBorder()       { return QStringLiteral("#bcc0cc"); }
    inline QString rxLabel() { return QStringLiteral("#1e66f5"); }
    inline QString txLabel() { return QStringLiteral("#df8e1d"); }
    inline QString rxDefault()    { return QStringLiteral("#4c4f69"); }
    inline QString txDisplay()    { return QStringLiteral("#1e66f5"); }
    inline QString errorDisplay() { return QStringLiteral("#d20f39"); }
    inline QString filterMatch()  { return QStringLiteral("#df8e1d"); }
    inline QString systemDisplay(){ return QStringLiteral("#6c6f85"); }
    inline QString sentDisplay()  { return QStringLiteral("#40a02b"); }
} // namespace Light

// ── 主题感知的当前颜色（自动根据应用主题切换）────────────────────────────

namespace Current
{
    inline bool isDarkTheme() {
        if (!qApp) return true;
        // 检查应用程序背景色来判断主题
        QPalette pal = qApp->palette();
        return pal.color(QPalette::Window).lightness() < 128;
    }

    inline QString statusConnected()    { return isDarkTheme() ? Dark::statusConnected()    : Light::statusConnected(); }
    inline QString statusDisconnected() { return isDarkTheme() ? Dark::statusDisconnected() : Light::statusDisconnected(); }
    inline QString statusBorder()       { return isDarkTheme() ? Dark::statusBorder()       : Light::statusBorder(); }
    inline QString rxLabel() { return isDarkTheme() ? Dark::rxLabel() : Light::rxLabel(); }
    inline QString txLabel() { return isDarkTheme() ? Dark::txLabel() : Light::txLabel(); }
    inline QString rxDefault()    { return isDarkTheme() ? Dark::rxDefault()    : Light::rxDefault(); }
    inline QString txDisplay()    { return isDarkTheme() ? Dark::txDisplay()    : Light::txDisplay(); }
    inline QString errorDisplay() { return isDarkTheme() ? Dark::errorDisplay() : Light::errorDisplay(); }
    inline QString filterMatch()  { return isDarkTheme() ? Dark::filterMatch()  : Light::filterMatch(); }
    inline QString systemDisplay(){ return isDarkTheme() ? Dark::systemDisplay(): Light::systemDisplay(); }
    inline QString sentDisplay()  { return isDarkTheme() ? Dark::sentDisplay()  : Light::sentDisplay(); }
} // namespace Current

} // namespace ThemeColors
