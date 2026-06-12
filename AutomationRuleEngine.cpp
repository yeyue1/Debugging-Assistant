#include "AutomationRuleEngine.h"

#include "RecordStore.h"

#include <QDateTime>
#include <QRegularExpression>

AutomationRuleEngine::AutomationRuleEngine()
{
}

void AutomationRuleEngine::resetTriggerState()
{
    m_triggerCount = 0;
    m_lastTriggerTime = 0;
}

bool AutomationRuleEngine::shouldAutoReply(const SerialRecord& record)
{
    if (!m_enabled || record.direction != SerialRecordDirection::Rx) {
        return false;
    }

    // ── 防循环保护 ──────────────────────────────────────────────────────

    // onceOnly: 已触发过一次则不再触发
    if (m_onceOnly && m_triggerCount > 0) {
        return false;
    }

    // maxTriggerCount: 达到上限则不再触发（0 = 不限制）
    if (m_maxTriggerCount > 0 && m_triggerCount >= m_maxTriggerCount) {
        return false;
    }

    // cooldownMs: 冷却时间内不触发
    if (m_cooldownMs > 0 && m_lastTriggerTime > 0) {
        const int64_t now = QDateTime::currentMSecsSinceEpoch();
        if (now - m_lastTriggerTime < m_cooldownMs) {
            return false;
        }
    }

    // ── 模式匹配 ────────────────────────────────────────────────────────

    const QString pattern = m_pattern.trimmed();
    if (pattern.isEmpty()) {
        return false;
    }

    const QString searchText = RecordStore::recordSearchText(record);

    bool matched = false;
    if (!m_useRegex) {
        matched = searchText.contains(pattern, Qt::CaseInsensitive);
    } else {
        const QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
        if (!regex.isValid()) {
            return false;
        }
        matched = regex.match(searchText).hasMatch();
    }

    // 匹配成功时更新触发状态
    if (matched) {
        ++m_triggerCount;
        m_lastTriggerTime = QDateTime::currentMSecsSinceEpoch();
    }

    return matched;
}
