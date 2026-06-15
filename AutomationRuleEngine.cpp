#include "AutomationRuleEngine.h"

#include "RecordStore.h"

#include <QDateTime>
#include <QRegularExpression>

// ── 静态成员初始化 ────────────────────────────────────────────────────────
QList<int64_t> AutomationRuleEngine::s_recentTriggerTimes;
int AutomationRuleEngine::s_globalMaxPerMinute = 20;
int AutomationRuleEngine::s_nextRuleIndex = 0;

AutomationRuleEngine::AutomationRuleEngine(QObject* parent)
    : QObject(parent)
    , m_ruleIndex(s_nextRuleIndex++)
{
}

void AutomationRuleEngine::resetTriggerState()
{
    m_triggerCount = 0;
    m_lastTriggerTime = 0;
}

void AutomationRuleEngine::resetAllRules()
{
    s_recentTriggerTimes.clear();
}

void AutomationRuleEngine::setGlobalMaxPerMinute(int maxCount)
{
    s_globalMaxPerMinute = maxCount;
}

void AutomationRuleEngine::pruneGlobalTriggerTimes(int64_t now)
{
    // 移除超出时间窗口的记录
    while (!s_recentTriggerTimes.isEmpty() &&
           (now - s_recentTriggerTimes.first()) >= s_globalWindowMs) {
        s_recentTriggerTimes.removeFirst();
    }
}

bool AutomationRuleEngine::shouldAutoReply(const SerialRecord& record)
{
    if (!m_enabled || record.direction != SerialRecordDirection::Rx) {
        return false;
    }

    // ── 防循环保护（per-rule）──────────────────────────────────────────────

    // onceOnly: 已触发过一次则不再触发
    if (m_onceOnly && m_triggerCount > 0) {
        return false;
    }

    // maxTriggerCount: 达到上限则不再触发（0 = 不限制）
    if (m_maxTriggerCount > 0 && m_triggerCount >= m_maxTriggerCount) {
        emit ruleDisabled(m_ruleIndex);
        return false;
    }

    // cooldownMs: 冷却时间内不触发
    if (m_cooldownMs > 0 && m_lastTriggerTime > 0) {
        const int64_t now = QDateTime::currentMSecsSinceEpoch();
        if (now - m_lastTriggerTime < m_cooldownMs) {
            emit cooldownActive(m_ruleIndex);
            return false;
        }
    }

    // ── 全局速率限制（跨所有规则）──────────────────────────────────────────

    {
        const int64_t now = QDateTime::currentMSecsSinceEpoch();
        pruneGlobalTriggerTimes(now);

        if (s_globalMaxPerMinute > 0 &&
            s_recentTriggerTimes.size() >= s_globalMaxPerMinute) {
            emit globalRateLimitReached();
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

        // 记录到全局触发历史
        s_recentTriggerTimes.append(m_lastTriggerTime);
    }

    return matched;
}
