#include "optionswindow.h"
#include "ui_optionswindow.h"

OptionsWindow::OptionsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Options");
}

OptionsWindow::~OptionsWindow()
{
    delete ui;
}

void OptionsWindow::joystickChanged()
{
    
}
