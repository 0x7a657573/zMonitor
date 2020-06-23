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
}

void Xdebuger::LoadMainView(QHBoxLayout *lay)
{
    MainEd =new  QTextEdit();
    Listvu =new QListView(this);

    lay->addWidget(MainEd);
    lay->addWidget(Listvu);
}

void Xdebuger::LoadStatusBar(QHBoxLayout *lay)
{
    Status = new QStatusBar(this);

    StatusIconLbl = new QLabel;
    StatusIconLbl->setPixmap(QPixmap(":/icon/disconnect"));

    StatusMessageLbl  = new QLabel;
    StatusMessageLbl->setText(tr("Disconnect"));

    Status->addWidget(StatusIconLbl);
    Status->addWidget(StatusMessageLbl);
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
     TopLay->setAlignment(Qt::AlignLeft);
}

void Xdebuger::LockPortOpen(bool Look)
{
    xPort->setEnabled(Look);
    xBaud->setEnabled(Look);
    btnRefresh->setEnabled(Look);
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
            btnConnect->setIcon(QIcon(":/icon/connect30"));
            btnConnect->setIconSize(QSize(20,20));
            Port_IsOpen = true;
            LockPortOpen(false);

            /*Update Status Bar*/
            StatusMessageLbl->setText(QString("Connected {%1 @ %2}").arg(xPort->currentText(), xBaud->currentText()));
            StatusIconLbl->setPixmap(QPixmap(":/icon/connect"));
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
    const QByteArray data = sPort->readAll();
    MainEd->insertPlainText(QString::fromStdString(data.toStdString()));
    sPort->write(data);
}

Xdebuger::~Xdebuger()
{

}

