#include "filesearchwidget.h"
#include "../model/filesearchmodel.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QModelIndex>

FileSearchWidget::FileSearchWidget(QWidget *parent)
    : QWidget(parent)
{
    FileSearchModel *model = new FileSearchModel;

    m_searchEdit = new QLineEdit;
    m_searchView = new QListView;
    m_searchView->setModel(model);
    m_searchView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_searchView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->addStretch();
    searchLayout->addWidget(new QLabel("搜索："));
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addStretch();

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout(searchLayout);
    centralLayout->addWidget(m_searchView);

    setLayout(centralLayout);

    connect(m_searchEdit, &QLineEdit::textChanged, model, &FileSearchModel::search);
    connect(m_searchView, &QListView::clicked, this, &FileSearchWidget::onItemClicked);
}

void FileSearchWidget::onItemClicked(const QModelIndex &idx)
{
    const QString &path = idx.data(FileSearchModel::FilePathRole).toString();

    QProcess::startDetached("xdg-open", QStringList() << path);
}
