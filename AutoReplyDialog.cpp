#include "AutoReplyDialog.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

AutoReplyDialog::AutoReplyDialog(AutomationRuleEngine* engine, QWidget* parent)
    : QDialog(parent), m_engine(engine)
{
    setWindowTitle(tr("自动回复规则管理"));
    setMinimumSize(800, 400);

    loadFromEngine();

    auto* mainLayout = new QVBoxLayout(this);

    // 说明文字
    auto* infoLabel = new QLabel(tr("配置多条自动回复规则。匹配到接收数据时，将自动发送对应的回复内容。\n"
                                     "支持 HEX 格式（如 AABBCC）和文本格式，编码可选 UTF-8 或 GBK。"));
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    // 规则表格
    m_ruleTable = new QTableWidget(this);
    m_ruleTable->setColumnCount(8);
    m_ruleTable->setHorizontalHeaderLabels({
        tr("启用"), tr("匹配模式"), tr("回复内容"),
        tr("HEX"), tr("编码"), tr("正则"),
        tr("冷却(ms)"), tr("最大次数")
    });
    m_ruleTable->horizontalHeader()->setStretchLastSection(true);
    m_ruleTable->setSelectionBehavior(QTableWidget::SelectRows);
    m_ruleTable->setSelectionMode(QTableWidget::SingleSelection);
    m_ruleTable->setEditTriggers(QTableWidget::DoubleClicked | QTableWidget::SelectedClicked);
    m_ruleTable->verticalHeader()->hide();
    m_ruleTable->setColumnWidth(0, 40);   // 启用
    m_ruleTable->setColumnWidth(1, 150);  // 匹配模式
    m_ruleTable->setColumnWidth(2, 180);  // 回复内容
    m_ruleTable->setColumnWidth(3, 50);   // HEX
    m_ruleTable->setColumnWidth(4, 80);   // 编码
    m_ruleTable->setColumnWidth(5, 50);   // 正则
    m_ruleTable->setColumnWidth(6, 90);   // 冷却
    mainLayout->addWidget(m_ruleTable);

    // 按钮栏
    auto* buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton(tr("添加"), this);
    m_editButton = new QPushButton(tr("编辑"), this);
    m_removeButton = new QPushButton(tr("删除"), this);
    m_editButton->setEnabled(false);
    m_removeButton->setEnabled(false);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addStretch();

    auto* okButton = new QPushButton(tr("确定"), this);
    auto* cancelButton = new QPushButton(tr("取消"), this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // 信号连接
    connect(m_addButton, &QPushButton::clicked, this, &AutoReplyDialog::addRule);
    connect(m_editButton, &QPushButton::clicked, this, &AutoReplyDialog::editRule);
    connect(m_removeButton, &QPushButton::clicked, this, &AutoReplyDialog::removeRule);
    connect(m_ruleTable, &QTableWidget::itemSelectionChanged, this, &AutoReplyDialog::onSelectionChanged);
    connect(okButton, &QPushButton::clicked, this, &AutoReplyDialog::onAccepted);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // 暗色主题适配
    setStyleSheet(R"(
        QTableWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
            gridline-color: #45475a;
            border: 1px solid #45475a;
            selection-background-color: #585b70;
            font-size: 13px;
        }
        QTableWidget::item { padding: 4px; }
        QTableWidget::item:selected { background-color: #585b70; color: #cdd6f4; }
        QHeaderView::section {
            background-color: #313244; color: #cdd6f4;
            border: 1px solid #45475a; padding: 6px;
            font-weight: bold; font-size: 13px;
        }
        QCheckBox { spacing: 6px; }
        QCheckBox::indicator {
            width: 16px; height: 16px;
            border: 2px solid #585b70; border-radius: 3px;
            background-color: #1e1e2e;
        }
        QCheckBox::indicator:checked { background-color: #89b4fa; border-color: #89b4fa; }
        QLineEdit, QSpinBox, QComboBox {
            background-color: #313244; color: #cdd6f4;
            border: 1px solid #45475a; border-radius: 4px;
            padding: 4px 8px; font-size: 13px;
        }
        QLabel { color: #cdd6f4; font-size: 13px; }
        QPushButton {
            background-color: #313244; color: #cdd6f4;
            border: 1px solid #45475a; border-radius: 4px;
            padding: 6px 16px; font-size: 13px;
        }
        QPushButton:hover { background-color: #45475a; }
        QPushButton:pressed { background-color: #585b70; }
    )");

    refreshTable();
}

void AutoReplyDialog::loadFromEngine()
{
    m_rules = m_engine->rules();
}

void AutoReplyDialog::refreshTable()
{
    m_ruleTable->setRowCount(m_rules.size());

    for (int i = 0; i < m_rules.size(); ++i) {
        const AutoReplyRule& rule = m_rules[i];

        // 启用
        auto* enableCheck = new QCheckBox();
        enableCheck->setChecked(rule.enabled);
        m_ruleTable->setCellWidget(i, 0, enableCheck);

        // 匹配模式（可编辑）
        m_ruleTable->setItem(i, 1, new QTableWidgetItem(rule.pattern));

        // 回复内容（可编辑）
        m_ruleTable->setItem(i, 2, new QTableWidgetItem(rule.replyText));

        // HEX
        auto* hexCheck = new QCheckBox();
        hexCheck->setChecked(rule.isHexReply);
        m_ruleTable->setCellWidget(i, 3, hexCheck);

        // 编码
        auto* encodingCombo = new QComboBox();
        encodingCombo->addItems({"UTF-8", "GBK"});
        encodingCombo->setCurrentText(rule.encoding);
        m_ruleTable->setCellWidget(i, 4, encodingCombo);

        // 正则
        auto* regexCheck = new QCheckBox();
        regexCheck->setChecked(rule.useRegex);
        m_ruleTable->setCellWidget(i, 5, regexCheck);

        // 冷却
        auto* cooldownSpin = new QSpinBox();
        cooldownSpin->setRange(0, 60000);
        cooldownSpin->setValue(rule.cooldownMs);
        cooldownSpin->setSuffix(tr(" ms"));
        m_ruleTable->setCellWidget(i, 6, cooldownSpin);

        // 最大次数
        auto* maxSpin = new QSpinBox();
        maxSpin->setRange(0, 10000);
        maxSpin->setValue(rule.maxTriggerCount);
        maxSpin->setSpecialValueText(tr("不限"));
        m_ruleTable->setCellWidget(i, 7, maxSpin);
    }

    m_ruleTable->resizeColumnsToContents();
}

void AutoReplyDialog::onSelectionChanged()
{
    bool hasSelection = !m_ruleTable->selectedItems().isEmpty();
    m_editButton->setEnabled(hasSelection);
    m_removeButton->setEnabled(hasSelection);
}

void AutoReplyDialog::addRule()
{
    AutoReplyRule rule;
    rule.enabled = true;
    rule.pattern = tr(".*");
    rule.replyText = tr("OK");
    rule.useRegex = false;
    rule.isHexReply = false;
    rule.encoding = "UTF-8";
    rule.cooldownMs = 500;
    rule.maxTriggerCount = 10;
    rule.onceOnly = false;

    m_rules.append(rule);
    refreshTable();
    m_ruleTable->selectRow(m_rules.size() - 1);
}

void AutoReplyDialog::removeRule()
{
    int row = m_ruleTable->currentRow();
    if (row < 0 || row >= m_rules.size()) return;
    m_rules.removeAt(row);
    refreshTable();
}

void AutoReplyDialog::editRule()
{
    int row = m_ruleTable->currentRow();
    if (row < 0 || row >= m_rules.size()) return;
    m_ruleTable->editItem(m_ruleTable->item(row, 1));
}

void AutoReplyDialog::onAccepted()
{
    // 从表格读取所有规则的最终状态
    for (int i = 0; i < m_rules.size(); ++i) {
        auto* enableCheck = qobject_cast<QCheckBox*>(m_ruleTable->cellWidget(i, 0));
        auto* hexCheck = qobject_cast<QCheckBox*>(m_ruleTable->cellWidget(i, 3));
        auto* encodingCombo = qobject_cast<QComboBox*>(m_ruleTable->cellWidget(i, 4));
        auto* regexCheck = qobject_cast<QCheckBox*>(m_ruleTable->cellWidget(i, 5));
        auto* cooldownSpin = qobject_cast<QSpinBox*>(m_ruleTable->cellWidget(i, 6));
        auto* maxSpin = qobject_cast<QSpinBox*>(m_ruleTable->cellWidget(i, 7));

        if (enableCheck) m_rules[i].enabled = enableCheck->isChecked();
        if (m_ruleTable->item(i, 1)) m_rules[i].pattern = m_ruleTable->item(i, 1)->text();
        if (m_ruleTable->item(i, 2)) m_rules[i].replyText = m_ruleTable->item(i, 2)->text();
        if (hexCheck) m_rules[i].isHexReply = hexCheck->isChecked();
        if (encodingCombo) m_rules[i].encoding = encodingCombo->currentText();
        if (regexCheck) m_rules[i].useRegex = regexCheck->isChecked();
        if (cooldownSpin) m_rules[i].cooldownMs = cooldownSpin->value();
        if (maxSpin) m_rules[i].maxTriggerCount = maxSpin->value();
    }

    // 写回 engine
    m_engine->mutableRules() = m_rules;
    emit m_engine->rulesChanged();

    accept();
}
