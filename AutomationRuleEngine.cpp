#include "AutomationRuleEngine.h"

#include "RecordStore.h"

#include <QRegularExpression>

AutomationRuleEngine::AutomationRuleEngine()
{
}

bool AutomationRuleEngine::shouldAutoReply(const SerialRecord& record) const
{
    if (!m_enabled || record.direction != SerialRecordDirection::Rx) {
        return false;
    }

    const QString pattern = m_pattern.trimmed();
    if (pattern.isEmpty()) {
        return false;
    }

    const QString searchText = RecordStore::recordSearchText(record);

    if (!m_useRegex) {
        return searchText.contains(pattern, Qt::CaseInsensitive);
    }

    const QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
    if (!regex.isValid()) {
        return false;
    }
    return regex.match(searchText).hasMatch();
}
