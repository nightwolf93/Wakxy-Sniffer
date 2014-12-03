#include "sniffer.h"

Sniffer::Sniffer(QString adresse, qint16 port)
{
    m_server = new QTcpServer();
    m_socket = new QTcpSocket();
    m_adresse = QHostAddress(adresse);
    m_port = port;
    m_sniffer_state = eSnifferState::STOP;
    m_capture_state = eSnifferState::STOP;
}

void Sniffer::Start()
{
    if (!this->m_server->listen(QHostAddress::LocalHost, m_port))
        return;

    m_sniffer_state = eSnifferState::START;
}

void Sniffer::Stop()
{
    m_socket->abort();
    m_server->close();

    m_sniffer_state = eSnifferState::STOP;
}


void Sniffer::StartCapture()
{
}
