#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTabWidget>
#include <QStatusBar>
#include <QActionGroup>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QJsonObject>
#include <QByteArray>

#include "NetworkPanel.h"
#include "SerialPanel.h"
#include "ThemeManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupUiFromForm();
    setupConnections();

    if (!loadSettings()) {
        addSerialPort();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiFromForm()
{
    // Theme action group (exclusive selection)
    m_themeGroup = new QActionGroup(this);
    m_themeGroup->setExclusive(true);
    ui->darkThemeAction->setActionGroup(m_themeGroup);
    ui->lightThemeAction->setActionGroup(m_themeGroup);

    statusBar()->showMessage(tr("就绪    标签: 0"));

    ThemeManager::instance().applyDarkTheme();
}

void MainWindow::setupConnections()
{
    connect(ui->addPortAction, &QAction::triggered, this, &MainWindow::addSerialPort);
    connect(ui->addNetworkAction, &QAction::triggered, this, &MainWindow::addNetworkPort);
    connect(ui->closeTabAction, &QAction::triggered, this, [this]() {
        const int current = ui->tabWidget->currentIndex();
        if (current >= 0) {
            closeSerialPort(current);
        }
    });
    connect(ui->applySerialConfigAction, &QAction::triggered, this, &MainWindow::applyCurrentConfigToAll);
    connect(ui->exitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->darkThemeAction, &QAction::triggered, this, &MainWindow::applyDarkTheme);
    connect(ui->lightThemeAction, &QAction::triggered, this, &MainWindow::applyLightTheme);
    connect(ui->aboutAction, &QAction::triggered, this, &MainWindow::about);

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeSerialPort);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::addSerialPort()
{
    createSerialPortPanel();
}

void MainWindow::addNetworkPort()
{
    createNetworkPanel();
}

SerialPanel* MainWindow::createSerialPortPanel()
{
    m_tabCounter++;
    auto* panel = new SerialPanel(this);

    QString tabName = tr("串口 %1").arg(m_tabCounter);
    int index = ui->tabWidget->addTab(panel, tabName);
    ui->tabWidget->setCurrentIndex(index);

    connect(panel, &SerialPanel::connectionStateChanged, this, &MainWindow::onConnectionStateChanged);

    updateTabCount();
    return panel;
}

NetworkPanel* MainWindow::createNetworkPanel()
{
    m_tabCounter++;
    auto* panel = new NetworkPanel(this);

    const QString tabName = tr("网口 %1").arg(m_tabCounter);
    const int index = ui->tabWidget->addTab(panel, tabName);
    ui->tabWidget->setCurrentIndex(index);

    connect(panel, &NetworkPanel::connectionStateChanged, this, &MainWindow::onConnectionStateChanged);

    updateTabCount();
    return panel;
}

void MainWindow::closeSerialPort(int index)
{
    if (ui->tabWidget->count() <= 1) {
        return;
    }

    QWidget* widget = ui->tabWidget->widget(index);
    auto* serialPanel = qobject_cast<SerialPanel*>(widget);
    auto* networkPanel = qobject_cast<NetworkPanel*>(widget);
    const bool connected = (serialPanel && serialPanel->isConnected())
        || (networkPanel && networkPanel->isConnected());

    if (widget) {
        if (connected) {
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                tr("确认"), tr("当前连接正在使用中，确定要关闭吗？"),
                QMessageBox::Yes | QMessageBox::No);

            if (reply != QMessageBox::Yes) {
                return;
            }
        }

        ui->tabWidget->removeTab(index);
        delete widget;
        updateTabCount();
    }
}

void MainWindow::onTabChanged(int index)
{
    QString status = tr("就绪");
    if (index >= 0) {
        auto* panel = qobject_cast<SerialPanel*>(ui->tabWidget->widget(index));
        if (panel) {
            QString portName = panel->portName();
            if (portName.isEmpty()) {
                status = tr("就绪");
            } else {
                status = tr("当前串口: %1").arg(portName);
            }
        }

        auto* networkPanel = qobject_cast<NetworkPanel*>(ui->tabWidget->widget(index));
        if (networkPanel) {
            status = tr("当前网口: %1").arg(networkPanel->connectionName());
        }
    }
    statusBar()->showMessage(tr("%1    标签: %2").arg(status).arg(ui->tabWidget->count()));
}

void MainWindow::onConnectionStateChanged(bool)
{
    onTabChanged(ui->tabWidget->currentIndex());
}

void MainWindow::applyCurrentConfigToAll()
{
    auto* source = qobject_cast<SerialPanel*>(ui->tabWidget->currentWidget());
    if (!source) {
        statusBar()->showMessage(tr("当前标签不是串口配置页    标签: %1").arg(ui->tabWidget->count()));
        return;
    }

    const QJsonObject configTemplate = source->connectionTemplate();
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        auto* panel = qobject_cast<SerialPanel*>(ui->tabWidget->widget(i));
        if (panel && panel != source) {
            panel->applyConnectionTemplate(configTemplate, false);
        }
    }

    statusBar()->showMessage(tr("已将当前串口配置应用到全部串口标签页    标签: %1").arg(ui->tabWidget->count()));
}

void MainWindow::applyDarkTheme()
{
    ThemeManager::instance().applyDarkTheme();
}

void MainWindow::applyLightTheme()
{
    ThemeManager::instance().applyLightTheme();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("关于"),
        tr("<h3>通信调试工具</h3>"
           "<p>Written by yeyue</p>"
           "<hr>"
           "<p><b>快捷键:</b></p>"
           "<table cellspacing='6'>"
           "<tr><td><b>Ctrl+N</b></td><td>增加串口</td></tr>"
           "<tr><td><b>Ctrl+Shift+N</b></td><td>增加网口</td></tr>"
           "<tr><td><b>Ctrl+W</b></td><td>删除当前标签页</td></tr>"
           "<tr><td><b>Ctrl+Q</b></td><td>退出</td></tr>"
           "</table>"));
}

void MainWindow::updateTabCount()
{
    onTabChanged(ui->tabWidget->currentIndex());
}

bool MainWindow::loadSettings()
{
    QSettings settings(QStringLiteral("yeyue"), QStringLiteral("serial_prot"));
    int panelCount = settings.beginReadArray(QStringLiteral("panels"));

    for (int i = 0; i < panelCount; ++i) {
        settings.setArrayIndex(i);
        const QString type = settings.value(QStringLiteral("type"), QStringLiteral("serial")).toString();

        if (type == QStringLiteral("network")) {
            NetworkConfig config;
            config.load(settings);

            NetworkPanel* panel = createNetworkPanel();
            panel->setNetworkConfig(config);
            continue;
        }

        SerialConfig config;
        config.load(settings);

        SerialPanel* panel = createSerialPortPanel();
        panel->setSerialConfig(config);
    }

    settings.endArray();

    if (panelCount <= 0) {
        return loadLegacySerialSettings();
    }

    const int currentTab = settings.value(QStringLiteral("currentTab"), 0).toInt();
    if (currentTab >= 0 && currentTab < ui->tabWidget->count()) {
        ui->tabWidget->setCurrentIndex(currentTab);
    }
    onTabChanged(ui->tabWidget->currentIndex());

    return true;
}

bool MainWindow::loadLegacySerialSettings()
{
    QSettings settings(QStringLiteral("yeyue"), QStringLiteral("serial_prot"));
    const int panelCount = settings.beginReadArray(QStringLiteral("serialPanels"));

    for (int i = 0; i < panelCount; ++i) {
        settings.setArrayIndex(i);

        SerialConfig config;
        config.load(settings);

        SerialPanel* panel = createSerialPortPanel();
        panel->setSerialConfig(config);
    }

    settings.endArray();

    if (panelCount <= 0) {
        return false;
    }

    const int currentTab = settings.value(QStringLiteral("currentTab"), 0).toInt();
    if (currentTab >= 0 && currentTab < ui->tabWidget->count()) {
        ui->tabWidget->setCurrentIndex(currentTab);
    }
    onTabChanged(ui->tabWidget->currentIndex());

    return true;
}

void MainWindow::saveSettings() const
{
    QSettings settings(QStringLiteral("yeyue"), QStringLiteral("serial_prot"));
    settings.setValue(QStringLiteral("currentTab"), ui->tabWidget->currentIndex());
    settings.beginWriteArray(QStringLiteral("panels"), ui->tabWidget->count());

    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        settings.setArrayIndex(i);
        auto* panel = qobject_cast<SerialPanel*>(ui->tabWidget->widget(i));
        if (panel) {
            settings.setValue(QStringLiteral("type"), QStringLiteral("serial"));
            panel->serialConfig().save(settings);
            continue;
        }

        auto* networkPanel = qobject_cast<NetworkPanel*>(ui->tabWidget->widget(i));
        if (networkPanel) {
            settings.setValue(QStringLiteral("type"), QStringLiteral("network"));
            networkPanel->networkConfig().save(settings);
        }
    }

    settings.endArray();
    settings.sync();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}
