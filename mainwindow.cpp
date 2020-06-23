#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <xdebuger/xdebug.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Xdebuger *xdbg = new Xdebuger(this);
    setCentralWidget(xdbg);
    centralWidget()->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

