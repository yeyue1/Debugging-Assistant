#pragma once

#include <QString>
#include <cstdint>

#include "SerialRecord.h"

// 负责接收数据匹配和自动回复规则的触发判断
// SerialPanel 调用 shouldAutoReply() 判断是否触发，触发后发送回复内容
//
// 防循环保护机制：
//   - cooldownMs:    两次触发之间的最小间隔（毫秒），防止高频循环
//   - maxTriggerCount: 最大触发次数，0 表示不限制
//   - onceOnly:      仅触发一次后自动禁用
class AutomationRuleEngine
{
public:
    AutomationRuleEngine();

    // 规则设置
    void setEnabled(bool enabled) { m_enabled = enabled; }
    void setPattern(const QString& pattern) { m_pattern = pattern; }
    void setUseRegex(bool regex) { m_useRegex = regex; }

    // 防循环保护设置
    void setCooldownMs(int ms) { m_cooldownMs = ms; }
    void setMaxTriggerCount(int count) { m_maxTriggerCount = count; }
    void setOnceOnly(bool once) { m_onceOnly = once; }

    // 重置触发计数和时间戳（重新启用规则时调用）
    void resetTriggerState();

    // 判断一条 Rx 记录是否触发自动回复
    bool shouldAutoReply(const SerialRecord& record);

private:
    bool m_enabled = false;
    QString m_pattern;
    bool m_useRegex = false;

    // 防循环保护
    int m_cooldownMs = 500;        // 触发冷却时间（毫秒）
    int m_maxTriggerCount = 0;     // 最大触发次数，0 = 不限制
    bool m_onceOnly = false;       // 仅触发一次

    // 运行时状态（mutable 允许在 const 语义下由 shouldAutoReply 更新）
    int m_triggerCount = 0;        // 已触发次数
    int64_t m_lastTriggerTime = 0;  // 上次触发时间（毫秒时间戳）
};
