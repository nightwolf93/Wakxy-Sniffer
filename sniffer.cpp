#include "sniffer.h"

Sniffer::Sniffer(QString adresse, qint16 port)
{
    m_proxy = new QTcpServer();

    m_remoteSocket = new QTcpSocket(this);
    m_remotePktSize = 0;

    m_localSocket = NULL;
    m_localPktSize = 0;

    m_countPackets = 0;

    m_adresse = QHostAddress(adresse);
    m_port = port;

    m_proxyState = SnifferState::STOP;
    m_captureState = SnifferState::STOP;

    //===================
    //events link

    //local
    connect(m_remoteSocket, SIGNAL(connected()), this, SLOT(OnRemoteConnect()));
    connect(m_remoteSocket, SIGNAL(disconnected()), this, SLOT(OnRemoteDisconnect()));
    connect(m_remoteSocket, SIGNAL(readyRead()), this, SLOT(OnRemovePacketRecv()));
    connect(m_remoteSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnRemoteError(QAbstractSocket::SocketError)));

    //proxy
    connect(m_proxy, SIGNAL(newConnection()), this, SLOT(OnProxyConnect()));
    //===============================================================

}

//=====================
//SETTER & GETTER =====
//=====================

void Sniffer::setCaptureState(SnifferState state)
{
    m_captureState = state;

    if(state == STOP)
    {
        m_countPackets = 0;
    }
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
    QDataStream in(m_localSocket);
    LocalPacketRecv(); //event

    while (m_localSocket->bytesAvailable() > 0)
    {
        //packet size is 0
        if (m_localPktSize == 0)
        {
            if (m_localSocket->bytesAvailable() < (sizeof(qint16)))
                return;

            //add this size in m_remotePktSize
            in >> m_localPktSize;
        }

        //1 packet have not be receive
        if ((PACKET_SIZE_WAKFU + m_localSocket->bytesAvailable()) < m_localPktSize)
            return;

        //1 packet is receive
        QByteArray buffer;

        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream << m_localPktSize;

        buffer += in.device()->read(m_localPktSize - sizeof(quint16)); //add the packet to buffer, remove part of the next

        //create same packet
        Packet packet;
        packet.raw = buffer;
        packet.delayedTime = 0;

        QueuePacket(packet, true); //send

        m_localPktSize = 0; // reset packet size

    }
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

void Sniffer::OnRemoteError(QAbstractSocket::SocketError socketError)
{
    RemoteError(socketError);
}

void Sniffer::OnRemovePacketRecv()
{
    QDataStream in(m_remoteSocket);
    RemotePacketRecv(); //event

    while (m_remoteSocket->bytesAvailable() > 0)
    {
        //packet size is 0
        if (m_remotePktSize == 0)
        {
            if (m_remoteSocket->bytesAvailable() < (sizeof(qint16)))
                return;

            //add this size in m_remotePktSize
            in >> m_remotePktSize;
        }

       //1 packet have not be receive
       if ((PACKET_SIZE_WAKFU + m_remoteSocket->bytesAvailable() )  < m_remotePktSize)
        return;

       //1 packet is receive
       QByteArray buffer;

       QDataStream stream(&buffer, QIODevice::WriteOnly);
       stream << m_remotePktSize;

       buffer += in.device()->read(m_remotePktSize - sizeof(qint16)); //add the packet to buffer, part of next is remove

       //create same packet
       Packet packet;
       packet.raw = buffer;
       packet.delayedTime = 0;

       QueuePacket(packet, false); //send

       m_remotePktSize = 0; // reset packet size
    }
}

//proxy
void Sniffer::OnProxyConnect()
{
    m_localSocket = m_proxy->nextPendingConnection();

    if (!m_localSocket)
        return;

   connect(m_localSocket, SIGNAL(connected()), this, SLOT(OnLocalConnect()));
   connect(m_localSocket, SIGNAL(readyRead()), this, SLOT(OnLocalPacketRecv()));
   connect(m_localSocket, SIGNAL(disconnected()), this, SLOT(OnLocalDisconnect()));
   connect(m_localSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnLocalError(QAbstractSocket::SocketError)));

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

    m_proxyState = START;
}

void Sniffer::Stop()
{
    if(m_localSocket) //1 close the local socket
    {
        m_localSocket->abort();
        m_localSocket->deleteLater();
    }

    m_remoteSocket->abort(); //2 close the remote socket
    m_proxy->close(); //3 stop the proxy

    //reset packet size
    m_remotePktSize = 0;
    m_localPktSize = 0;

    //state sniffer
    m_proxyState = STOP;
    StopCapture();
}

void Sniffer::QueuePacket(Packet packet, bool isLocalPacket)
{
    if(isLocalPacket)
    {
        m_remoteSocket->write(packet.raw);

        if(m_captureState == START)
        {
            LocalPacketSend(packet);
            m_countPackets++;
        }
    }
    else
    {
        m_localSocket->write(packet.raw);

        if(m_captureState == START)
        {
            RemotePacketSend(packet);
            m_countPackets++;
        }
    }
}
