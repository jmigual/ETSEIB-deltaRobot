#include "ax12.h"

AX12::AX12(dynamixel &dxl, int ID, QObject *parent) : 
    QObject(parent),
    _dxl(dxl),
    _ID(ID)
{
    if (_ID < 0) return;
    _dxl.write_byte(_ID, RAM::TorqueEnable, true);
}

AX12::~AX12()
{
    
}

double AX12::getCurrentLoad()
{
    int load = _dxl.read_word(_ID, RAM::PresentLoad);
    load -= 1024;
    if (load == -1024) load = 0;
    return double((load/1023)*100);
}

double AX12::getCurrentPos()
{
    int pos = _dxl.read_word(_ID, RAM::PresentPosition);
    return double((pos/1023)*300);  
}

int AX12::getCurrentTemp()
{
    int temp = _dxl.read_byte(_ID, RAM::PresentTemperature);
    return temp;
}

double AX12::getCurrentSpeed()
{
    int speed = _dxl.read_word(_ID, RAM::PresentSpeed);
    speed -= 1024;
    if (speed == -1024) speed = 0;
    return double((speed/1023)*100);
}

double AX12::getCurrentVoltage()
{
    char voltage = _dxl.read_byte(_ID, RAM::PresentVoltage);
    return double(voltage/10.0);
}

void AX12::setGoalPosition(double goal)
{
    if (goal > 300.0) goal = 300.0;
    else if (goal < 0) goal = 0;
    _dxl.write_word(_ID, RAM::GoalPosition, int(goal));
}

void AX12::setID(int ID)
{
    _ID = ID;
    if (ID < 0) return;
    _dxl.write_byte(_ID, RAM::TorqueEnable, true);
}

void AX12::setJointMode(bool mode)
{
    _mode = mode;
    if (_mode) {
        _dxl.write_word(_ID, ROM::CWAngleLimit, 0);
        _dxl.write_word(_ID, ROM::CCWAngleLimit, 1023);
    }
    else {
        _dxl.write_word(_ID, ROM::CWAngleLimit, 0);
        _dxl.write_word(_ID, ROM::CCWAngleLimit, 0);
    }
}

void AX12::setMinMax(double min, double max)
{
    if (min > max) std::swap(min, max);
    
    if (min < 0.0) min = 0;
    if (max > 300.0) max = 300;
    
    min = (min/300)*1023;
    max = (max/300)*1023;
    
    _dxl.write_word(_ID, ROM::CWAngleLimit, int (min));
    _dxl.write_word(_ID, ROM::CCWAngleLimit, int (max));
}

