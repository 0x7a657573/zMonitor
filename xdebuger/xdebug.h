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
#include "xdbg.h"
#include <QList>
#include <QTimer>
#include <QSettings>
#include <QLineEdit>

#define StartInterPreter    0xC8
#define EndInterPreter      0xFA

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
    void handel_ConDisConAction_UseIP();
    void handel_DataReady();
    void handel_clearLogs();
    void handel_TimerUpdate();
    void handel_viewChange(bool newstatus);
    void handel_ResetSetting();
    void handel_AboutMe();
    void handel_SendSerial();

private:
    void LoadToolBar(QHBoxLayout *lay);
    void LoadMainView(QHBoxLayout *lay);
    void LoadStatusBar(QHBoxLayout *lay);
    void LockPortOpen(bool Look);
    void ProsessIncomingData(QByteArray &data);
    void IncertDataRow(int Id,QString &text);

protected:

    QSettings *xSettings;

    /*Main Layout*/
    QVBoxLayout *Layout;
    QHBoxLayout *TopLay;
    QHBoxLayout *MainLay;
    QHBoxLayout *BottomLay;

    bool UseIP;
    /*Tcp Port*/
    QLineEdit *HostEdit;


    /*Serial Port*/
    bool    Port_IsOpen;
    QSerialPort *sPort;
    QComboBox *xPort;
    QComboBox *xBaud;
    QPushButton *btnRefresh;
    QPushButton *btnConnect;
    unsigned int TotlaRx;
    QTimer *Timer;

    /*Status Bar*/
    QStatusBar  *Status;
    QLabel      *StatusIconLbl;
    QLabel      *StatusMessageLbl;
    QLabel      *StatusStatisticsLbl;

    /*toolsbar*/
    QLineEdit *CmdInput;
    QPushButton *btnSend;

    /*main view*/
    QList<xdbg*> ViewList;

};

#endif // XDEBUG_H
