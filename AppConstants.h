#pragma once

// 应用程序全局常量定义
// 将散落在各文件中的魔法数字集中管理，提高可维护性
namespace AppConstants
{

// ── 串口默认配置 ──────────────────────────────────────────────────────
inline constexpr int kDefaultBaudIndex    = 4;     // 默认波特率索引 (115200)
inline constexpr int kDefaultDataBitsIndex = 3;    // 默认数据位索引 (8 位)

// ── 定时器 ────────────────────────────────────────────────────────────
inline constexpr int kPortScanIntervalMs  = 1000;  // 端口扫描间隔 (毫秒)

// ── 历史记录 ──────────────────────────────────────────────────────────
inline constexpr int kMaxHistoryItems     = 50;    // 发送历史最大条目数

// ── 发送队列 ──────────────────────────────────────────────────────────
inline constexpr int kMaxRepeatCount      = 9999;  // 队列项最大重复次数
inline constexpr int kMinIntervalMs       = 10;    // 队列项最小间隔 (毫秒)
inline constexpr int kMaxIntervalMs       = 600000; // 队列项最大间隔 (毫秒，10 分钟)

} // namespace AppConstants
