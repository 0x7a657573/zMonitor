#include "xdebug.h"
#include <QDebug>
#include <stdint.h>
#include <QWidget>
#include <QObject>
#include <QtCore>
#include <QString>
#include <QPushButton>
#include <QtCore>

Xdebuger::Xdebuger(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);

    Layout = new QVBoxLayout(this);
    MainLay = new QHBoxLayout();
    TopLay = new QHBoxLayout();
    BottomLay = new QHBoxLayout();

    /*Load ToolBar*/
    LoadToolBar(TopLay);

    Layout->addLayout(TopLay);
    Layout->addLayout(MainLay);
    Layout->addLayout(BottomLay);

    /*Load Serial Port*/
    LoadSerialPort();
}

void Xdebuger::LoadToolBar(QHBoxLayout *lay)
{
    /*Tools Bar*/
    QPushButton *btnZoomin = new QPushButton(this);
     btnZoomin->setToolTip("Zoom in");
     btnZoomin->setAutoFillBackground(true);
     //btnZoomin->setIcon(QIcon(":/Icon/Zoomin"));
     //btnZoomin->setIconSize(QSize(30,30));
     btnZoomin->setFixedSize(40,40);
     //btnZoomin->setFlat(true);

     /*Serial Port List*/
     xPort = new QComboBox(this);
     xPort->setToolTip(tr("Select Serial Port"));
     connect(xPort,SIGNAL (currentIndexChanged(int)),this,SLOT(handel_Combo_PortChange(int)));

    /*Baud Rate*/
     int Bauds[] = {2400,4800,9600,19200,38400,57600,115200,230400,460800,921600};
     xBaud = new QComboBox(this);
     xBaud->setToolTip(tr("set BaudRate"));
     for(long i=0;i<sizeof(Bauds)/sizeof(int);i++)
     {
       xBaud->addItem( QString::number(Bauds[i]) );
     }
     xBaud->setCurrentIndex(6); /*Set in 115200*/


     lay->addWidget(xPort);
     lay->addWidget(xBaud);
     lay->addWidget(btnZoomin);
     TopLay->setAlignment(Qt::AlignLeft);
}

void Xdebuger::LoadSerialPort()
{
    xPort->clear();
    int i=0;
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
    {
        //   qDebug() << (port.portName()) << port.description() << port.manufacturer();
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


Xdebuger::~Xdebuger()
{

}

