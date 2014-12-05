#ifndef PROXY_H
#define PROXY_H

#include <QtNetwork>
#include <QTcpServer>
#include <QList>

#include "define.h"


enum eSnifferState
{
    START,
    STOP
};

//packet struct
struct Packet
{
    QByteArray raw;
    Packet* reader;
    qint64 delayedTime;
};

//usefull typdef
//list of packet
typedef QList<Packet> Packets;

//sniffer class
//help to make a sniffer
//packet sniffing can be getted by getPackets()
class Sniffer
{

private:
    QTcpServer* m_proxy;

    QTcpSocket* m_remoteSocket;
    quint16 m_remotePktSize;

    QTcpSocket* m_localSocket;
    quint16 m_localPktSize;

    Packets m_packets;
    QHostAddress m_adresse;
    qint16 m_port;

    eSnifferState m_snifferState;
    eSnifferState m_captureState;

public slots:
    void OnLocalConnect(); //local connection
    void OnLocalPacketRecv(); //local packet receive
    void OnLocalDisconnect(); //local connection is close
    void OnLocalError(QAbstractSocket::SocketError); //local socket error

    void OnRemoteConnect(); //remote connection
    void OnRemovePacketRecv(); //remote packet receive
    void OnRemoteDisconnect(); //remote connection is close
    void OnRemoveError(QAbstractSocket::SocketError); //remote socket error

public:
    Sniffer(QString adresse, qint16 port);

    void Start();
    void Stop();

    void StartCapture();
    void StopCapture();

    Packets getPackets() { return this->m_packets; }
    QTcpServer* getProxy() { return this->m_proxy; }
    QTcpSocket* getRemoteSocket() { return this->m_remoteSocket; }

    eSnifferState getSnifferState() { return this->m_snifferState; }
    eSnifferState getCaptureState() { return this->m_snifferState; }
};

#endif // PROXY_H
