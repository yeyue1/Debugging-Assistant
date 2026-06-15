#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QtGlobal>
#include <cstdint>

#include "SerialRecord.h"

// 单条自动回复规则
struct AutoReplyRule {
    bool enabled = true;
    QString pattern;        // 匹配模式
    QString replyText;      // 回复内容
    bool useRegex = false;  // 是否正则匹配
    bool isHexReply = false; // 回复内容是否为 HEX 格式
    QString encoding = "UTF-8"; // 回复编码：UTF-8 / GBK
    int cooldownMs = 500;   // 触发冷却（毫秒）
    int maxTriggerCount = 10; // 最大触发次数（0=不限制）
    bool onceOnly = false;  // 仅触发一次

    // 运行时状态（不序列化）
    int triggerCount = 0;
    int64_t lastTriggerTime = 0;
};

// 多规则自动回复引擎
// 每个面板持有独立实例，支持多条规则并行匹配
//
// 防循环保护机制（每条规则独立）：
//   - cooldownMs:       两次触发之间的最小间隔
//   - maxTriggerCount:  最大触发次数（0=不限制）
//   - onceOnly:         仅触发一次后自动禁用
//   - 全局限制:          所有规则共享每分钟最大触发次数
class AutomationRuleEngine : public QObject
{
    Q_OBJECT

public:
    explicit AutomationRuleEngine(QObject* parent = nullptr);

    // 全局开关
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    // 规则管理
    void addRule(const AutoReplyRule& rule);
    void removeRule(int index);
    void updateRule(int index, const AutoReplyRule& rule);
    const QList<AutoReplyRule>& rules() const { return m_rules; }
    QList<AutoReplyRule>& mutableRules() { return m_rules; }

    // 重置所有规则的触发状态
    void resetTriggerState();

    // 全局重置（所有引擎共享）
    static void resetAllRules();

    // 全局限制设置
    static void setGlobalMaxPerMinute(int maxCount);
    static int globalMaxPerMinute() { return s_globalMaxPerMinute; }

    // 自动回复匹配结果
    struct MatchResult {
        QString replyText;      // 回复内容
        bool isHex = false;     // 是否 HEX 格式
        QString encoding = "UTF-8"; // 编码
    };

    // 检查是否有规则匹配，返回匹配结果（replyText 为空=不触发）
    MatchResult checkAutoReply(const SerialRecord& record);

signals:
    void ruleDisabled(int ruleIndex);
    void cooldownActive(int ruleIndex);
    void globalRateLimitReached();
    void rulesChanged();

private:
    bool m_enabled = false;
    QList<AutoReplyRule> m_rules;

    // 全局状态（静态，所有引擎共享）
    static QList<int64_t> s_recentTriggerTimes;
    static int s_globalMaxPerMinute;
    static constexpr int64_t s_globalWindowMs = 60000;

    static void pruneGlobalTriggerTimes(int64_t now);
};
