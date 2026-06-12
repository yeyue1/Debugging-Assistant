// TestSendQueue.cpp
// 测试发送队列：验证入队/出队、循环队列、插入/删除、队列状态信号等功能

#include <QtTest>
#include <QSignalSpy>
#include "SendQueue.h"

class TestSendQueue : public QObject
{
    Q_OBJECT

private:
    SendQueue* queue;

private slots:
    void init()
    {
        queue = new SendQueue(this);
    }

    void cleanup()
    {
        delete queue;
        queue = nullptr;
    }

    // ── 测试入队/出队 ──────────────────────────────────────────────────────────

    void testEnqueue()
    {
        // 测试目的：验证基本入队操作
        QSignalSpy queueChangedSpy(queue, &SendQueue::queueChanged);

        queue->enqueue(QByteArray::fromHex("AABB"), "Test Data", 1);

        QCOMPARE(queue->pendingItemCount(), 1);
        QCOMPARE(queue->pendingSendCount(), 1);
        QCOMPARE(queueChangedSpy.count(), 1);
    }

    void testEnqueueMultipleItems()
    {
        // 测试目的：验证多个项目入队
        queue->enqueue(QByteArray::fromHex("01"), "Item 1", 1);
        queue->enqueue(QByteArray::fromHex("02"), "Item 2", 2);
        queue->enqueue(QByteArray::fromHex("03"), "Item 3", 3);

        QCOMPARE(queue->pendingItemCount(), 3);
        QCOMPARE(queue->pendingSendCount(), 6);  // 1 + 2 + 3
    }

    void testEnqueueWithRepeatCount()
    {
        // 测试目的：验证带重复次数的入队
        queue->enqueue(QByteArray::fromHex("AA"), "Repeat", 5);

        QCOMPARE(queue->pendingItemCount(), 1);
        QCOMPARE(queue->pendingSendCount(), 5);

        SendQueue::QueueItem item = queue->itemAt(0);
        QCOMPARE(item.repeatCount, 5);
        QCOMPARE(item.remaining, 5);
    }

    void testEnqueueInvalidPayload()
    {
        // 测试目的：验证空负载不会被添加到队列
        queue->enqueue(QByteArray(), "Empty", 1);

        QCOMPARE(queue->pendingItemCount(), 0);
    }

    void testEnqueueInvalidRepeatCount()
    {
        // 测试目的：验证重复次数为0或负数时不会被添加
        queue->enqueue(QByteArray::fromHex("AA"), "Zero", 0);
        queue->enqueue(QByteArray::fromHex("BB"), "Negative", -1);

        QCOMPARE(queue->pendingItemCount(), 0);
    }

    void testItemAtIndex()
    {
        // 测试目的：验证能正确获取指定位置的项目
        queue->enqueue(QByteArray::fromHex("01"), "First", 1, 1000);
        queue->enqueue(QByteArray::fromHex("02"), "Second", 2, 2000);

        SendQueue::QueueItem item0 = queue->itemAt(0);
        SendQueue::QueueItem item1 = queue->itemAt(1);

        QCOMPARE(item0.displayText, QString("First"));
        QCOMPARE(item0.repeatCount, 1);
        QCOMPARE(item0.intervalMs, 1000);

        QCOMPARE(item1.displayText, QString("Second"));
        QCOMPARE(item1.repeatCount, 2);
        QCOMPARE(item1.intervalMs, 2000);
    }

    void testItemAtIndexOutOfBounds()
    {
        // 测试目的：验证越界访问返回空项目
        queue->enqueue(QByteArray::fromHex("01"), "Only", 1);

        SendQueue::QueueItem invalidItem = queue->itemAt(-1);
        QVERIFY(invalidItem.payload.isEmpty());

        invalidItem = queue->itemAt(1);
        QVERIFY(invalidItem.payload.isEmpty());
    }

    // ── 测试循环队列 ──────────────────────────────────────────────────────────

    void testLoopDisabled()
    {
        // 测试目的：验证非循环模式下队列处理完后停止
        QSignalSpy sendSpy(queue, &SendQueue::sendRequested);
        QSignalSpy finishedSpy(queue, &SendQueue::finished);

        queue->enqueue(QByteArray::fromHex("01"), "Item 1", 1, 10);
        queue->setLoopEnabled(false);

        queue->start();

        // 等待发送完成
        QTest::qWait(50);

        QCOMPARE(sendSpy.count(), 1);
        QCOMPARE(queue->pendingItemCount(), 0);
        QVERIFY(!queue->isRunning());
    }

    void testLoopEnabled()
    {
        // 测试目的：验证循环模式下队列会循环发送
        QSignalSpy sendSpy(queue, &SendQueue::sendRequested);

        queue->enqueue(QByteArray::fromHex("01"), "Loop Item", 1, 50);
        queue->setLoopEnabled(true);

        queue->start();

        // 等待多次发送
        QTest::qWait(300);

        // 应该发送多次
        QVERIFY(sendSpy.count() >= 2);
        // 项目应该还在队列中
        QCOMPARE(queue->pendingItemCount(), 1);
    }

    void testLoopEnabledMovesItemToEnd()
    {
        // 测试目的：验证循环模式下完成的项目移到队列末尾
        QSignalSpy sendSpy(queue, &SendQueue::sendRequested);

        queue->enqueue(QByteArray::fromHex("01"), "First", 1, 50);
        queue->enqueue(QByteArray::fromHex("02"), "Second", 1, 50);
        queue->setLoopEnabled(true);

        queue->start();

        // 等待第一个项目完成
        QTest::qWait(100);

        // 第一个项目应该被移到末尾
        QCOMPARE(queue->itemAt(0).displayText, QString("Second"));
        QCOMPARE(queue->itemAt(1).displayText, QString("First"));
    }

    // ── 测试插入/删除 ──────────────────────────────────────────────────────────

    void testInsertAtIndex()
    {
        // 测试目的：验证能在指定位置插入项目
        queue->enqueue(QByteArray::fromHex("01"), "First", 1);
        queue->enqueue(QByteArray::fromHex("03"), "Third", 1);

        queue->insert(1, QByteArray::fromHex("02"), "Second", 1);

        QCOMPARE(queue->pendingItemCount(), 3);
        QCOMPARE(queue->itemAt(0).displayText, QString("First"));
        QCOMPARE(queue->itemAt(1).displayText, QString("Second"));
        QCOMPARE(queue->itemAt(2).displayText, QString("Third"));
    }

    void testInsertAtBeginning()
    {
        // 测试目的：验证能在队列开头插入项目
        queue->enqueue(QByteArray::fromHex("02"), "Second", 1);

        queue->insert(0, QByteArray::fromHex("01"), "First", 1);

        QCOMPARE(queue->pendingItemCount(), 2);
        QCOMPARE(queue->itemAt(0).displayText, QString("First"));
    }

    void testInsertAtEnd()
    {
        // 测试目的：验证能在队列末尾插入项目
        queue->enqueue(QByteArray::fromHex("01"), "First", 1);

        queue->insert(10, QByteArray::fromHex("02"), "Second", 1);  // 超出范围应插入到末尾

        QCOMPARE(queue->pendingItemCount(), 2);
        QCOMPARE(queue->itemAt(1).displayText, QString("Second"));
    }

    void testInsertInvalidPayload()
    {
        // 测试目的：验证无效负载不会被插入
        queue->insert(0, QByteArray(), "Empty", 1);

        QCOMPARE(queue->pendingItemCount(), 0);
    }

    void testRemoveAt()
    {
        // 测试目的：验证能删除指定位置的项目
        queue->enqueue(QByteArray::fromHex("01"), "First", 1);
        queue->enqueue(QByteArray::fromHex("02"), "Second", 1);
        queue->enqueue(QByteArray::fromHex("03"), "Third", 1);

        queue->removeAt(1);

        QCOMPARE(queue->pendingItemCount(), 2);
        QCOMPARE(queue->itemAt(0).displayText, QString("First"));
        QCOMPARE(queue->itemAt(1).displayText, QString("Third"));
    }

    void testRemoveAtOutOfBounds()
    {
        // 测试目的：验证越界删除不会崩溃
        queue->enqueue(QByteArray::fromHex("01"), "Only", 1);

        queue->removeAt(-1);
        queue->removeAt(1);

        QCOMPARE(queue->pendingItemCount(), 1);
    }

    void testRemoveLastItemStopsQueue()
    {
        // 测试目的：验证删除最后一个项目时队列停止
        QSignalSpy queueChangedSpy(queue, &SendQueue::queueChanged);

        queue->enqueue(QByteArray::fromHex("01"), "Only", 1);
        queue->start();

        queue->removeAt(0);

        QCOMPARE(queue->pendingItemCount(), 0);
        QVERIFY(!queue->isRunning());
    }

    void testClear()
    {
        // 测试目的：验证清空队列
        queue->enqueue(QByteArray::fromHex("01"), "Item 1", 1);
        queue->enqueue(QByteArray::fromHex("02"), "Item 2", 1);
        queue->enqueue(QByteArray::fromHex("03"), "Item 3", 1);

        queue->clear();

        QCOMPARE(queue->pendingItemCount(), 0);
        QCOMPARE(queue->pendingSendCount(), 0);
        QVERIFY(!queue->isRunning());
    }

    // ── 测试队列状态信号 ──────────────────────────────────────────────────────

    void testQueueChangedSignalOnEnqueue()
    {
        // 测试目的：验证入队时发出queueChanged信号
        QSignalSpy queueChangedSpy(queue, &SendQueue::queueChanged);

        queue->enqueue(QByteArray::fromHex("01"), "Test", 1);

        QCOMPARE(queueChangedSpy.count(), 1);
        QList<QVariant> args = queueChangedSpy.at(0);
        QCOMPARE(args.at(0).toInt(), 1);  // pendingItems
        QCOMPARE(args.at(1).toInt(), 1);  // pendingSends
        QCOMPARE(args.at(2).toBool(), false);  // running
    }

    void testQueueChangedSignalOnStart()
    {
        // 测试目的：验证启动时发出queueChanged信号
        QSignalSpy queueChangedSpy(queue, &SendQueue::queueChanged);

        queue->enqueue(QByteArray::fromHex("01"), "Test", 1);
        queueChangedSpy.clear();

        queue->start();

        QVERIFY(queueChangedSpy.count() >= 1);
        QList<QVariant> args = queueChangedSpy.at(0);
        QCOMPARE(args.at(2).toBool(), true);  // running = true
    }

    void testQueueChangedSignalOnStop()
    {
        // 测试目的：验证停止时发出queueChanged信号
        QSignalSpy queueChangedSpy(queue, &SendQueue::queueChanged);

        queue->enqueue(QByteArray::fromHex("01"), "Test", 1);
        queue->start();
        queueChangedSpy.clear();

        queue->stop();

        QVERIFY(queueChangedSpy.count() >= 1);
        QList<QVariant> args = queueChangedSpy.at(0);
        QCOMPARE(args.at(2).toBool(), false);  // running = false
    }

    void testSendRequestedSignal()
    {
        // 测试目的：验证发送时发出sendRequested信号
        QSignalSpy sendSpy(queue, &SendQueue::sendRequested);

        queue->enqueue(QByteArray::fromHex("AABB"), "Test Data", 1, 10, "localhost", 1234, "peer1");
        queue->start();

        // 等待发送
        QTest::qWait(50);

        QCOMPARE(sendSpy.count(), 1);
        QList<QVariant> args = sendSpy.at(0);
        QCOMPARE(args.at(0).toByteArray(), QByteArray::fromHex("AABB"));
        QCOMPARE(args.at(1).toString(), QString("Test Data"));
        QCOMPARE(args.at(2).toString(), QString("localhost"));
        QCOMPARE(args.at(3).toUInt(), 1234);
        QCOMPARE(args.at(4).toString(), QString("peer1"));
    }

    void testFinishedSignal()
    {
        // 测试目的：验证队列完成时发出finished信号
        QSignalSpy finishedSpy(queue, &SendQueue::finished);

        queue->enqueue(QByteArray::fromHex("01"), "Test", 1, 10);
        queue->setLoopEnabled(false);
        queue->start();

        // 等待发送完成
        QTest::qWait(50);

        QCOMPARE(finishedSpy.count(), 1);
    }

    // ── 测试队列设置 ──────────────────────────────────────────────────────────

    void testSetIntervalMs()
    {
        // 测试目的：验证设置发送间隔
        queue->setIntervalMs(500);
        QCOMPARE(queue->intervalMs(), 500);
    }

    void testSetIntervalMsInvalid()
    {
        // 测试目的：验证无效间隔不会被设置
        int originalInterval = queue->intervalMs();
        queue->setIntervalMs(0);
        QCOMPARE(queue->intervalMs(), originalInterval);

        queue->setIntervalMs(-100);
        QCOMPARE(queue->intervalMs(), originalInterval);
    }

    void testSetLoopEnabled()
    {
        // 测试目的：验证设置循环模式
        queue->setLoopEnabled(true);
        QVERIFY(queue->loopEnabled());

        queue->setLoopEnabled(false);
        QVERIFY(!queue->loopEnabled());
    }

    void testUpdateItem()
    {
        // 测试目的：验证更新队列项目
        queue->enqueue(QByteArray::fromHex("01"), "Original", 1);

        SendQueue::QueueItem updatedItem;
        updatedItem.payload = QByteArray::fromHex("02");
        updatedItem.displayText = "Updated";
        updatedItem.repeatCount = 2;
        updatedItem.remaining = 2;
        updatedItem.intervalMs = 500;

        queue->updateItem(0, updatedItem);

        SendQueue::QueueItem item = queue->itemAt(0);
        QCOMPARE(item.displayText, QString("Updated"));
        QCOMPARE(item.repeatCount, 2);
    }

    void testUpdateItemInvalid()
    {
        // 测试目的：验证无效更新不会修改队列
        queue->enqueue(QByteArray::fromHex("01"), "Original", 1);

        // 尝试用无效数据更新
        SendQueue::QueueItem invalidItem;
        invalidItem.payload = QByteArray();  // 空负载
        invalidItem.repeatCount = 1;
        invalidItem.remaining = 1;
        invalidItem.intervalMs = 1000;

        queue->updateItem(0, invalidItem);

        // 原始数据应该保持不变
        QCOMPARE(queue->itemAt(0).displayText, QString("Original"));
    }

    // ── 测试运行状态 ──────────────────────────────────────────────────────────

    void testInitialState()
    {
        // 测试目的：验证初始状态
        QVERIFY(!queue->isRunning());
        QVERIFY(!queue->loopEnabled());
        QCOMPARE(queue->pendingItemCount(), 0);
        QCOMPARE(queue->pendingSendCount(), 0);
    }

    void testStartEmptyQueue()
    {
        // 测试目的：验证空队列不能启动
        queue->start();
        QVERIFY(!queue->isRunning());
    }

    void testStartStopSequence()
    {
        // 测试目的：验证启动/停止序列
        queue->enqueue(QByteArray::fromHex("01"), "Test", 3, 100);

        queue->start();
        QVERIFY(queue->isRunning());

        queue->stop();
        QVERIFY(!queue->isRunning());
    }

    void testRepeatCountDecrements()
    {
        // 测试目的：验证发送后remaining递减
        QSignalSpy sendSpy(queue, &SendQueue::sendRequested);

        queue->enqueue(QByteArray::fromHex("01"), "Test", 3, 10);
        queue->start();

        // 等待第一次发送
        QTest::qWait(50);

        QCOMPARE(sendSpy.count(), 1);
        QCOMPARE(queue->itemAt(0).remaining, 2);
    }
};

QTEST_MAIN(TestSendQueue)
#include "TestSendQueue.moc"
