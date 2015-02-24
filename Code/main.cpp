#include <QCoreApplication>
#include <QDebug>
#include "dynamixel.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
    dynamixel dxl;
    dxl.initialize(QString("COM3"), 1000000);
    
    return a.exec();
}
