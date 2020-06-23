#ifndef XDEBUGR_H
#define XDEBUGR_H

#include <stdint.h>
#include <QWidget>
#include <QObject>
#include <QtCore>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QComboBox>

class Xdebuger : public QWidget
{
    Q_OBJECT

public:
    explicit Xdebuger(QWidget *parent=0);
    virtual ~Xdebuger();

private slots:
    void handel_Combo_PortChange(int index);

private:
    void LoadToolBar(QHBoxLayout *lay);
    void LoadSerialPort();

protected:
    /*Main Layout*/
    QVBoxLayout *Layout;
    QHBoxLayout *TopLay;
    QHBoxLayout *MainLay;
    QHBoxLayout *BottomLay;

    /*Serial Port*/
    QComboBox *xPort;
    QComboBox *xBaud;

};

#endif // XDEBUG_H
