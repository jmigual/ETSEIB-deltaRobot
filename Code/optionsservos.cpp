#include "optionsservos.h"
#include "ui_optionsservos.h"

OptionsServos::OptionsServos(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionsServos)
{
    ui->setupUi(this);
}

OptionsServos::~OptionsServos()
{
    delete ui;
}
