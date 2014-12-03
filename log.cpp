#include "log.h"
#include "define.h"

Log::Log(QTextBrowser* parent)
{
    m_parent = parent;
    m_parent->setReadOnly(true);

    Add(LogLevel::NORMAL, TXT_LOG_DEFAULT);
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
        case LogLevel::ERROR :
            startHtml = "<font color=\"Red\">";
            break;

        case LogLevel::NORMAL :
            startHtml = "<font color=\"Black\">";
            break;

        case LogLevel::INFO :
            startHtml = "<font color=\"Blue\">";
            break;
    }

    QString endHtml = "</font>";
    m_parent->setHtml(m_parent->toHtml() + startHtml + ">> " + message + endHtml);

    //scrollbar to bottom
    MoveToBottom();
}
