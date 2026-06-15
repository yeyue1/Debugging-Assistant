// TestDeviceRegistry.cpp
// 测试设备注册表：验证设备识别、别名保存/加载、端口查找等功能

#include <QtTest>
#include "DeviceRegistry.h"

class TestDeviceRegistry : public QObject
{
    Q_OBJECT

private:
    DeviceRegistry* registry;

private slots:
    void init()
    {
        registry = new DeviceRegistry(this);
    }

    void cleanup()
    {
        delete registry;
        registry = nullptr;
    }

    // ── 测试设备Key生成 ──────────────────────────────────────────────────────

    void testDeviceKeyForNonExistentPort()
    {
        // 测试目的：验证不存在的端口返回空字符串
        QString key = registry->deviceKeyForPort(QStringLiteral("COM99"));
        QVERIFY(key.isEmpty());
    }

    void testDeviceKeyForEmptyPort()
    {
        // 测试目的：验证空端口名返回空字符串
        QString key = registry->deviceKeyForPort(QString());
        QVERIFY(key.isEmpty());
    }

    // ── 测试设备摘要 ──────────────────────────────────────────────────────────

    void testDeviceSummaryForNonExistentPort()
    {
        // 测试目的：验证不存在的端口返回端口名本身
        QString summary = registry->deviceSummaryForPort(QStringLiteral("COM99"));
        QCOMPARE(summary, QStringLiteral("COM99"));
    }

    void testDeviceSummaryForEmptyPort()
    {
        // 测试目的：验证空端口名返回空字符串
        QString summary = registry->deviceSummaryForPort(QString());
        QVERIFY(summary.isEmpty());
    }

    // ── 测试别名保存/加载 ──────────────────────────────────────────────────────

    void testAliasSaveAndLoad()
    {
        // 测试目的：验证别名保存后能正确读取
        QString key = QStringLiteral("usb:1234:5678:SN001");
        QString alias = QStringLiteral("我的设备");

        registry->saveAlias(key, alias);
        QString loaded = registry->aliasForKey(key);

        QCOMPARE(loaded, alias);
    }

    void testAliasOverwrite()
    {
        // 测试目的：验证重复保存别名会覆盖旧值
        QString key = QStringLiteral("port:COM3");

        registry->saveAlias(key, QStringLiteral("旧名称"));
        registry->saveAlias(key, QStringLiteral("新名称"));
        QString loaded = registry->aliasForKey(key);

        QCOMPARE(loaded, QStringLiteral("新名称"));
    }

    void testAliasEmptyKey()
    {
        // 测试目的：验证空key的别名查询返回空字符串
        QString loaded = registry->aliasForKey(QString());
        QVERIFY(loaded.isEmpty());
    }

    void testAliasNonExistentKey()
    {
        // 测试目的：验证不存在的key返回空字符串
        QString loaded = registry->aliasForKey(QStringLiteral("nonexistent_key_xyz"));
        QVERIFY(loaded.isEmpty());
    }

    void testSaveAliasEmptyKey()
    {
        // 测试目的：验证空key不会保存别名（不崩溃）
        registry->saveAlias(QString(), QStringLiteral("should not save"));
        // 不应崩溃，且空key查询仍返回空
        QVERIFY(registry->aliasForKey(QString()).isEmpty());
    }

    // ── 测试端口查找 ──────────────────────────────────────────────────────────

    void testFindPortForKeyEmpty()
    {
        // 测试目的：验证空key返回空字符串
        QString port = registry->findPortForKey(QString());
        QVERIFY(port.isEmpty());
    }

    void testFindPortForKeyNonExistent()
    {
        // 测试目的：验证不存在的key返回空字符串
        QString port = registry->findPortForKey(QStringLiteral("port:COM99"));
        QVERIFY(port.isEmpty());
    }

    void testFindPortForKeyUsbFormat()
    {
        // 测试目的：验证USB格式的key查找（无可用端口时返回空）
        QString port = registry->findPortForKey(QStringLiteral("usb:0483:df11:SN001"));
        QVERIFY(port.isEmpty());
    }
};

QTEST_MAIN(TestDeviceRegistry)
#include "TestDeviceRegistry.moc"
