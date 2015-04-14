/// @file optionswindow.cpp Contains the OptionsWindow class implementation
#include "optionswindow.h"
#include "ui_optionswindow.h"

OptionsWindow::OptionsWindow(XJoystick &J, ServoThread *servo, QWidget *parent) :
    QDialog(parent),
    _joy(J),
    _portSize(-1),
    _servo(servo),
    _timer(this),
    ui(new Ui::OptionsWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Options");
    
    QVector< QString > A(_joy.getAllAxis());
    
    ui->joyMX->addItem("None", -1);
    ui->joyMY->addItem("None", -1);
    ui->joyMZ->addItem("None", -1);
    
    for (int i = 0; i < A.size(); ++i) ui->joyMX->addItem(A[i], i);
    for (int i = 0; i < A.size(); ++i) ui->joyMY->addItem(A[i], i);
    for (int i = 0; i < A.size(); ++i) ui->joyMZ->addItem(A[i], i);
    
    joystickChanged();
    
    _timer.setInterval(500);
    _timer.setSingleShot(false);
    _timer.start();
    connect(&_timer, SIGNAL(timeout()), this, SLOT(events()));
    
}

OptionsWindow::~OptionsWindow()
{
    delete ui;
}

void OptionsWindow::storeData()
{
    // Storing joystick data
    _joy.select(ui->joySel->currentData().toInt());
    
    
}

void OptionsWindow::joystickChanged()
{
    // Clear all the items and write the new items
    ui->joySel->clear();
    ui->joySel->addItem("None", -1);
    
    // Adding items and searching the current
    int pos = 0;
    QVector<XJoystick::Info> V(_joy.available());
    for (int i = 0; i < V.size(); ++i) {
        QString text(V[i].name);
        text += ": " + QString::number(V[i].ID);
        if (V[i].ID == _joy.current()) pos = i;
        ui->joySel->addItem(text, V[i].ID);
    }
    ui->joySel->setCurrentIndex(pos);
    
    ui->joyN->setText(QString::number(V.size()));
}

void OptionsWindow::events()
{
    auto ports = QSerialPortInfo::availablePorts();
    
    if (ports.size() != _portSize) {
        _portSize = ports.size();
        
        QString portC(ui->portC->currentData().toString());
        QString portS(ui->portS->currentData().toString());
        int selC = 0, selS = 0;
        
        ui->portC->clear();
        ui->portS->clear();
        
        ui->portC->addItem("None", "");
        ui->portS->addItem("None", "");
        
        for (int i = 0; i < ports.size(); ++i) {
            QString text(ports[i].portName());
            text += ": " + ports[i].description();
            ui->portC->addItem(text, ports[i].portName());
            ui->portS->addItem(text, ports[i].portName());
            
            if (ports[i].portName() == portC) selC = i;
            if (ports[i].portName() == portS) selS = i;
        }
        
        ui->portC->setCurrentIndex(selC);
        ui->portS->setCurrentIndex(selS);
    }
}

void OptionsWindow::on_servoRefresh_clicked()
{
    dynamixel dxl;
    QString port;
    int baud;
    _servo->getServoPortInfo(port, baud);
    
    dxl.initialize(port, baud);
    
}
