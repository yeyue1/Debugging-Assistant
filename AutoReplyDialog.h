#pragma once

#include <QDialog>
#include <QList>

#include "AutomationRuleEngine.h"

class QTableWidget;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QComboBox;

// 自动回复规则管理对话框
// 支持添加、编辑、删除多条规则
class AutoReplyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoReplyDialog(AutomationRuleEngine* engine, QWidget* parent = nullptr);

private slots:
    void addRule();
    void removeRule();
    void editRule();
    void onSelectionChanged();
    void onAccepted();

private:
    void refreshTable();
    void loadFromEngine();
    void editRuleAtIndex(int index);

    AutomationRuleEngine* m_engine;
    QTableWidget* m_ruleTable = nullptr;
    QPushButton* m_addButton = nullptr;
    QPushButton* m_removeButton = nullptr;
    QPushButton* m_editButton = nullptr;
    QList<AutoReplyRule> m_rules; // 临时编辑副本
};
