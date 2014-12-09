#ifndef UTILS_H
#define UTILS_H

#include <QtNetwork>

class Utils
{
public:
   /**
   * Functions based on IntPe9
   * https://github.com/Intline9/IntPe9/blob/master/IntPe9/Src/Packet.cpp
   */

    static QString ToHexString(QByteArray packet);
    static QByteArray FromHexString(QString packet);
    static QString ToASCII(QByteArray packet);
};

#endif // UTILS_H
