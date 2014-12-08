#ifndef LOG_H
#define LOG_H

#include <QTextBrowser>
#include <QString>
#include <QScrollBar>


class Log
{
private:
    QTextEdit* m_parent;

public:
    enum LogLevel
    {
        NORMAL, //black
        ERROR, //red
        INFO, //blue
    };

    Log(QTextBrowser* parent);
    void Add(LogLevel level, QString message);
    void Clear();

    void MoveToTop();
    void MoveToBottom();
};

#endif // LOG_H
