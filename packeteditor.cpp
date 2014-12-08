#include "packeteditor.h"

PacketEditor::PacketEditor(QByteArray packet, PacketType packetType)
{
    m_packet = packet;
    m_packet_reader = new QDataStream(m_packet);

    m_packet_type = packetType;
    m_size = 0;
    m_opcode = 0;

    ReadHeader();
}

//read the header of packet
void PacketEditor::ReadHeader()
{
    *m_packet_reader >> m_size; //ushort

    if(getPacketType() == PacketEditor::PACKET_CLIENT)
    {
        qint8 unk1;
        *m_packet_reader >> unk1; //unk int8 in header
    }

    *m_packet_reader >> m_opcode; //ushort
}
