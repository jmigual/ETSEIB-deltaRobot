#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Included libraries
#include <QMainWindow>
#include <QDebug>
#include <QVector>

// User libraries
#include "ax12.h"
#include "dynamixel.h"
#include "optionswindow.h"
#include "servothread.h"
#include "xjoystick.h"

namespace Ui {
class MainWindow;
}

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

private:    
    
    /// Contains all the servos utilization
    QVector< AX12 > _servos;
    
    /// Contains the user interface
    Ui::MainWindow *ui;
    
    /// Contains the thread controlling all the servos and external hardware
    ServoThread _sT;

        
private slots:
    
    /// To select the options
    void on_actionOptions_triggered();
};

#endif // MAINWINDOW_H
