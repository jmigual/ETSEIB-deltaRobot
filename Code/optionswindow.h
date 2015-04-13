#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

// Qt libraries
#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>
#include <QStackedWidget>

// Other libraries
#include <xjoystick.h>

// User libraries
#include "servothread.h"

namespace Ui {
class OptionsWindow;
}

class OptionsWindow : public QDialog
{
    Q_OBJECT
    
public:
    
    /// Default constructor
    explicit OptionsWindow(XJoystick &J, ServoThread *servo, 
                           QWidget *parent = 0);
    
    /// Destructor
    ~OptionsWindow();
    
    /// Stores all data
    void storeData();
    
public slots:
    
    /// To handle the change of a joystick
    void joystickChanged();

private slots:
    void on_servoRefresh_clicked();
    
private:
    
    /// Contains the Joystick to handle options
    XJoystick &_joy;
    
    /// Pointer to the servo thread class
    ServoThread *_servo;
    
    /// Containsh the GUI
    Ui::OptionsWindow *ui;
};

#endif // OPTIONSWINDOW_H

// TODO: Finish options window
