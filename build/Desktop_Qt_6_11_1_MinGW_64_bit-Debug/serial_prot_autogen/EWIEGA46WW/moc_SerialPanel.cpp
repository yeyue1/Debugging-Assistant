/****************************************************************************
** Meta object code from reading C++ file 'SerialPanel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../SerialPanel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SerialPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11SerialPanelE_t {};
} // unnamed namespace

template <> constexpr inline auto SerialPanel::qt_create_metaobjectdata<qt_meta_tag_ZN11SerialPanelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SerialPanel",
        "connectionStateChanged",
        "",
        "connected",
        "panelDestroyed",
        "portName",
        "refreshPortList",
        "applyConfigFromUi",
        "onOpenClicked",
        "onCloseClicked",
        "onSendClicked",
        "onClearClicked",
        "onHexToggled",
        "enabled",
        "onProtocolChanged",
        "index",
        "onAutoScrollToggled",
        "onTimestampToggled",
        "onSendFileClicked",
        "onTimerSendToggled",
        "onTimerSendIntervalChanged",
        "value",
        "onPortSelectionChanged",
        "onOpened",
        "onClosed",
        "onDataReceived",
        "data",
        "onFrameReady",
        "payload",
        "info",
        "onRawDataReady",
        "onFrameError",
        "message",
        "onSerialError",
        "QSerialPort::SerialPortError",
        "code",
        "onSaveDataClicked",
        "onClearHistoryClicked",
        "onDtrToggled",
        "onRtsToggled",
        "onNewLineChanged",
        "onEncodingChanged",
        "onSaveDeviceAliasClicked",
        "onReconnectClicked",
        "onExportConfigTemplateClicked",
        "onImportConfigTemplateClicked",
        "onFilterChanged",
        "onAddQueueClicked",
        "onInsertQueueClicked",
        "onRemoveQueueClicked",
        "onClearQueueClicked",
        "onStartQueueClicked",
        "onStopQueueClicked",
        "onQueueTableItemChanged",
        "QTableWidgetItem*",
        "item",
        "onQueueSendRequested",
        "displayText",
        "remoteHost",
        "remotePort",
        "peer",
        "updateSendQueueStatus",
        "pendingItems",
        "pendingSends",
        "running"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'panelDestroyed'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'refreshPortList'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'applyConfigFromUi'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCloseClicked'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSendClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClearClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onHexToggled'
        QtMocHelpers::SlotData<void(bool)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'onProtocolChanged'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onAutoScrollToggled'
        QtMocHelpers::SlotData<void(bool)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'onTimestampToggled'
        QtMocHelpers::SlotData<void(bool)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'onSendFileClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTimerSendToggled'
        QtMocHelpers::SlotData<void(bool)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'onTimerSendIntervalChanged'
        QtMocHelpers::SlotData<void(int)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 21 },
        }}),
        // Slot 'onPortSelectionChanged'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpened'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClosed'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDataReceived'
        QtMocHelpers::SlotData<void(const QByteArray &)>(25, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 26 },
        }}),
        // Slot 'onFrameReady'
        QtMocHelpers::SlotData<void(const QByteArray &, const QString &)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 28 }, { QMetaType::QString, 29 },
        }}),
        // Slot 'onRawDataReady'
        QtMocHelpers::SlotData<void(const QByteArray &)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 26 },
        }}),
        // Slot 'onFrameError'
        QtMocHelpers::SlotData<void(const QString &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 32 },
        }}),
        // Slot 'onSerialError'
        QtMocHelpers::SlotData<void(QSerialPort::SerialPortError, const QString &)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 34, 35 }, { QMetaType::QString, 32 },
        }}),
        // Slot 'onSaveDataClicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClearHistoryClicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDtrToggled'
        QtMocHelpers::SlotData<void(bool)>(38, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'onRtsToggled'
        QtMocHelpers::SlotData<void(bool)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'onNewLineChanged'
        QtMocHelpers::SlotData<void(int)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onEncodingChanged'
        QtMocHelpers::SlotData<void(int)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onSaveDeviceAliasClicked'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReconnectClicked'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportConfigTemplateClicked'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImportConfigTemplateClicked'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFilterChanged'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddQueueClicked'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onInsertQueueClicked'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemoveQueueClicked'
        QtMocHelpers::SlotData<void()>(49, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClearQueueClicked'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartQueueClicked'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStopQueueClicked'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onQueueTableItemChanged'
        QtMocHelpers::SlotData<void(QTableWidgetItem *)>(53, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 54, 55 },
        }}),
        // Slot 'onQueueSendRequested'
        QtMocHelpers::SlotData<void(const QByteArray &, const QString &, const QString &, quint16, const QString &)>(56, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 28 }, { QMetaType::QString, 57 }, { QMetaType::QString, 58 }, { QMetaType::UShort, 59 },
            { QMetaType::QString, 60 },
        }}),
        // Slot 'updateSendQueueStatus'
        QtMocHelpers::SlotData<void(int, int, bool)>(61, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 62 }, { QMetaType::Int, 63 }, { QMetaType::Bool, 64 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SerialPanel, qt_meta_tag_ZN11SerialPanelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SerialPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11SerialPanelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11SerialPanelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11SerialPanelE_t>.metaTypes,
    nullptr
} };

void SerialPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SerialPanel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->panelDestroyed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->refreshPortList(); break;
        case 3: _t->applyConfigFromUi(); break;
        case 4: _t->onOpenClicked(); break;
        case 5: _t->onCloseClicked(); break;
        case 6: _t->onSendClicked(); break;
        case 7: _t->onClearClicked(); break;
        case 8: _t->onHexToggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->onProtocolChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->onAutoScrollToggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->onTimestampToggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->onSendFileClicked(); break;
        case 13: _t->onTimerSendToggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 14: _t->onTimerSendIntervalChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->onPortSelectionChanged(); break;
        case 16: _t->onOpened(); break;
        case 17: _t->onClosed(); break;
        case 18: _t->onDataReceived((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 19: _t->onFrameReady((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 20: _t->onRawDataReady((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 21: _t->onFrameError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->onSerialError((*reinterpret_cast<std::add_pointer_t<QSerialPort::SerialPortError>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 23: _t->onSaveDataClicked(); break;
        case 24: _t->onClearHistoryClicked(); break;
        case 25: _t->onDtrToggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 26: _t->onRtsToggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 27: _t->onNewLineChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->onEncodingChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 29: _t->onSaveDeviceAliasClicked(); break;
        case 30: _t->onReconnectClicked(); break;
        case 31: _t->onExportConfigTemplateClicked(); break;
        case 32: _t->onImportConfigTemplateClicked(); break;
        case 33: _t->onFilterChanged(); break;
        case 34: _t->onAddQueueClicked(); break;
        case 35: _t->onInsertQueueClicked(); break;
        case 36: _t->onRemoveQueueClicked(); break;
        case 37: _t->onClearQueueClicked(); break;
        case 38: _t->onStartQueueClicked(); break;
        case 39: _t->onStopQueueClicked(); break;
        case 40: _t->onQueueTableItemChanged((*reinterpret_cast<std::add_pointer_t<QTableWidgetItem*>>(_a[1]))); break;
        case 41: _t->onQueueSendRequested((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5]))); break;
        case 42: _t->updateSendQueueStatus((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SerialPanel::*)(bool )>(_a, &SerialPanel::connectionStateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SerialPanel::*)(const QString & )>(_a, &SerialPanel::panelDestroyed, 1))
            return;
    }
}

const QMetaObject *SerialPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11SerialPanelE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SerialPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 43)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 43;
    }
    return _id;
}

// SIGNAL 0
void SerialPanel::connectionStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void SerialPanel::panelDestroyed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
