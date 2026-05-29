#include "ThemeManager.h"

#include <QApplication>
#include <QStyle>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

QString ThemeManager::darkTheme() const
{
    return QStringLiteral(R"(
        QMainWindow, QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
        }
        QLabel {
            background-color: transparent;
            border: none;
            padding: 2px;
        }
        QGroupBox {
            background-color: #313244;
            border: 2px solid #45475a;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 15px;
            font-weight: bold;
            color: #89b4fa;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 8px;
            background-color: #313244;
        }
        QPushButton {
            background-color: #45475a;
            color: #cdd6f4;
            border: 2px solid #585b70;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            min-height: 25px;
        }
        QPushButton:hover {
            background-color: #585b70;
            border-color: #89b4fa;
        }
        QPushButton:pressed {
            background-color: #313244;
        }
        QPushButton:disabled {
            background-color: #1e1e2e;
            color: #585b70;
        }
        QPushButton#openButton {
            background-color: #a6e3a1;
            color: #1e1e2e;
            border-color: #a6e3a1;
        }
        QPushButton#openButton:hover {
            background-color: #94e2d5;
        }
        QPushButton#closeButton {
            background-color: #f38ba8;
            color: #1e1e2e;
            border-color: #f38ba8;
        }
        QPushButton#closeButton:hover {
            background-color: #eba0ac;
        }
        QPushButton#sendButton {
            background-color: #89b4fa;
            color: #1e1e2e;
            border-color: #89b4fa;
        }
        QPushButton#sendButton:hover {
            background-color: #74c7ec;
        }
        QComboBox {
            background-color: #45475a;
            color: #cdd6f4;
            border: 2px solid #585b70;
            border-radius: 6px;
            padding: 6px 12px;
            min-height: 25px;
        }
        QComboBox:hover {
            border-color: #89b4fa;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #cdd6f4;
            margin-right: 10px;
        }
        QComboBox QAbstractItemView {
            background-color: #313244;
            color: #cdd6f4;
            border: 2px solid #45475a;
            selection-background-color: #585b70;
        }
        QTextEdit {
            background-color: #181825;
            color: #a6e3a1;
            border: 2px solid #45475a;
            border-radius: 6px;
            padding: 8px;
            font-family: "Cascadia Code", "Consolas", "Courier New", monospace;
            font-size: 12px;
            selection-background-color: #585b70;
        }
        QTextEdit:focus {
            border-color: #89b4fa;
        }
        QCheckBox {
            color: #cdd6f4;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #585b70;
            border-radius: 4px;
            background-color: #313244;
        }
        QCheckBox::indicator:checked {
            background-color: #89b4fa;
            border-color: #89b4fa;
        }
        QCheckBox::indicator:hover {
            border-color: #89b4fa;
        }
        QSpinBox {
            background-color: #45475a;
            color: #cdd6f4;
            border: 2px solid #585b70;
            border-radius: 6px;
            padding: 6px;
            min-height: 25px;
        }
        QSpinBox:focus {
            border-color: #89b4fa;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #585b70;
            border: none;
            width: 20px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background-color: #89b4fa;
        }
        QListWidget {
            background-color: #181825;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-radius: 6px;
            padding: 4px;
            alternate-background-color: #1e1e2e;
        }
        QListWidget::item {
            padding: 6px 8px;
            border-radius: 4px;
        }
        QListWidget::item:selected {
            background-color: #585b70;
            color: #cdd6f4;
        }
        QListWidget::item:hover {
            background-color: #45475a;
        }
        QSplitter::handle {
            background-color: #45475a;
            width: 4px;
            border-radius: 2px;
        }
        QSplitter::handle:hover {
            background-color: #89b4fa;
        }
        QTabWidget::pane {
            background-color: #1e1e2e;
            border: 2px solid #45475a;
            border-top: none;
        }
        QTabBar::tab {
            background-color: #313244;
            color: #cdd6f4;
            border: 2px solid #45475a;
            border-bottom: none;
            padding: 10px 20px;
            margin-right: 2px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            font-weight: bold;
        }
        QTabBar::tab:selected {
            background-color: #45475a;
            color: #89b4fa;
            border-color: #89b4fa;
        }
        QTabBar::tab:hover:!selected {
            background-color: #585b70;
        }
        QTabBar::close-button {
            image: none;
            subcontrol-position: right;
            padding: 4px;
        }
        QStatusBar {
            background-color: #313244;
            color: #cdd6f4;
            border-top: 2px solid #45475a;
        }
        QStatusBar::item {
            border: none;
        }
        QStatusBar QLabel {
            padding: 4px 12px;
            border-right: 1px solid #45475a;
        }
        QScrollBar:vertical {
            background-color: #1e1e2e;
            width: 12px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background-color: #45475a;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #585b70;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background-color: #1e1e2e;
            height: 12px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background-color: #45475a;
            border-radius: 6px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background-color: #585b70;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QLabel#statusConnected {
            color: #a6e3a1;
            font-weight: bold;
        }
        QLabel#statusDisconnected {
            color: #f38ba8;
            font-weight: bold;
        }
        QLabel#rxLabel {
            color: #89b4fa;
            font-weight: bold;
        }
        QLabel#txLabel {
            color: #f9e2af;
            font-weight: bold;
        }
        QToolBar {
            background-color: #313244;
            border-bottom: 2px solid #45475a;
            spacing: 8px;
            padding: 4px;
        }
        QToolButton {
            background-color: #45475a;
            color: #cdd6f4;
            border: 2px solid #585b70;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QToolButton:hover {
            background-color: #585b70;
            border-color: #89b4fa;
        }
    )");
}

QString ThemeManager::lightTheme() const
{
    return QStringLiteral(R"(
        QMainWindow, QWidget {
            background-color: #eff1f5;
            color: #4c4f69;
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
        }
        QLabel {
            background-color: transparent;
            border: none;
            padding: 2px;
        }
        QGroupBox {
            background-color: #e6e9ef;
            border: 2px solid #ccd0da;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 15px;
            font-weight: bold;
            color: #1e66f5;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 8px;
            background-color: #e6e9ef;
        }
        QPushButton {
            background-color: #ccd0da;
            color: #4c4f69;
            border: 2px solid #bcc0cc;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            min-height: 25px;
        }
        QPushButton:hover {
            background-color: #bcc0cc;
            border-color: #1e66f5;
        }
        QPushButton:pressed {
            background-color: #acb0be;
        }
        QPushButton:disabled {
            background-color: #e6e9ef;
            color: #bcc0cc;
        }
        QPushButton#openButton {
            background-color: #40a02b;
            color: #ffffff;
            border-color: #40a02b;
        }
        QPushButton#openButton:hover {
            background-color: #26a049;
        }
        QPushButton#closeButton {
            background-color: #d20f39;
            color: #ffffff;
            border-color: #d20f39;
        }
        QPushButton#closeButton:hover {
            background-color: #e02040;
        }
        QPushButton#sendButton {
            background-color: #1e66f5;
            color: #ffffff;
            border-color: #1e66f5;
        }
        QPushButton#sendButton:hover {
            background-color: #2a7aff;
        }
        QComboBox {
            background-color: #e6e9ef;
            color: #4c4f69;
            border: 2px solid #ccd0da;
            border-radius: 6px;
            padding: 6px 12px;
            min-height: 25px;
        }
        QComboBox:hover {
            border-color: #1e66f5;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #4c4f69;
            margin-right: 10px;
        }
        QComboBox QAbstractItemView {
            background-color: #e6e9ef;
            color: #4c4f69;
            border: 2px solid #ccd0da;
            selection-background-color: #bcc0cc;
        }
        QTextEdit {
            background-color: #dce0e8;
            color: #4c4f69;
            border: 2px solid #ccd0da;
            border-radius: 6px;
            padding: 8px;
            font-family: "Cascadia Code", "Consolas", "Courier New", monospace;
            font-size: 12px;
            selection-background-color: #bcc0cc;
        }
        QTextEdit:focus {
            border-color: #1e66f5;
        }
        QCheckBox {
            color: #4c4f69;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #bcc0cc;
            border-radius: 4px;
            background-color: #e6e9ef;
        }
        QCheckBox::indicator:checked {
            background-color: #1e66f5;
            border-color: #1e66f5;
        }
        QCheckBox::indicator:hover {
            border-color: #1e66f5;
        }
        QSpinBox {
            background-color: #e6e9ef;
            color: #4c4f69;
            border: 2px solid #ccd0da;
            border-radius: 6px;
            padding: 6px;
            min-height: 25px;
        }
        QSpinBox:focus {
            border-color: #1e66f5;
        }
        QListWidget {
            background-color: #dce0e8;
            color: #4c4f69;
            border: 2px solid #ccd0da;
            border-radius: 6px;
            padding: 4px;
            alternate-background-color: #e6e9ef;
        }
        QListWidget::item {
            padding: 6px 8px;
            border-radius: 4px;
        }
        QListWidget::item:selected {
            background-color: #bcc0cc;
            color: #4c4f69;
        }
        QListWidget::item:hover {
            background-color: #ccd0da;
        }
        QSplitter::handle {
            background-color: #ccd0da;
            width: 4px;
            border-radius: 2px;
        }
        QSplitter::handle:hover {
            background-color: #1e66f5;
        }
        QTabWidget::pane {
            background-color: #eff1f5;
            border: 2px solid #ccd0da;
            border-top: none;
        }
        QTabBar::tab {
            background-color: #e6e9ef;
            color: #4c4f69;
            border: 2px solid #ccd0da;
            border-bottom: none;
            padding: 10px 20px;
            margin-right: 2px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            font-weight: bold;
        }
        QTabBar::tab:selected {
            background-color: #ccd0da;
            color: #1e66f5;
            border-color: #1e66f5;
        }
        QTabBar::tab:hover:!selected {
            background-color: #bcc0cc;
        }
        QStatusBar {
            background-color: #e6e9ef;
            color: #4c4f69;
            border-top: 2px solid #ccd0da;
        }
        QStatusBar QLabel {
            padding: 4px 12px;
            border-right: 1px solid #ccd0da;
        }
        QScrollBar:vertical {
            background-color: #eff1f5;
            width: 12px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background-color: #ccd0da;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #bcc0cc;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background-color: #eff1f5;
            height: 12px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background-color: #ccd0da;
            border-radius: 6px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background-color: #bcc0cc;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QLabel#statusConnected {
            color: #40a02b;
            font-weight: bold;
        }
        QLabel#statusDisconnected {
            color: #d20f39;
            font-weight: bold;
        }
        QLabel#rxLabel {
            color: #1e66f5;
            font-weight: bold;
        }
        QLabel#txLabel {
            color: #df8e1d;
            font-weight: bold;
        }
        QToolBar {
            background-color: #e6e9ef;
            border-bottom: 2px solid #ccd0da;
            spacing: 8px;
            padding: 4px;
        }
        QToolButton {
            background-color: #ccd0da;
            color: #4c4f69;
            border: 2px solid #bcc0cc;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QToolButton:hover {
            background-color: #bcc0cc;
            border-color: #1e66f5;
        }
    )");
}

void ThemeManager::applyDarkTheme()
{
    qApp->setStyleSheet(darkTheme());
}

void ThemeManager::applyLightTheme()
{
    qApp->setStyleSheet(lightTheme());
}
