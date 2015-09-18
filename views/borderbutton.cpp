#include "borderbutton.h"
#include <QMouseEvent>
#include <QEvent>
#include <QWidget>
#include <QDebug>
#include <QButtonGroup>
#include <QStyle>
#include "app/global.h"
#include "widgets/util.h"
#include "launcherframe.h"
#include "appitem.h"
#include "widgets/elidelabel.h"

BorderButton::BorderButton(QWidget *parent) : QPushButton(parent)
{
    setCheckable(true);
    setFocusPolicy(Qt::NoFocus);
    setObjectName("BorderButton");
    setProperty("state", "normal");
    setStyleSheet(getQssFromFile(":/qss/skin/qss/main.qss"));
}

void BorderButton::startDrag(QMouseEvent *event){
    QMimeData* mimeData = new QMimeData;
    QJsonObject obj;
    obj.insert("appKey", dynamic_cast<AppItem*>(parent())->getAppKey());
    obj.insert("appName", dynamic_cast<AppItem*>(parent())->getAppName());
    mimeData->setData("RequestDock", QJsonDocument(obj).toJson());

    QLabel* iconLabel = new QLabel;
    iconLabel->setObjectName("Icon");
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(dynamic_cast<AppItem*>(parent())->getAppIcon());
    iconLabel->setFixedSize(48, 48);
    iconLabel->setStyleSheet("background-color: transparent");
    QPixmap dragPixmap = iconLabel->grab();
    QDrag* pDrag = new QDrag(this);
    pDrag->setMimeData(mimeData);
    pDrag->setPixmap(dragPixmap);

    pDrag->setHotSpot(event->pos() + QPoint(-36, -10));
//    pDrag->setDragCursor(QCursor(Qt::PointingHandCursor).pixmap(), Qt::MoveAction);

    Qt::DropAction action = pDrag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction);
    if (action == Qt::MoveAction){
        qDebug() << "not support MoveAction";
    }else{
        qDebug() << action;
    }
    iconLabel->deleteLater();
}

void BorderButton::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton){
       emit rightClicked(mapToGlobal(event->pos()));
       return;
    }
    QPushButton::mousePressEvent(event);
}

void BorderButton::mouseMoveEvent(QMouseEvent *event){
    startDrag(event);
    QPushButton::mouseMoveEvent(event);
}

void BorderButton::enterEvent(QEvent *event){
    setChecked(true);
    QPushButton::enterEvent(event);
}

void BorderButton::updateStyle(){
    style()->unpolish(this);
    style()->polish(this);
    update();
}

bool BorderButton::isHighlight() const {
    return m_isHighlight;
}

void BorderButton::setHighlight(bool isHightlight){
    m_isHighlight = isHightlight;
    if (isHightlight){
        setProperty("state", "Highlight");
    }else{
        setProperty("state", "normal");
    }
    updateStyle();
}

void BorderButton::toggleHighlight(){
    setHighlight(!isHighlight());
}

BorderButton::~BorderButton()
{

}
