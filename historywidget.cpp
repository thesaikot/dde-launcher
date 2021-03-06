/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "historywidget.h"
#include "model/historymodel.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QProcess>

HistoryWidget::HistoryWidget(QWidget *parent)
    : QWidget(parent),

      m_historyModel(new HistoryModel),
      m_historyView(new QListView),
      m_clearBtn(new QPushButton)
{
    m_historyView->setModel(m_historyModel);
    m_historyView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_historyView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_clearBtn->setText("清除");

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_historyView);
    centralLayout->addWidget(m_clearBtn);
    centralLayout->setAlignment(m_clearBtn, Qt::AlignBottom | Qt::AlignCenter);
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(centralLayout);

    connect(m_historyView, &QListView::clicked, this, &HistoryWidget::onItemClicked);
    connect(m_clearBtn, &QPushButton::clicked, m_historyModel, &HistoryModel::clear);
}

void HistoryWidget::onItemClicked(const QModelIndex &index)
{
    const QString &fullpath = index.data(HistoryModel::ItemFullPathRole).toString();

    QProcess::startDetached("xdg-open", QStringList() << fullpath);
}
