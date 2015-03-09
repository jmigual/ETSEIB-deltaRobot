/// @file joystick.cpp
/// @brief Contains the XJoystick's class implementation
#include "xjoystick.h"
#include <QDebug>

XJoystick::XJoystick(int ID, float filter) :  
    _aID(J::AxisCount),
    _axis(J::AxisCount),
    _axisH(J::AxisCount),
    _buttons(J::ButtonCount),
    _filter(abs(filter)),
    _hold(J::AxisCount),
    _ID(ID)
{
    _aID[0] = Axis(J::X, "X");
    _aID[1] = Axis(J::Y, "Y");
    _aID[2] = Axis(J::Z, "Z");
    _aID[3] = Axis(J::R, "R");
    _aID[4] = Axis(J::U, "U");
    _aID[5] = Axis(J::V, "V");
    _aID[6] = Axis(J::PovX, "PovX");
    _aID[7] = Axis(J::PovY, "PovY");
    
    
    _timer.setInterval(500);
    _timer.setTimerType(Qt::CoarseTimer);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(loopEvent()));
    _timer.start();
}

XJoystick::~XJoystick()
{
    
}

QVector<XJoystick::Info> XJoystick::available()
{
    QVector< Info > joys(_joys.size());
    
    for (int i = 0; i < _joys.size(); ++i) {
        joys[i].ID = _joys[i];
        Joystick::Identification id = Joystick::getIdentification(_joys[i]);
        joys[i].name = id.name.toAnsiString().data();
    }
    return joys;
}

bool XJoystick::button(int n)
{
    if (n < 0 or n > J::ButtonCount) return false;
    return J::isButtonPressed(_ID, n);
}

void XJoystick::axisPress(unsigned char a, float value)
{
    if (a >= _axis.size()) return;
    
    _axis[a] = value;
    _axisH[a] = true;
}

void XJoystick::axisRelease(unsigned char a)
{
    if (a >= _axis.size()) return;
    
    _axisH[a] = false;
}

bool XJoystick::select(int s)
{
    bool found = false;
    int i = 0;
    while (i < _joys.size() and not found) {
        if (_joys[i] == s) found = true;
        else ++i;
    }
    if (found) {
        _ID = s;
        _aName.clear();
        for (int i = 0; i < _axis.size(); ++i) {
            if (J::hasAxis(_ID, _aID[i].axis)) 
                _aName.push_back(P(_aID[i].name, i));
        }
        
        _butCount = Joystick::getButtonCount(_ID);
    }
    else _aName.clear();
    
    return found;
}

void XJoystick::update()
{
    if (_ID < 0) return;
    J::update();
    
    for (int i = 0; i < J::AxisCount; ++i) {
        if (not _axisH[i]) {
            // We must check if the Axis exists
            if (J::hasAxis(_ID, _aID[i].axis)) 
                _axis[i] = J::getAxisPosition(_ID, _aID[i].axis);
            else _axis[i] = 0;
        }
    }
        
    if (_filter) filter();
    
    for (int i = 0; i < J::ButtonCount; ++i) {
        _buttons[i] = J::isButtonPressed(_ID, i);
    }
}

void XJoystick::filter()
{
    for (float &n : _axis) if (abs(n) < _filter) n = 0;
}

void XJoystick::loopEvent()
{
    J::update();
    QVector< int > J;
    for (int i = 0; i < J::Count; ++i) {
        if (J::isConnected(i)) J.push_back(i);
    }
    
    bool found = false;
    if (_joys.size() != J.size()) found = true;
    else for (int i = 0; i < J.size() and not found; ++i) {
        found = (J[i] != _joys[i]);
    }
    
    _joys = J;
    
    if (found) emit changed();
}

/*
+kkkkkkkkkkkkkkkkkkkkkkkkkkkkk


























kkkkkkkkkkkkkkkkkkkk
*/
