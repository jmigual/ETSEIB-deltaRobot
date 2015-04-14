/// @file ax12.cpp
/// Contains the AX12 class implementation
#include "ax12.h"

AX12::AX12(dynamixel &dxl, int ID, QObject *parent) : 
    QObject(parent),
    dxl(dxl),
    _ID(ID),
   _mode(true) 
{
    if (_ID < 0) return;
    dxl.write_byte(_ID, RAM::TorqueEnable, true);
}

AX12::AX12(const AX12 &a) :
    QObject(a.parent()),
    dxl(a.dxl),
    _ID(a._ID),
    _mode(true)
{
    
}

AX12::~AX12()
{
    
}

QVector<int> AX12::connectedID()
{
    QVector <int> res;
    for (int i = 0; i < 256; ++i) {
        dxl.ping(i);
        if (dxl.get_comm_result() == COMM_RXSUCCESS) res.push_back(i);
    }
    
    return res;
}

double AX12::getCurrentLoad()
{
    if (_ID < 0) return 0;
    int load = dxl.read_word(_ID, RAM::PresentLoad);
    load -= 1024;
    if (load == -1024) load = 0;
    return double((load/1023)*100);
}

double AX12::getCurrentPos()
{
    if (_ID < 0) return 0;
    int pos = dxl.read_word(_ID, RAM::PresentPosition);
    if (dxl.get_comm_result() != COMM_RXSUCCESS) return -1;
    return double((pos/1023.0)*300);  
}

int AX12::getCurrentTemp()
{
    if (_ID < 0) return 0;
    int temp = dxl.read_byte(_ID, RAM::PresentTemperature);
    if (dxl.get_comm_result() != COMM_RXSUCCESS) return -1;
    return temp;
}

double AX12::getCurrentSpeed()
{
    if (_ID < 0) return 0;
    int speed = dxl.read_word(_ID, RAM::PresentSpeed);
    if (dxl.get_comm_result() != COMM_RXSUCCESS) return -1;
    speed -= 1024;
    if (speed == -1024) speed = 0;
    return double((speed/1023.0)*100);
}

double AX12::getCurrentVoltage()
{
    if (_ID < 0) return 0;
    char voltage = dxl.read_byte(_ID, RAM::PresentVoltage);
    if (dxl.get_comm_result() != COMM_RXSUCCESS) return -1;
    return double(voltage/10.0);
}

void AX12::setGoalPosition(double goal)
{
    if (_ID < 0) return;
    if (goal > 300.0) goal = 300.0;
    else if (goal < 0) goal = 0;
    dxl.write_word(_ID, RAM::GoalPosition, int((goal/300.0)*1023));
}

void AX12::setID(int ID)
{
    _ID = ID;
    if (ID < 0) return;
    dxl.write_byte(_ID, RAM::TorqueEnable, true);
}

void AX12::setJointMode(bool mode)
{
    if (_ID < 0) return;
    _mode = mode;
    if (_mode) {
        dxl.write_word(_ID, ROM::CWAngleLimit, 0);
        dxl.write_word(_ID, ROM::CCWAngleLimit, 1023);
    }
    else {
        dxl.write_word(_ID, ROM::CWAngleLimit, 0);
        dxl.write_word(_ID, ROM::CCWAngleLimit, 0);
    }
}

void AX12::setMinMax(double min, double max)
{
    if (_ID < 0) return;
    
    if (min > max) std::swap(min, max);
    
    if (min < 0.0) min = 0;
    if (max > 300.0) max = 300;
    
    min = (min/300)*1023;
    max = (max/300)*1023;
    
    dxl.write_word(_ID, ROM::CWAngleLimit, int (min));
    dxl.write_word(_ID, ROM::CCWAngleLimit, int (max));
}

void AX12::setSpeed(double speed)
{
    if (_ID < 0) return;
    if (speed > 100.0) speed = 100.0;
    if (_mode) {
        if (speed < 0.0) speed = 0.0;
        
        int byte = int((speed/100.0) * 1024.0);
        if (speed == 100.0) byte = 0;
        dxl.write_byte(_ID, RAM::MovingSpeed, byte);        
    }
    else {
        if (speed < -100.0) speed = -100.0;   
        
        int byte = int(((speed + 100)/100.0) * 1024);
        dxl.write_byte(_ID, RAM::MovingSpeed, byte);
    }

}

