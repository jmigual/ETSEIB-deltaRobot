#ifndef SERVOTHREAD_H
#define SERVOTHREAD_H

// Qt Libraries
#include <QDir>
#include <QMutex>
#include <QObject>
#include <QStandardPaths>
#include <QThread>
#include <QVector>
#include <QWaitCondition>

// Other libraries
#include <xjoystick.h>

// User libraries
#include "dxl/ax12.h"

/// The ServoThread's class handles the comunication between the delta robot
/// servos and the PC.
class ServoThread : public QThread
{
    Q_OBJECT
    
public:
    
    /// Default constructor
    ServoThread();
    
    /// Default destructor
    ~ServoThread();
    
    /// Continues program's execution
    inline void cont()
    {
        _mutex.lock();
        _pause = false;
        _cond.wakeOne();
        _mutex.unlock();
    }
    
    /// Ends the execution
    inline void end()
    {
        _mutex.lock();
        _end = true;
        _cond.wakeOne();
        _mutex.unlock();
        
        wait();
    }
    
    /// Adds the loaded data
    void loadData(QVector< float > &aV, QVector<bool> &buts);
    
    /// Pauses the execution
    inline void pause()
    {
        _mutex.lock();
        _pause = true;
        _mutex.unlock();
    }
    
    /// Writes data to the selected directory
    void write(QString &dir); 
    
private:
    
    /// Contains the axis value
    QVector < float > _axis;
    
    /// Contains the buttons value
    QVector < bool > _buts;
    
    /// To start and pause the thread
    QWaitCondition _cond;
    
    /// True when we must end executino
    bool _end;
    
    /// To prevent memory errors
    QMutex _mutex;
    
    /// Pauses the execution of the thread
    bool _pause;
    
    /// Contains the used baud rate to comunicate with the servos
    int _sBaud;
    
    /// Contains the servos used in the robot
    QVector < AX12 > _servos;
    
    /// Contains the selected com port used in the comunication with servos
    QString _sPort;
    
    /// True if the servos port changes
    bool _sPortChanged;
    
    /// Used to create another thread
    void run();
    
};

#endif // SERVOTHREAD_H
