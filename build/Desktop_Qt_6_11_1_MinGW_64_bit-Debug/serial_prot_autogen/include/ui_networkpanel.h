/********************************************************************************
** Form generated from reading UI file 'networkpanel.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NETWORKPANEL_H
#define UI_NETWORKPANEL_H

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

class Ui_NetworkPanel
{
public:
    QVBoxLayout *mainLayout;
    QGroupBox *connectionGroup;
    QVBoxLayout *connectionLayout;
    QHBoxLayout *modeRowLayout;
    QLabel *modeLabel;
    QComboBox *modeCombo;
    QSpacerItem *modeSpacerA;
    QLabel *localAddressLabel;
    QLineEdit *localAddressEdit;
    QLabel *localPortLabel;
    QSpinBox *localPortSpin;
    QSpacerItem *modeRowSpacer;
    QHBoxLayout *remoteRowLayout;
    QLabel *remoteHostLabel;
    QLineEdit *remoteHostEdit;
    QLabel *remotePortLabel;
    QSpinBox *remotePortSpin;
    QSpacerItem *remoteSpacerA;
    QPushButton *openButton;
    QPushButton *closeButton;
    QLabel *statusIndicator;
    QLabel *statusLabel;
    QSpacerItem *remoteRowSpacer;
    QPushButton *clearButton;
    QSplitter *mainSplitter;
    QWidget *leftWidget;
    QVBoxLayout *leftLayout;
    QSplitter *leftSplitter;
    QGroupBox *receiveGroup;
    QVBoxLayout *receiveLayout;
    QHBoxLayout *receiveOptionsLayout;
    QCheckBox *hexDisplayCheck;
    QCheckBox *timestampCheck;
    QCheckBox *autoScrollCheck;
    QSpacerItem *receiveOptionsSpacer;
    QLabel *rxCountLabel;
    QLabel *txCountLabel;
    QTextEdit *rxEdit;
    QGroupBox *sendGroup;
    QVBoxLayout *sendLayout;
    QHBoxLayout *sendOptionsLayout;
    QCheckBox *hexSendCheck;
    QLabel *sendTargetLabel;
    QComboBox *sendTargetCombo;
    QLabel *encodingLabel;
    QComboBox *encodingCombo;
    QLabel *sendPreviewLabel;
    QSpacerItem *sendOptionsSpacer;
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
    QHBoxLayout *sendButtonLayout;
    QPushButton *sendButton;
    QPushButton *addQueueButton;
    QPushButton *insertQueueButton;
    QPushButton *removeQueueButton;
    QPushButton *clearQueueButton;
    QPushButton *startQueueButton;
    QPushButton *stopQueueButton;
    QPushButton *sendFileButton;
    QPushButton *saveDataButton;
    QSpacerItem *sendButtonSpacer;
    QCheckBox *timerSendCheck;
    QSpinBox *timerSendInterval;
    QWidget *rightWidget;
    QVBoxLayout *rightLayout;
    QSplitter *rightSplitter;
    QGroupBox *peerGroup;
    QVBoxLayout *peerLayout;
    QListWidget *peerListWidget;
    QGroupBox *statsGroup;
    QVBoxLayout *statsLayout;
    QLabel *modeInfoLabel;
    QLabel *endpointInfoLabel;
    QLabel *lastPeerLabel;
    QLabel *errorLabel;

    void setupUi(QWidget *NetworkPanel)
    {
        if (NetworkPanel->objectName().isEmpty())
            NetworkPanel->setObjectName("NetworkPanel");
        NetworkPanel->resize(1200, 760);
        mainLayout = new QVBoxLayout(NetworkPanel);
        mainLayout->setSpacing(6);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(8, 8, 8, 8);
        connectionGroup = new QGroupBox(NetworkPanel);
        connectionGroup->setObjectName("connectionGroup");
        connectionGroup->setMaximumSize(QSize(16777215, 125));
        connectionLayout = new QVBoxLayout(connectionGroup);
        connectionLayout->setSpacing(8);
        connectionLayout->setObjectName("connectionLayout");
        modeRowLayout = new QHBoxLayout();
        modeRowLayout->setObjectName("modeRowLayout");
        modeLabel = new QLabel(connectionGroup);
        modeLabel->setObjectName("modeLabel");

        modeRowLayout->addWidget(modeLabel);

        modeCombo = new QComboBox(connectionGroup);
        modeCombo->addItem(QString());
        modeCombo->addItem(QString());
        modeCombo->addItem(QString());
        modeCombo->setObjectName("modeCombo");
        modeCombo->setMinimumSize(QSize(120, 0));

        modeRowLayout->addWidget(modeCombo);

        modeSpacerA = new QSpacerItem(12, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        modeRowLayout->addItem(modeSpacerA);

        localAddressLabel = new QLabel(connectionGroup);
        localAddressLabel->setObjectName("localAddressLabel");

        modeRowLayout->addWidget(localAddressLabel);

        localAddressEdit = new QLineEdit(connectionGroup);
        localAddressEdit->setObjectName("localAddressEdit");
        localAddressEdit->setMinimumSize(QSize(130, 0));

        modeRowLayout->addWidget(localAddressEdit);

        localPortLabel = new QLabel(connectionGroup);
        localPortLabel->setObjectName("localPortLabel");

        modeRowLayout->addWidget(localPortLabel);

        localPortSpin = new QSpinBox(connectionGroup);
        localPortSpin->setObjectName("localPortSpin");
        localPortSpin->setMinimum(0);
        localPortSpin->setMaximum(65535);
        localPortSpin->setValue(9000);

        modeRowLayout->addWidget(localPortSpin);

        modeRowSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        modeRowLayout->addItem(modeRowSpacer);


        connectionLayout->addLayout(modeRowLayout);

        remoteRowLayout = new QHBoxLayout();
        remoteRowLayout->setObjectName("remoteRowLayout");
        remoteHostLabel = new QLabel(connectionGroup);
        remoteHostLabel->setObjectName("remoteHostLabel");

        remoteRowLayout->addWidget(remoteHostLabel);

        remoteHostEdit = new QLineEdit(connectionGroup);
        remoteHostEdit->setObjectName("remoteHostEdit");
        remoteHostEdit->setMinimumSize(QSize(180, 0));

        remoteRowLayout->addWidget(remoteHostEdit);

        remotePortLabel = new QLabel(connectionGroup);
        remotePortLabel->setObjectName("remotePortLabel");

        remoteRowLayout->addWidget(remotePortLabel);

        remotePortSpin = new QSpinBox(connectionGroup);
        remotePortSpin->setObjectName("remotePortSpin");
        remotePortSpin->setMinimum(1);
        remotePortSpin->setMaximum(65535);
        remotePortSpin->setValue(9000);

        remoteRowLayout->addWidget(remotePortSpin);

        remoteSpacerA = new QSpacerItem(12, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        remoteRowLayout->addItem(remoteSpacerA);

        openButton = new QPushButton(connectionGroup);
        openButton->setObjectName("openButton");

        remoteRowLayout->addWidget(openButton);

        closeButton = new QPushButton(connectionGroup);
        closeButton->setObjectName("closeButton");
        closeButton->setEnabled(false);

        remoteRowLayout->addWidget(closeButton);

        statusIndicator = new QLabel(connectionGroup);
        statusIndicator->setObjectName("statusIndicator");
        statusIndicator->setMinimumSize(QSize(12, 12));
        statusIndicator->setMaximumSize(QSize(12, 12));

        remoteRowLayout->addWidget(statusIndicator);

        statusLabel = new QLabel(connectionGroup);
        statusLabel->setObjectName("statusLabel");

        remoteRowLayout->addWidget(statusLabel);

        remoteRowSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        remoteRowLayout->addItem(remoteRowSpacer);

        clearButton = new QPushButton(connectionGroup);
        clearButton->setObjectName("clearButton");

        remoteRowLayout->addWidget(clearButton);


        connectionLayout->addLayout(remoteRowLayout);


        mainLayout->addWidget(connectionGroup);

        mainSplitter = new QSplitter(NetworkPanel);
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
        receiveGroup->setMinimumSize(QSize(0, 320));
        receiveLayout = new QVBoxLayout(receiveGroup);
        receiveLayout->setObjectName("receiveLayout");
        receiveOptionsLayout = new QHBoxLayout();
        receiveOptionsLayout->setObjectName("receiveOptionsLayout");
        hexDisplayCheck = new QCheckBox(receiveGroup);
        hexDisplayCheck->setObjectName("hexDisplayCheck");

        receiveOptionsLayout->addWidget(hexDisplayCheck);

        timestampCheck = new QCheckBox(receiveGroup);
        timestampCheck->setObjectName("timestampCheck");

        receiveOptionsLayout->addWidget(timestampCheck);

        autoScrollCheck = new QCheckBox(receiveGroup);
        autoScrollCheck->setObjectName("autoScrollCheck");
        autoScrollCheck->setChecked(true);

        receiveOptionsLayout->addWidget(autoScrollCheck);

        receiveOptionsSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        receiveOptionsLayout->addItem(receiveOptionsSpacer);

        rxCountLabel = new QLabel(receiveGroup);
        rxCountLabel->setObjectName("rxCountLabel");

        receiveOptionsLayout->addWidget(rxCountLabel);

        txCountLabel = new QLabel(receiveGroup);
        txCountLabel->setObjectName("txCountLabel");

        receiveOptionsLayout->addWidget(txCountLabel);


        receiveLayout->addLayout(receiveOptionsLayout);

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
        sendGroup->setMaximumSize(QSize(16777215, 350));
        sendLayout = new QVBoxLayout(sendGroup);
        sendLayout->setObjectName("sendLayout");
        sendOptionsLayout = new QHBoxLayout();
        sendOptionsLayout->setObjectName("sendOptionsLayout");
        hexSendCheck = new QCheckBox(sendGroup);
        hexSendCheck->setObjectName("hexSendCheck");

        sendOptionsLayout->addWidget(hexSendCheck);

        sendTargetLabel = new QLabel(sendGroup);
        sendTargetLabel->setObjectName("sendTargetLabel");

        sendOptionsLayout->addWidget(sendTargetLabel);

        sendTargetCombo = new QComboBox(sendGroup);
        sendTargetCombo->addItem(QString());
        sendTargetCombo->setObjectName("sendTargetCombo");

        sendOptionsLayout->addWidget(sendTargetCombo);

        encodingLabel = new QLabel(sendGroup);
        encodingLabel->setObjectName("encodingLabel");

        sendOptionsLayout->addWidget(encodingLabel);

        encodingCombo = new QComboBox(sendGroup);
        encodingCombo->addItem(QString());
        encodingCombo->addItem(QString());
        encodingCombo->addItem(QString());
        encodingCombo->setObjectName("encodingCombo");
        encodingCombo->setMinimumSize(QSize(90, 0));

        sendOptionsLayout->addWidget(encodingCombo);

        sendPreviewLabel = new QLabel(sendGroup);
        sendPreviewLabel->setObjectName("sendPreviewLabel");

        sendOptionsLayout->addWidget(sendPreviewLabel);

        sendOptionsSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        sendOptionsLayout->addItem(sendOptionsSpacer);


        sendLayout->addLayout(sendOptionsLayout);

        txEdit = new QTextEdit(sendGroup);
        txEdit->setObjectName("txEdit");
        txEdit->setMaximumSize(QSize(16777215, 58));

        sendLayout->addWidget(txEdit);

        queueOptionsLayout = new QHBoxLayout();
        queueOptionsLayout->setObjectName("queueOptionsLayout");
        queueRepeatLabel = new QLabel(sendGroup);
        queueRepeatLabel->setObjectName("queueRepeatLabel");

        queueOptionsLayout->addWidget(queueRepeatLabel);

        queueRepeatSpin = new QSpinBox(sendGroup);
        queueRepeatSpin->setObjectName("queueRepeatSpin");
        queueRepeatSpin->setMinimum(1);
        queueRepeatSpin->setMaximum(9999);
        queueRepeatSpin->setValue(1);

        queueOptionsLayout->addWidget(queueRepeatSpin);

        queueIntervalLabel = new QLabel(sendGroup);
        queueIntervalLabel->setObjectName("queueIntervalLabel");

        queueOptionsLayout->addWidget(queueIntervalLabel);

        queueIntervalSpin = new QSpinBox(sendGroup);
        queueIntervalSpin->setObjectName("queueIntervalSpin");
        queueIntervalSpin->setMinimum(10);
        queueIntervalSpin->setMaximum(600000);
        queueIntervalSpin->setValue(1000);

        queueOptionsLayout->addWidget(queueIntervalSpin);

        queueLoopCheck = new QCheckBox(sendGroup);
        queueLoopCheck->setObjectName("queueLoopCheck");

        queueOptionsLayout->addWidget(queueLoopCheck);

        queueStatusLabel = new QLabel(sendGroup);
        queueStatusLabel->setObjectName("queueStatusLabel");

        queueOptionsLayout->addWidget(queueStatusLabel);

        queueOptionsSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        queueOptionsLayout->addItem(queueOptionsSpacer);


        sendLayout->addLayout(queueOptionsLayout);

        queueTable = new QTableWidget(sendGroup);
        if (queueTable->columnCount() < 8)
            queueTable->setColumnCount(8);
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
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        queueTable->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        queueTable->setObjectName("queueTable");
        queueTable->setMinimumSize(QSize(0, 78));
        queueTable->setAlternatingRowColors(true);
        queueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        queueTable->setSelectionMode(QAbstractItemView::SingleSelection);

        sendLayout->addWidget(queueTable);

        sendButtonLayout = new QHBoxLayout();
        sendButtonLayout->setObjectName("sendButtonLayout");
        sendButton = new QPushButton(sendGroup);
        sendButton->setObjectName("sendButton");

        sendButtonLayout->addWidget(sendButton);

        addQueueButton = new QPushButton(sendGroup);
        addQueueButton->setObjectName("addQueueButton");

        sendButtonLayout->addWidget(addQueueButton);

        insertQueueButton = new QPushButton(sendGroup);
        insertQueueButton->setObjectName("insertQueueButton");

        sendButtonLayout->addWidget(insertQueueButton);

        removeQueueButton = new QPushButton(sendGroup);
        removeQueueButton->setObjectName("removeQueueButton");

        sendButtonLayout->addWidget(removeQueueButton);

        clearQueueButton = new QPushButton(sendGroup);
        clearQueueButton->setObjectName("clearQueueButton");

        sendButtonLayout->addWidget(clearQueueButton);

        startQueueButton = new QPushButton(sendGroup);
        startQueueButton->setObjectName("startQueueButton");

        sendButtonLayout->addWidget(startQueueButton);

        stopQueueButton = new QPushButton(sendGroup);
        stopQueueButton->setObjectName("stopQueueButton");

        sendButtonLayout->addWidget(stopQueueButton);

        sendFileButton = new QPushButton(sendGroup);
        sendFileButton->setObjectName("sendFileButton");

        sendButtonLayout->addWidget(sendFileButton);

        saveDataButton = new QPushButton(sendGroup);
        saveDataButton->setObjectName("saveDataButton");

        sendButtonLayout->addWidget(saveDataButton);

        sendButtonSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        sendButtonLayout->addItem(sendButtonSpacer);

        timerSendCheck = new QCheckBox(sendGroup);
        timerSendCheck->setObjectName("timerSendCheck");

        sendButtonLayout->addWidget(timerSendCheck);

        timerSendInterval = new QSpinBox(sendGroup);
        timerSendInterval->setObjectName("timerSendInterval");
        timerSendInterval->setEnabled(false);
        timerSendInterval->setMaximumSize(QSize(100, 16777215));
        timerSendInterval->setMinimum(100);
        timerSendInterval->setMaximum(60000);
        timerSendInterval->setValue(1000);

        sendButtonLayout->addWidget(timerSendInterval);


        sendLayout->addLayout(sendButtonLayout);

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
        rightWidget->setMaximumSize(QSize(520, 16777215));
        rightLayout = new QVBoxLayout(rightWidget);
        rightLayout->setSpacing(6);
        rightLayout->setObjectName("rightLayout");
        rightLayout->setContentsMargins(0, 0, 0, 0);
        rightSplitter = new QSplitter(rightWidget);
        rightSplitter->setObjectName("rightSplitter");
        rightSplitter->setHandleWidth(6);
        rightSplitter->setOrientation(Qt::Vertical);
        rightSplitter->setChildrenCollapsible(false);
        peerGroup = new QGroupBox(rightSplitter);
        peerGroup->setObjectName("peerGroup");
        peerGroup->setMinimumSize(QSize(0, 280));
        peerLayout = new QVBoxLayout(peerGroup);
        peerLayout->setObjectName("peerLayout");
        peerListWidget = new QListWidget(peerGroup);
        peerListWidget->setObjectName("peerListWidget");

        peerLayout->addWidget(peerListWidget);

        rightSplitter->addWidget(peerGroup);
        statsGroup = new QGroupBox(rightSplitter);
        statsGroup->setObjectName("statsGroup");
        statsGroup->setMaximumSize(QSize(16777215, 160));
        statsLayout = new QVBoxLayout(statsGroup);
        statsLayout->setObjectName("statsLayout");
        modeInfoLabel = new QLabel(statsGroup);
        modeInfoLabel->setObjectName("modeInfoLabel");

        statsLayout->addWidget(modeInfoLabel);

        endpointInfoLabel = new QLabel(statsGroup);
        endpointInfoLabel->setObjectName("endpointInfoLabel");

        statsLayout->addWidget(endpointInfoLabel);

        lastPeerLabel = new QLabel(statsGroup);
        lastPeerLabel->setObjectName("lastPeerLabel");

        statsLayout->addWidget(lastPeerLabel);

        errorLabel = new QLabel(statsGroup);
        errorLabel->setObjectName("errorLabel");

        statsLayout->addWidget(errorLabel);

        rightSplitter->addWidget(statsGroup);

        rightLayout->addWidget(rightSplitter);

        mainSplitter->addWidget(rightWidget);

        mainLayout->addWidget(mainSplitter);


        retranslateUi(NetworkPanel);

        QMetaObject::connectSlotsByName(NetworkPanel);
    } // setupUi

    void retranslateUi(QWidget *NetworkPanel)
    {
        connectionGroup->setTitle(QCoreApplication::translate("NetworkPanel", "\347\275\221\345\217\243\350\277\236\346\216\245", nullptr));
        modeLabel->setText(QCoreApplication::translate("NetworkPanel", "\346\250\241\345\274\217:", nullptr));
        modeCombo->setItemText(0, QCoreApplication::translate("NetworkPanel", "TCP \345\256\242\346\210\267\347\253\257", nullptr));
        modeCombo->setItemText(1, QCoreApplication::translate("NetworkPanel", "TCP \346\234\215\345\212\241\347\253\257", nullptr));
        modeCombo->setItemText(2, QCoreApplication::translate("NetworkPanel", "UDP", nullptr));

        localAddressLabel->setText(QCoreApplication::translate("NetworkPanel", "\346\234\254\345\234\260\345\234\260\345\235\200:", nullptr));
        localAddressEdit->setText(QCoreApplication::translate("NetworkPanel", "0.0.0.0", nullptr));
        localPortLabel->setText(QCoreApplication::translate("NetworkPanel", "\346\234\254\345\234\260\347\253\257\345\217\243:", nullptr));
        remoteHostLabel->setText(QCoreApplication::translate("NetworkPanel", "\350\277\234\347\253\257\345\234\260\345\235\200:", nullptr));
        remoteHostEdit->setText(QCoreApplication::translate("NetworkPanel", "127.0.0.1", nullptr));
        remotePortLabel->setText(QCoreApplication::translate("NetworkPanel", "\350\277\234\347\253\257\347\253\257\345\217\243:", nullptr));
        openButton->setText(QCoreApplication::translate("NetworkPanel", "\346\211\223\345\274\200\347\275\221\345\217\243", nullptr));
        closeButton->setText(QCoreApplication::translate("NetworkPanel", "\345\205\263\351\227\255\347\275\221\345\217\243", nullptr));
        statusLabel->setText(QCoreApplication::translate("NetworkPanel", "\346\234\252\346\211\223\345\274\200", nullptr));
        clearButton->setText(QCoreApplication::translate("NetworkPanel", "\346\270\205\347\251\272", nullptr));
        receiveGroup->setTitle(QCoreApplication::translate("NetworkPanel", "\346\216\245\346\224\266\345\214\272", nullptr));
        hexDisplayCheck->setText(QCoreApplication::translate("NetworkPanel", "\345\215\201\345\205\255\350\277\233\345\210\266\346\230\276\347\244\272", nullptr));
        timestampCheck->setText(QCoreApplication::translate("NetworkPanel", "\346\227\266\351\227\264\346\210\263", nullptr));
        autoScrollCheck->setText(QCoreApplication::translate("NetworkPanel", "\350\207\252\345\212\250\346\273\232\345\212\250", nullptr));
        rxCountLabel->setText(QCoreApplication::translate("NetworkPanel", "\346\216\245\346\224\266: 0", nullptr));
        txCountLabel->setText(QCoreApplication::translate("NetworkPanel", "\345\217\221\351\200\201: 0", nullptr));
        sendGroup->setTitle(QCoreApplication::translate("NetworkPanel", "\345\217\221\351\200\201\345\214\272", nullptr));
        hexSendCheck->setText(QCoreApplication::translate("NetworkPanel", "\345\215\201\345\205\255\350\277\233\345\210\266\345\217\221\351\200\201", nullptr));
        sendTargetLabel->setText(QCoreApplication::translate("NetworkPanel", "\345\217\221\351\200\201\347\233\256\346\240\207:", nullptr));
        sendTargetCombo->setItemText(0, QCoreApplication::translate("NetworkPanel", "\345\205\250\351\203\250\345\256\242\346\210\267\347\253\257", nullptr));

        encodingLabel->setText(QCoreApplication::translate("NetworkPanel", "\347\274\226\347\240\201:", nullptr));
        encodingCombo->setItemText(0, QCoreApplication::translate("NetworkPanel", "UTF-8", nullptr));
        encodingCombo->setItemText(1, QCoreApplication::translate("NetworkPanel", "GBK", nullptr));
        encodingCombo->setItemText(2, QCoreApplication::translate("NetworkPanel", "ASCII", nullptr));

        sendPreviewLabel->setText(QCoreApplication::translate("NetworkPanel", "\345\217\221\351\200\201\351\242\204\344\274\260: 0 B", nullptr));
        queueRepeatLabel->setText(QCoreApplication::translate("NetworkPanel", "\345\205\245\351\230\237\346\254\241\346\225\260:", nullptr));
        queueIntervalLabel->setText(QCoreApplication::translate("NetworkPanel", "\345\205\245\351\230\237\351\227\264\351\232\224:", nullptr));
        queueIntervalSpin->setSuffix(QCoreApplication::translate("NetworkPanel", " ms", nullptr));
        queueLoopCheck->setText(QCoreApplication::translate("NetworkPanel", "\345\276\252\347\216\257\351\230\237\345\210\227", nullptr));
        queueStatusLabel->setText(QCoreApplication::translate("NetworkPanel", "\351\230\237\345\210\227: 0 \351\241\271 / 0 \346\254\241", nullptr));
        QTableWidgetItem *___qtablewidgetitem = queueTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("NetworkPanel", "\345\272\217\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = queueTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("NetworkPanel", "\345\211\251\344\275\231", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = queueTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("NetworkPanel", "\346\254\241\346\225\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = queueTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("NetworkPanel", "\351\227\264\351\232\224(ms)", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = queueTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("NetworkPanel", "\345\255\227\350\212\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = queueTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("NetworkPanel", "\347\233\256\346\240\207", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = queueTable->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("NetworkPanel", "\345\215\201\345\205\255\350\277\233\345\210\266", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = queueTable->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("NetworkPanel", "\345\206\205\345\256\271", nullptr));
        sendButton->setText(QCoreApplication::translate("NetworkPanel", "\345\217\221\351\200\201", nullptr));
        addQueueButton->setText(QCoreApplication::translate("NetworkPanel", "\345\212\240\345\205\245\351\230\237\345\210\227", nullptr));
        insertQueueButton->setText(QCoreApplication::translate("NetworkPanel", "\346\217\222\345\205\245\351\200\211\344\270\255\345\211\215", nullptr));
        removeQueueButton->setText(QCoreApplication::translate("NetworkPanel", "\345\210\240\351\231\244\351\200\211\344\270\255", nullptr));
        clearQueueButton->setText(QCoreApplication::translate("NetworkPanel", "\346\270\205\347\251\272\351\230\237\345\210\227", nullptr));
        startQueueButton->setText(QCoreApplication::translate("NetworkPanel", "\345\274\200\345\247\213\351\230\237\345\210\227", nullptr));
        stopQueueButton->setText(QCoreApplication::translate("NetworkPanel", "\345\201\234\346\255\242", nullptr));
        sendFileButton->setText(QCoreApplication::translate("NetworkPanel", "\345\217\221\351\200\201\346\226\207\344\273\266", nullptr));
        saveDataButton->setText(QCoreApplication::translate("NetworkPanel", "\344\277\235\345\255\230\346\225\260\346\215\256", nullptr));
        timerSendCheck->setText(QCoreApplication::translate("NetworkPanel", "\345\256\232\346\227\266\345\217\221\351\200\201", nullptr));
        timerSendInterval->setSuffix(QCoreApplication::translate("NetworkPanel", " ms", nullptr));
        peerGroup->setTitle(QCoreApplication::translate("NetworkPanel", "\350\277\236\346\216\245\345\257\271\350\261\241", nullptr));
        statsGroup->setTitle(QCoreApplication::translate("NetworkPanel", "\347\212\266\346\200\201\347\273\237\350\256\241", nullptr));
        modeInfoLabel->setText(QCoreApplication::translate("NetworkPanel", "\346\250\241\345\274\217: TCP \345\256\242\346\210\267\347\253\257", nullptr));
        endpointInfoLabel->setText(QCoreApplication::translate("NetworkPanel", "\347\253\257\347\202\271: 127.0.0.1:9000", nullptr));
        lastPeerLabel->setText(QCoreApplication::translate("NetworkPanel", "\346\234\200\350\277\221\346\235\245\346\272\220: -", nullptr));
        errorLabel->setText(QCoreApplication::translate("NetworkPanel", "\346\234\200\350\277\221\351\224\231\350\257\257: -", nullptr));
        (void)NetworkPanel;
    } // retranslateUi

};

namespace Ui {
    class NetworkPanel: public Ui_NetworkPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NETWORKPANEL_H
