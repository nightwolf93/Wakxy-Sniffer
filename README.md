Wakxy-Sniffer
=============

Wakxy-Sniffer est un sniffer par méthode proxy pour le jeux Wakfu.
Il est développé en C++/QT

Configuration
-------------

Dans le fichier de configuration *settings.ini* vous avez la possibilité de parametrer les choses suivantes :
  * Les proxys que vous voulez
  * Le répertoire des scripts pour la lecture des packets
  * Le répertoire des json pour le spoofing des packets
  
Fichier
-------

Vous avez la possibilité de sauvegarder/charger et de partager vos packets a l'aide d'un fichier .wxy.
Ce fichier contient l'ensemble des packets que vous avez récupéré.
  
Scripting
---------

Le proxy inclut un système de scripting en java.
L'ensemble des méthodes possibles d'appeler sont les suivantes :

```cpp
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
```

Spoofing
--------

Le proxy inclut aussi la possibilité de remplacer un packet client server ou server client.
Pour celà, vous devez créer un fichier de spoofing (.json) contenant la structure complète du packet ainsi que son opcode.
Afin de faciliter l'édition, vous pouvez generer un fichier de spoofing a partir d'un packet déjà chargé.

Autheur
-------

- SGT Fatality
- Totomakers



