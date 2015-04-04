/// @file main.cpp
/// Contains the Main of the program 
#include <QApplication>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
