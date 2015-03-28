#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("DeltaRobot Control");    
    _sT.start();
    _timer.setInterval(10);
    _timer.start();
    
    connect(&_joy, SIGNAL(changed()), this, SLOT(joyChanged()));
    connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
    
    QVector< pair<QString, int> > V(_joy.getAllAxis());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::joyChanged()
{
    int sel = _joy.current();
    
    QVector< XJoystick::Info > V(_joy.available());
    bool found = false;
    int i = 0;
    while (i < V.size() and not found) { found = V[i].ID == sel; ++i; }
    
    if (not found) {
        if (V.size() > 0) _joy.select(V[0].ID);
        else {
            _joy.select(-1);
            // TODO: Amagar finestra joystic 
        }
    }
}


void MainWindow::on_actionOptions_triggered()
{
    OptionsWindow o;
    o.exec();
    
    if (o.result()) qDebug() << "Acceptat";
    else qDebug() << "Rebutjat";
}

void MainWindow::update()
{
    
}
