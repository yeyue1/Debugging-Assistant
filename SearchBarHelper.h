#pragma once

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QTextCursor>
#include <QTextDocument>
#include <QRegularExpression>
#include <QKeyEvent>

// 可复用的接收区搜索栏组件
// SerialPanel 和 NetworkPanel 共用此组件实现 Ctrl+F 搜索功能
class SearchBarHelper : public QObject
{
    Q_OBJECT

public:
    explicit SearchBarHelper(QTextEdit* textEdit, QWidget* parentWidget, QObject* parent = nullptr);

    QWidget* searchBarWidget() const { return m_searchBarWidget; }

    // 切换搜索栏显示/隐藏
    void toggle();

public slots:
    void show() { toggle(); }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onSearchTextChanged();
    void onFindNext();
    void onFindPrevious();

private:
    void countMatches();
    void updateCountLabel();

    QTextEdit* m_textEdit;
    QWidget* m_searchBarWidget = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QCheckBox* m_searchCaseCheck = nullptr;
    QCheckBox* m_searchRegexCheck = nullptr;
    QLabel* m_searchCountLabel = nullptr;
    int m_searchCurrentIndex = 0;
    int m_searchTotalCount = 0;
};

// ── 内联实现 ──────────────────────────────────────────────────────────────

inline SearchBarHelper::SearchBarHelper(QTextEdit* textEdit, QWidget* parentWidget, QObject* parent)
    : QObject(parent), m_textEdit(textEdit)
{
    // 创建搜索栏 widget
    m_searchBarWidget = new QWidget(parentWidget);
    auto* layout = new QHBoxLayout(m_searchBarWidget);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(4);

    auto* searchLabel = new QLabel(QObject::tr("查找:"), m_searchBarWidget);
    layout->addWidget(searchLabel);

    m_searchEdit = new QLineEdit(m_searchBarWidget);
    m_searchEdit->setPlaceholderText(QObject::tr("输入搜索内容..."));
    m_searchEdit->setMinimumWidth(180);
    layout->addWidget(m_searchEdit, 1);

    m_searchCaseCheck = new QCheckBox(QObject::tr("区分大小写"), m_searchBarWidget);
    layout->addWidget(m_searchCaseCheck);

    m_searchRegexCheck = new QCheckBox(QObject::tr("正则"), m_searchBarWidget);
    layout->addWidget(m_searchRegexCheck);

    auto* findPrevButton = new QPushButton(QObject::tr("上一个"), m_searchBarWidget);
    layout->addWidget(findPrevButton);

    auto* findNextButton = new QPushButton(QObject::tr("下一个"), m_searchBarWidget);
    layout->addWidget(findNextButton);

    m_searchCountLabel = new QLabel(m_searchBarWidget);
    m_searchCountLabel->setMinimumWidth(100);
    layout->addWidget(m_searchCountLabel);

    auto* closeButton = new QPushButton(QObject::tr("关闭"), m_searchBarWidget);
    layout->addWidget(closeButton);

    // 将搜索栏插入到 rxEdit 之前
    QBoxLayout* parentLayout = qobject_cast<QBoxLayout*>(textEdit->parentWidget()->layout());
    if (parentLayout) {
        for (int i = 0; i < parentLayout->count(); ++i) {
            QLayoutItem* item = parentLayout->itemAt(i);
            if (item && item->widget() == textEdit) {
                parentLayout->insertWidget(i, m_searchBarWidget);
                break;
            }
        }
    }

    m_searchBarWidget->hide();

    // 信号连接
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SearchBarHelper::onSearchTextChanged);
    connect(m_searchCaseCheck, &QCheckBox::toggled, this, &SearchBarHelper::onSearchTextChanged);
    connect(m_searchRegexCheck, &QCheckBox::toggled, this, &SearchBarHelper::onSearchTextChanged);
    connect(findNextButton, &QPushButton::clicked, this, &SearchBarHelper::onFindNext);
    connect(findPrevButton, &QPushButton::clicked, this, &SearchBarHelper::onFindPrevious);
    connect(closeButton, &QPushButton::clicked, this, &SearchBarHelper::toggle);

    // 在父 widget 上安装事件过滤器以捕获 Ctrl+F
    parentWidget->installEventFilter(this);
}

inline bool SearchBarHelper::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->modifiers() & Qt::ControlModifier && keyEvent->key() == Qt::Key_F) {
            toggle();
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

inline void SearchBarHelper::toggle()
{
    if (m_searchBarWidget->isVisible()) {
        m_searchBarWidget->hide();
        m_searchEdit->clear();
        m_searchCountLabel->clear();
        m_searchCurrentIndex = 0;
        m_searchTotalCount = 0;
    } else {
        m_searchBarWidget->show();
        m_searchEdit->setFocus();
        m_searchEdit->selectAll();
    }
}

inline void SearchBarHelper::countMatches()
{
    m_searchCurrentIndex = 0;
    m_searchTotalCount = 0;

    const QString searchText = m_searchEdit->text();
    if (searchText.isEmpty()) {
        m_searchCountLabel->clear();
        return;
    }

    QTextDocument* doc = m_textEdit->document();
    QTextCursor cursor(doc);
    cursor.movePosition(QTextCursor::Start);

    QTextDocument::FindFlags flags;
    if (m_searchCaseCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    if (m_searchRegexCheck->isChecked()) {
        QRegularExpression regex(searchText);
        if (!regex.isValid()) {
            m_searchCountLabel->setText(QObject::tr("无效正则"));
            return;
        }
        while (true) {
            QTextCursor match = doc->find(regex, cursor, flags);
            if (match.isNull()) break;
            ++m_searchTotalCount;
            cursor = match;
        }
    } else {
        while (true) {
            QTextCursor match = doc->find(searchText, cursor, flags);
            if (match.isNull()) break;
            ++m_searchTotalCount;
            cursor = match;
        }
    }

    if (m_searchTotalCount > 0) {
        m_searchCurrentIndex = 1;
        updateCountLabel();
        onFindNext();
    } else {
        m_searchCountLabel->setText(QObject::tr("0 个匹配"));
    }
}

inline void SearchBarHelper::updateCountLabel()
{
    m_searchCountLabel->setText(QObject::tr("%1 / %2 个匹配")
        .arg(m_searchCurrentIndex).arg(m_searchTotalCount));
}

inline void SearchBarHelper::onSearchTextChanged()
{
    countMatches();
}

inline void SearchBarHelper::onFindNext()
{
    const QString searchText = m_searchEdit->text();
    if (searchText.isEmpty() || m_searchTotalCount == 0) return;

    QTextDocument::FindFlags flags;
    if (m_searchCaseCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    bool found = false;
    if (m_searchRegexCheck->isChecked()) {
        found = m_textEdit->find(QRegularExpression(searchText), flags);
    } else {
        found = m_textEdit->find(searchText, flags);
    }

    if (found) {
        ++m_searchCurrentIndex;
        if (m_searchCurrentIndex > m_searchTotalCount) {
            m_searchCurrentIndex = 1;
        }
    } else if (m_searchTotalCount > 0) {
        // 循环到开头
        QTextCursor cursor = m_textEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        m_textEdit->setTextCursor(cursor);

        if (m_searchRegexCheck->isChecked()) {
            found = m_textEdit->find(QRegularExpression(searchText), flags);
        } else {
            found = m_textEdit->find(searchText, flags);
        }
        if (found) {
            m_searchCurrentIndex = 1;
        }
    }
    updateCountLabel();
}

inline void SearchBarHelper::onFindPrevious()
{
    const QString searchText = m_searchEdit->text();
    if (searchText.isEmpty() || m_searchTotalCount == 0) return;

    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    if (m_searchCaseCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    bool found = false;
    if (m_searchRegexCheck->isChecked()) {
        found = m_textEdit->find(QRegularExpression(searchText), flags);
    } else {
        found = m_textEdit->find(searchText, flags);
    }

    if (found) {
        --m_searchCurrentIndex;
        if (m_searchCurrentIndex < 1) {
            m_searchCurrentIndex = m_searchTotalCount;
        }
    } else if (m_searchTotalCount > 0) {
        // 循环到末尾
        QTextCursor cursor = m_textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_textEdit->setTextCursor(cursor);

        if (m_searchRegexCheck->isChecked()) {
            found = m_textEdit->find(QRegularExpression(searchText), flags);
        } else {
            found = m_textEdit->find(searchText, flags);
        }
        if (found) {
            m_searchCurrentIndex = m_searchTotalCount;
        }
    }
    updateCountLabel();
}
