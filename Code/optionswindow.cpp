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
    
    QVector<int> V(_servo->getServosID());
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
    ui->joySel->clear();
    ui->joySel->addItem("None", -1);
    
    QVector<XJoystick::Info> V(_joy.available());
    for (XJoystick::Info &j : V) {
        QString text(j.name);
        text += ": " + QString::number(j.ID);
        ui->joySel->addItem(text, j.ID);
    }
    
    ui->joyN->setText(QString::number(V.size()));
}
