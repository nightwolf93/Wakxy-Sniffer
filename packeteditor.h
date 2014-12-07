#ifndef PACKETEDITOR_H
#define PACKETEDITOR_H

#include <QtNetwork>
#include <QTcpServer>
#include "sniffer.h"

class PacketEditor
{
public:
    //packet type
    //server or client
    enum PacketType
    {
        PACKET_SERVER,
        PACKET_CLIENT,
    };

    PacketEditor(QByteArray packet, PacketType type);

private:
    QByteArray m_packet;
    PacketType m_packet_type;

};

#endif // PACKETEDITOR_H
