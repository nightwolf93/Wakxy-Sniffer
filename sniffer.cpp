#include "sniffer.h"

Sniffer::Sniffer(QString adresse, qint16 port)
{
    m_proxy = new QTcpServer();

    m_localSocket = new QTcpSocket();
    m_localPktSize = 0;

    m_remoteSocket = NULL;
    m_remotePktSize = 0;

    m_adresse = QHostAddress(adresse);
    m_port = port;

    m_snifferState = eSnifferState::STOP;
    m_captureState = eSnifferState::STOP;
}

void Sniffer::Start()
{
    if (!this->m_proxy->listen(QHostAddress::LocalHost, m_port))
    {
        qDebug() << m_proxy->errorString();
        return;
    }

    m_snifferState = eSnifferState::START;
}

void Sniffer::Stop()
{
    if(m_remoteSocket) //1 close the remote socket
    {
        m_remoteSocket->abort();
        m_remoteSocket->deleteLater();
    }

    m_localSocket->abort(); //2 close the local socket
    m_proxy->close(); //3 stop the proxy

    m_snifferState = eSnifferState::STOP;
}


void Sniffer::StartCapture()
{
}
