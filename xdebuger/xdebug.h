#ifndef XDEBUGR_H
#define XDEBUGR_H

#include <stdint.h>
#include <QWidget>
#include <QObject>
#include <QtCore>
#include <QString>


namespace Ui {
class Xdebuger: public Xdebuger {};
}


class Xdebuger: public QWidget
{
    Q_OBJECT

public:
    explicit Xdebuger(QWidget *parent=0);
    virtual ~Xdebuger();

};

#endif // XDEBUG_H
