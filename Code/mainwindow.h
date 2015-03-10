#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Included libraries
#include <QMainWindow>
#include <QDebug>

// User libraries
#include "ax12.h"
#include "dynamixel.h"
#include "Options/optionswindow.h"
#include "xjoystick.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
private:
    /// Contains the comunication with the motors
    dynamixel _dxl;
    
    /// Contains the user interface
    Ui::MainWindow *ui;
    
public:
    
    /// Default constructor
    explicit MainWindow(QWidget *parent = 0);
    
    /// Default destructor
    ~MainWindow();
    
private slots:
    
    /// To select the options
    void on_actionOptions_triggered();
};

#endif // MAINWINDOW_H
