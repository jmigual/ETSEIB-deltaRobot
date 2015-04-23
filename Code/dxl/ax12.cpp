/// @file ax12.cpp
/// Contains the AX12 class implementation
#include "ax12.h"

AX12::AX12() :
    _dxl(NULL),
    _ID(-1),
    _mode(true),
    _rads(false)
{
    
}

AX12::AX12(dynamixel *dxl, int ID) : 
    _dxl(dxl),
    _ID(ID),
   _mode(true),
   _rads(false)
{
    if (_ID < 0 or _dxl == NULL) return;
    dxl->write_byte(_ID, RAM::TorqueEnable, true);
}

AX12::AX12(const AX12 &a) :
    _dxl(a._dxl),
    _ID(a._ID),
    _mode(a._mode),
    _rads(a._rads)
{
    
}

AX12::~AX12()
{
    
}

QVector<int> AX12::connectedID()
{
    if (_dxl == NULL) return QVector<int> (0);
    
    QVector <int> res;
    for (int i = 0; i < 256; ++i) {
        _dxl->ping(i);
        if (_dxl->get_comm_result() == COMM_RXSUCCESS) res.push_back(i);
    }
    
    return res;
}

double AX12::getCurrentLoad()
{
    if (_ID < 0 or _dxl == NULL) return 0;
    int load = _dxl->read_word(_ID, RAM::PresentLoad);
    load -= 1024;
    if (load == -1024) load = 0;
    return double((load/1023)*100);
}

double AX12::getCurrentPos()
{
    if (_ID < 0 or _dxl == NULL) return 0;
    int pos = _dxl->read_word(_ID, RAM::PresentPosition);
    if (_dxl->get_comm_result() != COMM_RXSUCCESS) return -1;
    
    if (_rads) return double((pos/1023.0)*5*M_PI/3);
    return double((pos/1023.0)*300);
}

int AX12::getCurrentTemp()
{
    if (_ID < 0 or _dxl == NULL) return 0;
    int temp = _dxl->read_byte(_ID, RAM::PresentTemperature);
    if (_dxl->get_comm_result() != COMM_RXSUCCESS) return -1;
    return temp;
}

double AX12::getCurrentSpeed()
{
    if (_ID < 0 or _dxl == NULL) return 0;
    int speed = _dxl->read_word(_ID, RAM::PresentSpeed);
    if (_dxl->get_comm_result() != COMM_RXSUCCESS) return -1;
    speed -= 1024;
    if (speed == -1024) speed = 0;
    return double((speed/1023.0)*100);
}

double AX12::getCurrentVoltage()
{
    if (_ID < 0 or _dxl == NULL) return 0;
    char voltage = _dxl->read_byte(_ID, RAM::PresentVoltage);
    if (_dxl->get_comm_result() != COMM_RXSUCCESS) return -1;
    return double(voltage/10.0);
}

void AX12::setGoalPosition(double goal)
{
    if (_ID < 0 or _dxl == NULL) return;
    if (goal > 300.0) goal = 300.0;
    else if (goal < 0) goal = 0;
    _dxl->write_word(_ID, RAM::GoalPosition, int((goal/300.0)*1023));
}

void AX12::setID(int ID)
{
    _ID = ID;
    if (_ID < 0 or _dxl == NULL) return;
    _dxl->write_byte(_ID, RAM::TorqueEnable, true);
}

void AX12::setJointMode(bool mode)
{
    if (_ID < 0 or _dxl == NULL) return;
    _mode = mode;
    if (_mode) {
        _dxl->write_word(_ID, ROM::CWAngleLimit, 0);
        _dxl->write_word(_ID, ROM::CCWAngleLimit, 1023);
    }
    else {
        _dxl->write_word(_ID, ROM::CWAngleLimit, 0);
        _dxl->write_word(_ID, ROM::CCWAngleLimit, 0);
    }
}

void AX12::setMinMax(double min, double max)
{
    if (_ID < 0 or _dxl == NULL) return;
    
    if (min > max)  {
        double aux = min;
        min = max;
        max = aux;
    }
    
    if (min < 0.0) min = 0;
    if (max > 300.0) max = 300;
    
    min = (min/300)*1023;
    max = (max/300)*1023;
    
    _dxl->write_word(_ID, ROM::CWAngleLimit, int (min));
    _dxl->write_word(_ID, ROM::CCWAngleLimit, int (max));
}

void AX12::setSpeed(double speed)
{
    if (_ID < 0 or _dxl == NULL) return;
    if (speed > 100.0) speed = 100.0;
    if (_mode) {
        if (speed < 0.0) speed = 0.0;
        
        int byte = int((speed/100.0) * 1024.0);
        if (speed == 100.0) byte = 0;
        _dxl->write_byte(_ID, RAM::MovingSpeed, byte);        
    }
    else {
        if (speed < -100.0) speed = -100.0;   
        
        int byte = int(((speed + 100)/100.0) * 1024);
        _dxl->write_byte(_ID, RAM::MovingSpeed, byte);
    }

}

