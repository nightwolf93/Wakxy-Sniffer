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
    m_packetCount = 0;

    //===============
    //settings loader
    InitSettings();
    ApplySettings();

    //==============
    //log
    m_log = new Log(ui->textBrowserLog);

    //==============
    //sniffer
    foreach(SnifferParams params, m_snifferParamsList)
        CreateSniffer(params);

    //==============
    //spoofing
    LoadSpoofingPacket();

    //==============
    //ui event signal
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
    m_packets.clear();
    m_packetCount = 0;
}

void MainWindow::ShowPacketZoom(QTreeWidgetItem *item)
{
    MwTablePackets::iterator itr = m_packets.find(item);
    if(itr != m_packets.end())
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
        MwTablePackets::iterator item = m_packets.find(*itr);
        if(item != m_packets.end())
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
    if(QTreeWidgetItem* item = ui->treeWidgetSniffer->currentItem())
    {
        MwTableSniffer::iterator itr = m_snifferList.find(item);
        if(itr != m_snifferList.end())
        {
           Sniffer* sniffer = itr.value();
           setProxyState(itr, (sniffer->getProxyState() == Sniffer::START) ? Sniffer::STOP : Sniffer::START);
        }
    }
}

void MainWindow::UpdateCaptureState()
{
    if(QTreeWidgetItem* item = ui->treeWidgetSniffer->currentItem())
    {
        MwTableSniffer::iterator itr = m_snifferList.find(item);
        if(itr != m_snifferList.end())
        {
           Sniffer* sniffer = itr.value();
           setCaptureState(itr, (sniffer->getCaptureState() == Sniffer::START) ? Sniffer::STOP : Sniffer::START);
        }
    }
}

//================
//settings =======

void MainWindow::ReloadConf()
{
    for(MwTableSniffer::iterator itr = m_snifferList.begin(); itr != m_snifferList.end(); ++itr)
    {
        setProxyState(itr, Sniffer::STOP);
    }

    InitSettings();
    ApplySettings();
    m_log->Add(Log::LOGLEVEL_NORMAL, TXT_LOG_RELOAD_SETTINGS);
}

//================

//===================================
//METHODS ===========================
//===================================

bool MainWindow::SpoofingPacket(Packet *packet, PacketEditor *packetEditor)
{
    for(MwSpoofPacket::iterator itr = m_spoofPackets.begin(); itr != m_spoofPackets.end(); ++itr)
    {
        if(itr->type == packetEditor->getPacketType() && itr->opcode == packetEditor->getOpcode() && itr->enabled == true)
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
           QTreeWidgetItem* item = new QTreeWidgetItem;
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

void MainWindow::CreateSniffer(SnifferParams params)
{
    //=================
    //Tree ============
    QTreeWidgetItem* treeItem = new QTreeWidgetItem;

    treeItem->setText(SNIFFERTABLE_ADRESS, params.adress.toString());
    treeItem->setText(SNIFFERTABLE_PORT, QString::number(params.port));
    treeItem->setText(SNIFFERTABLE_STATUT, TXT_UI_STOP_PROXY);
    treeItem->setTextColor(SNIFFERTABLE_STATUT, QColor("red"));
    treeItem->setText(SNIFFERTABLE_CAPTURE, TXT_UI_STOP_CAPTURE);
    treeItem->setTextColor(SNIFFERTABLE_CAPTURE, QColor("red"));

    ui->treeWidgetSniffer->addTopLevelItem(treeItem);
    ui->treeWidgetSniffer->resizeColumnToContents(0);

    //==================
    //Sniffer ==========

    Sniffer* sniffer = new Sniffer(params.adress.toString(), params.port);
    //proxy
    connect(sniffer, SIGNAL(ProxyConnect()), this, SLOT(OnProxyConnection()));

    //local
    connect(sniffer, SIGNAL(LocalConnect()), this, SLOT(OnLocalConnect()));
    connect(sniffer, SIGNAL(LocalDisconnect()), this, SLOT(OnLocalDisconnect()));
    connect(sniffer, SIGNAL(LocalPacketRecv()), this, SLOT(OnLocalPacketRecv()));
    connect(sniffer, SIGNAL(LocalError(QAbstractSocket::SocketError)), this, SLOT(OnLocalSocketError(QAbstractSocket::SocketError)));
    connect(sniffer, SIGNAL(LocalPacketSend(Packet)), this, SLOT(OnLocalPacketSend(Packet)));
    connect(sniffer, SIGNAL(LocalPacketHook(Packet*)), this, SLOT(OnLocalPacketHook(Packet*)));

    //remote
    connect(sniffer, SIGNAL(RemoteConnect()), this, SLOT(OnRemoteConnect()));
    connect(sniffer, SIGNAL(RemoteDisconnect()), this, SLOT(OnRemoteDisconnect()));
    connect(sniffer, SIGNAL(RemotePacketRecv()), this, SLOT(OnRemotePacketRecv()));
    connect(sniffer, SIGNAL(RemoteError(QAbstractSocket::SocketError)), this, SLOT(OnRemoteSocketError(QAbstractSocket::SocketError)));
    connect(sniffer, SIGNAL(RemotePacketSend(Packet)), this, SLOT(OnRemotePacketSend(Packet)));
    connect(sniffer, SIGNAL(RemotePacketHook(Packet*)), this, SLOT(OnRemotePacketHook(Packet*)));

    m_snifferList.insert(treeItem, sniffer);
}

void MainWindow::setProxyState(MwTableSniffer::iterator itr, Sniffer::SnifferState state)
{
    Sniffer* sniffer = itr.value();
    QTreeWidgetItem* treeItem = itr.key();

    //if sniffer is stopped we need to start them else start
    if (state == Sniffer::START)
    {
        sniffer->Start();
        treeItem->setText(SNIFFERTABLE_STATUT, TXT_UI_START_PROXY);
        treeItem->setTextColor(SNIFFERTABLE_STATUT, QColor("green"));

        m_log->Add(Log::LOGLEVEL_INFO, QString(TXT_LOG_PROXY_START)
                   .arg(sniffer->getAddress().toString(),
                        QString::number(sniffer->getPort()))
                        );

        setCaptureState(itr, Sniffer::START);
    }
    else
    {
        sniffer->Stop();
        treeItem->setText(SNIFFERTABLE_STATUT, TXT_UI_STOP_PROXY);
        treeItem->setTextColor(SNIFFERTABLE_STATUT, QColor("red"));
        m_log->Add(Log::LOGLEVEL_ERROR, QString(TXT_LOG_PROXY_STOP)
                   .arg(sniffer->getAddress().toString(),
                        QString::number(sniffer->getPort()))
                        );

        setCaptureState(itr, Sniffer::STOP);
    }
}

void MainWindow::setCaptureState(MwTableSniffer::iterator itr, Sniffer::SnifferState state)
{
    Sniffer* sniffer = itr.value();
    QTreeWidgetItem* treeItem = itr.key();

    if(state == Sniffer::START)
    {
         sniffer->setCaptureState(Sniffer::START);
         treeItem->setText(SNIFFERTABLE_CAPTURE, TXT_UI_START_CAPTURE);
         treeItem->setTextColor(SNIFFERTABLE_CAPTURE, QColor("green"));

         m_log->Add(Log::LOGLEVEL_INFO, QString(TXT_LOG_CAPTURE_START)
                    .arg(sniffer->getAddress().toString(),
                         QString::number(sniffer->getPort()))
                         );
    }
    else
    {
         sniffer->setCaptureState(Sniffer::STOP);
         treeItem->setText(SNIFFERTABLE_CAPTURE, TXT_UI_STOP_CAPTURE);
         treeItem->setTextColor(SNIFFERTABLE_CAPTURE, QColor("red"));

         m_log->Add(Log::LOGLEVEL_ERROR, QString(TXT_LOG_CAPTURE_STOP)
                    .arg(sniffer->getAddress().toString(),
                         QString::number(sniffer->getPort()))
                         );
    }
}

void MainWindow::AddPacketToTable(PacketEditor *packetEditor)
{
    m_packetCount++;
    QTreeWidgetItem* item = new QTreeWidgetItem;

    item->setText(MainWindow::PACKETTABLE_NUMBER, QString::number(m_packetCount)); //count packet

    //packet type
    item->setText(MainWindow::PACKETTABLE_TYPE, PacketEditor::getPacketTypeString(packetEditor->getPacketType()));

    item->setText(MainWindow::PACKETTABLE_SIZE, QString::number(packetEditor->getSize()));     //size
    item->setText(MainWindow::PACKETTABLE_OPCODE, QString::number(packetEditor->getOpcode()));  //opcode
    item->setText(MainWindow::PACKETTABLE_ASCII, Utils::ToASCII(packetEditor->getPacket()));   //ASCII
    item->setText(MainWindow::PACKETTABLE_HEX, Utils::ToHexString(packetEditor->getPacket())); //HEX

    //add item
    ui->treeWidgetPacket->addTopLevelItem(item);
    m_packets.insert(item, packetEditor);
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
    for(MwTablePackets::iterator itr = m_packets.begin(); itr != m_packets.end(); ++itr)
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
    //===============
    //clear
    m_snifferParamsList.clear();
    m_snifferList.clear();

    //===============
    //load setting
    int size = m_settings->beginReadArray("sniffer");

    if(size == 0)
    {
       SnifferParams params;
       params.adress = QHostAddress(SETTINGS_DEFAULT_AUTH_SERVER);
       params.port = SETTINGS_DEFAULT_AUTH_PORT;

       m_snifferParamsList.append(params);
    }

    for(int i = 0; i < size; i++)
    {
        m_settings->setArrayIndex(i);

        SnifferParams snifferParams;
        snifferParams.adress = QHostAddress(m_settings->value("server", SETTINGS_DEFAULT_AUTH_SERVER).value<QString>());
        snifferParams.port = m_settings->value("port", SETTINGS_DEFAULT_AUTH_PORT).value<qint16>();
        m_snifferParamsList.append(snifferParams);
    }
    m_settings->endArray();

    m_scriptDir = m_settings->value("packet/scriptDir", SETTING_PACKETEDITOR_SCRIPT_FOLDER).value<QString>();
    m_spoofingDir = m_settings->value("packet/spoofingDir", SETTING_SPOOFING_DEFAULT_FOLDER).value<QString>();

    //==================
    //save setting =====
    m_settings->beginWriteArray("sniffer");
    for (int i = 0; i < m_snifferParamsList.size(); ++i)
    {
        m_settings->setArrayIndex(i);
        m_settings->setValue("server", m_snifferParamsList.at(i).adress.toString());
        m_settings->setValue("port",  m_snifferParamsList.at(i).port);
    }
    m_settings->endArray();

    m_settings->setValue("packet/scriptDir", m_scriptDir);
    m_settings->setValue("packet/spoofingDir", m_spoofingDir);
    //==================
}

