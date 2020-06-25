#include "mylistwidget.h"
#include <QDebug>


//////////////////////////////////////////////////////////////////////////
void MyListWidget::mousePressEvent(QMouseEvent *event)
{
    emit xmousePressEvent(event);
    QListWidget::mousePressEvent(event);
}

void MyListWidget::mouseMoveEvent(QMouseEvent *event)
{
   QListWidget::mouseMoveEvent(event);
}

void MyListWidget::mouseReleaseEvent(QMouseEvent *event)
{
   QListWidget::mouseReleaseEvent(event);
}
//////////////////////////////////////////////////////////////////////////
