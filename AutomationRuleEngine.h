#pragma once

#include <QString>

#include "SerialRecord.h"

// 负责接收数据匹配和自动回复规则的触发判断
// SerialPanel 调用 shouldAutoReply() 判断是否触发，触发后发送回复内容
class AutomationRuleEngine
{
public:
    AutomationRuleEngine();

    // 规则设置
    void setEnabled(bool enabled) { m_enabled = enabled; }
    void setPattern(const QString& pattern) { m_pattern = pattern; }
    void setUseRegex(bool regex) { m_useRegex = regex; }

    // 判断一条 Rx 记录是否触发自动回复
    bool shouldAutoReply(const SerialRecord& record) const;

private:
    bool m_enabled = false;
    QString m_pattern;
    bool m_useRegex = false;
};
