#ifndef XDBG_H
#define XDBG_H

#include <QWidget>
#include <QListView>
#include <QDebug>
#include <QVBoxLayout>
#include <QListView>
#include <QListWidget>
#include <QMouseEvent>
#include "mylistwidget.h"
#include <QLabel>
#include <QColorDialog>

class xdbg : public QWidget
{
    Q_OBJECT
public:
    explicit xdbg(QWidget *parent = nullptr);

    void addItem(QString Item);


public slots:
    void handel_mousePressEvent(QMouseEvent *event);
    void handel_SetColor(const QColor &color);

    void clear();
    void CopyToClipboard();
    void ShowColorDialog();
    void ShowBColorDialog();

signals:
protected:
    MyListWidget   *ListView;
    QVBoxLayout *MayLayout;
    int MaxRow;
    QColor TextColor;
    QColor BackColor;

    /*PopUp*/
    QMenu *PopMenu;
    QColorDialog    ColorDialog;
};

#endif // XDBG_H
