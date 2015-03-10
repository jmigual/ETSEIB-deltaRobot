#ifndef OPTIONSSERVOS_H
#define OPTIONSSERVOS_H

#include <QWidget>

namespace Ui {
class OptionsServos;
}

class OptionsServos : public QWidget
{
    Q_OBJECT
    
public:
    explicit OptionsServos(QWidget *parent = 0);
    ~OptionsServos();
    
private:
    Ui::OptionsServos *ui;
};

#endif // OPTIONSSERVOS_H
