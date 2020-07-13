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
#include <QSettings>

#define IDSign  0x2e756400

class xdbg : public QWidget
{
    Q_OBJECT
public:
    explicit xdbg(QSettings *Setting,QWidget *parent = nullptr,int id = 0);
    void setID(int id);
    void addItem(QString Item);
    int getID();
    void ResetSetting();

public slots:
    void handel_mousePressEvent(QMouseEvent *event);
    void handel_SetColor(const QColor &color);
    void handel_ChangeID();
    void handel_SetupFilter();

    void clear();
    void CopyToClipboard();
    void ShowColorDialog();
    void ShowBColorDialog();

signals:
protected:
    QString xKey;
    QSettings *xSetting;
    int xID;
    int sID;
    MyListWidget   *ListView;
    QVBoxLayout *MayLayout;
    int MaxRow;
    QColor TextColor;
    QColor BackColor;

    /*filter*/
    typedef struct
    {
        bool Enable;
        bool Blankfilter;
        QString Filter;

    }Filter_t;
    Filter_t xFilter;
    QStringList Filters;

    /*PopUp*/
    QMenu *PopMenu;
    QColorDialog    ColorDialog;

    void LoadSetting();
};

#endif // XDBG_H
