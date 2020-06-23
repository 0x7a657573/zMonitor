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
#include <QPushButton>
#include <QStatusBar>
#include <QLabel>
#include <QTextEdit>
#include <QListView>

class Xdebuger : public QWidget
{
    Q_OBJECT

public:
    explicit Xdebuger(QWidget *parent=0);
    virtual ~Xdebuger();

private slots:
    void handel_Combo_PortChange(int index);
    void handel_LoadSerialPort();
    void handel_ConDisConAction();
    void handel_DataReady();

private:
    void LoadToolBar(QHBoxLayout *lay);
    void LoadMainView(QHBoxLayout *lay);
    void LoadStatusBar(QHBoxLayout *lay);
    void LockPortOpen(bool Look);

protected:
    /*Main Layout*/
    QVBoxLayout *Layout;
    QHBoxLayout *TopLay;
    QHBoxLayout *MainLay;
    QHBoxLayout *BottomLay;

    /*Serial Port*/
    bool    Port_IsOpen;
    QSerialPort *sPort;
    QComboBox *xPort;
    QComboBox *xBaud;
    QPushButton *btnRefresh;
    QPushButton *btnConnect;

    /*Status Bar*/
    QStatusBar  *Status;
    QLabel      *StatusIconLbl;
    QLabel      *StatusMessageLbl;

    /*main view*/
    QTextEdit   *MainEd;
    QListView   *Listvu;

};

#endif // XDEBUG_H
