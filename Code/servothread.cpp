#include "servothread.h"

ServoThread::ServoThread() :
    _axis(XJoystick::AxisCount),
    _buts(XJoystick::ButtonCount),
    _end(false),
    _pause(false),
    _sBaud(1000000),
    _sPort("COM3"),
    _sPortChanged(false)
{
    
}

ServoThread::~ServoThread()
{
    _mutex.lock();
    _end = true;
    _cond.wakeOne();
    _mutex.unlock();
    
    wait();
}

void ServoThread::loadData(QVector<float> &aV, QVector<bool> &buts)
{
    _mutex.lock();
    // Copying the joystick values
    _axis = aV;
    _buts = buts;
    
    _mutex.unlock();
}

void ServoThread::write(QString &dir)
{
    
}

void ServoThread::run()
{
    _mutex.lock();
    int sBaud = _sBaud;
    QString sPort = _sPort;
    _mutex.unlock();
    
    dxl.initialize(sPort, sBaud);
    
    QVector<AX12> servos(3);
    QSerialPort com;
    
    while (not _end) {
        
        
        msleep(10);
        _mutex.lock();
        if (not _end and _pause) _cond.wait(&_mutex);
        
        
        
        _mutex.unlock();
    }
}

