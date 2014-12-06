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
    //settings loader
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
    setProxyState(STOP);
    //==============

    //==============
    //event signal

    //proxy
    connect(m_sniffer, SIGNAL(ProxyConnect()), this, SLOT(OnProxyConnection()));

    //local
    connect(m_sniffer, SIGNAL(LocalConnect()), this, SLOT(OnLocalConnect()));
    connect(m_sniffer, SIGNAL(LocalDisconnect()), this, SLOT(OnLocalDisconnect()));
    connect(m_sniffer, SIGNAL(LocalPacketRecv()), this, SLOT(OnLocalPacketRecv()));

    //remote
    connect(m_sniffer, SIGNAL(RemoteConnect()), this, SLOT(OnRemoteConnect()));
    connect(m_sniffer, SIGNAL(RemoteDisconnect()), this, SLOT(OnRemoteDisconnect()));
    connect(m_sniffer, SIGNAL(RemotePacketRecv()), this, SLOT(OnRemotePacketRecv()));

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

//================
//local ==========

void MainWindow::OnLocalConnect()
{
    m_log->Add(NORMAL, "Local connect");
}

void MainWindow::OnLocalDisconnect()
{
    m_log->Add(NORMAL, "Local disconnect");
}

void MainWindow::OnLocalPacketRecv()
{
    m_log->Add(NORMAL, "Local receive packet");
}

//================
//remote =========

void MainWindow::OnRemoteConnect()
{
    m_log->Add(NORMAL, "Remote connect");
}

void MainWindow::OnRemoteDisconnect()
{
    m_log->Add(NORMAL, "Remote disconnect");
}

void MainWindow::OnRemotePacketRecv()
{
    m_log->Add(NORMAL, "Remote receive packet");
}

//================
//proxy ==========

void MainWindow::OnProxyConnection()
{
    m_log->Add(NORMAL, "Proxy connection");
}



void MainWindow::UpdateProxyState()
{
    setProxyState((m_sniffer->getSnifferState() == START) ? STOP : START);
}

//================
//settings =======

void MainWindow::ReloadConf()
{
    InitSettings();
    ApplySettings();

    m_log->Add(NORMAL, TXT_LOG_RELOAD_SETTINGS);

    setProxyState(STOP);
    m_sniffer = new Sniffer(m_authServer.toString(), m_authPort);
}

//================

//===================================
//FUNCTION ==========================
//===================================

void MainWindow::setProxyState(eSnifferState state)
{
    //if sniffer is stopped we need to start them else start
    if (state == START)
    {
        m_sniffer->Start();

        //==============
        //update ui
        ui->pushButtonProxy->setText(TXT_UI_BUTTON_STOP_PROXY);
        ui->pushButtonCapture->setText(TXT_UI_BUTTON_START_CAPTURE);
        ui->pushButtonCapture->setEnabled(true);
        ui->groupBoxSnifferType->setEnabled(false);
        //==============

        m_log->Add(INFO, TXT_LOG_PROXY_START);
    }
    else
    {
        m_sniffer->Stop();

        //=================
        //update ui
        ui->pushButtonProxy->setText(TXT_UI_BUTTON_START_PROXY);
        ui->pushButtonCapture->setText(TXT_UI_BUTTON_START_CAPTURE);
        ui->pushButtonCapture->setEnabled(false);
        ui->groupBoxSnifferType->setEnabled(true);
        //=================

        m_log->Add(ERROR, TXT_LOG_PROXY_STOP);
    }
}


//===================================
//SETTINGS ==========================
//===================================

void MainWindow::InitSettings()
{
    //create setting instance
    //in application dir
    m_settings = new QSettings(QApplication::applicationDirPath() + "/" + SETTINGS_FILE, QSettings::IniFormat);
}

void MainWindow::ApplySettings()
{
    //load setting
    m_authServer = QHostAddress(m_settings->value("auth/server", SETTINGS_DEFAULT_AUTH_SERVER).value<QString>());
    m_authPort = m_settings->value("auth/port", SETTINGS_DEFAULT_AUTH_PORT).value<qint16>();

    //save setting
    m_settings->setValue("auth/server", m_authServer.toString());
    m_settings->setValue("auth/port", m_authPort);

    //ui update
    ui->labelServer->setText(TXT_UI_LABEL_SERVER + m_authServer.toString() + ":" + QString::number(m_authPort));
}
