#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QtNetwork>
#include <QMainWindow>
#include <QSettings>
#include <QTreeWidget>
#include <QWidgetItem>
#include <QFileDialog>

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

typedef QMap<QTreeWidgetItem*, PacketEditor*> MwTablePackets;
typedef QMap<QTreeWidgetItem*, SpoofPacket> MwSpoofPacket;

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
    void ActionOpen();
    void ActionSave();

    //spoofing
    void TableSpoofingColumnChanged(QTreeWidgetItem* item, int column);

private:
    Ui::MainWindow *ui;

    QSettings *m_settings; //settings
    Sniffer* m_sniffer; //sniffer
    int m_sniffer_packet_count;
    Log* m_log; //log

    MwTablePackets m_tableItemPackets;
    MwSpoofPacket m_spoofPackets;

    //===============================
    //setting from file =============
    QHostAddress m_authServer;
    qint16 m_authPort;
    QString m_scriptDir; //script directory
    QString m_spoofingDir; //spoofing directory
    //===============================

    //===============================
    //spoofing function =============
    bool spoofingPacket(Packet* packet, PacketEditor *packetEditor);
    void loadSpoofingPacket(); //load spoofing packet script
    //===============================

    void InitSettings();
    void ApplySettings();
    void AddPacketToTable(PacketEditor* packetEditor);

    void setProxyState(Sniffer::SnifferState state);
    void setCaptureState(Sniffer::SnifferState state);

    void SaveCapture();
    void LoadCapture();
};

#endif // MAINWINDOW_H
