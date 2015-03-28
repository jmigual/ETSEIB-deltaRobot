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

// User libraries
#include "ax12.h"
#include <xjoystick.h>

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
    
    /// Adds the loaded data
    void loadData();
    
private:
    
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
    
    /// Contains the path to the data directory
    QString _writeD;
    
    /// Used to create another thread
    void run();
    
};

// TODO: Create data path

#endif // SERVOTHREAD_H
