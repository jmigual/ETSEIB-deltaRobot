#include "optionswindow.h"
#include "ui_optionswindow.h"

OptionsWindow::OptionsWindow(XJoystick &J, ServoThread *servo, QWidget *parent) :
    QDialog(parent),
    _joy(J),
    _servo(servo),
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

void OptionsWindow::on_servoRefresh_clicked()
{
    dynamixel dxl;
}
