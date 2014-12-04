#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtNetwork>
#include <QMainWindow>
#include <QSettings>

#include "sniffer.h" //sniffer helper
#include "log.h" //log helper
#include "define.h" //all default value and some text

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void OnServerConnect();
    void UpdateProxyState();
    void ReloadConf();

private:
    Ui::MainWindow *ui;
    QSettings *m_settings;
    Sniffer* m_sniffer;
    Log* m_log;

    //setting in file
    QHostAddress m_authServer;
    qint16 m_authPort;

    void InitSettings();
    void ApplySettings();

    void setProxyState(eSnifferState state);
};

#endif // MAINWINDOW_H
