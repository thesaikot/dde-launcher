#include "recentlyused.h"

#include <QTimer>
#include <QFile>
#include <QDebug>

RecentlyUsed::RecentlyUsed(QObject *parent)
    : QObject(parent),

      m_xmlRdr(std::unique_ptr<QXmlStreamReader>(new QXmlStreamReader))
{
    QTimer::singleShot(1, this, &RecentlyUsed::reload);
}

void RecentlyUsed::reload()
{
    m_history.clear();

    QFile *f = new QFile("/home/.local/share/recently-used.xbel");
    f->open(QIODevice::ReadOnly);

    m_xmlRdr->setDevice(f);
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
}