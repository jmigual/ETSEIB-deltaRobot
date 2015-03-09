#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    dxl.initialize("COM3", 1000000);
}

MainWindow::~MainWindow()
{
    delete ui;
}
