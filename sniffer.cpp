#include "sniffer.h"

Sniffer::Sniffer(QString adresse, qint16 port)
{
    m_proxy = new QTcpServer();

    m_localSocket = new QTcpSocket(this);
    m_localPktSize = 0;

    m_remoteSocket = NULL;
    m_remotePktSize = 0;

    m_adresse = QHostAddress(adresse);
    m_port = port;

    m_snifferState = STOP;
    m_captureState = STOP;

    //===================
    //events link

    //local
    connect(m_localSocket, SIGNAL(connected()), this, SLOT(OnLocalConnect()));
    connect(m_localSocket, SIGNAL(disconnected()), this, SLOT(OnLocalDisconnect()));
    connect(m_localSocket, SIGNAL(readyRead()), this, SLOT(OnLocalPacketRecv()));
    connect(m_localSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnLocalError(QAbstractSocket::SocketError)));

    //remote don't connect event now
    //connect(m_remoteSocket, SIGNAL(connected()), this, SLOT(OnRemoteConnect()));
    //connect(m_remoteSocket, SIGNAL(connected()), this, SLOT(OnRemoteConnect()));
    //connect(m_remoteSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnLocalError(QAbstractSocket::SocketError)));

    //proxy
    connect(m_proxy, SIGNAL(newConnection()), this, SLOT(OnProxyConnect()));
    //===============================================================

}

//=====================
//SLOTS & SIGNALS =====
//=====================

//local
void Sniffer::OnLocalConnect()
{
    LocalConnect();
}

void Sniffer::OnLocalDisconnect()
{
    LocalDisconnect();
}

void Sniffer::OnLocalError(QAbstractSocket::SocketError socketError)
{
    LocalError(socketError);
}

void Sniffer::OnLocalPacketRecv()
{
     LocalPacketRecv();
}

//remote
void Sniffer::OnRemoteConnect()
{
    RemoteConnect();
}

void Sniffer::OnRemoteDisconnect()
{
    RemoteDisconnect();
}

void Sniffer::OnRemoveError(QAbstractSocket::SocketError socketError)
{
    RemoteError(socketError);
}

void Sniffer::OnRemovePacketRecv()
{
    /*
    if(m_remoteSocket->readBufferSize() < (qint64)sizeof(quint16))
            return;

    //data remote want to send
    QByteArray data = m_remoteSocket->readAll();

    //create a packet copy
    Packet packet;
    packet.raw = data;
    packet.delayedTime = 0;

    this->QueuePacket(packet, false); //send
    */

    RemotePacketRecv();
}

//proxy

void Sniffer::OnProxyConnect()
{
    m_remoteSocket = m_proxy->nextPendingConnection();

    if (!m_remoteSocket)
        return;

   connect(m_remoteSocket, SIGNAL(connected()), this, SLOT(OnRemoteConnect()));
   connect(m_remoteSocket, SIGNAL(readyRead()), this, SLOT(OnRemovePacketRecv()));
   connect(m_remoteSocket, SIGNAL(disconnected()), this, SLOT(OnRemoteDisconnect()));
   connect(m_remoteSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnClientError(QAbstractSocket::SocketError)));

   m_remoteSocket->connectToHost(m_adresse, m_port);
   if(m_remoteSocket->waitForConnected(2000))
    ProxyConnect();
}

//=================

//=========================
//METHODS =================
//=========================
void Sniffer::Start()
{
    if (!this->m_proxy->listen(QHostAddress::LocalHost, m_port))
    {
        qDebug() << m_proxy->errorString();
        return;
    }

    m_snifferState = START;
}

void Sniffer::Stop()
{
    if(m_remoteSocket) //1 close the remote socket
    {
        m_remoteSocket->abort();
    }

    m_localSocket->abort(); //2 close the local socket
    m_proxy->close(); //3 stop the proxy

    m_snifferState = STOP;
}


void Sniffer::StartCapture()
{
}


void Sniffer::QueuePacket(Packet packet, bool isLocalPacket)
{
    (isLocalPacket) ? m_remoteSocket->write(packet.raw) : m_localSocket->write(packet.raw);
}
