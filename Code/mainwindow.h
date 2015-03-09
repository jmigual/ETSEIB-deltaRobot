#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Included libraries
#include <QMainWindow>

// User libraries
#include "dynamixel.h"
#include "ax12.h"

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
    
};

#endif // MAINWINDOW_H
