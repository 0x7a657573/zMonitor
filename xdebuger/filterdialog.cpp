#include "filterdialog.h"
#include <QLabel>
#include <QDebug>

FilterDialog::FilterDialog(bool &Status,bool &Bfilter,QString &Filter,QWidget *parent) : QDialog(parent)
{


    Mainlayout = new QVBoxLayout(this);

    /*label*/
    lbdetails = new QLabel(tr("Filter Support [RegEx] Format"),this);
    /*Line Edit*/
    Edit = new QTextEdit(this);
    Edit->setText(Filter);

    /*CheckBox*/
    EnableCheckBox = new QCheckBox();
    EnableCheckBox->setText("Enable Filter");
    EnableCheckBox->setChecked(Status);

    BlankCheckBox = new QCheckBox();
    BlankCheckBox->setText("Filter BlankLine");
    BlankCheckBox->setChecked(Bfilter);

    btnOK = new QPushButton("Ok");
    connect(btnOK,SIGNAL(clicked()),this,SLOT(accept()));

    Mainlayout->addWidget(lbdetails);
    Mainlayout->addWidget(Edit);
    Mainlayout->addWidget(EnableCheckBox);
    Mainlayout->addWidget(BlankCheckBox);
    Mainlayout->addWidget(btnOK);

    setLayout(Mainlayout);
    setWindowTitle(tr("Filter Setting"));
}

QString FilterDialog::GetText()
{
    return Edit->toPlainText();
}

bool FilterDialog::GetEnable()
{
    return EnableCheckBox->isChecked();
}

bool FilterDialog::GetBlankFilter()
{
    return BlankCheckBox->isChecked();
}

FilterDialog::~FilterDialog()
{
    delete lbdetails;
    delete Edit;
    delete btnOK;
    delete EnableCheckBox;
    delete BlankCheckBox;
    delete Mainlayout;
}


