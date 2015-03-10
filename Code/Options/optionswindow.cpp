#include "optionswindow.h"
#include "ui_optionswindow.h"

OptionsWindow::OptionsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Options");
    
    ui->stack->addWidget(new OptionsServos);
    ui->stack->addWidget(new QWidget);
}

OptionsWindow::~OptionsWindow()
{
    delete ui;
}


void OptionsWindow::on_option_currentItemChanged(QListWidgetItem *item, 
                                                 QListWidgetItem *)
{
    if (item->text() == "Servos") {
        qDebug() << "Servos";
        ui->stack->setCurrentIndex(0);
    }
    else {
        ui->stack->setCurrentIndex(1);
    }
}
