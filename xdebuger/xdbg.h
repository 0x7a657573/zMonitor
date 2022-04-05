#ifndef XDBG_H
#define XDBG_H

#include <QWidget>
#include <QListView>
#include <QDebug>
#include <QVBoxLayout>
#include <QListView>
#include <QListWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QColorDialog>
#include <QSettings>
#include <QPlainTextEdit>
#include <QPaintEvent>

#define IDSign      0x2e756400
#define DepthSign   0xF0000000

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
    void handel_SetColor(const QColor &color);
    void handel_ChangeID();
    void handel_ChangeDepth();
    void handel_SetupFilter();
    void handel_logCustomContextMenuRequested(QPoint p);

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
    QVBoxLayout *MayLayout;
    QColor TextColor;
    QColor BackColor;
    QPlainTextEdit *log_view;

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
    QColorDialog    ColorDialog;

    void LoadSetting();
};

#endif // XDBG_H
