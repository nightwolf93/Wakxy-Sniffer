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
//packet sniffing can be find getted by getPackets()
class Sniffer
{

private:
    QTcpServer* m_server;
    QTcpSocket* m_socket;
    Packets m_packets;
    QHostAddress m_adresse;
    qint16 m_port;

    eSnifferState m_sniffer_state;
    eSnifferState m_capture_state;

public:
    Sniffer(QString adresse, qint16 port);

    void Start();
    void Stop();

    void StartCapture();
    void StopCapture();

    Packets getPackets() { return this->m_packets; }
    QTcpServer* getServer() { return this->m_server; }

    eSnifferState getSnifferState() { return this->m_sniffer_state; }
    eSnifferState getCaptureState() { return this->m_capture_state; }
};

#endif // PROXY_H
