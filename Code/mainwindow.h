/// @file mainwindow.h Contains the MainWindow class declaration
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Adding precompiled header
#include "stable.h"

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
    
    typedef ServoThread::Mode Mode;
    
    /// Contains the number of minimun servos to work
    static const int sCount = 3;
    
    /// Contains the number of additional servos used
    static const int aSCount = 0;
    
    enum Version {
        v_1_0
    };
    
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
    
    /// To handle the joystick
    XJoystick _joy;
    
    /// Contains the thread controlling all the servos and external hardware
    ServoThread _sT;
    
    /// To update the joystick value
    QTimer _timer;
    
    /// Contains the user interface
    Ui::MainWindow *ui;
    
    /// Handles the press of a key
    void keyPressEvent(QKeyEvent *event);
    
    /// Handles the realease of a key
    void keyReleaseEvent(QKeyEvent *event);
    
    /// Reads the data from the default location
    inline void read() { read(_dataP); }
    
    /// Reads the data from the selected path, overloaded function
    void read(QString path);
    
    /// Writes the data to the default location
    inline void write() { write(_dataP); }
    
    /// Writes the data to disk overloaded function
    void write(QString path);

        
private slots:
    
    /// Handles a joystick update
    void joyChanged();
    
    /// Handles the change of a mode in the thread
    void modeChanged(Mode m);
    
    /// To select the options
    void on_actionOptions_triggered();
    
    /// Opens the import of Dominoes file
    void on_actionImport_triggered();
    
    /// Handles the change of the mode
    void on_mode_clicked();
    
    /// Handles a reset
    void on_reset_clicked();
    
    /// Starts or stops the thread
    void on_start_clicked();
    
    /// Updates all data to the servo thread
    void update();
};

#endif // MAINWINDOW_H
