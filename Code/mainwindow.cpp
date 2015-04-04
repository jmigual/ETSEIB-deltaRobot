#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _axis(XJoystick::AxisCount),
    _axisV(XJoystick::AxisCount),
    _buts(XJoystick::ButtonCount),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _sT.start();
    _timer.setInterval(10);
    _timer.start();
    
    connect(&_joy, SIGNAL(changed()), this, SLOT(joyChanged()));
    connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
    
    // JOYSTICK
    QVector< QString > V(_joy.getAllAxis());
    // Adding axis
    QGridLayout *wL = new QGridLayout;
    for (int i = 0; i < XJoystick::AxisCount; ++i) {
        QHBoxLayout *L = new QHBoxLayout;
        L->addWidget(new QLabel(V[i].append(":"), this));
        _axis[i] = new QLabel("#");
        L->addWidget(_axis[i]);
        L->addStretch();
        wL->addLayout(L, i%3, i/3); 
    }
    ui->joyAxis->setLayout(wL);
    
    wL = new QGridLayout;
    for (int i = 0; i < XJoystick::ButtonCount; ++i) {
        _buts[i] = new QLabel(QString::number(i + 1));
        wL->addWidget(_buts[i], i/8, i%8);
        _buts[i]->setEnabled(false);
        _buts[i]->hide();
    }
    ui->joyButs->setLayout(wL);
    ui->joyAxis->hide();
    ui->joyButs->hide();
    ui->line->hide();
    
    // TODO: Create dataPath
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
        if (V.size() > 0) {
            _joy.select(V[0].ID);
            ui->line->hide();
            
            // Showing axis
            ui->joyAxis->show();
            
            // Showing buttons
            for (QLabel *l : _buts) l->hide();
            ui->joyButs->show();
            int n = _joy.buttonCount();
            for (int i = 0; i < n; ++i) _buts[i]->show();
        }
        else {
            _joy.select(-1);
            ui->joyAxis->hide();
            ui->joyButs->hide();
            ui->line->hide();
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
    _joy.update();
    for (int i = 0; i < XJoystick::AxisCount; ++i) {
        _axisV[i] = _joy[i];
    }
    
    // TODO: Finish update function
}
