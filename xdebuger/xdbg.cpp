#include "xdbg.h"
#include <QLabel>
#include <QtCore>
#include <QMenu>
#include <QPushButton>
#include "mylistwidget.h"
#include <QClipboard>
#include <QApplication>
#include <QInputDialog>
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

    ListView = new MyListWidget();
    //ListView->setStyleSheet("background-color:black;color:red;");

    connect(ListView,SIGNAL(xmousePressEvent(QMouseEvent*)),this,SLOT(handel_mousePressEvent(QMouseEvent*)));

    MayLayout->addWidget(ListView);

    MaxRow = 1000;

    PopMenu = new QMenu(this);
    PopMenu->addAction(QIcon(":/icon/clean30"),"Clear", this,SLOT(clear()));
    PopMenu->addAction(QIcon(":/icon/copy30"),"Copy All", this,SLOT(CopyToClipboard()));
    PopMenu->addAction(QIcon(":/icon/textcolor30"),"Text Color", this,SLOT(ShowColorDialog()));
    PopMenu->addAction(QIcon(":/icon/fillcolor30"),"Background", this,SLOT(ShowBColorDialog()));
    PopMenu->addAction(QIcon(":/icon/filter30"),"Filter", this,SLOT(handel_SetupFilter()));
    PopMenu->addAction(QIcon(":/icon/tag30"),"Change ID", this,SLOT(handel_ChangeID()));

    connect(&ColorDialog,SIGNAL(colorSelected(QColor)),this,SLOT(handel_SetColor(QColor)));

    LoadSetting();

    QPalette pal = ListView->palette();
    pal.setColor(QPalette::Base, BackColor);
    pal.setColor(QPalette::Text, TextColor);
    pal.setColor(QPalette::Background, (xFilter.Enable) ? Qt::red:Qt::black);
    ListView->setPalette(pal);
}

void xdbg::LoadSetting()
{
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

    QPalette pal = ListView->palette();
    pal.setColor(QPalette::Base, BackColor);
    pal.setColor(QPalette::Text, TextColor);
    ListView->setPalette(pal);

    xSetting->setValue(xKey + "xid",( sID | IDSign));
    xSetting->setValue(xKey + "bgcolor", BackColor);
    xSetting->setValue(xKey + "color", TextColor);
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

    QListWidgetItem *newItem= new QListWidgetItem();
    newItem->setData(Qt::UserRole, QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"));
    newItem->setText(Item);

    //newItem->setBackgroundColor(BackColor);
    //newItem->setTextColor(TextColor);

    ListView->addItem(newItem);

    if(ListView->count()>MaxRow)
    {
        ListView->removeItemWidget(ListView->takeItem(0));
    }
    ListView->setCurrentItem(ListView->item(ListView->count()-1));
}

void xdbg::clear()
{
    ListView->clear();
}

void xdbg::CopyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();

    int count = ListView->count();
    QString Clip = "";
    for(int i=0;i<count;i++)
    {
        QListWidgetItem *item = ListView->item(i);
        Clip += item->data(Qt::UserRole).toString() + "\t" + item->text() + "\r\n";
    }
    clipboard->setText(Clip);
}

void xdbg::handel_mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(event->button()==Qt::RightButton)
    {
        PopMenu->popup(this->mapToGlobal(event->pos()));
    }
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
        QPalette pal = ListView->palette();
        pal.setColor(QPalette::Base, color);
        ListView->setPalette(pal);
        BackColor = color;
        xSetting->setValue(xKey + "bgcolor", BackColor);
    }
    else
    {
        QPalette pal = ListView->palette();
        pal.setColor(QPalette::Text, color);
        ListView->setPalette(pal);
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

void xdbg::handel_SetupFilter()
{
    FilterDialog Filter(xFilter.Enable,xFilter.Blankfilter,xFilter.Filter,this);
    Filter.exec();
    xFilter.Enable = Filter.GetEnable();
    xFilter.Filter = Filter.GetText();
    xFilter.Blankfilter = Filter.GetBlankFilter();
    Filters = xFilter.Filter.split("\n");

    QPalette pal = ListView->palette();
    pal.setColor(QPalette::Background, (xFilter.Enable) ? Qt::red:Qt::black);
    ListView->setPalette(pal);

    xSetting->setValue(xKey + "filters", xFilter.Filter);
    xSetting->setValue(xKey + "filtere", xFilter.Enable);
    xSetting->setValue(xKey + "filterb", xFilter.Blankfilter);
    xSetting->sync();
}

//void xdbg::load_filter()
//{

//}
