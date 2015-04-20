/// @file servothread.h Contains the ServoThread class implementation
#include "servothread.h"

ServoThread::ServoThread() :
    _axis(XJoystick::AxisCount),
    _buts(XJoystick::ButtonCount),
    _cBaud(9600),
    _cPort("COM3"),
    _dChanged(false),
    _end(false),
    _mod(Mode::manual),
    _pause(true),
    _sBaud(1000000),
    _servos(3),
    _sPort("COM9"),
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
        df >> _cBaud >> _cPort >> _sBaud >> _sPort >> n;
        
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
    
    df << int(Version::v_1_0) << _cBaud << _cPort << _sBaud << _sPort
       << _servos.size();    
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
    QVector< AX12 > (_servos.size(), &dxl);
    
    while (not _end) {
        
        msleep(10);
        _mutex.lock();
        if (not _end and _pause) {
            dxl.terminate();
            _cond.wait(&_mutex);
            dxl.initialize(sPort, sBaud);
        }        
        if (_dChanged) {
            if (sPort != _sPort) {
                sPort = _sPort;
                sBaud = _sBaud;
                dxl.terminate();
                dxl.initialize(sPort, sBaud);
            }   
        }
        _dChanged = false;
        _mutex.unlock();
    }
    
    dxl.terminate();
    exit(0);
}

void ServoThread::setAngles(double x0, double y0, double z0, 
                            double &theta1, double &theta2, double &theta3)
{    
    double x1 = x0 + L2 - L1;
    double y1 = y0;
    double z1 = z0;
    theta1 = singleAngle(x1,y1,z1);
    
    double x2 = z0*sin60 - x0*cos60 + L2 - L1;
    double y2 = y0;
    double z2 = -z0*cos60 - x0*sin60;
    theta2 = singleAngle(x2,y2,z2);
    
    double x3 = -z0*sin60 - x0*cos60 + L2 - L1;
    double y3 = y0;
    double z3 = -z0*cos60 + x0*sin60;
    theta3 = singleAngle(x3,y3,z3);
}

double ServoThread::singleAngle(double x0, double y0, double z0)
{
    double n = b * b - a * a - z0 * z0 - x0 * x0 - y0 * y0;
    double raiz = sqrt (n*n*y0*y0 - 4*(x0*x0 + y0*y0)*(-x0*x0*a*a + n*n/4));
    
    if (x0 < 0) raiz *= -1;
    double y = (-n*y0 + raiz ) / (2*(x0*x0 + y0*y0));
    
    int signe = 1;
    if ((b*b - (y0 + a)*(y0 + a)) < (x0*x0 + z0*z0) && x0 < 0) signe *= -1;
    double x = sqrt(a*a - y*y)*signe;
    return atan2 (y,x);
}

