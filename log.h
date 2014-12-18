#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QTextBrowser>
#include <QString>
#include <QScrollBar>

class Log : public QObject
{
    Q_OBJECT
    Q_ENUMS(LogLevel)

public:
    enum LogLevel
    {
        LOGLEVEL_NORMAL, //black
        LOGLEVEL_ERROR, //red
        LOGLEVEL_INFO, //blue
    };

private:
    QTextEdit* m_parent;

public:
    Log(QTextBrowser* parent);

    void Add(LogLevel level, QString message);
    void Clear();

    void MoveToTop();
    void MoveToBottom();
};

#endif // LOG_H
