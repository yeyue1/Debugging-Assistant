#pragma once

// PanelHelper - SerialPanel and NetworkPanel 共用的 UI 辅助函数
// 从两个面板的匿名命名空间中提取，消除重复代码

#include <QBoxLayout>
#include <QCheckBox>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QTableWidget>
#include <QToolButton>
#include <QWidget>
#include <QtGlobal>

namespace PanelHelper {

// 在 layout 中 anchor 控件之后插入 widget
inline void insertAfter(QBoxLayout* layout, QWidget* anchor, QWidget* widget)
{
    const int index = layout ? layout->indexOf(anchor) : -1;
    if (index >= 0) {
        layout->insertWidget(index + 1, widget);
    } else if (layout) {
        layout->addWidget(widget);
    }
}

// 创建带文字弹出菜单按钮
inline QToolButton* createMenuButton(QWidget* parent, const QString& text)
{
    auto* button = new QToolButton(parent);
    button->setText(text);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    return button;
}

// 创建只读表格单元格
inline QTableWidgetItem* readOnlyTableItem(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

// 创建可编辑表格单元格
inline QTableWidgetItem* editableTableItem(const QString& text)
{
    return new QTableWidgetItem(text);
}

// 将队列文本中的换行符转为可见字符（用于显示）
inline QString visibleQueueText(QString text)
{
    text.replace(QLatin1Char('\r'), QStringLiteral("\\r"));
    text.replace(QLatin1Char('\n'), QStringLiteral("\\n"));
    return text;
}

// 将可见字符还原为实际换行符（用于解析）
inline QString queueTextFromVisible(QString text)
{
    text.replace(QStringLiteral("\\r"), QStringLiteral("\r"));
    text.replace(QStringLiteral("\\n"), QStringLiteral("\n"));
    return text;
}

// 解析单元格中的正整数值，失败时返回 fallback
inline int positiveCellValue(const QString& text, int fallback, int minimum, int maximum)
{
    bool ok = false;
    const int value = text.trimmed().toInt(&ok);
    return ok ? qBound(minimum, value, maximum) : fallback;
}

// 向菜单添加与 QCheckBox 双向绑定的可勾选动作
inline QAction* addCheckAction(QMenu* menu, const QString& text, QCheckBox* checkBox)
{
    QAction* action = menu->addAction(text);
    action->setCheckable(true);
    action->setChecked(checkBox->isChecked());
    QObject::connect(action, &QAction::toggled, checkBox, &QCheckBox::setChecked);
    QObject::connect(checkBox, &QCheckBox::toggled, action, &QAction::setChecked);
    return action;
}

} // namespace PanelHelper
