#include "xdbg.h"
#include <QLabel>
#include <QtCore>
#include <QMenu>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include "filterdialog.h"
#include <QColor>


xdbg::xdbg(QSettings *Setting,QWidget *parent,int id) : QWidget(parent)
{
    sID = id;
    xSetting = Setting;
    xKey = "xdbg/" + QString::number(id) + "/";
    MayLayout = new QVBoxLayout(this);
    MayLayout->setSpacing(0);
    MayLayout->setContentsMargins(0,0,0,0);

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
    //pal.setColor(QPalette::Background, (xFilter.Enable) ? Qt::red:Qt::black);
    log_view->setPalette(pal);

    //addItem("\x1B[31mHello \x1B[32mWorld");
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


QString xdbg::ConverANSIEscapeToHtml(QString AES,QString line)
{
    //See https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
    enum AnsiEscapeCodes
    {
        ResetFormat = 0,
        BoldText = 1,
        TextColorStart = 30,
        TextColorEnd = 37,
        RgbTextColor = 38,
        DefaultTextColor = 39,
        BackgroundColorStart = 40,
        BackgroundColorEnd = 47,
        RgbBackgroundColor = 48,
        DefaultBackgroundColor = 49
    };

    QStringList format = AES.mid(2,AES.length()-3).split(';');
    QList<int>  numbers;
    foreach(QString num, format)
        numbers.append(num.toInt());


    for (int i = 0; i < numbers.size(); ++i)
    {
        const uint code = numbers.at(i);

        if (code >= TextColorStart && code <= TextColorEnd)
        {
            int  GCode = code - TextColorStart;
            const int red   = GCode & 1 ? 170 : 0;
            const int green = GCode & 2 ? 170 : 0;
            const int blue  = GCode & 4 ? 170 : 0;
            Escape_fColor = QColor(red,green,blue).name();
        }
        else if (code >= BackgroundColorStart && code <= BackgroundColorEnd)
        {
            int  GCode = code - TextColorStart;
            const int red   = GCode & 1 ? 170 : 0;
            const int green = GCode & 2 ? 170 : 0;
            const int blue  = GCode & 4 ? 170 : 0;
            Escape_bColor = QColor(red,green,blue).name();
        }
        else
        {
            switch (code)
            {
                case ResetFormat:       break;
                case BoldText:          break;
                case DefaultTextColor:
                    Escape_fColor = "";
                break;
                case DefaultBackgroundColor:
                    Escape_bColor = "";
                break;
                case RgbTextColor:
                case RgbBackgroundColor:
                // See http://en.wikipedia.org/wiki/ANSI_escape_code#Colors
                if (++i >= numbers.size())
                     break;
                switch (numbers.at(i))
                {
                    case 2:
                    // RGB set with format: 38;2;<r>;<g>;<b>
                        if ((i + 3) < numbers.size())
                        {
                            QString sColor =  QColor(numbers.at(i + 1),
                                                     numbers.at(i + 2),
                                                     numbers.at(i + 3)).name();
                            Escape_fColor =  (code == RgbTextColor) ? sColor:Escape_fColor;
                            Escape_bColor =  (code == RgbTextColor) ? Escape_bColor:sColor;
                        }
                        i += 3;
                        break;

                    case 5:
                    // 256 color mode with format: 38;5;<i>
                        uint index = numbers.at(i + 1);

                        QColor color;
                        if (index < 8)
                        {
                            // The first 8 colors are standard low-intensity
                            // ANSI colors.
                            const int red   = index & 1 ? 170 : 0;
                            const int green = index & 2 ? 170 : 0;
                            const int blue  = index & 4 ? 170 : 0;
                            color =  QColor(red,green,blue);
                        }
                        else if (index < 16)
                        {
                            // The next 8 colors are standard high-intensity
                            // ANSI colors.
                            const int red   = (index - 8) & 1 ? 170 : 0;
                            const int green = (index - 8) & 2 ? 170 : 0;
                            const int blue  = (index - 8) & 4 ? 170 : 0;
                            color = QColor(red,green,blue).lighter(150);
                        }
                        else if (index < 232)
                        {
                            // The next 216 colors are a 6x6x6 RGB cube.
                            uint o = index - 16;
                            color = QColor((o / 36) * 51, ((o / 6) % 6) * 51,
                                           (o % 6) * 51);
                        }
                        else
                        {
                            // The last 24 colors are a greyscale gradient.
                            int grey = int((index - 232) * 11);
                            color = QColor(grey, grey, grey);
                        }

                        if (code == RgbTextColor)
                            Escape_fColor = color.name();
                        else
                            Escape_bColor = color.name();
                        ++i;
                        break;
                }
                break;
            default:
                break;
            }
        }
    }

    QString Export = "<span style=\"";
    if(Escape_fColor.length())
        Export += "color:"+Escape_fColor;
    if(Escape_bColor.length())
        Export += ";background-color:"+Escape_bColor;
    Export+="\">"+ line +"</span>";

    return Export;
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
                if(Item.indexOf(QRegularExpression(Rge))!=-1)
                {
                    passfilter = true;
                    break;
                }
            }
            if(!passfilter)
                return;
        }
    }

    QString ANSI_EscapeMark  = "\x1B";
    /*check input string for Escape command's*/
    if(Item.indexOf(ANSI_EscapeMark)<0)
    {
        log_view->appendPlainText(Item);
        return;
    }

    /*reset Escape Color*/
    Escape_fColor = "";
    Escape_bColor = "";

    /*Try parss ANSI Escape Sequences*/
    QString ExportHTML =  "";
    QString rgxe  =  "\x1b\[[0-9;]*m";
    ec.setPattern(rgxe);
    int pos = 0;
    do
    {
        const QRegularExpressionMatch match = ec.match(Item,pos);
        pos += match.capturedLength();
        int  Next = Item.indexOf(ANSI_EscapeMark,pos);
        QString PlainText = Item.mid(pos,(Next>0) ? Next-pos:-1);
        QString EscapeCode = match.captured(0);
        pos = Next;
        ExportHTML += ConverANSIEscapeToHtml(EscapeCode,PlainText);
    }while(pos!=-1);

    log_view->appendHtml(ExportHTML);
}

void xdbg::clear()
{
    log_view->clear();
}

void xdbg::CopyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();
    QString Clip = log_view->textCursor().selectedText();
    clipboard->setText(Clip);
}

void xdbg::CopyAllToClipboard()
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
    //pal.setColor(QPalette::Background, (xFilter.Enable) ? Qt::red:Qt::black);
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

    if(log_view->textCursor().selectedText().length())
        pMenu->addAction(QIcon(":/icon/copy30"),"Copy", this,SLOT(CopyToClipboard()));

    pMenu->addAction(QIcon(":/icon/copy30"),"Copy All", this,SLOT(CopyAllToClipboard()));
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
