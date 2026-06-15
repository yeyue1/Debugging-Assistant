#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <cstdint>

#include "SerialRecord.h"

// 负责接收数据匹配和自动回复规则的触发判断
// SerialPanel 调用 shouldAutoReply() 判断是否触发，触发后发送回复内容
//
// 防循环保护机制：
//   - cooldownMs:      两次触发之间的最小间隔（毫秒），防止高频循环
//   - maxTriggerCount: 最大触发次数，0 表示不限制
//   - onceOnly:        仅触发一次后自动禁用
//   - 全局限制:         每分钟最多 auto-reply 次数（所有规则共享）
class AutomationRuleEngine : public QObject
{
    Q_OBJECT

public:
    AutomationRuleEngine(QObject* parent = nullptr);

    // 规则设置
    void setEnabled(bool enabled) { m_enabled = enabled; }
    void setPattern(const QString& pattern) { m_pattern = pattern; }
    void setUseRegex(bool regex) { m_useRegex = regex; }

    // 防循环保护设置
    void setCooldownMs(int ms) { m_cooldownMs = ms; }
    void setMaxTriggerCount(int count) { m_maxTriggerCount = count; }
    void setOnceOnly(bool once) { m_onceOnly = once; }

    // 重置本规则的触发计数和时间戳
    void resetTriggerState();

    // 重置所有规则的触发状态（串口重新打开时调用）
    static void resetAllRules();

    // 全局限制设置
    static void setGlobalMaxPerMinute(int maxCount);
    static int globalMaxPerMinute() { return s_globalMaxPerMinute; }

    // 判断一条 Rx 记录是否触发自动回复
    bool shouldAutoReply(const SerialRecord& record);

signals:
    // 规则因达到最大触发次数而被禁用
    void ruleDisabled(int ruleIndex);
    // 规则因冷却时间而被抑制
    void cooldownActive(int ruleIndex);

    // 全局限制被触发
    void globalRateLimitReached();

private:
    bool m_enabled = false;
    QString m_pattern;
    bool m_useRegex = false;

    // 防循环保护
    int m_cooldownMs = 500;        // 触发冷却时间（毫秒）
    int m_maxTriggerCount = 10;    // 最大触发次数，0 = 不限制
    bool m_onceOnly = false;       // 仅触发一次

    // 运行时状态（mutable 允许在 const 语义下由 shouldAutoReply 更新）
    int m_triggerCount = 0;        // 已触发次数
    int64_t m_lastTriggerTime = 0;  // 上次触发时间（毫秒时间戳）

    // 全局状态（静态，所有规则共享）
    static QList<int64_t> s_recentTriggerTimes;   // 最近的触发时间戳
    static int s_globalMaxPerMinute;               // 每分钟最大触发次数
    static constexpr int64_t s_globalWindowMs = 60000; // 全局时间窗口（60秒）

    // 规则索引（用于信号）
    int m_ruleIndex = 0;
    static int s_nextRuleIndex;

    // 清理过期的全局触发记录
    static void pruneGlobalTriggerTimes(int64_t now);
};
