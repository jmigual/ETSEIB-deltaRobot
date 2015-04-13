#include "servothread.h"

ServoThread::ServoThread() :
    _axis(XJoystick::AxisCount),
    _buts(XJoystick::ButtonCount),
    _dChanged(false),
    _end(false),
    _mod(Mode::manual),
    _pause(true),
    _sBaud(1000000),
    _servos(3),
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

void ServoThread::load(QString &file)
{
    _mutex.lock();
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QDataStream df(&f);
    
    int ver;
    df >> ver;
    if (ver == Version::v_1_0) {
        int n;
        df >> _sBaud >> _sPort >> n;
        
        _servos.resize(n);
        for (Servo &s : _servos) df >> s.ID;
        _dChanged = true;
    }
    else qWarning() << "Not a valid file";    
    _mutex.unlock();
}

void ServoThread::setData(QVector<float> &aV, QVector<bool> &buts)
{
    _mutex.lock();
    // Copying the joystick values
    _axis = aV;
    _buts = buts;
    _dChanged = true;
    
    _mutex.unlock();
}

void ServoThread::write(QString &file)
{
    _mutex.lock();
    QFile f(file);
    f.open(QIODevice::WriteOnly);
    QDataStream df(&f);
    
    df << int(Version::v_1_0) << _sBaud << _sPort << _servos.size();    
    for (const Servo &s : _servos) df << s.ID;
    
    _mutex.unlock();
}

void ServoThread::run()
{
    _mutex.lock();
    int sBaud = _sBaud;
    QString sPort = _sPort;
    
    _mutex.unlock();
    dynamixel dxl(sPort, sBaud);
    QVector< AX12 > (_servos.size(), dxl);
    
    while (not _end) {
        
        msleep(10);
        _mutex.lock();
        if (not _end and _pause) {
            dxl.terminate();
            _cond.wait(&_mutex);
            dxl.initialize(sPort, sBaud);
        }        
        _dChanged = false;
        _mutex.unlock();
    }
    
    dxl.terminate();
    exit(0);
}

