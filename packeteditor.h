#ifndef PACKETEDITOR_H
#define PACKETEDITOR_H

#include <QtCore>
#include <QObject>
#include <QtNetwork>
#include <QTcpServer>
#include <QtScript>
#include <QTreeWidget>
#include "sniffer.h"

#define PACKETEDITOR_UNK_KEY "unk"
#define PACKETEDITOR_PACKET_SERVER "SMSG"
#define PACKETEDITOR_PACKET_CLIENT "CMSG"

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
    PacketEditor(QByteArray packet, PacketType packetType, QString scriptFolder);

    //====================
    //getter =============
    QByteArray getPacket() { return m_packet; }
    PacketType getPacketType() { return m_packetType; }
    int getLength() { return (int)m_packetReader->device()->bytesAvailable(); }
    ushort getSize() { return m_size; }
    ushort getOpcode() { return m_opcode; }
    QString getScriptName() { return m_scriptFileName; }

    //=====================
    //methods =============
    void ReloadScript();
    void ReadPacket(); //read the packet
    void DisplayTree(QTreeWidget* treeWidget);
    template<typename T> T Read(); //read function
    qint8 ReadInt8();
    quint8 ReadUInt8();
    static QString getPacketTypeString(PacketType type);
    //=====================


signals:
    //folder
    void ScriptFolderNotExist();
    void ScriptFolderExist();

    //file
    void ScriptFileNotExist();
    void ScriptFileExist();
    void ScriptFileEmpty();

    //script
    void ScriptCompileError(QString errorMessage);
    void ScriptCompileDone();

public slots:

    //=========================
    //Script engine ===========
    bool ReadBool(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read bool (1 or 4 byte)
    qint32 ReadInt(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read int (4 byte)
    quint32 ReadUInt(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read uint (4 byte)
    float ReadFloat(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read float (4 byte)
    double ReadDouble(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read double (8 byte)
    qint16 ReadShort(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read short (2 byte)
    quint16 ReadUShort(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read ushort (2 byte)
    char ReadByte(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read byte (1 byte)
    uchar ReadUByte(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read ubyte (1 byte)
    qint64 ReadLong(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read long (4 byte)
    quint64 ReadULong(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read ulong (4byte)
    QString ReadString(quint16 length = 0, QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read string length
    QString ReadCString(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read string length, if 0 determine the size.
    QString ReadBigString(QString key = QString(PACKETEDITOR_UNK_KEY), QString parentKey = ""); //read big string
    //=========================

private:
    QByteArray m_packet;
    QDataStream* m_packetReader;
    PacketType m_packetType;

    QScriptEngine* m_scriptEngine; //script engine
    QString m_scriptDir;
    QString m_scriptFileName; //script file
    QString m_script; //script
    QList<QTreeWidgetItem*> m_treeRoot; //list of root item
    QHash<QString, QTreeWidgetItem*> m_tree; //all item of the tree

    ushort m_size;
    ushort m_opcode;

    //===================
    //methods ============
    void ReadHeader();
    void CheckScriptFolder();
    void CheckScriptFile();
    void CompileScript();
    QTreeWidgetItem* AddToTree(QString key, QString value, QString parentKey = "");

};

#endif // PACKETEDITOR_H
