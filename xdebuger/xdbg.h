#ifndef XDBG_H
#define XDBG_H

#include <QWidget>
#include <QListView>
#include <QDebug>

class xdbg : public QWidget
{
    Q_OBJECT
public:
    explicit xdbg(QWidget *parent = nullptr);

signals:

};

#endif // XDBG_H
