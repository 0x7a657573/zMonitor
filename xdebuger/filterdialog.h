#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>

class FilterDialog : public QDialog
{
    Q_OBJECT
public:
    FilterDialog(bool &Status,bool &Bfilter,QString &Filter,QWidget *parent = nullptr);
    virtual ~FilterDialog();

    QString GetText();
    bool    GetEnable();
    bool    GetBlankFilter();
public slots:

private:

    QVBoxLayout *Mainlayout;
    QTextEdit *Edit;
    QCheckBox *EnableCheckBox;
    QCheckBox *BlankCheckBox;
    QPushButton *btnOK;
    QLabel *lbdetails;
};

#endif // FILTERDIALOG_H
