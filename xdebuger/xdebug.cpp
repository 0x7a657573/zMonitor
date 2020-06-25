#include "xdebug.h"
#include <QDebug>
#include <stdint.h>
#include <QWidget>
#include <QObject>
#include <QtCore>
#include <QString>
#include <QPushButton>
#include <QtCore>
#include <QMessageBox>
#include <QMenu>
#include <QAction>


Xdebuger::Xdebuger(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);

    Layout = new QVBoxLayout(this);
    MainLay = new QHBoxLayout();
    TopLay = new QHBoxLayout();
    BottomLay = new QHBoxLayout();

    /*Load ToolBar*/
    LoadToolBar(TopLay);

    /*LoadMainView*/
    LoadMainView(MainLay);

    /*Load StatusBar*/
    LoadStatusBar(BottomLay);

    Layout->addLayout(TopLay);
    Layout->addLayout(MainLay);
    Layout->addLayout(BottomLay);

    /*Load Serial Port*/
    sPort = new QSerialPort(this);
    connect(sPort, SIGNAL(readyRead()), this, SLOT(handel_DataReady()));
    Port_IsOpen = false;

    handel_LoadSerialPort();

    /*Timer Update Pramiter*/
    Timer = new QTimer(this);
    connect(Timer,SIGNAL(timeout()) , this, SLOT(handel_TimerUpdate()));
}

void Xdebuger::LoadMainView(QHBoxLayout *lay)
{
    QVBoxLayout *vLay = new QVBoxLayout();
    vLay->setSpacing(2);
    vLay->setMargin(2);
    for(int i=0;i<4;i++)
    {
        QHBoxLayout *hLay = new QHBoxLayout();
        for(int j=0;j<4;j++)
        {
            xdbg *gdb = new xdbg(this);
            hLay->addWidget(gdb);
            if(xSettings.value("view/"+QString::number(i*4+j)).toString() == "true")
            {
                gdb->setHidden(true);
            }
            ViewList.append(gdb);
        }
        vLay->addLayout(hLay);
    }

    lay->addLayout(vLay);
}

void Xdebuger::LoadStatusBar(QHBoxLayout *lay)
{
    Status = new QStatusBar(this);

    StatusIconLbl = new QLabel;
    StatusIconLbl->setPixmap(QPixmap(":/icon/disconnect"));

    StatusMessageLbl  = new QLabel;
    StatusMessageLbl->setText(tr("Disconnect"));

    StatusStatisticsLbl = new QLabel;

    Status->addWidget(StatusIconLbl);
    Status->addWidget(StatusMessageLbl);
    Status->addWidget(StatusStatisticsLbl);
    lay->addWidget(Status);
}

void Xdebuger::LoadToolBar(QHBoxLayout *lay)
{
    /*Connect btn*/
     btnConnect = new QPushButton(this);
     btnConnect->setAutoFillBackground(true);
     btnConnect->setIcon(QIcon(":/icon/disconnect30"));
     btnConnect->setIconSize(QSize(20,20));
     btnConnect->setFixedSize(30,30);
     btnConnect->setToolTip(tr("Try Connect To Port"));
     connect(btnConnect,SIGNAL(clicked()),this,SLOT(handel_ConDisConAction()));

     /*clean btn*/
     QPushButton *btnClean = new QPushButton(this);
          btnClean->setAutoFillBackground(true);
          btnClean->setIcon(QIcon(":/icon/clean30"));
          btnClean->setIconSize(QSize(20,20));
          btnClean->setFixedSize(30,30);
          btnClean->setToolTip(tr("clear All log's"));
          connect(btnClean,SIGNAL(clicked()),this,SLOT(handel_clearLogs()));

     /*view btn*/
     QPushButton *btnView = new QPushButton(this);
        btnView->setAutoFillBackground(true);
        btnView->setIcon(QIcon(":/icon/view30"));
        btnView->setIconSize(QSize(20,20));
        btnView->setFixedSize(30,30);
        btnView->setToolTip(tr("Enable/Disable Log View"));
        //connect(btnClean,SIGNAL(clicked()),this,SLOT(handel_clearLogs()));

        QMenu *menu = new QMenu(this);
        QVBoxLayout *xlayout = new QVBoxLayout(menu);

        for(int i=0;i<4;i++)
        {
            QHBoxLayout *ylayout = new QHBoxLayout();
            for(int j=0;j<4;j++)
            {
                QPushButton *xbtn = new QPushButton();
                xbtn->setFixedSize(25,25);

                xbtn->setCheckable(true);
                xbtn->setProperty("id", QString::number(i*4+j));
                if(xSettings.value("view/"+QString::number(i*4+j)).toString() == "true")
                {
                    xbtn->setIcon(QIcon(":/icon/delete30"));
                    xbtn->setIconSize(QSize(16,16));
                    xbtn->setChecked(true);
                }
                else
                {
                    xbtn->setIcon(QIcon(":/icon/check30"));
                    xbtn->setIconSize(QSize(16,16));
                }

                connect(xbtn,SIGNAL(toggled(bool)),this,SLOT(handel_viewChange(bool)));
                ylayout->addWidget(xbtn);
            }
            xlayout->addLayout(ylayout);
        }

        btnView->setMenu(menu);
        btnView->setStyleSheet("QPushButton::menu-indicator {width:0px;}");

    /*Tools Bar*/
     btnRefresh = new QPushButton(this);
     btnRefresh->setAutoFillBackground(true);
     btnRefresh->setIcon(QIcon(":/icon/update"));
     btnRefresh->setIconSize(QSize(20,20));
     btnRefresh->setFixedSize(30,30);
     btnRefresh->setToolTip(tr("Refresh Port List"));
     connect(btnRefresh,SIGNAL(clicked()),this,SLOT(handel_LoadSerialPort()));

     /*Serial Port List*/
     xPort = new QComboBox(this);
     xPort->setFixedHeight(30);
     xPort->setToolTip(tr("Select Serial Port"));
     connect(xPort,SIGNAL (currentIndexChanged(int)),this,SLOT(handel_Combo_PortChange(int)));

    /*Baud Rate*/
     int Bauds[] = {2400,4800,9600,19200,38400,57600,115200,230400,460800,921600};
     xBaud = new QComboBox(this);
     xBaud->setFixedHeight(30);
     xBaud->setToolTip(tr("set BaudRate"));
     for(long unsigned int i=0;i<(sizeof(Bauds)/sizeof(int));i++)
     {
       xBaud->addItem( QString::number(Bauds[i]) );
     }
     xBaud->setCurrentIndex(6); /*Set in 115200*/

     lay->addWidget(xPort);
     lay->addWidget(xBaud);
     lay->addWidget(btnRefresh);
     lay->addWidget(btnConnect);
     lay->addWidget(btnClean);
     lay->addWidget(btnView);
     TopLay->setAlignment(Qt::AlignLeft);
}

void Xdebuger::LockPortOpen(bool Look)
{
    xPort->setEnabled(Look);
    xBaud->setEnabled(Look);
    btnRefresh->setEnabled(Look);
    StatusStatisticsLbl->setVisible(!Look);
}

/*Btn Connect / DisConnect Action*/
void Xdebuger::handel_ConDisConAction()
{
    if(Port_IsOpen==false) /*we Need Open Port*/
    {
        /*Config Port Paramiter*/
        sPort->setBaudRate(xBaud->currentText().toInt());
        sPort->setPortName(xPort->currentText());

        /*try Open Port*/
        if(sPort->open(QIODevice::ReadWrite)==true)
        {
            TotlaRx = 0;
            btnConnect->setIcon(QIcon(":/icon/connect30"));
            btnConnect->setIconSize(QSize(20,20));
            Port_IsOpen = true;
            LockPortOpen(false);

            /*Update Status Bar*/
            StatusMessageLbl->setText(QString("Connected {%1 @ %2}").arg(xPort->currentText(), xBaud->currentText()));
            StatusIconLbl->setPixmap(QPixmap(":/icon/connect"));
            Timer->start(500);
        }
        else
        {
             QMessageBox::critical(this, tr("Error"), sPort->errorString());
        }
    }
    else
    {
        /*If Port Is Open Try Close It*/
        if(sPort->isOpen())
        {
            Port_IsOpen = false;
            sPort->close();
            btnConnect->setIcon(QIcon(":/icon/disconnect30"));
            btnConnect->setIconSize(QSize(20,20));
            LockPortOpen(true);

            /*Status Bar Update*/
            StatusMessageLbl->setText("Disconnect");
            StatusIconLbl->setPixmap(QPixmap(":/icon/disconnect"));
            Timer->stop();
        }
    }
}

void Xdebuger::handel_LoadSerialPort()
{
    xPort->clear();
    int i=0;
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
    {
        xPort->addItem(port.portName());
        if(i==0) xPort->setToolTip((port.description()=="") ? "None":port.description());
        xPort->setItemData(i++,(port.description()=="") ? "None":port.description(),Qt::ToolTipRole);
    }
}

void Xdebuger::handel_Combo_PortChange(int index)
{
    QString descrip = QVariant(xPort->itemData(index,Qt::ToolTipRole)).toString();
    xPort->setToolTip(descrip);
    //qDebug() << index << descrip;
}

void Xdebuger::handel_DataReady()
{
    QByteArray data = sPort->readAll();

    ProsessIncomingData(data);
}

void Xdebuger::ProsessIncomingData(QByteArray &data)
{
    static QByteArray StaticData;
    TotlaRx+=data.length();
    StaticData.append(data);

    int Start = StaticData.indexOf(StartInterPreter);
    int End   = StaticData.lastIndexOf(EndInterPreter);

    if(Start==End || End==-1)
        return;

    QByteArray Prosess_Data = StaticData.mid(Start,(End-Start));
    QByteArray Remind_Data = StaticData.mid(End);
    StaticData.clear();
    StaticData.append(Remind_Data);

    QList<QByteArray> list = Prosess_Data.split(StartInterPreter);
    foreach( QByteArray item, list )
    {
        QString Text = QString::fromStdString(item.mid(1,item.indexOf(EndInterPreter)-1).toStdString());
        int id = (uint8_t)item[0];
        if(id<=StartInterPreter) continue;
        id-=StartInterPreter;
        IncertDataRow((id),Text);
    }
}

void Xdebuger::IncertDataRow(int Id,QString &text)
{
    Id--; /*Start From 0*/
    //qDebug() << Id << text;
    if(ViewList.count()>Id)
        ViewList[Id]->addItem(text);
}

void Xdebuger::handel_clearLogs()
{
    TotlaRx = 0;
    foreach( xdbg* item, ViewList )
    {
        item->clear();
    }
}

void Xdebuger::handel_TimerUpdate()
{
   StatusStatisticsLbl->setText(QString("TotalRx:%2").arg(TotlaRx));
}

void Xdebuger::handel_viewChange(bool newstatus)
{
    QPushButton *xbtn = (QPushButton *)sender();
    if(newstatus==false)
    {
        xbtn->setIcon(QIcon(":/icon/check30"));
    }
    else
    {
        xbtn->setIcon(QIcon(":/icon/delete30"));
    }
    xbtn->setIconSize(QSize(16,16));

    ViewList[xbtn->property("id").toInt()]->setHidden(newstatus);
    xSettings.setValue("view/"+xbtn->property("id").toString(),newstatus ? "true":"false");
}

Xdebuger::~Xdebuger()
{

}

