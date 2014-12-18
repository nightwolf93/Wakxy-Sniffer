#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "define.h"
#include "log.h"
#include "packeteditor.h"
#include "utils.h"
#include "packetzoomdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_sniffer_packet_count = 0;

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
    //spoofing
    LoadSpoofingPacket();
    //=============

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
    connect(m_sniffer, SIGNAL(LocalPacketHook(Packet*)), this, SLOT(OnLocalPacketHook(Packet*)));

    //remote
    connect(m_sniffer, SIGNAL(RemoteConnect()), this, SLOT(OnRemoteConnect()));
    connect(m_sniffer, SIGNAL(RemoteDisconnect()), this, SLOT(OnRemoteDisconnect()));
    connect(m_sniffer, SIGNAL(RemotePacketRecv()), this, SLOT(OnRemotePacketRecv()));
    connect(m_sniffer, SIGNAL(RemoteError(QAbstractSocket::SocketError)), this, SLOT(OnRemoteSocketError(QAbstractSocket::SocketError)));
    connect(m_sniffer, SIGNAL(RemotePacketSend(Packet)), this, SLOT(OnRemotePacketSend(Packet)));
    connect(m_sniffer, SIGNAL(RemotePacketHook(Packet*)), this, SLOT(OnRemotePacketHook(Packet*)));

    connect(ui->pushButtonProxy, SIGNAL(clicked()), this, SLOT(UpdateProxyState()));
    connect(ui->pushButtonCapture, SIGNAL(clicked()), this, SLOT(UpdateCaptureState()));
    connect(ui->pushButtonReloadConf, SIGNAL(clicked()), this, SLOT(ReloadConf()));

    connect(ui->pushButtonClearLog, SIGNAL(clicked()), this, SLOT(ClearLog()));
    connect(ui->pushButtonClearTable, SIGNAL(clicked()), this, SLOT(ClearTable()));

    connect(ui->treeWidgetPacket, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(ShowPacketZoom(QTreeWidgetItem*)));
    connect(ui->actionOuvrir, SIGNAL(triggered()), this, SLOT(CaptureOpen()));
    connect(ui->actionSauvegarder, SIGNAL(triggered()), this, SLOT(CaptureSave()));

    connect(ui->treeWidgetPacketSpoofing, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(TableSpoofingColumnChanged(QTreeWidgetItem*,int)));
    connect(ui->pushButtonCreateSpoofing, SIGNAL(clicked()), this, SLOT(CreateSpoofing()));
    connect(ui->pushButtonReloadSpoofing, SIGNAL(clicked()), this, SLOT(ReloadSpoofing()));
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
    m_tableItemPackets.clear();
    m_sniffer_packet_count = 0;
}

void MainWindow::ShowPacketZoom(QTreeWidgetItem *item)
{
    MwTablePackets::iterator itr = m_tableItemPackets.find(item);
    if(itr != m_tableItemPackets.end())
    {
        PacketZoomDialog* dialog = new PacketZoomDialog(itr.value(), this);
        dialog->show();
    }
}

void MainWindow::CaptureOpen()
{
    LoadCapture();
}

void MainWindow::CaptureSave()
{
    SaveCapture();
}

void MainWindow::ReloadSpoofing()
{
    LoadSpoofingPacket();
}

void MainWindow::CreateSpoofing()
{
    if(ui->treeWidgetPacket->selectedItems().count() == 0)
        return;

    QList<QTreeWidgetItem*> list = ui->treeWidgetPacket->selectedItems();
    for(QList<QTreeWidgetItem*>::iterator itr = list.begin(); itr != list.end(); ++itr)
    {
        MwTablePackets::iterator item = m_tableItemPackets.find(*itr);
        if(item != m_tableItemPackets.end())
        {
            PacketEditor* packetEditor = item.value();

            QString defaultFileName = "packets_"+PacketEditor::getPacketTypeString(packetEditor->getPacketType());
            defaultFileName += "_"+QString::number(packetEditor->getOpcode())+".json";

            QString filename = QFileDialog::getSaveFileName(this, TXT_UI_ACTION_SAVE_AS, m_spoofingDir+"/"+defaultFileName, TXT_UI_ACTION_FILETYPE_2);

            //file is null
            if(filename.isNull())
                continue;

            QFile saveFile(filename); //create file
            if (!saveFile.open(QIODevice::WriteOnly)) //try open it in readOnly
            {
                qWarning(TXT_UI_ACTION_SAVE_WARNING_WRITE_FAIL);
                continue;
            }

            //=====================================
            //create a json object of packets
            QJsonObject obj;

            obj["type"] = (int)packetEditor->getPacketType();
            obj["opcode"] = (int)packetEditor->getOpcode();

            QString hexString = Utils::ToHexString(packetEditor->getPacket());
            QJsonArray bytesArrayJson;

            QStringList list = hexString.split(" ");
            foreach(QString str, list)
                bytesArrayJson.append(str);

            obj["raw"] = bytesArrayJson;

            //====================================

            //save the file
            QJsonDocument doc(obj);
            saveFile.write(doc.toJson());
            saveFile.close();
        }
    }

    LoadSpoofingPacket();
}

void MainWindow::TableSpoofingColumnChanged(QTreeWidgetItem *item, int column)
{
    if(column == (int)MainWindow::SPOOFINGTABLE_ENABLED)
    {
        MwSpoofPacket::iterator itr = m_spoofPackets.find(item);
        if( itr != m_spoofPackets.end())
        {
            itr.value().enabled = item->data(column, Qt::CheckStateRole).toBool();
            QString typeStr = PacketEditor::getPacketTypeString((PacketEditor::PacketType)itr.value().type);

            if(itr.value().enabled)
                m_log->Add(Log::LOGLEVEL_INFO, QString(TXT_LOG_SPOOFING_ENABLE).arg(typeStr, QString::number(itr.value().opcode)));
            else
                m_log->Add(Log::LOGLEVEL_ERROR, QString(TXT_LOG_SPOOFING_DISABLED).arg(typeStr, QString::number(itr.value().opcode)));
        }
    }
}

//================
//local ==========

void MainWindow::OnLocalConnect()
{
    m_log->Add(Log::LOGLEVEL_NORMAL, TXT_LOG_LOCAL_CONNECT);
}

void MainWindow::OnLocalDisconnect()
{
    m_log->Add(Log::LOGLEVEL_ERROR, TXT_LOG_LOCAL_DISCONNECT);
}

void MainWindow::OnLocalPacketRecv()
{
}

void MainWindow::OnLocalSocketError(QAbstractSocket::SocketError /*socketError*/)
{
    m_log->Add(Log::LOGLEVEL_NORMAL, m_sniffer->getLocalSocket()->errorString());
}

void MainWindow::OnLocalPacketSend(Packet packet)
{
    PacketEditor* packetEditor = new PacketEditor(packet.raw, PacketEditor::PACKET_CLIENT, m_scriptDir);
    AddPacketToTable(packetEditor);

    m_log->Add(Log::LOGLEVEL_INFO, TXT_LOG_LOCAL_PACKET_SEND);
}

void MainWindow::OnLocalPacketHook(Packet *packet)
{
    PacketEditor* packetEditor = new PacketEditor(packet->raw, PacketEditor::PACKET_CLIENT, m_scriptDir);

    if(SpoofingPacket(packet, packetEditor))
        m_log->Add(Log::LOGLEVEL_INFO, QString(TXT_LOG_REMOTE_PACKET_SPOOFING).arg(packetEditor->getOpcode()));
}

//================
//remote =========

void MainWindow::OnRemoteConnect()
{
    m_log->Add(Log::LOGLEVEL_NORMAL, TXT_LOG_REMOTE_CONNECT);
}

void MainWindow::OnRemoteDisconnect()
{
    m_log->Add(Log::LOGLEVEL_ERROR, TXT_LOG_REMOTE_DISCONNECT);
}

void MainWindow::OnRemotePacketRecv()
{
}

void MainWindow::OnRemoteSocketError(QAbstractSocket::SocketError /*socketError*/)
{
    m_log->Add(Log::LOGLEVEL_NORMAL, m_sniffer->getRemoteSocket()->errorString());
}

void MainWindow::OnRemotePacketSend(Packet packet)
{
     PacketEditor* packetEditor = new PacketEditor(packet.raw, PacketEditor::PACKET_SERVER, m_scriptDir);
     AddPacketToTable(packetEditor);

     m_log->Add(Log::LOGLEVEL_INFO, TXT_LOG_REMOTE_PACKET_SEND);
}

void MainWindow::OnRemotePacketHook(Packet *packet)
{
    PacketEditor* packetEditor = new PacketEditor(packet->raw, PacketEditor::PACKET_SERVER, m_scriptDir);

    if(SpoofingPacket(packet, packetEditor))
        m_log->Add(Log::LOGLEVEL_INFO, QString(TXT_LOG_REMOTE_PACKET_SPOOFING).arg(packetEditor->getOpcode()));
}

//===================
//proxy & capture ===

void MainWindow::OnProxyConnection()
{
    m_log->Add(Log::LOGLEVEL_NORMAL, TXT_LOG_PROXY_CONNEXION);
}

void MainWindow::UpdateProxyState()
{
    setProxyState((m_sniffer->getProxyState() == Sniffer::START) ? Sniffer::STOP : Sniffer::START);
}

void MainWindow::UpdateCaptureState()
{
   if(m_sniffer->getCaptureState() == Sniffer::STOP)
   {
        setCaptureState(Sniffer::START);
   }
   else
   {
       setCaptureState(Sniffer::STOP);
   }
}

//================
//settings =======

void MainWindow::ReloadConf()
{
    InitSettings();
    ApplySettings();

    m_log->Add(Log::LOGLEVEL_NORMAL, TXT_LOG_RELOAD_SETTINGS);

    setProxyState(Sniffer::STOP);
    m_sniffer = new Sniffer(m_authServer.toString(), m_authPort);
}

//================

//===================================
//METHODS ===========================
//===================================

bool MainWindow::SpoofingPacket(Packet *packet, PacketEditor *packetEditor)
{
    for(MwSpoofPacket::iterator itr = m_spoofPackets.begin(); itr != m_spoofPackets.end(); ++itr)
    {
        if(itr->type == packetEditor->getPacketType()
                && itr->opcode == packetEditor->getOpcode()
                && itr->enabled == true)
        {
            packet->raw = itr.value().raw;
            return true;
        }
    }

    return false;
}

void MainWindow::LoadSpoofingPacket()
{
    //Clear table
    ui->treeWidgetPacketSpoofing->clear();
    m_spoofPackets.clear();

    QDir().mkdir(m_spoofingDir); //check if folder exist, else create it
    QDir dir(m_spoofingDir);
    QDirIterator it(dir);


    while(it.hasNext())
    {
        it.next();
        QFileInfo fileInfo = it.fileInfo();

        //file & json file
        if(fileInfo.isFile() && fileInfo.completeSuffix() == "json")
        {
            //read the file
            QFile loadFile(fileInfo.filePath());

            if (!loadFile.open(QIODevice::ReadOnly)) //try open readOnly
                return;

            //load
            QByteArray data = loadFile.readAll();
            QJsonDocument loadDoc(QJsonDocument::fromJson(data));
            QJsonObject docJson = loadDoc.object();

            int type = docJson["type"].toInt();
            int opcode = docJson["opcode"].toInt();
            QJsonArray rawArray = docJson["raw"].toArray();
            QString hexString = "";

            for(int rawIndex = 0; rawIndex < rawArray.size(); ++rawIndex)
                hexString += rawArray[rawIndex].toString();

           //======================
           //create the struct ====
           QTreeWidgetItem* item = new QTreeWidgetItem();
           SpoofPacket spoofPacket;
           spoofPacket.enabled = false;
           spoofPacket.type = type;
           spoofPacket.opcode = opcode;
           spoofPacket.raw = Utils::FromHexString(hexString);
           //=======================

           QString strType = PacketEditor::getPacketTypeString((PacketEditor::PacketType)spoofPacket.type);

           //=======================
           //create tree item ====== 
           item->setText((int)MainWindow::SPOOFINGTABLE_TYPE, strType);
           item->setText((int)MainWindow::SPOOFINGTABLE_OPCODE, QString::number(opcode));  //opcode
           item->setData((int)MainWindow::SPOOFINGTABLE_ENABLED, Qt::CheckStateRole, spoofPacket.enabled); //enable
           //=======================

           m_spoofPackets.insert(item, spoofPacket);
           ui->treeWidgetPacketSpoofing->addTopLevelItem(item);

           m_log->Add(Log::LOGLEVEL_INFO, QString(TXT_LOG_SPOOFING_LOADED).arg(strType, QString::number(opcode)));
           loadFile.close();
        }
    }
}

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

        m_log->Add(Log::LOGLEVEL_INFO, TXT_LOG_PROXY_START);
        setCaptureState(Sniffer::START);
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

        m_log->Add(Log::LOGLEVEL_ERROR, TXT_LOG_PROXY_STOP);
        setCaptureState(Sniffer::STOP);
    }
}

void MainWindow::setCaptureState(Sniffer::SnifferState state)
{
    if(state == Sniffer::START)
    {
         ui->pushButtonCapture->setText(TXT_UI_BUTTON_STOP_CAPTURE);
         m_sniffer->setCaptureState(Sniffer::START);
         m_log->Add(Log::LOGLEVEL_INFO, TXT_LOG_CAPTURE_START);
    }
    else
    {
         ui->pushButtonCapture->setText(TXT_UI_BUTTON_START_CAPTURE);
         m_sniffer->setCaptureState(Sniffer::STOP);
         m_log->Add(Log::LOGLEVEL_ERROR, TXT_LOG_CAPTURE_STOP);
    }
}

void MainWindow::AddPacketToTable(PacketEditor *packetEditor)
{
    m_sniffer_packet_count++;
    QTreeWidgetItem* item = new QTreeWidgetItem;

    item->setText(MainWindow::PACKETTABLE_NUMBER, QString::number(m_sniffer_packet_count)); //count packet

    //packet type
    item->setText(MainWindow::PACKETTABLE_TYPE, PacketEditor::getPacketTypeString(packetEditor->getPacketType()));

    item->setText(MainWindow::PACKETTABLE_SIZE, QString::number(packetEditor->getSize()));     //size
    item->setText(MainWindow::PACKETTABLE_OPCODE, QString::number(packetEditor->getOpcode()));  //opcode
    item->setText(MainWindow::PACKETTABLE_ASCII, Utils::ToASCII(packetEditor->getPacket()));   //ASCII
    item->setText(MainWindow::PACKETTABLE_HEX, Utils::ToHexString(packetEditor->getPacket())); //HEX

    //add item
    ui->treeWidgetPacket->addTopLevelItem(item);
    m_tableItemPackets.insert(item, packetEditor);
}

void MainWindow::LoadCapture()
{
    QString filename = QFileDialog::getOpenFileName(this, TXT_UI_ACTION_OPEN, "", TXT_UI_ACTION_FILETYPE);

    if(filename.isNull())
        return;

    QFile loadFile(filename);
    if (!loadFile.open(QIODevice::ReadOnly)) //try open it readOnly
    {
        qWarning(TXT_UI_ACTION_SAVE_WARNING_READ_FAIL);
        return;
    }

    //load
    QByteArray data = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(data));
    QJsonObject docJson = loadDoc.object();
    QJsonArray packetsArray = docJson["packets"].toArray();

    for(int packetIndex = 0; packetIndex < packetsArray.size(); ++packetIndex)
    {
        QJsonObject packet = packetsArray[packetIndex].toObject();

        QString bytesString = packet["byteArray"].toString();
        int packetType = packet["type"].toInt();

        PacketEditor* packetEditor = new PacketEditor(Utils::FromHexString(bytesString), (PacketEditor::PacketType)packetType, m_scriptDir);
        AddPacketToTable(packetEditor);
    }

    loadFile.close();
    m_log->Add(Log::LOGLEVEL_INFO, TXT_LOG_FILE_LOAD);
}

void MainWindow::SaveCapture()
{
    QDateTime datetime = QDateTime::currentDateTime(); //current date time
    QString defaultFileName = "packets_"+datetime.toString("yyyyMMddhhmmss")+".wxy"; //default file name

    //save dialog
    QString filename = QFileDialog::getSaveFileName(this, TXT_UI_ACTION_SAVE_AS, defaultFileName, TXT_UI_ACTION_FILETYPE);

    //file is null capture is wrong
    if(filename.isNull())
        return;

    QFile saveFile(filename); //create file
    if (!saveFile.open(QIODevice::WriteOnly)) //try open it in readOnly
    {
        qWarning(TXT_UI_ACTION_SAVE_WARNING_WRITE_FAIL);
        return;
    }

    //=====================================
    //create a json object of packets
    QJsonArray packetsArray;

    int level = -1;
    for(MwTablePackets::iterator itr = m_tableItemPackets.begin(); itr != m_tableItemPackets.end(); ++itr)
    {
        QJsonObject packet;

        packet["byteArray"] = Utils::ToHexString(itr.value()->getPacket());
        packet["type"] = itr.value()->getPacketType();

        if(itr.key()->text(MainWindow::PACKETTABLE_NUMBER).toInt() < level)
            packetsArray.push_front(packet);
        else
            packetsArray.push_back(packet);

        level = itr.key()->text(MainWindow::PACKETTABLE_NUMBER).toInt();
    }

    QJsonObject obj;
    obj["packets"] = packetsArray;
    //====================================

    //save the file
    QJsonDocument doc(obj);
    saveFile.write(doc.toJson());
    saveFile.close();
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
    m_scriptDir = m_settings->value("packetEditor/scriptDir", SETTING_PACKETEDITOR_SCRIPT_FOLDER).value<QString>();
    m_spoofingDir = m_settings->value("packet/spoofingDir", SETTING_SPOOFING_DEFAULT_FOLDER).value<QString>();

    //save setting
    m_settings->setValue("auth/server", m_authServer.toString());
    m_settings->setValue("auth/port", m_authPort);
    m_settings->setValue("packetEdtitor/scriptDir", m_scriptDir);
    m_settings->setValue("packet/spoofingDir", m_spoofingDir);

    //ui update
    ui->labelServer->setText(TXT_UI_LABEL_SERVER + m_authServer.toString() + ":" + QString::number(m_authPort));
}

