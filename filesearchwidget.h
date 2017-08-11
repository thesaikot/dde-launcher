#ifndef FILESEARCHWIDGET_H
#define FILESEARCHWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPointer>
#include <QProcess>

class FileSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileSearchWidget(QWidget *parent = 0);

public slots:
    void search(const QString &text);

private slots:
    void searchStart();

    void onSearchDone();

private:
    QTimer *m_searchDelay;
    QPointer<QProcess> m_worker;
    QString m_searchText;
};

#endif // FILESEARCHWIDGET_H
