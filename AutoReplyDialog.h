#pragma once

#include <QDialog>
#include <QList>

#include "AutomationRuleEngine.h"

class QTableWidget;
class QPushButton;

class AutoReplyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoReplyDialog(AutomationRuleEngine* engine, QWidget* parent = nullptr);

private slots:
    void addRule();
    void removeRule();
    void onSelectionChanged();
    void onAccepted();

private:
    void refreshTable();
    void saveTableToRules();
    void loadFromEngine();

    AutomationRuleEngine* m_engine;
    QTableWidget* m_ruleTable = nullptr;
    QPushButton* m_addButton = nullptr;
    QPushButton* m_removeButton = nullptr;
    QList<AutoReplyRule> m_rules;
};
