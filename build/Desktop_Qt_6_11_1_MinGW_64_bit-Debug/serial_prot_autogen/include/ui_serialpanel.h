/********************************************************************************
** Form generated from reading UI file 'serialpanel.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERIALPANEL_H
#define UI_SERIALPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SerialPanel
{
public:
    QVBoxLayout *mainLayout;
    QSplitter *rootSplitter;
    QGroupBox *connectionGroup;
    QHBoxLayout *connectionLayout;
    QSplitter *connectionSplitter;
    QWidget *connectionConfigWidget;
    QHBoxLayout *configRowLayout;
    QLabel *portLabel;
    QComboBox *portCombo;
    QSpacerItem *configSpacerA;
    QLabel *baudLabel;
    QComboBox *baudCombo;
    QLabel *dataLabel;
    QComboBox *dataBitsCombo;
    QLabel *parityLabel;
    QComboBox *parityCombo;
    QLabel *stopLabel;
    QComboBox *stopBitsCombo;
    QSpacerItem *configRowSpacer;
    QWidget *connectionTemplateWidget;
    QHBoxLayout *templateRowLayout;
    QLabel *aliasLabel;
    QLineEdit *deviceAliasEdit;
    QPushButton *saveAliasButton;
    QPushButton *reconnectButton;
    QPushButton *exportTemplateButton;
    QPushButton *importTemplateButton;
    QSpacerItem *templateRowSpacer;
    QWidget *connectionStateWidget;
    QHBoxLayout *stateRowLayout;
    QCheckBox *dtrCheck;
    QCheckBox *rtsCheck;
    QSpacerItem *stateSpacerA;
    QPushButton *openButton;
    QPushButton *closeButton;
    QLabel *statusIndicator;
    QLabel *statusLabel;
    QSpacerItem *stateSpacerB;
    QLabel *rxCountLabel;
    QLabel *txCountLabel;
    QSpacerItem *stateRowSpacer;
    QPushButton *clearButton;
    QSplitter *mainSplitter;
    QWidget *leftWidget;
    QVBoxLayout *leftLayout;
    QSplitter *leftSplitter;
    QGroupBox *receiveGroup;
    QVBoxLayout *receiveLayout;
    QHBoxLayout *deviceLayout;
    QLabel *deviceInfoLabel;
    QLabel *healthLabel;
    QLabel *statsLabel;
    QHBoxLayout *receiveOptionsLayout;
    QCheckBox *hexCheck;
    QCheckBox *autoScrollCheck;
    QCheckBox *timestampCheck;
    QSpacerItem *receiveOptionsSpacer;
    QLabel *protocolLabel;
    QComboBox *protocolCombo;
    QHBoxLayout *filterLayout;
    QLabel *filterLabel;
    QLineEdit *filterEdit;
    QCheckBox *filterRegexCheck;
    QCheckBox *filterHideCheck;
    QTextEdit *rxEdit;
    QGroupBox *sendGroup;
    QVBoxLayout *sendLayout;
    QHBoxLayout *sendOptionsLayout;
    QLabel *newLineLabel;
    QComboBox *newLineCombo;
    QLabel *encodingLabel;
    QComboBox *encodingCombo;
    QLabel *sendPreviewLabel;
    QSpacerItem *sendOptionsSpacer;
    QSplitter *sendContentSplitter;
    QWidget *sendInputWidget;
    QVBoxLayout *sendInputLayout;
    QTextEdit *txEdit;
    QHBoxLayout *queueOptionsLayout;
    QLabel *queueRepeatLabel;
    QSpinBox *queueRepeatSpin;
    QLabel *queueIntervalLabel;
    QSpinBox *queueIntervalSpin;
    QCheckBox *queueLoopCheck;
    QLabel *queueStatusLabel;
    QSpacerItem *queueOptionsSpacer;
    QTableWidget *queueTable;
    QHBoxLayout *sendButtonsLayout;
    QPushButton *sendButton;
    QPushButton *addQueueButton;
    QPushButton *insertQueueButton;
    QPushButton *removeQueueButton;
    QPushButton *clearQueueButton;
    QPushButton *startQueueButton;
    QPushButton *stopQueueButton;
    QPushButton *sendFileButton;
    QPushButton *saveDataButton;
    QSpacerItem *sendButtonsSpacer;
    QCheckBox *timerSendCheck;
    QSpinBox *timerSendInterval;
    QWidget *rightWidget;
    QVBoxLayout *rightLayout;
    QSplitter *rightSplitter;
    QGroupBox *historyGroup;
    QVBoxLayout *historyLayout;
    QListWidget *historyList;
    QPushButton *clearHistoryButton;
    QGroupBox *automationGroup;
    QVBoxLayout *automationLayout;
    QHBoxLayout *replyLayout;
    QCheckBox *autoReplyCheck;
    QLineEdit *autoReplyPatternEdit;
    QLineEdit *autoReplyTextEdit;
    QCheckBox *autoReplyRegexCheck;
    QGroupBox *parsedGroup;
    QVBoxLayout *parsedLayout;
    QTableWidget *frameTable;
    QTextEdit *jsonPreviewEdit;

    void setupUi(QWidget *SerialPanel)
    {
        if (SerialPanel->objectName().isEmpty())
            SerialPanel->setObjectName("SerialPanel");
        SerialPanel->resize(1200, 760);
        mainLayout = new QVBoxLayout(SerialPanel);
        mainLayout->setSpacing(6);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(8, 8, 8, 8);
        rootSplitter = new QSplitter(SerialPanel);
        rootSplitter->setObjectName("rootSplitter");
        rootSplitter->setHandleWidth(6);
        rootSplitter->setOrientation(Qt::Vertical);
        rootSplitter->setChildrenCollapsible(false);
        connectionGroup = new QGroupBox(rootSplitter);
        connectionGroup->setObjectName("connectionGroup");
        connectionGroup->setMinimumSize(QSize(0, 96));
        connectionGroup->setMaximumSize(QSize(16777215, 220));
        connectionLayout = new QHBoxLayout(connectionGroup);
        connectionLayout->setSpacing(10);
        connectionLayout->setObjectName("connectionLayout");
        connectionLayout->setContentsMargins(12, 6, 12, 6);
        connectionSplitter = new QSplitter(connectionGroup);
        connectionSplitter->setObjectName("connectionSplitter");
        connectionSplitter->setHandleWidth(6);
        connectionSplitter->setOrientation(Qt::Horizontal);
        connectionSplitter->setChildrenCollapsible(false);
        connectionConfigWidget = new QWidget(connectionSplitter);
        connectionConfigWidget->setObjectName("connectionConfigWidget");
        connectionConfigWidget->setMinimumSize(QSize(620, 0));
        configRowLayout = new QHBoxLayout(connectionConfigWidget);
        configRowLayout->setSpacing(6);
        configRowLayout->setObjectName("configRowLayout");
        configRowLayout->setContentsMargins(0, 0, 0, 0);
        portLabel = new QLabel(connectionConfigWidget);
        portLabel->setObjectName("portLabel");

        configRowLayout->addWidget(portLabel);

        portCombo = new QComboBox(connectionConfigWidget);
        portCombo->setObjectName("portCombo");
        portCombo->setMinimumSize(QSize(110, 0));
        portCombo->setMaximumSize(QSize(160, 16777215));

        configRowLayout->addWidget(portCombo);

        configSpacerA = new QSpacerItem(12, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        configRowLayout->addItem(configSpacerA);

        baudLabel = new QLabel(connectionConfigWidget);
        baudLabel->setObjectName("baudLabel");

        configRowLayout->addWidget(baudLabel);

        baudCombo = new QComboBox(connectionConfigWidget);
        baudCombo->setObjectName("baudCombo");
        baudCombo->setMinimumSize(QSize(90, 0));

        configRowLayout->addWidget(baudCombo);

        dataLabel = new QLabel(connectionConfigWidget);
        dataLabel->setObjectName("dataLabel");

        configRowLayout->addWidget(dataLabel);

        dataBitsCombo = new QComboBox(connectionConfigWidget);
        dataBitsCombo->setObjectName("dataBitsCombo");
        dataBitsCombo->setMinimumSize(QSize(58, 0));

        configRowLayout->addWidget(dataBitsCombo);

        parityLabel = new QLabel(connectionConfigWidget);
        parityLabel->setObjectName("parityLabel");

        configRowLayout->addWidget(parityLabel);

        parityCombo = new QComboBox(connectionConfigWidget);
        parityCombo->setObjectName("parityCombo");
        parityCombo->setMinimumSize(QSize(76, 0));

        configRowLayout->addWidget(parityCombo);

        stopLabel = new QLabel(connectionConfigWidget);
        stopLabel->setObjectName("stopLabel");

        configRowLayout->addWidget(stopLabel);

        stopBitsCombo = new QComboBox(connectionConfigWidget);
        stopBitsCombo->setObjectName("stopBitsCombo");
        stopBitsCombo->setMinimumSize(QSize(58, 0));

        configRowLayout->addWidget(stopBitsCombo);

        configRowSpacer = new QSpacerItem(8, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        configRowLayout->addItem(configRowSpacer);

        connectionSplitter->addWidget(connectionConfigWidget);
        connectionTemplateWidget = new QWidget(connectionSplitter);
        connectionTemplateWidget->setObjectName("connectionTemplateWidget");
        connectionTemplateWidget->setMinimumSize(QSize(180, 0));
        templateRowLayout = new QHBoxLayout(connectionTemplateWidget);
        templateRowLayout->setSpacing(6);
        templateRowLayout->setObjectName("templateRowLayout");
        templateRowLayout->setContentsMargins(0, 0, 0, 0);
        aliasLabel = new QLabel(connectionTemplateWidget);
        aliasLabel->setObjectName("aliasLabel");

        templateRowLayout->addWidget(aliasLabel);

        deviceAliasEdit = new QLineEdit(connectionTemplateWidget);
        deviceAliasEdit->setObjectName("deviceAliasEdit");
        deviceAliasEdit->setMinimumSize(QSize(140, 0));
        deviceAliasEdit->setMaximumSize(QSize(260, 16777215));

        templateRowLayout->addWidget(deviceAliasEdit);

        saveAliasButton = new QPushButton(connectionTemplateWidget);
        saveAliasButton->setObjectName("saveAliasButton");

        templateRowLayout->addWidget(saveAliasButton);

        reconnectButton = new QPushButton(connectionTemplateWidget);
        reconnectButton->setObjectName("reconnectButton");

        templateRowLayout->addWidget(reconnectButton);

        exportTemplateButton = new QPushButton(connectionTemplateWidget);
        exportTemplateButton->setObjectName("exportTemplateButton");

        templateRowLayout->addWidget(exportTemplateButton);

        importTemplateButton = new QPushButton(connectionTemplateWidget);
        importTemplateButton->setObjectName("importTemplateButton");

        templateRowLayout->addWidget(importTemplateButton);

        templateRowSpacer = new QSpacerItem(8, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        templateRowLayout->addItem(templateRowSpacer);

        connectionSplitter->addWidget(connectionTemplateWidget);
        connectionStateWidget = new QWidget(connectionSplitter);
        connectionStateWidget->setObjectName("connectionStateWidget");
        connectionStateWidget->setMinimumSize(QSize(320, 0));
        stateRowLayout = new QHBoxLayout(connectionStateWidget);
        stateRowLayout->setSpacing(6);
        stateRowLayout->setObjectName("stateRowLayout");
        stateRowLayout->setContentsMargins(0, 0, 0, 0);
        dtrCheck = new QCheckBox(connectionStateWidget);
        dtrCheck->setObjectName("dtrCheck");

        stateRowLayout->addWidget(dtrCheck);

        rtsCheck = new QCheckBox(connectionStateWidget);
        rtsCheck->setObjectName("rtsCheck");

        stateRowLayout->addWidget(rtsCheck);

        stateSpacerA = new QSpacerItem(12, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        stateRowLayout->addItem(stateSpacerA);

        openButton = new QPushButton(connectionStateWidget);
        openButton->setObjectName("openButton");

        stateRowLayout->addWidget(openButton);

        closeButton = new QPushButton(connectionStateWidget);
        closeButton->setObjectName("closeButton");
        closeButton->setEnabled(false);

        stateRowLayout->addWidget(closeButton);

        statusIndicator = new QLabel(connectionStateWidget);
        statusIndicator->setObjectName("statusIndicator");
        statusIndicator->setMinimumSize(QSize(12, 12));
        statusIndicator->setMaximumSize(QSize(12, 12));

        stateRowLayout->addWidget(statusIndicator);

        statusLabel = new QLabel(connectionStateWidget);
        statusLabel->setObjectName("statusLabel");

        stateRowLayout->addWidget(statusLabel);

        stateSpacerB = new QSpacerItem(12, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        stateRowLayout->addItem(stateSpacerB);

        rxCountLabel = new QLabel(connectionStateWidget);
        rxCountLabel->setObjectName("rxCountLabel");

        stateRowLayout->addWidget(rxCountLabel);

        txCountLabel = new QLabel(connectionStateWidget);
        txCountLabel->setObjectName("txCountLabel");

        stateRowLayout->addWidget(txCountLabel);

        stateRowSpacer = new QSpacerItem(8, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        stateRowLayout->addItem(stateRowSpacer);

        clearButton = new QPushButton(connectionStateWidget);
        clearButton->setObjectName("clearButton");

        stateRowLayout->addWidget(clearButton);

        connectionSplitter->addWidget(connectionStateWidget);

        connectionLayout->addWidget(connectionSplitter);

        rootSplitter->addWidget(connectionGroup);
        mainSplitter = new QSplitter(rootSplitter);
        mainSplitter->setObjectName("mainSplitter");
        mainSplitter->setHandleWidth(6);
        mainSplitter->setOrientation(Qt::Horizontal);
        mainSplitter->setChildrenCollapsible(false);
        leftWidget = new QWidget(mainSplitter);
        leftWidget->setObjectName("leftWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(5);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(leftWidget->sizePolicy().hasHeightForWidth());
        leftWidget->setSizePolicy(sizePolicy);
        leftLayout = new QVBoxLayout(leftWidget);
        leftLayout->setSpacing(6);
        leftLayout->setObjectName("leftLayout");
        leftLayout->setContentsMargins(0, 0, 0, 0);
        leftSplitter = new QSplitter(leftWidget);
        leftSplitter->setObjectName("leftSplitter");
        leftSplitter->setHandleWidth(6);
        leftSplitter->setOrientation(Qt::Vertical);
        leftSplitter->setChildrenCollapsible(false);
        receiveGroup = new QGroupBox(leftSplitter);
        receiveGroup->setObjectName("receiveGroup");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(5);
        sizePolicy1.setHeightForWidth(receiveGroup->sizePolicy().hasHeightForWidth());
        receiveGroup->setSizePolicy(sizePolicy1);
        receiveGroup->setMinimumSize(QSize(0, 300));
        receiveLayout = new QVBoxLayout(receiveGroup);
        receiveLayout->setObjectName("receiveLayout");
        receiveLayout->setContentsMargins(8, 4, 8, 8);
        deviceLayout = new QHBoxLayout();
        deviceLayout->setObjectName("deviceLayout");
        deviceInfoLabel = new QLabel(receiveGroup);
        deviceInfoLabel->setObjectName("deviceInfoLabel");

        deviceLayout->addWidget(deviceInfoLabel);

        healthLabel = new QLabel(receiveGroup);
        healthLabel->setObjectName("healthLabel");

        deviceLayout->addWidget(healthLabel);

        statsLabel = new QLabel(receiveGroup);
        statsLabel->setObjectName("statsLabel");

        deviceLayout->addWidget(statsLabel);


        receiveLayout->addLayout(deviceLayout);

        receiveOptionsLayout = new QHBoxLayout();
        receiveOptionsLayout->setObjectName("receiveOptionsLayout");
        hexCheck = new QCheckBox(receiveGroup);
        hexCheck->setObjectName("hexCheck");

        receiveOptionsLayout->addWidget(hexCheck);

        autoScrollCheck = new QCheckBox(receiveGroup);
        autoScrollCheck->setObjectName("autoScrollCheck");
        autoScrollCheck->setChecked(true);

        receiveOptionsLayout->addWidget(autoScrollCheck);

        timestampCheck = new QCheckBox(receiveGroup);
        timestampCheck->setObjectName("timestampCheck");

        receiveOptionsLayout->addWidget(timestampCheck);

        receiveOptionsSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        receiveOptionsLayout->addItem(receiveOptionsSpacer);

        protocolLabel = new QLabel(receiveGroup);
        protocolLabel->setObjectName("protocolLabel");

        receiveOptionsLayout->addWidget(protocolLabel);

        protocolCombo = new QComboBox(receiveGroup);
        protocolCombo->setObjectName("protocolCombo");
        protocolCombo->setMinimumSize(QSize(120, 0));

        receiveOptionsLayout->addWidget(protocolCombo);


        receiveLayout->addLayout(receiveOptionsLayout);

        filterLayout = new QHBoxLayout();
        filterLayout->setObjectName("filterLayout");
        filterLabel = new QLabel(receiveGroup);
        filterLabel->setObjectName("filterLabel");

        filterLayout->addWidget(filterLabel);

        filterEdit = new QLineEdit(receiveGroup);
        filterEdit->setObjectName("filterEdit");

        filterLayout->addWidget(filterEdit);

        filterRegexCheck = new QCheckBox(receiveGroup);
        filterRegexCheck->setObjectName("filterRegexCheck");

        filterLayout->addWidget(filterRegexCheck);

        filterHideCheck = new QCheckBox(receiveGroup);
        filterHideCheck->setObjectName("filterHideCheck");

        filterLayout->addWidget(filterHideCheck);


        receiveLayout->addLayout(filterLayout);

        rxEdit = new QTextEdit(receiveGroup);
        rxEdit->setObjectName("rxEdit");
        rxEdit->setReadOnly(true);

        receiveLayout->addWidget(rxEdit);

        leftSplitter->addWidget(receiveGroup);
        sendGroup = new QGroupBox(leftSplitter);
        sendGroup->setObjectName("sendGroup");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(2);
        sizePolicy2.setHeightForWidth(sendGroup->sizePolicy().hasHeightForWidth());
        sendGroup->setSizePolicy(sizePolicy2);
        sendGroup->setMaximumSize(QSize(16777215, 16777215));
        sendLayout = new QVBoxLayout(sendGroup);
        sendLayout->setObjectName("sendLayout");
        sendLayout->setContentsMargins(8, 4, 8, 8);
        sendOptionsLayout = new QHBoxLayout();
        sendOptionsLayout->setObjectName("sendOptionsLayout");
        newLineLabel = new QLabel(sendGroup);
        newLineLabel->setObjectName("newLineLabel");

        sendOptionsLayout->addWidget(newLineLabel);

        newLineCombo = new QComboBox(sendGroup);
        newLineCombo->setObjectName("newLineCombo");
        newLineCombo->setMinimumSize(QSize(90, 0));

        sendOptionsLayout->addWidget(newLineCombo);

        encodingLabel = new QLabel(sendGroup);
        encodingLabel->setObjectName("encodingLabel");

        sendOptionsLayout->addWidget(encodingLabel);

        encodingCombo = new QComboBox(sendGroup);
        encodingCombo->setObjectName("encodingCombo");
        encodingCombo->setMinimumSize(QSize(100, 0));

        sendOptionsLayout->addWidget(encodingCombo);

        sendPreviewLabel = new QLabel(sendGroup);
        sendPreviewLabel->setObjectName("sendPreviewLabel");

        sendOptionsLayout->addWidget(sendPreviewLabel);

        sendOptionsSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        sendOptionsLayout->addItem(sendOptionsSpacer);


        sendLayout->addLayout(sendOptionsLayout);

        sendContentSplitter = new QSplitter(sendGroup);
        sendContentSplitter->setObjectName("sendContentSplitter");
        sendContentSplitter->setHandleWidth(6);
        sendContentSplitter->setOrientation(Qt::Vertical);
        sendContentSplitter->setChildrenCollapsible(false);
        sendInputWidget = new QWidget(sendContentSplitter);
        sendInputWidget->setObjectName("sendInputWidget");
        sendInputLayout = new QVBoxLayout(sendInputWidget);
        sendInputLayout->setSpacing(6);
        sendInputLayout->setObjectName("sendInputLayout");
        sendInputLayout->setContentsMargins(0, 0, 0, 0);
        txEdit = new QTextEdit(sendInputWidget);
        txEdit->setObjectName("txEdit");

        sendInputLayout->addWidget(txEdit);

        queueOptionsLayout = new QHBoxLayout();
        queueOptionsLayout->setObjectName("queueOptionsLayout");
        queueRepeatLabel = new QLabel(sendInputWidget);
        queueRepeatLabel->setObjectName("queueRepeatLabel");

        queueOptionsLayout->addWidget(queueRepeatLabel);

        queueRepeatSpin = new QSpinBox(sendInputWidget);
        queueRepeatSpin->setObjectName("queueRepeatSpin");
        queueRepeatSpin->setMinimum(1);
        queueRepeatSpin->setMaximum(9999);
        queueRepeatSpin->setValue(1);

        queueOptionsLayout->addWidget(queueRepeatSpin);

        queueIntervalLabel = new QLabel(sendInputWidget);
        queueIntervalLabel->setObjectName("queueIntervalLabel");

        queueOptionsLayout->addWidget(queueIntervalLabel);

        queueIntervalSpin = new QSpinBox(sendInputWidget);
        queueIntervalSpin->setObjectName("queueIntervalSpin");
        queueIntervalSpin->setMinimum(10);
        queueIntervalSpin->setMaximum(600000);
        queueIntervalSpin->setValue(1000);

        queueOptionsLayout->addWidget(queueIntervalSpin);

        queueLoopCheck = new QCheckBox(sendInputWidget);
        queueLoopCheck->setObjectName("queueLoopCheck");

        queueOptionsLayout->addWidget(queueLoopCheck);

        queueStatusLabel = new QLabel(sendInputWidget);
        queueStatusLabel->setObjectName("queueStatusLabel");

        queueOptionsLayout->addWidget(queueStatusLabel);

        queueOptionsSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        queueOptionsLayout->addItem(queueOptionsSpacer);


        sendInputLayout->addLayout(queueOptionsLayout);

        sendContentSplitter->addWidget(sendInputWidget);
        queueTable = new QTableWidget(sendContentSplitter);
        if (queueTable->columnCount() < 7)
            queueTable->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        queueTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        queueTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        queueTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        queueTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        queueTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        queueTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        queueTable->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        queueTable->setObjectName("queueTable");
        queueTable->setMinimumSize(QSize(0, 78));
        queueTable->setAlternatingRowColors(true);
        queueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        queueTable->setSelectionMode(QAbstractItemView::SingleSelection);
        sendContentSplitter->addWidget(queueTable);

        sendLayout->addWidget(sendContentSplitter);

        sendButtonsLayout = new QHBoxLayout();
        sendButtonsLayout->setObjectName("sendButtonsLayout");
        sendButton = new QPushButton(sendGroup);
        sendButton->setObjectName("sendButton");

        sendButtonsLayout->addWidget(sendButton);

        addQueueButton = new QPushButton(sendGroup);
        addQueueButton->setObjectName("addQueueButton");

        sendButtonsLayout->addWidget(addQueueButton);

        insertQueueButton = new QPushButton(sendGroup);
        insertQueueButton->setObjectName("insertQueueButton");

        sendButtonsLayout->addWidget(insertQueueButton);

        removeQueueButton = new QPushButton(sendGroup);
        removeQueueButton->setObjectName("removeQueueButton");

        sendButtonsLayout->addWidget(removeQueueButton);

        clearQueueButton = new QPushButton(sendGroup);
        clearQueueButton->setObjectName("clearQueueButton");

        sendButtonsLayout->addWidget(clearQueueButton);

        startQueueButton = new QPushButton(sendGroup);
        startQueueButton->setObjectName("startQueueButton");

        sendButtonsLayout->addWidget(startQueueButton);

        stopQueueButton = new QPushButton(sendGroup);
        stopQueueButton->setObjectName("stopQueueButton");

        sendButtonsLayout->addWidget(stopQueueButton);

        sendFileButton = new QPushButton(sendGroup);
        sendFileButton->setObjectName("sendFileButton");

        sendButtonsLayout->addWidget(sendFileButton);

        saveDataButton = new QPushButton(sendGroup);
        saveDataButton->setObjectName("saveDataButton");

        sendButtonsLayout->addWidget(saveDataButton);

        sendButtonsSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        sendButtonsLayout->addItem(sendButtonsSpacer);

        timerSendCheck = new QCheckBox(sendGroup);
        timerSendCheck->setObjectName("timerSendCheck");

        sendButtonsLayout->addWidget(timerSendCheck);

        timerSendInterval = new QSpinBox(sendGroup);
        timerSendInterval->setObjectName("timerSendInterval");
        timerSendInterval->setEnabled(false);
        timerSendInterval->setMaximumSize(QSize(100, 16777215));
        timerSendInterval->setMinimum(100);
        timerSendInterval->setMaximum(60000);
        timerSendInterval->setValue(1000);

        sendButtonsLayout->addWidget(timerSendInterval);


        sendLayout->addLayout(sendButtonsLayout);

        leftSplitter->addWidget(sendGroup);

        leftLayout->addWidget(leftSplitter);

        mainSplitter->addWidget(leftWidget);
        rightWidget = new QWidget(mainSplitter);
        rightWidget->setObjectName("rightWidget");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(2);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(rightWidget->sizePolicy().hasHeightForWidth());
        rightWidget->setSizePolicy(sizePolicy3);
        rightWidget->setMaximumSize(QSize(560, 16777215));
        rightLayout = new QVBoxLayout(rightWidget);
        rightLayout->setSpacing(6);
        rightLayout->setObjectName("rightLayout");
        rightLayout->setContentsMargins(0, 0, 0, 0);
        rightSplitter = new QSplitter(rightWidget);
        rightSplitter->setObjectName("rightSplitter");
        rightSplitter->setHandleWidth(6);
        rightSplitter->setOrientation(Qt::Vertical);
        rightSplitter->setChildrenCollapsible(false);
        historyGroup = new QGroupBox(rightSplitter);
        historyGroup->setObjectName("historyGroup");
        historyGroup->setMaximumSize(QSize(16777215, 220));
        historyLayout = new QVBoxLayout(historyGroup);
        historyLayout->setObjectName("historyLayout");
        historyLayout->setContentsMargins(8, 4, 8, 8);
        historyList = new QListWidget(historyGroup);
        historyList->setObjectName("historyList");
        historyList->setAlternatingRowColors(true);

        historyLayout->addWidget(historyList);

        clearHistoryButton = new QPushButton(historyGroup);
        clearHistoryButton->setObjectName("clearHistoryButton");

        historyLayout->addWidget(clearHistoryButton);

        rightSplitter->addWidget(historyGroup);
        automationGroup = new QGroupBox(rightSplitter);
        automationGroup->setObjectName("automationGroup");
        automationGroup->setMaximumSize(QSize(16777215, 95));
        automationLayout = new QVBoxLayout(automationGroup);
        automationLayout->setObjectName("automationLayout");
        replyLayout = new QHBoxLayout();
        replyLayout->setObjectName("replyLayout");
        autoReplyCheck = new QCheckBox(automationGroup);
        autoReplyCheck->setObjectName("autoReplyCheck");

        replyLayout->addWidget(autoReplyCheck);

        autoReplyPatternEdit = new QLineEdit(automationGroup);
        autoReplyPatternEdit->setObjectName("autoReplyPatternEdit");

        replyLayout->addWidget(autoReplyPatternEdit);

        autoReplyTextEdit = new QLineEdit(automationGroup);
        autoReplyTextEdit->setObjectName("autoReplyTextEdit");

        replyLayout->addWidget(autoReplyTextEdit);

        autoReplyRegexCheck = new QCheckBox(automationGroup);
        autoReplyRegexCheck->setObjectName("autoReplyRegexCheck");

        replyLayout->addWidget(autoReplyRegexCheck);


        automationLayout->addLayout(replyLayout);

        rightSplitter->addWidget(automationGroup);
        parsedGroup = new QGroupBox(rightSplitter);
        parsedGroup->setObjectName("parsedGroup");
        parsedGroup->setMinimumSize(QSize(0, 220));
        parsedLayout = new QVBoxLayout(parsedGroup);
        parsedLayout->setObjectName("parsedLayout");
        frameTable = new QTableWidget(parsedGroup);
        if (frameTable->columnCount() < 4)
            frameTable->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        frameTable->setHorizontalHeaderItem(0, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        frameTable->setHorizontalHeaderItem(1, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        frameTable->setHorizontalHeaderItem(2, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        frameTable->setHorizontalHeaderItem(3, __qtablewidgetitem10);
        frameTable->setObjectName("frameTable");
        frameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        frameTable->setAlternatingRowColors(true);

        parsedLayout->addWidget(frameTable);

        jsonPreviewEdit = new QTextEdit(parsedGroup);
        jsonPreviewEdit->setObjectName("jsonPreviewEdit");
        jsonPreviewEdit->setMaximumSize(QSize(16777215, 120));
        jsonPreviewEdit->setReadOnly(true);

        parsedLayout->addWidget(jsonPreviewEdit);

        rightSplitter->addWidget(parsedGroup);

        rightLayout->addWidget(rightSplitter);

        mainSplitter->addWidget(rightWidget);
        rootSplitter->addWidget(mainSplitter);

        mainLayout->addWidget(rootSplitter);


        retranslateUi(SerialPanel);

        QMetaObject::connectSlotsByName(SerialPanel);
    } // setupUi

    void retranslateUi(QWidget *SerialPanel)
    {
        connectionGroup->setTitle(QCoreApplication::translate("SerialPanel", "\350\277\236\346\216\245\350\256\276\347\275\256", nullptr));
        portLabel->setText(QCoreApplication::translate("SerialPanel", "\347\253\257\345\217\243:", nullptr));
        baudLabel->setText(QCoreApplication::translate("SerialPanel", "\346\263\242\347\211\271\347\216\207:", nullptr));
        dataLabel->setText(QCoreApplication::translate("SerialPanel", "\346\225\260\346\215\256\344\275\215:", nullptr));
        parityLabel->setText(QCoreApplication::translate("SerialPanel", "\346\240\241\351\252\214\344\275\215:", nullptr));
        stopLabel->setText(QCoreApplication::translate("SerialPanel", "\345\201\234\346\255\242\344\275\215:", nullptr));
        aliasLabel->setText(QCoreApplication::translate("SerialPanel", "\345\210\253\345\220\215:", nullptr));
        deviceAliasEdit->setPlaceholderText(QCoreApplication::translate("SerialPanel", "\350\256\276\345\244\207\345\210\253\345\220\215", nullptr));
        saveAliasButton->setText(QCoreApplication::translate("SerialPanel", "\344\277\235\345\255\230\345\210\253\345\220\215", nullptr));
        reconnectButton->setText(QCoreApplication::translate("SerialPanel", "\344\270\200\351\224\256\345\244\215\350\277\236", nullptr));
        exportTemplateButton->setText(QCoreApplication::translate("SerialPanel", "\345\257\274\345\207\272\346\250\241\346\235\277", nullptr));
        importTemplateButton->setText(QCoreApplication::translate("SerialPanel", "\345\257\274\345\205\245\346\250\241\346\235\277", nullptr));
        dtrCheck->setText(QCoreApplication::translate("SerialPanel", "DTR", nullptr));
        rtsCheck->setText(QCoreApplication::translate("SerialPanel", "RTS", nullptr));
        openButton->setText(QCoreApplication::translate("SerialPanel", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        closeButton->setText(QCoreApplication::translate("SerialPanel", "\345\205\263\351\227\255\344\270\262\345\217\243", nullptr));
        statusLabel->setText(QCoreApplication::translate("SerialPanel", "\346\234\252\350\277\236\346\216\245", nullptr));
        rxCountLabel->setText(QCoreApplication::translate("SerialPanel", "\346\216\245\346\224\266: 0", nullptr));
        txCountLabel->setText(QCoreApplication::translate("SerialPanel", "\345\217\221\351\200\201: 0", nullptr));
        clearButton->setText(QCoreApplication::translate("SerialPanel", "\346\270\205\347\251\272", nullptr));
        receiveGroup->setTitle(QCoreApplication::translate("SerialPanel", "\346\216\245\346\224\266\345\214\272", nullptr));
        deviceInfoLabel->setText(QCoreApplication::translate("SerialPanel", "\350\256\276\345\244\207: -", nullptr));
        healthLabel->setText(QCoreApplication::translate("SerialPanel", "\345\201\245\345\272\267: \351\207\215\350\277\236 0\357\274\214\346\226\255\345\274\200 0\357\274\214\351\224\231\350\257\257 0\357\274\214\346\234\200\350\277\221 -", nullptr));
        statsLabel->setText(QCoreApplication::translate("SerialPanel", "\347\273\237\350\256\241: \350\256\260\345\275\225 0/0\357\274\214\345\270\247 0\357\274\214\351\224\231\350\257\257\347\216\207 0.0%", nullptr));
        hexCheck->setText(QCoreApplication::translate("SerialPanel", "\345\215\201\345\205\255\350\277\233\345\210\266\346\230\276\347\244\272", nullptr));
        autoScrollCheck->setText(QCoreApplication::translate("SerialPanel", "\350\207\252\345\212\250\346\273\232\345\212\250", nullptr));
        timestampCheck->setText(QCoreApplication::translate("SerialPanel", "\346\227\266\351\227\264\346\210\263", nullptr));
        protocolLabel->setText(QCoreApplication::translate("SerialPanel", "\345\215\217\350\256\256:", nullptr));
        filterLabel->setText(QCoreApplication::translate("SerialPanel", "\350\277\207\346\273\244:", nullptr));
        filterEdit->setPlaceholderText(QCoreApplication::translate("SerialPanel", "\345\205\263\351\224\256\345\255\227\343\200\201\345\215\201\345\205\255\350\277\233\345\210\266\343\200\201\344\277\241\346\201\257\346\210\226\346\255\243\345\210\231", nullptr));
        filterRegexCheck->setText(QCoreApplication::translate("SerialPanel", "\346\255\243\345\210\231", nullptr));
        filterHideCheck->setText(QCoreApplication::translate("SerialPanel", "\351\232\220\350\227\217\346\234\252\345\214\271\351\205\215", nullptr));
        sendGroup->setTitle(QCoreApplication::translate("SerialPanel", "\345\217\221\351\200\201\345\214\272", nullptr));
        newLineLabel->setText(QCoreApplication::translate("SerialPanel", "\346\215\242\350\241\214:", nullptr));
        encodingLabel->setText(QCoreApplication::translate("SerialPanel", "\347\274\226\347\240\201:", nullptr));
        sendPreviewLabel->setText(QCoreApplication::translate("SerialPanel", "\345\217\221\351\200\201\351\242\204\344\274\260: 0 B", nullptr));
        queueRepeatLabel->setText(QCoreApplication::translate("SerialPanel", "\345\205\245\351\230\237\346\254\241\346\225\260:", nullptr));
        queueIntervalLabel->setText(QCoreApplication::translate("SerialPanel", "\345\205\245\351\230\237\351\227\264\351\232\224:", nullptr));
        queueIntervalSpin->setSuffix(QCoreApplication::translate("SerialPanel", " ms", nullptr));
        queueLoopCheck->setText(QCoreApplication::translate("SerialPanel", "\345\276\252\347\216\257\351\230\237\345\210\227", nullptr));
        queueStatusLabel->setText(QCoreApplication::translate("SerialPanel", "\351\230\237\345\210\227: 0 \351\241\271 / 0 \346\254\241", nullptr));
        QTableWidgetItem *___qtablewidgetitem = queueTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("SerialPanel", "\345\272\217\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = queueTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("SerialPanel", "\345\211\251\344\275\231", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = queueTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("SerialPanel", "\346\254\241\346\225\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = queueTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("SerialPanel", "\351\227\264\351\232\224(ms)", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = queueTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("SerialPanel", "\345\255\227\350\212\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = queueTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("SerialPanel", "\345\215\201\345\205\255\350\277\233\345\210\266", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = queueTable->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("SerialPanel", "\345\206\205\345\256\271", nullptr));
        sendButton->setText(QCoreApplication::translate("SerialPanel", "\345\217\221\351\200\201", nullptr));
        addQueueButton->setText(QCoreApplication::translate("SerialPanel", "\345\212\240\345\205\245\351\230\237\345\210\227", nullptr));
        insertQueueButton->setText(QCoreApplication::translate("SerialPanel", "\346\217\222\345\205\245\351\200\211\344\270\255\345\211\215", nullptr));
        removeQueueButton->setText(QCoreApplication::translate("SerialPanel", "\345\210\240\351\231\244\351\200\211\344\270\255", nullptr));
        clearQueueButton->setText(QCoreApplication::translate("SerialPanel", "\346\270\205\347\251\272\351\230\237\345\210\227", nullptr));
        startQueueButton->setText(QCoreApplication::translate("SerialPanel", "\345\274\200\345\247\213\351\230\237\345\210\227", nullptr));
        stopQueueButton->setText(QCoreApplication::translate("SerialPanel", "\345\201\234\346\255\242", nullptr));
        sendFileButton->setText(QCoreApplication::translate("SerialPanel", "\345\217\221\351\200\201\346\226\207\344\273\266", nullptr));
        saveDataButton->setText(QCoreApplication::translate("SerialPanel", "\344\277\235\345\255\230\346\225\260\346\215\256", nullptr));
        timerSendCheck->setText(QCoreApplication::translate("SerialPanel", "\345\256\232\346\227\266\345\217\221\351\200\201", nullptr));
        timerSendInterval->setSuffix(QCoreApplication::translate("SerialPanel", " ms", nullptr));
        historyGroup->setTitle(QCoreApplication::translate("SerialPanel", "\345\217\221\351\200\201\345\216\206\345\217\262", nullptr));
        clearHistoryButton->setText(QCoreApplication::translate("SerialPanel", "\346\270\205\347\251\272\345\216\206\345\217\262", nullptr));
        automationGroup->setTitle(QCoreApplication::translate("SerialPanel", "\350\207\252\345\212\250\345\214\226", nullptr));
        autoReplyCheck->setText(QCoreApplication::translate("SerialPanel", "\350\207\252\345\212\250\345\233\236\345\244\215", nullptr));
        autoReplyPatternEdit->setPlaceholderText(QCoreApplication::translate("SerialPanel", "\346\216\245\346\224\266\345\214\271\351\205\215", nullptr));
        autoReplyTextEdit->setPlaceholderText(QCoreApplication::translate("SerialPanel", "\345\233\236\345\244\215\345\206\205\345\256\271", nullptr));
        autoReplyRegexCheck->setText(QCoreApplication::translate("SerialPanel", "\346\255\243\345\210\231", nullptr));
        parsedGroup->setTitle(QCoreApplication::translate("SerialPanel", "\350\247\243\346\236\220\347\273\223\346\236\234", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = frameTable->horizontalHeaderItem(0);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("SerialPanel", "\345\255\227\346\256\265", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = frameTable->horizontalHeaderItem(1);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("SerialPanel", "\345\200\274", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = frameTable->horizontalHeaderItem(2);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("SerialPanel", "\345\215\201\345\205\255\350\277\233\345\210\266", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = frameTable->horizontalHeaderItem(3);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("SerialPanel", "\344\277\241\346\201\257", nullptr));
        (void)SerialPanel;
    } // retranslateUi

};

namespace Ui {
    class SerialPanel: public Ui_SerialPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERIALPANEL_H
