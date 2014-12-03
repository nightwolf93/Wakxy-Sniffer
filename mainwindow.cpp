#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "define.h"
#include "log.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //default is auth parsing
    ui->radioButtonAuthSniffing->setChecked(true);


    //===============
    //setting loader
    InitSettings();
    ApplySettings();
    //===============

    //==============
    //log
    m_log = new Log(ui->textBrowserLog);
    //==============

    //==============
    //sniffer
    m_sniffer = new Sniffer(m_authServer.toString(), m_authPort);
    setProxyState(eSnifferState::STOP);
    //==============

    //==============
    //event signal
    connect(m_sniffer->getServer(), SIGNAL(connected()), this, SLOT(OnServerConnect()));
    connect(ui->pushButtonProxy, SIGNAL(clicked()), this, SLOT(UpdateProxyState()));
    connect(ui->pushButtonReloadConf, SIGNAL(clicked()), this, SLOT(ReloadConf()));
    //==============
}

//delete the ui
//don't forget to delete your object
MainWindow::~MainWindow()
{
    delete ui;
}

//===================================
//EVENT =============================
//===================================

void MainWindow::OnServerConnect()
{

}

void MainWindow::UpdateProxyState()
{
    setProxyState((m_sniffer->getSnifferState() == eSnifferState::START) ? eSnifferState::STOP : eSnifferState::START);
}

void MainWindow::ReloadConf()
{
    InitSettings();
    ApplySettings();

    m_log->Add(LogLevel::NORMAL, "Rechargement des fichiers de configuration ! Le proxy et la capture va être arrêté");

    setProxyState(eSnifferState::STOP);
    m_sniffer = new Sniffer(m_authServer.toString(), m_authPort);
}

//===================================
//FUNCTION ==========================
//===================================

void MainWindow::setProxyState(eSnifferState state)
{
    //if sniffer is stopped we need to start them else start
    if (state == eSnifferState::START)
    {
        m_sniffer->Start();

        //==============
        //update ui
        ui->pushButtonProxy->setText(TXT_UI_BUTTON_STOP_PROXY);
        ui->pushButtonCapture->setText(TXT_UI_BUTTON_START_CAPTURE);
        ui->pushButtonCapture->setEnabled(true);
        //==============

        m_log->Add(LogLevel::INFO, "Proxy démarré");
    }
    else
    {
        m_sniffer->Stop();

        //=================
        //update ui
        ui->pushButtonProxy->setText(TXT_UI_BUTTON_START_PROXY);
        ui->pushButtonCapture->setText(TXT_UI_BUTTON_START_CAPTURE);
        ui->pushButtonCapture->setEnabled(false);
        //=================

        m_log->Add(LogLevel::ERROR, "Proxy arrêté");
    }
}


//===================================
//SETTING ===========================
//===================================

void MainWindow::InitSettings()
{
    //create setting instance
    //in application dir
    m_setting = new QSettings(QApplication::applicationDirPath() + "/" + SETTINGS_FILE, QSettings::IniFormat);
}

void MainWindow::ApplySettings()
{
    //load setting
    m_authServer = QHostAddress(m_setting->value("auth/server", SETTINGS_DEFAULT_AUTH_SERVER).value<QString>());
    m_authPort = m_setting->value("auth/port", SETTINGS_DEFAULT_AUTH_PORT).value<qint16>();

    //save setting
    m_setting->setValue("auth/server", m_authServer.toString());
    m_setting->setValue("auth/port", m_authPort);

    //ui update
    ui->labelServer->setText(TXT_UI_LABEL_SERVER + m_authServer.toString() + ":" + QString::number(m_authPort));
}
