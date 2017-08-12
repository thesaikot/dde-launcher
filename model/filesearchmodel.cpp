#include "filesearchmodel.h"

#include <QDebug>

FileSearchModel::FileSearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_searchDelay = new QTimer(this);
    m_searchDelay->setInterval(200);
    m_searchDelay->setSingleShot(true);

    connect(m_searchDelay, &QTimer::timeout, this, &FileSearchModel::searchStart, Qt::QueuedConnection);
}

int FileSearchModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_data.size();
}

QVariant FileSearchModel::data(const QModelIndex &index, int role) const
{
    const int r = index.row();

    switch (role)
    {
    case FileNameRole:      return m_data[r].filePath();
    case FilePathRole:      return m_data[r].filePath();
    default:;
    }

    return QVariant();
}

void FileSearchModel::search(const QString &text)
{
    if (!m_worker.isNull())
        m_worker->kill();

    m_searchText = text.trimmed();
    m_searchDelay->start();
}

void FileSearchModel::searchStart()
{
    if (m_searchText.isEmpty())
        return clearSearchResult();

    if (!m_worker.isNull())
        m_worker->kill();

    QProcess *proc = new QProcess;
    connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &FileSearchModel::onSearchDone);

    proc->start("locate", QStringList() << "--limit=100" << m_searchText);

    m_worker = proc;
}

void FileSearchModel::onSearchDone()
{
    QProcess *proc = static_cast<QProcess *>(sender());

    const QString &output = proc->readAllStandardOutput();
    m_data.clear();
    for (const auto f : output.split('\n'))
    {
        const QFileInfo info(f);
        if (info.isFile())
            m_data << info;
    }

    emit layoutChanged();

    proc->deleteLater();
}

void FileSearchModel::clearSearchResult()
{
    m_data.clear();

    emit layoutChanged();
}
