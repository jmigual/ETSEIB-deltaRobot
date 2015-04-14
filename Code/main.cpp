/// @file main.cpp Contains the Main of the program 
#include <QApplication>
#include "mainwindow.h"

/// @mainpage
/// This project is a Delta robot controller using Dynamixel AX12 servos.
/// This type of robot can pick and place objects 
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
