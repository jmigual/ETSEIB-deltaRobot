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
    
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), 
            this, SLOT(buttonClicked(QAbstractButton*)));
    
    connect(&_sF, SIGNAL(completion(int)),
            ui->progressBar, SLOT(setValue(int)));
    
    connect(&_sF, SIGNAL(finished()), this, SLOT(refreshFinish()));
    
    connect(&_timer, SIGNAL(timeout()), this, SLOT(events()));
    
    
    // Configuring event funcion
    _timer.setInterval(500);
    _timer.setSingleShot(false);
    _timer.start();
    
    status = new QStatusBar(this);
    status->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(status);    
    
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
    
    // Adding servos
    _servoC.push_back(ui->servo0);
    _servoC.push_back(ui->servo1);
    _servoC.push_back(ui->servo2);
    _servoC.push_back(ui->servo3);
    
    for(QComboBox *s : _servoC) s->addItem("None", -1);
    
    QVector<ServoThread::Servo> S(_servo->getServosInfo());
    Q_ASSERT(S.size() == _servo->getServosNum());
    
    for (int i = 0; i < S.size(); ++i) {
        int ID = S[i].ID;
        
        if (ID >= 0) {
            _servoC[i]->addItem(QString::number(ID), ID);
            _servoC[i]->setCurrentIndex(1);
        }
    }
    
    // Obtaining Servo Port information
    QString port;
    int baud;
    _servo->getServoPortInfo(port, baud);
    ui->baudRS->setValue(baud);
    ui->portS->addItem("", port);
}

OptionsWindow::~OptionsWindow()
{
    delete ui;
    if (_sF.isRunning()) _sF.exit();
}

void OptionsWindow::storeData()
{
    status->showMessage("Data Stored", 2000);
    
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
    for (QComboBox *s : _servoC) sID.push_back(s->currentData().toInt());
    
    _servo->setSID(sID);
    _servo->setSpeed(ui->speed->value());
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
            qDebug() << ports[i].portName();    
            if (ports[i].portName() == portC) selC = i + 1;
            if (ports[i].portName() == portS) selS = i + 1;
        }
        
        if (selS == 0 && ports.size() > 0) selS = 1;

        ui->portC->setCurrentIndex(selC);
        ui->portS->setCurrentIndex(selS);
    }
}

void OptionsWindow::buttonClicked(QAbstractButton *but)
{
    QDB::ButtonRole role = ui->buttonBox->buttonRole(but);
    switch(role) {
    case QDB::ApplyRole:
        this->storeData();
        break;
        
    default:
        break;
    }
}

void OptionsWindow::on_servoRefresh_clicked()
{
    if (_sF.isRunning()) return;
    QString port;
    int baud;
    _servo->getServoPortInfo(port, baud);
    int min = ui->min->value();
    int max = ui->max->value();
    _sF.setData(_servoC, port, baud, min, max);
    _sF.start();
}

void OptionsWindow::refreshFinish()
{
    ui->progressBar->setValue(0);
}

void OptionsWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) return;
    QDialog::keyPressEvent(event);
}
