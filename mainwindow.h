#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QtNetwork>
#include <QMainWindow>
#include <QSettings>
#include <QTreeWidget>
#include <QWidgetItem>
#include <QFileDialog>
#include <QStringList>

#include "sniffer.h" //sniffer helper
#include "log.h" //log helper
#include "define.h" //all default value and some text
#include "packeteditor.h" //packet editor

namespace Ui {
class MainWindow;
}

struct SpoofPacket
{
    bool enabled;
    ushort opcode;
    int type;
    QByteArray raw;
};

struct SnifferParams
{
    QHostAddress adress;
    qint16 port;
};

typedef QMap<QTreeWidgetItem*, PacketEditor*> MwTablePackets;
typedef QMap<QTreeWidgetItem*, SpoofPacket> MwSpoofPacket;
typedef QMap<QTreeWidgetItem*, Sniffer*> MwTableSniffer;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_ENUMS(PacketTableColumns)

public:
    //packet table enum
    enum PacketTableColumns
    {
        PACKETTABLE_NUMBER,
        PACKETTABLE_TYPE,
        PACKETTABLE_SIZE,
        PACKETTABLE_OPCODE,
        PACKETTABLE_ASCII,
        PACKETTABLE_HEX,
    };

    enum SpoofingTableColumns
    {
        SPOOFINGTABLE_TYPE,
        SPOOFINGTABLE_OPCODE,
        SPOOFINGTABLE_ENABLED,
    };

    enum SnifferTableColumns
    {
        SNIFFERTABLE_ADRESS,
        SNIFFERTABLE_PORT,
        SNIFFERTABLE_STATUT,
        SNIFFERTABLE_CAPTURE,
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //local
    void OnLocalConnect();
    void OnLocalDisconnect();
    void OnLocalPacketRecv();
    void OnLocalSocketError(QAbstractSocket::SocketError);
    void OnLocalPacketSend(Packet packet);
    void OnLocalPacketHook(Packet* packet);

    //remote
    void OnRemoteConnect();
    void OnRemoteDisconnect();
    void OnRemotePacketRecv();
    void OnRemoteSocketError(QAbstractSocket::SocketError /*socketError*/);
    void OnRemotePacketSend(Packet packet);
    void OnRemotePacketHook(Packet* packet);

    //proxy
    void OnProxyConnection();
    void UpdateProxyState();
    void UpdateCaptureState();

    //conf
    void ReloadConf();

    //log
    void ClearLog();
    void ClearTable();

    //packets
    void ShowPacketZoom(QTreeWidgetItem* item);
    void CaptureOpen();
    void CaptureSave();

    //spoofing
    void TableSpoofingColumnChanged(QTreeWidgetItem* item, int column);
    void CreateSpoofing();
    void ReloadSpoofing();

private:
    Ui::MainWindow *ui;
    Log* m_log; //log
    MwTablePackets m_packets;

    //=====================
    //sniffer =============
    QList<SnifferParams> m_snifferParamsList;
    MwTableSniffer m_snifferList;
    int m_packetCount;

    void CreateSniffer(SnifferParams params);
    void setProxyState(MwTableSniffer::iterator itr, Sniffer::SnifferState state);
    void setCaptureState(MwTableSniffer::iterator itr, Sniffer::SnifferState state);

    void SaveCapture();
    void LoadCapture();

    //===============================
    //spoofing  =====================
    MwSpoofPacket m_spoofPackets;

    bool SpoofingPacket(Packet* packet, PacketEditor *packetEditor);
    void LoadSpoofingPacket(); //load spoofing packet script
    void CreateSpoofingFile(); //create a spoofing file of selected packet

    //===============================
    //settings ======================
    QSettings *m_settings; //settings

    QString m_scriptDir; //script directory
    QString m_spoofingDir; //spoofing directory

    void InitSettings();
    void ApplySettings();
    void AddPacketToTable(PacketEditor* packetEditor);
};

#endif // MAINWINDOW_H
