/// @file mainwindow.h Contains the MainWindow class declaration
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt Libraries
#include <QDebug>
#include <QLabel>
#include <QMainWindow>
#include <QVector>
#include <QStandardPaths>

// Other libraries
#include <xjoystick.h>

// User libraries
#include "dxl/ax12.h"
#include "dxl/dynamixel.h"
#include "optionswindow.h"
#include "servothread.h"

/// Namespace to work with a User Interface Qt Form
namespace Ui {
class MainWindow;
}

using namespace std;

/// Contains all the windows and other classes
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    /// Contains the number of minimun servos to work
    static const int sCount = 3;
    
    /// Contains the number of additional servos used
    static const int aSCount = 0;
    
public:
    
    /// Default constructor
    explicit MainWindow(QWidget *parent = 0);
    
    /// Default destructor
    ~MainWindow();
    
signals:
    
    /// Emmitted when a joystick changes
    void joystickChanged();

private:  
    
    /// Handles all the axis labels
    QVector< QLabel *> _axis;
    
    /// Contains the axis value;
    QVector < float > _axisV;
    
    /// Handles all the button labels
    QVector< QLabel *> _buts;
    
    /// Handles all buttons values
    QVector < bool > _butsV;
    
    /// Contains the path to the data location
    QString _dataP;
    
    int _jAxisX = -1;   ///< Axis for the X value
    int _jAxisY = -1;   ///< Axis for the Y value
    int _jAxisZ = -1;   ///< AXis for the Z value
    
    /// To handle the joystick
    XJoystick _joy;
    
    /// Contains the thread controlling all the servos and external hardware
    ServoThread _sT;
    
    /// To update the joystick value
    QTimer _timer;
    
    /// Contains the user interface
    Ui::MainWindow *ui;

        
private slots:
    
    /// Handles a joystick update
    void joyChanged();
    
    /// To select the options
    void on_actionOptions_triggered();
    
    /// Updates all data to the servo thread
    void update();
};

#endif // MAINWINDOW_H
