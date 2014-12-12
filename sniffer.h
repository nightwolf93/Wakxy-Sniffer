#ifndef PROXY_H
#define PROXY_H

#include <QObject>
#include <QtNetwork>
#include <QTcpServer>
#include <QList>
#include <QtCore>
#include "define.h"

#define PACKET_SIZE_WAKFU 2

//packet struct
struct Packet
{
    QByteArray raw;
    qint64 delayedTime;
};

//usefull typdef
//list of packet
typedef QList<Packet> Packets;


//sniffer class
//help to make a sniffer
//packet sniffing can be getted by getPackets()
class Sniffer : public QObject
{
    Q_OBJECT
    Q_ENUMS(SnifferState)
    Q_ENUMS(CaptureState)

public:
    //state
    enum SnifferState
    {
        START,
        STOP
    };

private:
    QTcpServer* m_proxy;

    QTcpSocket* m_remoteSocket;
    qint16 m_remotePktSize;

    QTcpSocket* m_localSocket;
    qint16 m_localPktSize;

    Packets m_packets;
    QHostAddress m_adresse;
    qint16 m_port;

    SnifferState m_proxyState;
    SnifferState m_captureState;

    int m_countPackets;

//signals can be connected in ui
signals:
    void LocalConnect();
    void LocalPacketRecv();
    void LocalDisconnect();
    void LocalError(QAbstractSocket::SocketError socketError);
    void LocalPacketSend(Packet packet);

    void RemoteConnect();
    void RemotePacketRecv();
    void RemoteDisconnect();
    void RemoteError(QAbstractSocket::SocketError socketError);
    void RemotePacketSend(Packet packet);

    void ProxyConnect();

public slots:
    void OnLocalConnect(); //local connection
    void OnLocalPacketRecv(); //local packet receive
    void OnLocalDisconnect(); //local connection is close
    void OnLocalError(QAbstractSocket::SocketError socketError); //local socket error

    void OnRemoteConnect(); //remote connection
    void OnRemovePacketRecv(); //remote packet receive
    void OnRemoteDisconnect(); //remote connection is close
    void OnRemoteError(QAbstractSocket::SocketError socketError); //remote socket error

    void OnProxyConnect();

public:
    Sniffer(QString adresse, qint16 port);

    void Start();
    void Stop();

    void StartCapture() { m_captureState = Sniffer::START; }
    void StopCapture() { m_captureState = Sniffer::STOP; }

    void QueuePacket(Packet packet, bool isLocalPacket);

    Packets getPackets() { return m_packets; }
    QTcpServer* getProxy() { return m_proxy; }

    QTcpSocket* getRemoteSocket() { return m_remoteSocket; }
    QTcpSocket* getLocalSocket() { return m_localSocket; }

    SnifferState getProxyState() { return m_proxyState; }
    SnifferState getCaptureState() { return m_captureState; }
    void setCaptureState(SnifferState state);

    int getCountPackets() { return m_countPackets; }

private:
    void resetCountPackets() { m_countPackets = 0; }
};

#endif // PROXY_H
