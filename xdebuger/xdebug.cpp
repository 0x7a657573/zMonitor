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
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QSpacerItem>
#include <QSplashScreen>
#include <QLineEdit>

Xdebuger::Xdebuger(QWidget *parent) : QWidget(parent)
{
    xSettings = new QSettings("config.ini",QSettings::IniFormat,this);
    //xSettings->setValue("USEIP",true);
    UseIP = xSettings->value("USEIP",false).toBool();

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

//    QByteArray me ;
//    me.append(StartInterPreter);
//    me.append(StartInterPreter+1);
//    me.append(tr("\x1B[31mHello \x1B[32mWorld").toLocal8Bit());
//    me.append(EndInterPreter);
//    ProsessIncomingData(me);
}

void Xdebuger::LoadMainView(QHBoxLayout *lay)
{
    QVBoxLayout *vLay = new QVBoxLayout();
    vLay->setSpacing(2);
    vLay->setContentsMargins(2,2,2,2);

    for(int i=0;i<4;i++)
    {
        QHBoxLayout *hLay = new QHBoxLayout();
        for(int j=0;j<4;j++)
        {
            xdbg *gdb = new xdbg(xSettings,this,i*4+j);
            hLay->addWidget(gdb);
            if(xSettings->value("view/"+QString::number(i*4+j)).toString() == "true")
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
    /*make socket Object*/
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()),this, SLOT(handel_tcp_connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(handel_tcp_disconnected()));
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),this, SLOT(handel_tcp_errorOccurred(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()),this, SLOT(handel_tcp_dataready()));

    /*Connect btn*/
     btnConnect = new QPushButton(this);
     btnConnect->setAutoFillBackground(true);
     btnConnect->setIcon(QIcon(":/icon/disconnect30"));
     btnConnect->setIconSize(QSize(20,20));
     btnConnect->setFixedSize(30,30);
     btnConnect->setToolTip(tr("Try Connect To Port"));
     if(UseIP)
     {
        connect(btnConnect,SIGNAL(clicked()),this,SLOT(handel_ConDisConAction_UseIP()));
     }
     else
     {
        connect(btnConnect,SIGNAL(clicked()),this,SLOT(handel_ConDisConAction()));
     }

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

        /*Reset Setting btn*/
     QPushButton *btnResetSetting = new QPushButton(this);
        btnResetSetting->setAutoFillBackground(true);
        btnResetSetting->setIcon(QIcon(":/icon/restart30"));
        btnResetSetting->setIconSize(QSize(20,20));
        btnResetSetting->setFixedSize(30,30);
        btnResetSetting->setToolTip(tr("Reset All Setting"));
        connect(btnResetSetting,SIGNAL(clicked()),this,SLOT(handel_ResetSetting()));

     /*About me btn*/
     QPushButton *btnAboutme = new QPushButton(this);
        btnAboutme->setAutoFillBackground(true);
        btnAboutme->setIcon(QIcon(":/icon/info30"));
        btnAboutme->setIconSize(QSize(20,20));
        btnAboutme->setFixedSize(30,30);
        btnAboutme->setToolTip(tr("About Me"));
        connect(btnAboutme,SIGNAL(clicked()),this,SLOT(handel_AboutMe()));

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
                if(xSettings->value("view/"+QString::number(i*4+j)).toString() == "true")
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

    /*Host input For IP:Port*/
    HostEdit = new QLineEdit(this);
    HostEdit->setMaximumWidth(200);
    HostEdit->setFixedHeight(30);
    HostEdit->setToolTip(tr("Enter Ip:Port for connect"));
    HostEdit->setText("127.0.0.1:7666");

     /*QINput Data To Esn*/
     CmdInput = new QLineEdit();
     CmdInput->setMaximumWidth(200);
     CmdInput->setFixedHeight(30);
     CmdInput->setToolTip(tr("Enter text/cmd to send from serial"));
     CmdInput->setEnabled(false);

     btnSend = new QPushButton(this);
     btnSend->setAutoFillBackground(true);
     btnSend->setIcon(QIcon(":/icon/esend"));
     btnSend->setIconSize(QSize(20,20));
     btnSend->setFixedSize(30,30);
     btnSend->setToolTip(tr("Send From Serial"));
     btnSend->setEnabled(false);
     connect(btnSend,SIGNAL(clicked()),this,SLOT(handel_Send()));

     QSpacerItem *SendSpitem = new QSpacerItem(10,0, QSizePolicy::Fixed, QSizePolicy::Minimum);

     /*spacer*/
     QSpacerItem *Spitem = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum);

     if(UseIP)
     {
        lay->addWidget(HostEdit);

        xPort->hide();
        xBaud->hide();
        btnRefresh->hide();
     }
     else
     {
        lay->addWidget(xPort);
        lay->addWidget(xBaud);
        lay->addWidget(btnRefresh);

        HostEdit->hide();
     }

     lay->addWidget(btnConnect);
     lay->addWidget(btnClean);
     lay->addSpacerItem(SendSpitem);
     lay->addWidget(CmdInput);
     lay->addWidget(btnSend);

     lay->addSpacerItem(Spitem);
     lay->addWidget(btnView);
     lay->addWidget(btnResetSetting);
     lay->addWidget(btnAboutme);


     TopLay->setAlignment(Qt::AlignLeft);
}

void Xdebuger::LockPortOpen(bool Look)
{
    xPort->setEnabled(Look);
    xBaud->setEnabled(Look);
    btnRefresh->setEnabled(Look);
    StatusStatisticsLbl->setVisible(!Look);
    btnSend->setEnabled(!Look);
    CmdInput->setEnabled(!Look);
}

/*connect from IP*/
void Xdebuger::handel_tcp_connected()
{
    TotlaRx = 0;
    btnConnect->setIcon(QIcon(":/icon/connect30"));
    btnConnect->setIconSize(QSize(20,20));
    Port_IsOpen = true;
    LockPortOpen(false);

    /*Update Status Bar*/
    StatusMessageLbl->setText(QString("Connected {%1}").arg(HostEdit->text()));
    StatusIconLbl->setPixmap(QPixmap(":/icon/connect"));
    Timer->start(500);
    btnConnect->setEnabled(true);
}

void Xdebuger::handel_tcp_errorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    Port_IsOpen = false;
    btnConnect->setIcon(QIcon(":/icon/disconnect30"));
    btnConnect->setIconSize(QSize(20,20));
    LockPortOpen(true);

    /*Status Bar Update*/
    StatusMessageLbl->setText("Disconnect");
    StatusIconLbl->setPixmap(QPixmap(":/icon/disconnect"));
    Timer->stop();
    btnConnect->setEnabled(true);

    QMessageBox::critical(this, tr("Error"), socket->errorString());
}

void Xdebuger::handel_tcp_disconnected()
{
    Port_IsOpen = false;
    btnConnect->setIcon(QIcon(":/icon/disconnect30"));
    btnConnect->setIconSize(QSize(20,20));
    LockPortOpen(true);

    /*Status Bar Update*/
    StatusMessageLbl->setText("Disconnect");
    StatusIconLbl->setPixmap(QPixmap(":/icon/disconnect"));
    Timer->stop();
    btnConnect->setEnabled(true);
}


void Xdebuger::handel_ConDisConAction_UseIP()
{
    QString Host = tr("127.0.0.1");//HostEdit->text();
    uint16_t Port = 7666;

    if(Port_IsOpen==false) /*we Need Open Port*/
    {
        btnConnect->setEnabled(false);
        // Try Connect to Server
        socket->connectToHost(Host, Port);
    }
    else
    {

        /*If Port Is Open Try Close It*/
        if(socket->isOpen())
        {
            btnConnect->setEnabled(false);
            socket->close();
        }
    }
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

void Xdebuger::handel_tcp_dataready()
{
    QByteArray data = socket->readAll();

    ProsessIncomingData(data);
}

void Xdebuger::handel_DataReady()
{
    QByteArray data = sPort->readAll();

    ProsessIncomingData(data);
}

void Xdebuger::ProsessIncomingData(QByteArray &data)
{

    // mesage format : <0xC8><ID>TEXT....<0xFA>
    TotlaRx+=data.length();
    static QByteArray StaticData;
    StaticData.append(data);

    int Start = StaticData.indexOf(StartInterPreter);
    int End   = StaticData.lastIndexOf(EndInterPreter);

    if(Start==End || End==-1)
        return;

    End++;
    QByteArray Prosess_Data = StaticData.mid(Start,(End-Start));
    QByteArray Remind_Data = StaticData.mid(End);
    //qDebug() << QString::fromStdString(Prosess_Data.toStdString()) << QString::fromStdString(Remind_Data.toStdString()) ;
    StaticData.clear();
    StaticData.append(Remind_Data);

    QList<QByteArray> list = Prosess_Data.split(StartInterPreter);
    foreach( QByteArray item, list )
    {
        if(item.length()<2) /*ID + EndMark*/
            continue;
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
    foreach (xdbg* item, ViewList)
    {
        if(item->getID()==Id)
        {
            item->addItem(text);
        }
    }
}

void Xdebuger::handel_clearLogs()
{
    TotlaRx = 0;
    foreach( xdbg* item, ViewList )
    {
        item->clear();
    }
}

void Xdebuger::handel_ResetSetting()
{
    int result = QMessageBox::question(this,tr("are you sure !!!"),tr("Are you sure want to reset your setting ?"),QDialogButtonBox::Yes,QDialogButtonBox::No);
    if(result != QDialogButtonBox::Yes)
        return;

    foreach( xdbg* item, ViewList )
    {
        item->ResetSetting();
    }

    QMessageBox::information(this,tr("Reset setting"),tr("Reset All Setting."),QDialogButtonBox::Ok);
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
    xSettings->setValue("view/"+xbtn->property("id").toString(),newstatus ? "true":"false");
}

void Xdebuger::handel_AboutMe()
{
    QString translatedTextAboutQtCaption;
        translatedTextAboutQtCaption = QMessageBox::tr(
            "<h3>zMonitor</h3>"
            "<p>This program Publish under GPL3 license</p>"
            "<p>This program uses Qt version %1.</p>"
            ).arg(QLatin1String(QT_VERSION_STR));

        QString translatedTextAboutQtText;
        translatedTextAboutQtText = QMessageBox::tr(
            "<p>Website: <a href=\"http://%1/\">%1</a>.</p>"
            "<p>Git: <a href=\"http://%2/\">GitHub</a>.</p>"
            ""
            ).arg(QLatin1String("0x7a657573.com"),
                  QLatin1String("github.com/0x7a657573/zMonitor"));

        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setAttribute(Qt::WA_DeleteOnClose);
        msgBox->setWindowTitle("About Me");
        msgBox->setIconPixmap(QPixmap(":/icon/about"));
        msgBox->setText(translatedTextAboutQtCaption);
        msgBox->setInformativeText(translatedTextAboutQtText);
        msgBox->exec();

}

void Xdebuger::handel_Send()
{
    QString data = CmdInput->text() + "\r\n";
    QByteArray sendarray = data.toUtf8();

    if(UseIP)
    {
        socket->write(sendarray);
        if(socket->bytesToWrite() > 0)
            socket->flush();
    }
    else
    {
        sPort->write(sendarray);
        if ( sPort->bytesToWrite() > 0)
        {
             sPort->flush();
        }
    }
}

Xdebuger::~Xdebuger()
{

}

