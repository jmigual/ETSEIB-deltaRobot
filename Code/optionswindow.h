/// @file optionswindow.h Contains the OptionsWindow class declaration
#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

// Qt libraries
#include <QDebug>
#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

// Other libraries
#include <xjoystick.h>

// User libraries
#include "servothread.h"

namespace Ui {
class OptionsWindow;
}

/// Class used to handle a Window to set the options
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
    
    /// Handles events that need to be updated continously
    void events();
    
    /// Refreshes all the servos connected to the port
    void on_servoRefresh_clicked();
    
private:
    
    /// Contains the Joystick to handle options
    XJoystick &_joy;
    
    /// Contains the size of the ports
    int _portSize;
    
    /// Pointer to the servo thread class
    ServoThread *_servo;
    
    /// Waits for a new COM port
    QTimer _timer;
    
    /// Containsh the GUI
    Ui::OptionsWindow *ui;
};

#endif // OPTIONSWINDOW_H

// TODO: Finish options window
