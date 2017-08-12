#ifndef FILESEARCHWIDGET_H
#define FILESEARCHWIDGET_H

#include <QWidget>
#include <QListView>

class FileSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileSearchWidget(QWidget *parent = 0);

public slots:
    void onItemClicked(const QModelIndex &idx);

private:
    QLineEdit *m_searchEdit;
    QListView *m_searchView;
};

#endif // FILESEARCHWIDGET_H
