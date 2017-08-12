#ifndef FILESEARCHMODEL_H
#define FILESEARCHMODEL_H

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QPointer>
#include <QAbstractListModel>
#include <QFileInfo>

class FileSearchModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FileSearchModel(QObject *parent = 0);

    enum FileSarchItemRole
    {
        FileNameRole = Qt::DisplayRole,
        UnusedRole = Qt::UserRole,
        FilePathRole,
    };

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void search(const QString &text);

private slots:
    void searchStart();

    void onSearchDone();

private:
    void clearSearchResult();

private:
    QTimer *m_searchDelay;
    QPointer<QProcess> m_worker;
    QString m_searchText;
    QList<QFileInfo> m_data;
};

#endif // FILESEARCHMODEL_H
