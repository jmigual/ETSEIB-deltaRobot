#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _servos(sCount + aSCount, _dxl),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("DeltaRobot Control");
    _dxl.initialize("COM3", 1000000);
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
