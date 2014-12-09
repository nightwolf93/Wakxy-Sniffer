#ifndef PACKETEDITOR_H
#define PACKETEDITOR_H

#include <QObject>
#include <QtNetwork>
#include <QTcpServer>
#include "sniffer.h"

//packet type
class PacketEditor : public QObject
{
    Q_OBJECT
    Q_ENUMS(PacketType)

public:
    enum PacketType
    {
        PACKET_SERVER,
        PACKET_CLIENT,
    };

public:
    PacketEditor(QByteArray packet, PacketType packetType);

    //====================
    //getter =============
    QByteArray getPacket() { return m_packet; }
    PacketType getPacketType() { return m_packet_type; }
    int getLength() { return (int)m_packet_reader->device()->bytesAvailable(); }
    ushort getSize() { return m_size; }
    ushort getOpcode() { return m_opcode; }


private:    
    QByteArray m_packet;
    QDataStream* m_packet_reader;
    PacketType m_packet_type;

    ushort m_size;
    ushort m_opcode;

    //===================
    //method ============
    void ReadHeader();
    //===================
};

#endif // PACKETEDITOR_H
