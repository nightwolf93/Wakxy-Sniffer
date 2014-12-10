#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "define.h"
#include "log.h"
#include "packeteditor.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    setProxyState(Sniffer::STOP);
    //==============

    //==============
    //event signal

    //proxy
    connect(m_sniffer, SIGNAL(ProxyConnect()), this, SLOT(OnProxyConnection()));

    //local
    connect(m_sniffer, SIGNAL(LocalConnect()), this, SLOT(OnLocalConnect()));
    connect(m_sniffer, SIGNAL(LocalDisconnect()), this, SLOT(OnLocalDisconnect()));
    connect(m_sniffer, SIGNAL(LocalPacketRecv()), this, SLOT(OnLocalPacketRecv()));
    connect(m_sniffer, SIGNAL(LocalError(QAbstractSocket::SocketError)), this, SLOT(OnLocalSocketError(QAbstractSocket::SocketError)));
    connect(m_sniffer, SIGNAL(LocalPacketSend(Packet)), this, SLOT(OnLocalPacketSend(Packet)));

    //remote
    connect(m_sniffer, SIGNAL(RemoteConnect()), this, SLOT(OnRemoteConnect()));
    connect(m_sniffer, SIGNAL(RemoteDisconnect()), this, SLOT(OnRemoteDisconnect()));
    connect(m_sniffer, SIGNAL(RemotePacketRecv()), this, SLOT(OnRemotePacketRecv()));
    connect(m_sniffer, SIGNAL(RemoteError(QAbstractSocket::SocketError)), this, SLOT(OnRemoteSocketError(QAbstractSocket::SocketError)));
    connect(m_sniffer, SIGNAL(RemotePacketSend(Packet)), this, SLOT(OnRemotePacketSend(Packet)));

    connect(ui->pushButtonProxy, SIGNAL(clicked()), this, SLOT(UpdateProxyState()));
    connect(ui->pushButtonCapture, SIGNAL(clicked()), this, SLOT(UpdateCaptureState()));
    connect(ui->pushButtonReloadConf, SIGNAL(clicked()), this, SLOT(ReloadConf()));

    connect(ui->pushButtonClearLog, SIGNAL(clicked()), this, SLOT(ClearLog()));
    connect(ui->pushButtonClearTable, SIGNAL(clicked()), this, SLOT(ClearTable()));
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
//ui =============

void MainWindow::ClearLog()
{
    m_log->Clear();
}

void MainWindow::ClearTable()
{
    ui->treeWidgetPacket->clear();
    m_sniffer->resetCountPackets();
}

//================
//local ==========

void MainWindow::OnLocalConnect()
{
    m_log->Add(Log::NORMAL, TXT_LOG_LOCAL_CONNECT);
}

void MainWindow::OnLocalDisconnect()
{
    m_log->Add(Log::ERROR, TXT_LOG_LOCAL_DISCONNECT);
}

void MainWindow::OnLocalPacketRecv()
{
}

void MainWindow::OnLocalSocketError(QAbstractSocket::SocketError /*socketError*/)
{
    m_log->Add(Log::NORMAL, m_sniffer->getLocalSocket()->errorString());
}

void MainWindow::OnLocalPacketSend(Packet packet)
{
    PacketEditor* packetEditor = new PacketEditor(packet.raw, PacketEditor::PACKET_CLIENT);
    AddPacketToTable(packetEditor);

    m_log->Add(Log::INFO, TXT_LOG_LOCAL_PACKET_SEND);
}

//================
//remote =========

void MainWindow::OnRemoteConnect()
{
    m_log->Add(Log::NORMAL, TXT_LOG_REMOTE_CONNECT);
}

void MainWindow::OnRemoteDisconnect()
{
    m_log->Add(Log::ERROR, TXT_LOG_REMOTE_DISCONNECT);
}

void MainWindow::OnRemotePacketRecv()
{
}

void MainWindow::OnRemoteSocketError(QAbstractSocket::SocketError /*socketError*/)
{
    m_log->Add(Log::NORMAL, m_sniffer->getRemoteSocket()->errorString());
}

void MainWindow::OnRemotePacketSend(Packet packet)
{
     PacketEditor* packetEditor = new PacketEditor(packet.raw, PacketEditor::PACKET_SERVER);
     AddPacketToTable(packetEditor);

     m_log->Add(Log::INFO, TXT_LOG_REMOTE_PACKET_SEND);
}

//===================
//proxy & capture ===

void MainWindow::OnProxyConnection()
{
    m_log->Add(Log::NORMAL, TXT_LOG_PROXY_CONNEXION);
}

void MainWindow::UpdateProxyState()
{
    setProxyState((m_sniffer->getProxyState() == Sniffer::START) ? Sniffer::STOP : Sniffer::START);
}

//capture state
//enable/disable event *PacketSend
void MainWindow::UpadteCaptureState()
{
   m_sniffer->setCaptureState(
               (m_sniffer->getCaptureState() == Sniffer::START) ? Sniffer::START : Sniffer::STOP
           );
}

//================
//settings =======

void MainWindow::ReloadConf()
{
    InitSettings();
    ApplySettings();

    m_log->Add(Log::NORMAL, TXT_LOG_RELOAD_SETTINGS);

    setProxyState(Sniffer::STOP);
    m_sniffer = new Sniffer(m_authServer.toString(), m_authPort);
}

//================

//===================================
//METHODS ===========================
//===================================

void MainWindow::setProxyState(Sniffer::SnifferState state)
{
    //if sniffer is stopped we need to start them else start
    if (state == Sniffer::START)
    {
        m_sniffer->Start();

        //==============
        //update ui
        ui->pushButtonProxy->setText(TXT_UI_BUTTON_STOP_PROXY);
        ui->pushButtonCapture->setText(TXT_UI_BUTTON_START_CAPTURE);
        ui->pushButtonCapture->setEnabled(true);
        //==============

        m_log->Add(Log::INFO, TXT_LOG_PROXY_START);
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

        m_log->Add(Log::ERROR, TXT_LOG_PROXY_STOP);
    }
}

void MainWindow::AddPacketToTable(PacketEditor *packetEditor)
{
    QTreeWidgetItem* item = new QTreeWidgetItem;

    //count packet
    item->setText(MainWindow::NUMBER, QString::number(m_sniffer->getCountPackets()));

    //packet type
    if (packetEditor->getPacketType() == PacketEditor::PACKET_SERVER)
        item->setText(MainWindow::TYPE, TXT_UI_TABLE_PACKET_SERVER);
    else
        item->setText(MainWindow::TYPE, TXT_UI_TABLE_PACKET_CLIENT);

    //size
    item->setText(MainWindow::SIZE, QString::number(packetEditor->getSize()));

    //opcode
    item->setText(MainWindow::OPCODE, QString::number(packetEditor->getOpcode()));

    //ASCII
    item->setText(MainWindow::ASCII, Utils::ToASCII(packetEditor->getPacket()));

    //HEX
    item->setText(MainWindow::HEX, Utils::ToHexString(packetEditor->getPacket()));

    //add item
    ui->treeWidgetPacket->addTopLevelItem(item);
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

