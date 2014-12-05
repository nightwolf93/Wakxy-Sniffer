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
    setProxyState(eSnifferState::STOP);
    //==============

    //==============
    //event signal
    connect(m_sniffer->getProxy(), SIGNAL(connected()), this, SLOT(OnServerConnect()));
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
    m_log->Add(LogLevel::NORMAL,
               QString("Le proxy a reussi a creer un pont avec le socket distant %1:%2")
               .arg(m_sniffer->getRemoteSocket()->localAddress().toString(),
                    QString::number(m_sniffer->getRemoteSocket()->localPort()))
               );
}

void MainWindow::UpdateProxyState()
{
    setProxyState((m_sniffer->getSnifferState() == eSnifferState::START) ? eSnifferState::STOP : eSnifferState::START);
}

void MainWindow::ReloadConf()
{
    InitSettings();
    ApplySettings();

    m_log->Add(LogLevel::NORMAL, TXT_LOG_RELOAD_SETTINGS);

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
        ui->groupBoxSnifferType->setEnabled(false);
        //==============

        m_log->Add(LogLevel::INFO, TXT_LOG_PROXY_START);
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

        m_log->Add(LogLevel::ERROR, TXT_LOG_PROXY_STOP);
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
