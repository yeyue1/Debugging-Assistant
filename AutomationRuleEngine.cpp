#include "AutomationRuleEngine.h"

#include "RecordStore.h"

#include <QDateTime>
#include <QRegularExpression>

// ── 静态成员初始化 ────────────────────────────────────────────────────────
QList<int64_t> AutomationRuleEngine::s_recentTriggerTimes;
int AutomationRuleEngine::s_globalMaxPerMinute = 20;

AutomationRuleEngine::AutomationRuleEngine(QObject* parent)
    : QObject(parent)
{
}

// ── 规则管理 ──────────────────────────────────────────────────────────────

void AutomationRuleEngine::addRule(const AutoReplyRule& rule)
{
    m_rules.append(rule);
    emit rulesChanged();
}

void AutomationRuleEngine::removeRule(int index)
{
    if (index < 0 || index >= m_rules.size()) return;
    m_rules.removeAt(index);
    emit rulesChanged();
}

void AutomationRuleEngine::updateRule(int index, const AutoReplyRule& rule)
{
    if (index < 0 || index >= m_rules.size()) return;
    m_rules[index] = rule;
    emit rulesChanged();
}

// ── 重置 ──────────────────────────────────────────────────────────────────

void AutomationRuleEngine::resetTriggerState()
{
    for (auto& rule : m_rules) {
        rule.triggerCount = 0;
        rule.lastTriggerTime = 0;
    }
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
    while (!s_recentTriggerTimes.isEmpty() &&
           (now - s_recentTriggerTimes.first()) >= s_globalWindowMs) {
        s_recentTriggerTimes.removeFirst();
    }
}

// ── 核心匹配逻辑 ──────────────────────────────────────────────────────────

AutomationRuleEngine::MatchResult AutomationRuleEngine::checkAutoReply(const SerialRecord& record)
{
    if (!m_enabled || record.direction != SerialRecordDirection::Rx) {
        return {};
    }

    const int64_t now = QDateTime::currentMSecsSinceEpoch();

    // 遍历所有规则，找到第一个匹配的
    for (int i = 0; i < m_rules.size(); ++i) {
        AutoReplyRule& rule = m_rules[i];

        if (!rule.enabled || rule.pattern.isEmpty() || rule.replyText.isEmpty()) {
            continue;
        }

        // ── 防循环保护（per-rule）──────────────────────────────────────

        if (rule.onceOnly && rule.triggerCount > 0) {
            continue;
        }

        if (rule.maxTriggerCount > 0 && rule.triggerCount >= rule.maxTriggerCount) {
            emit ruleDisabled(i);
            continue;
        }

        if (rule.cooldownMs > 0 && rule.lastTriggerTime > 0) {
            if (now - rule.lastTriggerTime < rule.cooldownMs) {
                emit cooldownActive(i);
                continue;
            }
        }

        // ── 全局速率限制 ──────────────────────────────────────────────

        pruneGlobalTriggerTimes(now);
        if (s_globalMaxPerMinute > 0 &&
            s_recentTriggerTimes.size() >= s_globalMaxPerMinute) {
            emit globalRateLimitReached();
            return {};
        }

        // ── 模式匹配 ─────────────────────────────────────────────────

        // 用原始接收文本做匹配，不用 recordSearchText（后者拼接了所有字段）
        const QString searchText = record.text;

        bool matched = false;
        if (!rule.useRegex) {
            // 精确匹配：接收数据必须完全等于模式
            matched = (searchText.compare(rule.pattern, Qt::CaseInsensitive) == 0);
        } else {
            const QRegularExpression regex(rule.pattern,
                QRegularExpression::CaseInsensitiveOption);
            if (!regex.isValid()) continue;
            matched = regex.match(searchText).hasMatch();
        }

        if (matched) {
            ++rule.triggerCount;
            rule.lastTriggerTime = now;
            s_recentTriggerTimes.append(now);
            return {rule.replyText, rule.isHexReply, rule.encoding};
        }
    }

    return {};
}
