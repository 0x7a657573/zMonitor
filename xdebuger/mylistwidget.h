#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H
#include <QListWidget>
#include <QMouseEvent>

class MyListWidget: public QListWidget
{ // QListWidget clone then add xmousePressEvent signal
    Q_OBJECT
public:
    MyListWidget():QListWidget() {}


signals:
    void xmousePressEvent(QMouseEvent *event);
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MYLISTWIDGET_H
