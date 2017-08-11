#include "filesearchwidget.h"

#include <QDebug>

FileSearchWidget::FileSearchWidget(QWidget *parent)
    : QWidget(parent),

      m_searchDelay(new QTimer(this))
{
    m_searchDelay->setInterval(100);
    m_searchDelay->setSingleShot(true);

    connect(m_searchDelay, &QTimer::timeout, this, &FileSearchWidget::searchStart, Qt::QueuedConnection);
}

void FileSearchWidget::search(const QString &text)
{
    m_searchText = text.trimmed();
    m_searchDelay->start();
}

void FileSearchWidget::searchStart()
{
    if (!m_worker.isNull())
    {
        m_worker->terminate();
        m_worker->deleteLater();
    }

    QProcess *proc = new QProcess;
    connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &FileSearchWidget::onSearchDone);

    proc->start("locate", QStringList() << m_searchText);

    m_worker = proc;
}

void FileSearchWidget::onSearchDone()
{
    QProcess *proc = static_cast<QProcess *>(sender());

    qDebug() << proc->readAllStandardOutput();

    proc->deleteLater();
}
