#include "recentlyused.h"

#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QDir>

RecentlyUsed::RecentlyUsed(QObject *parent)
    : QObject(parent),

      m_xmlRdr(std::unique_ptr<QXmlStreamReader>(new QXmlStreamReader))
{
    QTimer *t = new QTimer(this);
    t->setSingleShot(false);
    t->setInterval(1000);
    t->start();

    connect(t, &QTimer::timeout, this, &RecentlyUsed::reload, Qt::QueuedConnection);
}

void RecentlyUsed::clear()
{
    QFile f(QDir::homePath() + "/.local/share/recently-used.xbel");
    f.remove();

    QTimer::singleShot(1, this, &RecentlyUsed::reload);
}

void RecentlyUsed::reload()
{
    m_history.clear();

    QFile f(QDir::homePath() + "/.local/share/recently-used.xbel");
    if (f.open(QIODevice::ReadOnly))
    {
        m_xmlRdr->setDevice(&f);
        while (!m_xmlRdr->atEnd())
        {
            if (!m_xmlRdr->readNextStartElement())
                continue;
            if (m_xmlRdr->name() != "bookmark")
                continue;

            const auto &location = m_xmlRdr->attributes().value("href");
            if (!location.isEmpty())
                m_history.append(location.toString());
        }
        f.close();
    }

    emit dataChanged(m_history);
}
