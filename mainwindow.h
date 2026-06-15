#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QActionGroup;
class SerialPanel;
class NetworkPanel;
class AutomationRuleEngine;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addSerialPort();
    void addNetworkPort();
    void closeSerialPort(int index);
    void onTabChanged(int index);
    void onConnectionStateChanged(bool connected);
    void applyCurrentConfigToAll();

    void applyDarkTheme();
    void applyLightTheme();
    void about();
    void openAutoReplyDialog();

private:
    void setupUiFromForm();
    void setupConnections();
    void updateTabCount();
    SerialPanel* createSerialPortPanel();
    NetworkPanel* createNetworkPanel();
    bool loadSettings();
    bool loadLegacySerialSettings();
    void saveSettings() const;
    AutomationRuleEngine* currentPanelAutoReplyEngine() const;

    Ui::MainWindow* ui = nullptr;
    QActionGroup* m_themeGroup = nullptr;

    int m_tabCounter = 0;
};
#endif // MAINWINDOW_H
