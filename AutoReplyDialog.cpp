#include "AutoReplyDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

AutoReplyDialog::AutoReplyDialog(AutomationRuleEngine* engine, QWidget* parent)
    : QDialog(parent), m_engine(engine)
{
    setWindowTitle(tr("自动回复规则管理"));
    setMinimumSize(900, 450);

    loadFromEngine();

    auto* mainLayout = new QVBoxLayout(this);

    auto* infoLabel = new QLabel(tr("配置多条自动回复规则。匹配到接收数据时，将自动发送对应的回复内容。\n"
                                     "编码格式：UTF-8 / GBK / ASCII（文本模式）或 HEX（原始字节模式）。"));
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    // 规则表格
    m_ruleTable = new QTableWidget(this);
    m_ruleTable->setColumnCount(8);
    m_ruleTable->setHorizontalHeaderLabels({
        tr("启用"), tr("匹配模式"), tr("回复内容"),
        tr("编码"), tr("正则"), tr("冷却(ms)"), tr("最大次数"), tr("已触发")
    });
    m_ruleTable->horizontalHeader()->setStretchLastSection(true);
    m_ruleTable->setSelectionBehavior(QTableWidget::SelectRows);
    m_ruleTable->setSelectionMode(QTableWidget::SingleSelection);
    m_ruleTable->setEditTriggers(QTableWidget::DoubleClicked | QTableWidget::SelectedClicked);
    m_ruleTable->verticalHeader()->hide();
    m_ruleTable->verticalHeader()->setDefaultSectionSize(36);
    m_ruleTable->setColumnWidth(0, 45);
    m_ruleTable->setColumnWidth(1, 180);
    m_ruleTable->setColumnWidth(2, 200);
    m_ruleTable->setColumnWidth(3, 100);
    m_ruleTable->setColumnWidth(4, 50);
    m_ruleTable->setColumnWidth(5, 100);
    m_ruleTable->setColumnWidth(6, 100);
    m_ruleTable->setColumnWidth(7, 80);
    mainLayout->addWidget(m_ruleTable);

    // 按钮栏
    auto* buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton(tr("添加"), this);
    m_removeButton = new QPushButton(tr("删除"), this);
    m_removeButton->setEnabled(false);
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addStretch();

    auto* okButton = new QPushButton(tr("确定"), this);
    auto* cancelButton = new QPushButton(tr("取消"), this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(m_addButton, &QPushButton::clicked, this, &AutoReplyDialog::addRule);
    connect(m_removeButton, &QPushButton::clicked, this, &AutoReplyDialog::removeRule);
    connect(m_ruleTable, &QTableWidget::itemSelectionChanged, this, &AutoReplyDialog::onSelectionChanged);
    connect(okButton, &QPushButton::clicked, this, &AutoReplyDialog::onAccepted);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // 暗色主题
    setStyleSheet(R"(
        QTableWidget {
            background-color: #1e1e2e; color: #cdd6f4;
            gridline-color: #45475a; border: 1px solid #45475a;
            selection-background-color: #585b70; font-size: 13px;
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

// ── 关键：先把表格当前状态保存回 m_rules，再重建 ──────────────────────

void AutoReplyDialog::saveTableToRules()
{
    for (int i = 0; i < m_ruleTable->rowCount() && i < m_rules.size(); ++i) {
        auto* enableCheck = qobject_cast<QCheckBox*>(m_ruleTable->cellWidget(i, 0));
        auto* encodingCombo = qobject_cast<QComboBox*>(m_ruleTable->cellWidget(i, 3));
        auto* regexCheck = qobject_cast<QCheckBox*>(m_ruleTable->cellWidget(i, 4));
        auto* cooldownSpin = qobject_cast<QSpinBox*>(m_ruleTable->cellWidget(i, 5));
        auto* maxSpin = qobject_cast<QSpinBox*>(m_ruleTable->cellWidget(i, 6));

        if (enableCheck) m_rules[i].enabled = enableCheck->isChecked();
        if (m_ruleTable->item(i, 1)) m_rules[i].pattern = m_ruleTable->item(i, 1)->text();
        if (m_ruleTable->item(i, 2)) m_rules[i].replyText = m_ruleTable->item(i, 2)->text();
        if (encodingCombo) {
            QString enc = encodingCombo->currentText();
            if (enc == "HEX") {
                m_rules[i].isHexReply = true;
            } else {
                m_rules[i].isHexReply = false;
                m_rules[i].encoding = enc;
            }
        }
        if (regexCheck) m_rules[i].useRegex = regexCheck->isChecked();
        if (cooldownSpin) m_rules[i].cooldownMs = cooldownSpin->value();
        if (maxSpin) m_rules[i].maxTriggerCount = maxSpin->value();
    }
}

void AutoReplyDialog::refreshTable()
{
    m_ruleTable->setRowCount(m_rules.size());

    for (int i = 0; i < m_rules.size(); ++i) {
        const AutoReplyRule& rule = m_rules[i];

        auto* enableCheck = new QCheckBox();
        enableCheck->setChecked(rule.enabled);
        m_ruleTable->setCellWidget(i, 0, enableCheck);

        m_ruleTable->setItem(i, 1, new QTableWidgetItem(rule.pattern));
        m_ruleTable->setItem(i, 2, new QTableWidgetItem(rule.replyText));

        auto* encodingCombo = new QComboBox();
        encodingCombo->addItems({"UTF-8", "GBK", "ASCII", "HEX"});
        encodingCombo->setCurrentText(rule.isHexReply ? "HEX" : rule.encoding);
        m_ruleTable->setCellWidget(i, 3, encodingCombo);

        auto* regexCheck = new QCheckBox();
        regexCheck->setChecked(rule.useRegex);
        m_ruleTable->setCellWidget(i, 4, regexCheck);

        auto* cooldownSpin = new QSpinBox();
        cooldownSpin->setRange(0, 60000);
        cooldownSpin->setValue(rule.cooldownMs);
        cooldownSpin->setSuffix(tr(" ms"));
        m_ruleTable->setCellWidget(i, 5, cooldownSpin);

        auto* maxSpin = new QSpinBox();
        maxSpin->setRange(0, 99999);
        maxSpin->setValue(rule.maxTriggerCount);
        maxSpin->setSpecialValueText(tr("0 = 不限"));
        m_ruleTable->setCellWidget(i, 6, maxSpin);

        // 已触发次数（只读显示）
        auto* countItem = new QTableWidgetItem(QString::number(rule.triggerCount));
        countItem->setFlags(countItem->flags() & ~Qt::ItemIsEditable);
        countItem->setTextAlignment(Qt::AlignCenter);
        m_ruleTable->setItem(i, 7, countItem);
    }
}

void AutoReplyDialog::onSelectionChanged()
{
    bool hasSelection = !m_ruleTable->selectedItems().isEmpty();
    m_removeButton->setEnabled(hasSelection);
}

void AutoReplyDialog::addRule()
{
    saveTableToRules(); // 先保存当前编辑状态

    AutoReplyRule rule;
    rule.enabled = true;
    rule.pattern = ".*";
    rule.replyText = "OK";
    rule.encoding = "UTF-8";
    rule.cooldownMs = 500;
    rule.maxTriggerCount = 0;

    m_rules.append(rule);
    refreshTable();
    m_ruleTable->selectRow(m_rules.size() - 1);
}

void AutoReplyDialog::removeRule()
{
    int row = m_ruleTable->currentRow();
    if (row < 0 || row >= m_rules.size()) return;

    saveTableToRules(); // 先保存当前编辑状态
    m_rules.removeAt(row);
    refreshTable();
}

void AutoReplyDialog::onAccepted()
{
    saveTableToRules();

    m_engine->mutableRules() = m_rules;
    emit m_engine->rulesChanged();
    accept();
}
