#include "log.h"
#include "define.h"

Log::Log(QTextBrowser* parent)
{
    m_parent = parent;
    m_parent->setReadOnly(true);

    Add(LOGLEVEL_NORMAL, TXT_LOG_DEFAULT);
}

void Log::Clear()
{
    m_parent->clear();
}

void Log::MoveToBottom()
{
    QScrollBar *v = m_parent->verticalScrollBar();
    v->setValue(v->maximum());
}

void Log::MoveToTop()
{
    QScrollBar *v = m_parent->verticalScrollBar();
    v->setValue(v->minimum());
}

void Log::Add(LogLevel level, QString message)
{
    QString startHtml = "<font color=>";
    switch(level)
    {
        case LOGLEVEL_ERROR :
            startHtml = "<font color=\"Red\">";
            break;

        case LOGLEVEL_NORMAL :
            startHtml = "<font color=\"Black\">";
            break;

        case LOGLEVEL_INFO :
            startHtml = "<font color=\"Blue\">";
            break;
    }

    QString endHtml = "</font>\n";
    m_parent->append(startHtml + ">> " + message + endHtml);

    //scrollbar to bottom
    MoveToBottom();
}
