#include "historymodel.h"

#include <QTimer>
#include <QDebug>
#include <QUrl>

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&m_history, &RecentlyUsed::dataChanged, this, &HistoryModel::updateHistory);
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_data.size();
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case ItemNameRole:
        return m_data[index.row()].fileName();
    case ItemFullPathRole:
        return m_data[index.row()].absoluteFilePath();
    default:;
    }

    return QVariant();
}

void HistoryModel::clear()
{
    m_history.clear();
}

void HistoryModel::reload()
{
    m_history.reload();
}

void HistoryModel::updateHistory(const QList<QString> &history)
{
    m_data.clear();

    for (const auto &h : history)
    {
        const QFileInfo info(QUrl(h).toLocalFile());
        if (!info.exists())
            continue;

        m_data << info;
    }

    emit layoutChanged();
}
