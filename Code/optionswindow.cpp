/// @file optionswindow.cpp Contains the OptionsWindow class implementation
#include "optionswindow.h"
#include "ui_optionswindow.h"

OptionsWindow::OptionsWindow(XJoystick &J, ServoThread *servo, int &aX, 
                             int &aY, int &aZ, QWidget *parent) :
    QDialog(parent),
    _jAxisX(aX),
    _jAxisY(aY),
    _jAxisZ(aZ),
    _joy(J),
    _portSize(-1),
    _servo(servo),
    _timer(this),
    ui(new Ui::OptionsWindow)
{
    ui->setupUi(this);
    
    QVector< QString > A(_joy.getAllAxis());
    
    // Adding joystick axis movement
    ui->joyMX->addItem("None", -1);
    ui->joyMY->addItem("None", -1);
    ui->joyMZ->addItem("None", -1);
    
    for (int i = 0; i < A.size(); ++i) ui->joyMX->addItem(A[i], i);
    for (int i = 0; i < A.size(); ++i) ui->joyMY->addItem(A[i], i);
    for (int i = 0; i < A.size(); ++i) ui->joyMZ->addItem(A[i], i);
    
    ui->joyMX->setCurrentIndex(_jAxisX + 1);
    ui->joyMY->setCurrentIndex(_jAxisY + 1);
    ui->joyMZ->setCurrentIndex(_jAxisZ + 1);
    
    // Updating joystick data
    joystickChanged();
    
    // Configuring event funcion
    _timer.setInterval(500);
    _timer.setSingleShot(false);
    _timer.start();
    connect(&_timer, SIGNAL(timeout()), this, SLOT(events()));
    
    // Adding servos
    ui->servo0->addItem("None", -1);
    ui->servo1->addItem("None", -1);
    ui->servo2->addItem("None", -1);
    ui->servo3->addItem("None", -1);
    
    this->on_servoRefresh_clicked();
}

OptionsWindow::~OptionsWindow()
{
    delete ui;
}

void OptionsWindow::storeData()
{
    // Storing joystick data
    _joy.select(ui->joySel->currentData().toInt());
    
    // Joystick movement axis
    _jAxisX = ui->joyMX->currentData().toInt();
    _jAxisY = ui->joyMY->currentData().toInt();
    _jAxisZ = ui->joyMZ->currentData().toInt();
    
    QString portS(ui->portS->currentData().toString());
    int baudS(ui->baudRS->value());
    _servo->setServoPortInfo(portS, baudS);
    
    QVector<int> sID;
    if (ui->servo0->count()) sID.push_back(ui->servo0->currentData().toInt());
    if (ui->servo1->count()) sID.push_back(ui->servo1->currentData().toInt());
    if (ui->servo2->count()) sID.push_back(ui->servo2->currentData().toInt());
    if (ui->servo3->count()) sID.push_back(ui->servo3->currentData().toInt());
    
    // TODO: Write data to disk here
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
    ui->portN->setText(QString::number(ports.size()));
    
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
            
            if (ports[i].portName() == portC) selC = i + 1;
            if (ports[i].portName() == portS) selS = i + 1;
        }

        ui->portC->setCurrentIndex(selC);
        ui->portS->setCurrentIndex(selS);
    }
}

void OptionsWindow::on_servoRefresh_clicked()
{
    
    QString port;
    int baud;
    _servo->getServoPortInfo(port, baud);
    dynamixel dxl(port, baud);
    
    int s0 = ui->servo0->currentData().toInt();
    int s1 = ui->servo1->currentData().toInt();
    int s2 = ui->servo2->currentData().toInt();
    int s3 = ui->servo3->currentData().toInt();
    
    ui->servo0->clear();
    ui->servo1->clear();
    ui->servo2->clear();
    ui->servo3->clear();
    
    int index = 0;
    int p0 = 0; p1 = 0, p2 = 0, p3 = 0;
    
    for (int i = 0; i <= MAX_ID; ++i) {
        dxl.ping(i);
        if (dxl.get_comm_result() == COMM_RXSUCCESS) {
            servo.push_back(i);
            if (i == s0) p0 = index;
            if (i == s1) p1 = index;
            if (i == s2) p2 = index;
            if (i == s3) p3 = index;
            
            ui->servo0->addItem(QString::number(i), i);
            ui->servo1->addItem(QString::number(i), i);
            ui->servo2->addItem(QString::number(i), i);
            ui->servo3->addItem(QString::number(i), i);
            
            ++index;
        }
    }
    
    ui->servo0->setCurrentIndex(p0);
    ui->servo1->setCurrentIndex(p1);
    ui->servo2->setCurrentIndex(p2);
    ui->servo3->setCurrentIndex(p3);
        
}
