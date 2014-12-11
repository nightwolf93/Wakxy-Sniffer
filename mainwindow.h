#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QtNetwork>
#include <QMainWindow>
#include <QSettings>
#include <QTreeWidget>
#include <QFileDialog>

#include "sniffer.h" //sniffer helper
#include "log.h" //log helper
#include "define.h" //all default value and some text
#include "packeteditor.h" //packet editor

namespace Ui {
class MainWindow;
}

typedef QHash<QTreeWidgetItem*, PacketEditor*> MwTablePackets;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_ENUMS(PacketTableColumns)

public:
    //packet table enum
    enum PacketTableColumns
    {
        NUMBER,
        TYPE,
        SIZE,
        OPCODE,
        ASCII,
        HEX,
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void OnLocalConnect();
    void OnLocalDisconnect();
    void OnLocalPacketRecv();
    void OnLocalSocketError(QAbstractSocket::SocketError);
    void OnLocalPacketSend(Packet packet);

    void OnRemoteConnect();
    void OnRemoteDisconnect();
    void OnRemotePacketRecv();
    void OnRemoteSocketError(QAbstractSocket::SocketError /*socketError*/);
    void OnRemotePacketSend(Packet packet);

    void OnProxyConnection();
    void UpdateProxyState();
    void UpdateCaptureState();

    void ReloadConf();
    void ClearLog();
    void ClearTable();
    void PacketZoom(QTreeWidgetItem* item);
    void ActionOpen();
    void ActionSave();

private:
    Ui::MainWindow *ui;

    QSettings *m_settings; //settings
    Sniffer* m_sniffer; //sniffer
    Log* m_log; //log

    MwTablePackets m_tableItemPackets;

    //===============================
    //setting from file =============
    QHostAddress m_authServer;
    qint16 m_authPort;
    //===============================

    void InitSettings();
    void ApplySettings();
    void AddPacketToTable(PacketEditor* packetEditor);

    void setProxyState(Sniffer::SnifferState state);
    void setCaptureState(Sniffer::SnifferState state);

    void SaveCapture(); //save all captured packets
    void LoadCapture(); //load all captured packets

};

#endif // MAINWINDOW_H
