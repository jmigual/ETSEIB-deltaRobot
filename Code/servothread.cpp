#include "servothread.h"

ServoThread::ServoThread() :
    _end(false),
    _sBaud(1000000),
    _sPort("COM3")
{
    
    // Searching for the system's writable location
    QStandardPaths::StandardLocation loc = QStandardPaths::AppLocalDataLocation;
    _writeD = QStandardPaths::writableLocation(loc);
    
    // Checking if the path exists
    QDir data(_writeD);
    if (!data.exists()) data.mkpath(_writeD);
}

ServoThread::~ServoThread()
{
    _mutex.lock();
    _end = true;
    _cond.wakeOne();
    _mutex.unlock();
    
    wait();
}

void ServoThread::loadData()
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

