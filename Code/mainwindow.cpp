#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("DeltaRobot Control");    
    _sT.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOptions_triggered()
{
    OptionsWindow o;
    o.exec();
    
    if (o.result()) qDebug() << "Acceptat";
    else qDebug() << "Rebutjat";
}
