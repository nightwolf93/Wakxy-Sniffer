#include "packeteditor.h"

PacketEditor::PacketEditor(QByteArray packet, PacketType packetType, QString scriptDir)
{
    m_scriptEngine = NULL;
    m_scriptDir = scriptDir;

    m_packet = packet;
    m_packetReader = new QDataStream(m_packet);

    m_packetType = packetType;
    m_size = 0;
    m_opcode = 0;

    ReadHeader(); //header
}

//=====================================================
//SLOTS & EVENTS ======================================
//=====================================================

//bool
bool PacketEditor::ReadBool(QString key, QString parentKey)
{
    bool value = Read<bool>();
    (value) ? AddToTree(key, "TRUE", parentKey) : AddToTree(key, "FALSE", parentKey);
    return value;
}

//int32
qint32 PacketEditor::ReadInt(QString key, QString parentKey)
{
    qint32 value = Read<qint32>();
    AddToTree(key, QString::number(value), parentKey);
    return value;
}

//uint32
quint32 PacketEditor::ReadUInt(QString key, QString parentKey)
{
    quint32 value = Read<quint32>();
    AddToTree(key, QString::number(value), parentKey);
    return value;
}

//float
float PacketEditor::ReadFloat(QString key, QString parentKey)
{
    float value = Read<float>();
    AddToTree(key, QString::number(value), parentKey);
    return value;
}

//double
double PacketEditor::ReadDouble(QString key, QString parentKey)
{
    double value = Read<double>();
    AddToTree(key, QString::number(value), parentKey);
    return value;
}

//short
qint16 PacketEditor::ReadShort(QString key, QString parentKey)
{
   qint16 value = Read<qint16>();
   AddToTree(key, QString::number(value), parentKey);
   return value;
}

//ushort
quint16 PacketEditor::ReadUShort(QString key, QString parentKey)
{
   quint16 value = Read<quint16>();
   AddToTree(key, QString::number(value), parentKey);
   return value;
}

//byte
char PacketEditor::ReadByte(QString key, QString parentKey)
{
   char value = Read<qint8>();
   AddToTree(key, QString::number(value), parentKey);
   return value;
}

//ubyte
uchar PacketEditor::ReadUByte(QString key, QString parentKey)
{
   uchar value = Read<quint8>();
   AddToTree(key, QString::number(value), parentKey);
   return value;
}

//long
qint64 PacketEditor::ReadLong(QString key, QString parentKey)
{
    qint64 value = Read<qint64>();
    AddToTree(key, QString::number(value), parentKey);
    return value;
}

//ulong
quint64 PacketEditor::ReadULong(QString key, QString parentKey)
{
    quint64 value = Read<quint64>();
    AddToTree(key, QString::number(value), parentKey);
    return value;
}

//global function for read string
QString PacketEditor::ReadString(quint16 length, QString key, QString parentKey)
{
    //create a byte array of length
    QByteArray bytes;
    bytes.resize(length);

    for (quint16 i = 0; i < length; ++i)
        bytes[i] = Read<qint8>(); //read byte per byte

    QString string = QString(bytes); //convert to string
    AddToTree(key, string, parentKey); //add root not need in ReadCString and ReadBigString
    return string;
}

//string
QString PacketEditor::ReadCString(QString key, QString parentKey)
{
    uchar size;
    *m_packetReader >> size;
    QString string = ReadString(size, key, parentKey);
    return string;
}


//bigstring
QString PacketEditor::ReadBigString(QString key, QString parentKey)
{
   quint16 size;
   *m_packetReader >> size;

   return ReadString(size, key, parentKey);
}

//=====================================================
//METHODS =============================================
//=====================================================

//check if folder exit
void PacketEditor::CheckScriptFolder()
{
    (QDir().mkdir(m_scriptDir)) ? ScriptFolderNotExist() : ScriptFolderNotExist();
}

void PacketEditor::CheckScriptFile()
{
    m_scriptFileName = "";
    m_scriptFileName += m_scriptDir + "/";
    m_scriptFileName += (getPacketType() == PACKET_CLIENT ? "cmsg_" :  "smsg_");
    m_scriptFileName += QString::number(getOpcode()) + ".js";

    QFile file(m_scriptFileName);

    bool result = false;
    if((result = file.open(QIODevice::ReadWrite)))  //create the file if not exist
    {
        QTextStream in(file.readAll());
        m_script = in.readAll();
        file.close();
    }

    (result) ? ScriptFileExist() : ScriptFileNotExist();
}

void PacketEditor::CompileScript()
{

    m_packetReader->device()->seek(0); //begin
    ReadHeader(); //read header

    if(m_script.isEmpty())
    {
        ScriptFileEmpty();
        return;
    }

    if(!m_scriptEngine)
    {
        m_scriptEngine = new QScriptEngine(this); //instance of packetReader
        m_scriptEngine->globalObject().setProperty("packet", m_scriptEngine->newQObject(this));
    }

    QScriptValue script = m_scriptEngine->evaluate(m_script);
    if (script.isError())
    {
        QString errorMessage;
        ScriptCompileError(errorMessage);
        return;
    }

    ScriptCompileDone();
}

//numerical type except string can be read like this
template<typename T> T PacketEditor::Read()
{
    T variant;
    *m_packetReader >> variant;
    return variant;
}

//read the header of packet
void PacketEditor::ReadHeader()
{
    *m_packetReader >> m_size; //ushort

    if(getPacketType() == PacketEditor::PACKET_CLIENT)
    {
        qint8 unk1;
        *m_packetReader >> unk1; //unk int8 in header
    }

    *m_packetReader >> m_opcode; //ushort
}

void PacketEditor::ReadPacket()
{
    m_treeRoot.clear();
    CheckScriptFolder(); //check script folder
    CheckScriptFile(); //check script file
    CompileScript(); // compile the script
}

QString PacketEditor::getPacketTypeString(PacketType type)
{
    if(type == PACKET_CLIENT)
        return PACKETEDITOR_PACKET_CLIENT;
    else
        return PACKETEDITOR_PACKET_SERVER;
}

//====================
//tree ===============

void PacketEditor::DisplayTree(QTreeWidget* treeWidget)
{
    treeWidget->clear();
    treeWidget->addTopLevelItems(m_treeRoot);
}

QTreeWidgetItem* PacketEditor::AddToTree(QString key, QString value, QString parentKey)
{

    QTreeWidgetItem* item;
    bool rootItem = true;

    //find the parent key
    QHash<QString, QTreeWidgetItem*>::iterator itr = m_tree.find(parentKey);

    //if parent key and parent key value is not null
    //is a child
    if(itr != m_tree.end() && itr.value() != NULL)
        rootItem = false;

    //constructor
    if(rootItem)
        item = new QTreeWidgetItem;
    else
        item = new QTreeWidgetItem(itr.value());

    //value
    item->setText(0, key);
    item->setText(1, value);

    //root item append on tree root
    if(rootItem)
        m_treeRoot.append(item);

    m_tree.insert(key, item); //insert on hash map
    return item;
}

//=====================================================
