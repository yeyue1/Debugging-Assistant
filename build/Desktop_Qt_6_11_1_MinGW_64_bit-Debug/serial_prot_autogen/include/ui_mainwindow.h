/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *addPortAction;
    QAction *addNetworkAction;
    QAction *closeTabAction;
    QAction *applySerialConfigAction;
    QAction *exitAction;
    QAction *darkThemeAction;
    QAction *lightThemeAction;
    QAction *aboutAction;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuTheme;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1200, 800);
        addPortAction = new QAction(MainWindow);
        addPortAction->setObjectName("addPortAction");
        addNetworkAction = new QAction(MainWindow);
        addNetworkAction->setObjectName("addNetworkAction");
        closeTabAction = new QAction(MainWindow);
        closeTabAction->setObjectName("closeTabAction");
        applySerialConfigAction = new QAction(MainWindow);
        applySerialConfigAction->setObjectName("applySerialConfigAction");
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName("exitAction");
        darkThemeAction = new QAction(MainWindow);
        darkThemeAction->setObjectName("darkThemeAction");
        darkThemeAction->setCheckable(true);
        darkThemeAction->setChecked(true);
        lightThemeAction = new QAction(MainWindow);
        lightThemeAction->setObjectName("lightThemeAction");
        lightThemeAction->setCheckable(true);
        aboutAction = new QAction(MainWindow);
        aboutAction->setObjectName("aboutAction");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setDocumentMode(true);
        tabWidget->setTabsClosable(true);
        tabWidget->setMovable(true);

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1200, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuTheme = new QMenu(menubar);
        menuTheme->setObjectName("menuTheme");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName("mainToolBar");
        mainToolBar->setMovable(false);
        mainToolBar->setIconSize(QSize(24, 24));
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuTheme->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(addPortAction);
        menuFile->addAction(addNetworkAction);
        menuFile->addAction(closeTabAction);
        menuFile->addSeparator();
        menuFile->addAction(exitAction);
        menuTheme->addAction(darkThemeAction);
        menuTheme->addAction(lightThemeAction);
        menuHelp->addAction(aboutAction);
        mainToolBar->addAction(addPortAction);
        mainToolBar->addAction(addNetworkAction);
        mainToolBar->addSeparator();
        mainToolBar->addAction(closeTabAction);
        mainToolBar->addSeparator();
        mainToolBar->addAction(applySerialConfigAction);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\351\200\232\344\277\241\350\260\203\350\257\225\345\267\245\345\205\267", nullptr));
        addPortAction->setText(QCoreApplication::translate("MainWindow", "\345\242\236\345\212\240\344\270\262\345\217\243(&A)", nullptr));
#if QT_CONFIG(shortcut)
        addPortAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        addNetworkAction->setText(QCoreApplication::translate("MainWindow", "\345\242\236\345\212\240\347\275\221\345\217\243(&W)", nullptr));
#if QT_CONFIG(shortcut)
        addNetworkAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+N", nullptr));
#endif // QT_CONFIG(shortcut)
        closeTabAction->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\345\275\223\345\211\215(&D)", nullptr));
#if QT_CONFIG(shortcut)
        closeTabAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+W", nullptr));
#endif // QT_CONFIG(shortcut)
        applySerialConfigAction->setText(QCoreApplication::translate("MainWindow", "\345\272\224\347\224\250\344\270\262\345\217\243\351\205\215\347\275\256\345\210\260\345\205\250\351\203\250\344\270\262\345\217\243", nullptr));
        exitAction->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272(&X)", nullptr));
#if QT_CONFIG(shortcut)
        exitAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        darkThemeAction->setText(QCoreApplication::translate("MainWindow", "\346\232\227\350\211\262\344\270\273\351\242\230", nullptr));
        lightThemeAction->setText(QCoreApplication::translate("MainWindow", "\344\272\256\350\211\262\344\270\273\351\242\230", nullptr));
        aboutAction->setText(QCoreApplication::translate("MainWindow", "\345\205\263\344\272\216(&A)", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "\345\212\237\350\203\275(&F)", nullptr));
        menuTheme->setTitle(QCoreApplication::translate("MainWindow", "\344\270\273\351\242\230(&T)", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251(&H)", nullptr));
        mainToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "\344\270\273\345\267\245\345\205\267\346\240\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
