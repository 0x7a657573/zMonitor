#include "xdbg.h"
#include <QLabel>
#include <QtCore>
#include <QMenu>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <QInputDialog>
#include <QPlainTextEdit>
#include "filterdialog.h"
#include <QColor>

xdbg::xdbg(QSettings *Setting,QWidget *parent,int id) : QWidget(parent)
{
    sID = id;
    xSetting = Setting;
    xKey = "xdbg/" + QString::number(id) + "/";
    MayLayout = new QVBoxLayout(this);
    MayLayout->setSpacing(0);
    MayLayout->setMargin(0);

    log_view = new QPlainTextEdit();
    log_view->setReadOnly(true);
    log_view->setMaximumBlockCount(1000);
    log_view->setContextMenuPolicy( Qt::CustomContextMenu);
    connect(log_view,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(handel_logCustomContextMenuRequested(QPoint)));
    MayLayout->addWidget(log_view);
    connect(&ColorDialog,SIGNAL(colorSelected(QColor)),this,SLOT(handel_SetColor(QColor)));

    LoadSetting();

    QPalette pal = log_view->palette();
    pal.setColor(QPalette::Base, BackColor);
    pal.setColor(QPalette::Text, TextColor);
    pal.setColor(QPalette::Background, (xFilter.Enable) ? Qt::red:Qt::black);
    log_view->setPalette(pal);
}

void xdbg::LoadSetting()
{
    /*load dept*/
    int depth = xSetting->value(xKey + "depth").value<int>();
    if((depth&DepthSign) != DepthSign)
    {
        xSetting->setValue(xKey + "depth",(log_view->maximumBlockCount()|DepthSign) );
        xSetting->sync();
    }
    else
    {
        depth&=0xFFFFFF;
        log_view->setMaximumBlockCount(depth);
    }

    /*Load Id From Setting*/
    int xid = xSetting->value(xKey + "xid").value<int>();
    if((xid&IDSign) != IDSign)
    {
        xSetting->setValue(xKey + "xid",( sID | IDSign));
        xSetting->sync();
        xID = sID;
    }
    else
    {
        xID = xid & 0xFF;
    }

    /*load color setting*/
    BackColor = xSetting->value(xKey + "bgcolor").value<QColor>();
    if(!BackColor.isValid())
    {
        BackColor = Qt::white;
        xSetting->setValue(xKey + "bgcolor", BackColor);
        xSetting->sync();
    }

    TextColor = xSetting->value(xKey + "color").value<QColor>();
    if(!TextColor.isValid())
    {
        TextColor = Qt::black;
        xSetting->setValue(xKey + "color", TextColor);
        xSetting->sync();
    }

    /*load filter setting*/
    xFilter.Filter = xSetting->value(xKey + "filters").value<QString>();
    xFilter.Enable = xSetting->value(xKey + "filtere").value<bool>();
    xFilter.Blankfilter = xSetting->value(xKey + "filterb").value<bool>();
    Filters = xFilter.Filter.split("\n");
}

void xdbg::setID(int id)
{
    xID = id;
    xSetting->setValue(xKey + "xid",(xID| IDSign));
    xSetting->sync();
}

int xdbg::getID()
{
    return xID;
}

void xdbg::ResetSetting()
{
    xID = sID;
    BackColor = Qt::white;
    TextColor = Qt::black;

    QPalette pal = log_view->palette();
    pal.setColor(QPalette::Base, BackColor);
    pal.setColor(QPalette::Text, TextColor);
    log_view->setPalette(pal);

    xSetting->setValue(xKey + "xid",( sID | IDSign));
    xSetting->setValue(xKey + "bgcolor", BackColor);
    xSetting->setValue(xKey + "color", TextColor);
    xSetting->setValue(xKey + "depth", log_view->maximumBlockCount());
    xSetting->sync();
}

void xdbg::addItem(QString Item)
{
    if(xFilter.Enable)
    {
        if(!(xFilter.Blankfilter && Item.isEmpty()))
        {
            bool passfilter = false;
            foreach (const QString &fItem, Filters)
            {
                QString Rge = fItem;
                Rge = Rge.replace("\r","");
                if(Item.indexOf(QRegExp(Rge))!=-1)
                {
                    passfilter = true;
                    break;
                }
            }
            if(!passfilter)
                return;
        }
    }

    log_view->appendPlainText(Item);
}

void xdbg::clear()
{
    log_view->clear();
}

void xdbg::CopyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();
    QString Clip = log_view->toPlainText();
    clipboard->setText(Clip);
}


void xdbg::ShowColorDialog()
{
    ColorDialog.show();
    ColorDialog.setProperty("role","textcolor");
}

void xdbg::ShowBColorDialog()
{
    ColorDialog.show();
    ColorDialog.setProperty("role","background");
}

void xdbg::handel_SetColor(const QColor &color)
{
    if(ColorDialog.property("role").toString() == "background")
    {
        QPalette pal = log_view->palette();
        pal.setColor(QPalette::Base, color);
        log_view->setPalette(pal);
        BackColor = color;
        xSetting->setValue(xKey + "bgcolor", BackColor);
    }
    else
    {
        QPalette pal = log_view->palette();
        pal.setColor(QPalette::Text, color);
        log_view->setPalette(pal);
        TextColor = color;
        xSetting->setValue(xKey + "color", TextColor);
    }
    xSetting->sync();
}

void xdbg::handel_ChangeID()
{
    bool ok = false;
    int id = QInputDialog::getInt(this, tr("Id Selection"), tr("xID (0~15)"), xID,0,15,1, &ok);
    if (ok)
    {
     setID(id);
    }
}

void xdbg::handel_ChangeDepth()
{
    bool ok = false;
    int maxb = QInputDialog::getInt(this, tr("set maximum log depth"), tr("log depth (1~65536)"), log_view->maximumBlockCount(),1,65536,1, &ok);
    if (ok)
    {
        log_view->setMaximumBlockCount(maxb);
        xSetting->setValue(xKey + "depth",(maxb| DepthSign));
        xSetting->sync();
    }
}

void xdbg::handel_SetupFilter()
{
    FilterDialog Filter(xFilter.Enable,xFilter.Blankfilter,xFilter.Filter,this);
    Filter.exec();
    xFilter.Enable = Filter.GetEnable();
    xFilter.Filter = Filter.GetText();
    xFilter.Blankfilter = Filter.GetBlankFilter();
    Filters = xFilter.Filter.split("\n");

    QPalette pal = log_view->palette();
    pal.setColor(QPalette::Background, (xFilter.Enable) ? Qt::red:Qt::black);
    log_view->setPalette(pal);

    xSetting->setValue(xKey + "filters", xFilter.Filter);
    xSetting->setValue(xKey + "filtere", xFilter.Enable);
    xSetting->setValue(xKey + "filterb", xFilter.Blankfilter);
    xSetting->sync();
}

void xdbg::handel_logCustomContextMenuRequested( QPoint p )
{
    // Start with the standard menu.
    QMenu * pMenu = log_view->createStandardContextMenu();

    pMenu->clear();
    pMenu->addAction(QIcon(":/icon/clean30"),"Clear", this,SLOT(clear()));
    pMenu->addAction(QIcon(":/icon/copy30"),"Copy All", this,SLOT(CopyToClipboard()));
    pMenu->addAction(QIcon(":/icon/textcolor30"),"Text Color", this,SLOT(ShowColorDialog()));
    pMenu->addAction(QIcon(":/icon/fillcolor30"),"Background", this,SLOT(ShowBColorDialog()));
    pMenu->addAction(QIcon(":/icon/filter30"),"Filter", this,SLOT(handel_SetupFilter()));
    pMenu->addAction(QIcon(":/icon/tag30"),"Change ID", this,SLOT(handel_ChangeID()));
    pMenu->addAction(QIcon(":/icon/depth30"),"Log Depth", this,SLOT(handel_ChangeDepth()));

    // Show the menu.
    QPoint q = log_view->mapToGlobal( p );
    pMenu->exec( q );

    // The menu's ownership is transferred to me, so I must destroy it.
    delete pMenu;
}
