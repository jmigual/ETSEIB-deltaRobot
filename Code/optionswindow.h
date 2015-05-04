/// @file optionswindow.h Contains the OptionsWindow class declaration
#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include "stable.h"

// User libraries
#include "servothread.h"
#include "servofind.h"

namespace Ui {
class OptionsWindow;
}

/// Class used to handle a Window to set the options
class OptionsWindow : public QDialog
{
    Q_OBJECT
    
    typedef QDialogButtonBox QDB;
    
public:
    
    /// Default constructor must be intialized with a few values
    /// @param J Refernce to the Joystick handler
    /// @param servo Pointer to the ServoThread
    /// @param aX Axis for the X value
    /// @param aY Axis for the Y value
    /// @param aZ Axis for the Z value
    explicit OptionsWindow(XJoystick &J, ServoThread *servo, QWidget *parent = 0);
    
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
    
    /// Handles a button clicked
    void buttonClicked(QAbstractButton *but);
    
    /// Refreshes all the servos connected to the port
    void on_servoRefresh_clicked();
    
    /// Handles the endig of refresh function
    void refreshFinish();
    
private:
    
    /// Contains the Joystick to handle options
    XJoystick &_joy;
    
    /// Contains the size of the ports
    int _portSize;
    
    /// Pointer to the servo thread class
    ServoThread *_servo;
    
    /// Contains all servo QComboBoxes
    QVector< QComboBox *> _servoC;
    
    /// Thread to find the servos in a non blocking operation
    ServoFind _sF;
    
    /// Status bar
    QStatusBar *status;
    
    /// Waits for a new COM port
    QTimer _timer;
    
    /// Containsh the GUI
    Ui::OptionsWindow *ui;
    
    void keyPressEvent(QKeyEvent *event);
};

#endif // OPTIONSWINDOW_H
