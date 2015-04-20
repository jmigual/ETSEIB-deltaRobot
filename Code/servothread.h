/// @file servothread.h Contains the ServoThread class declaration
#ifndef SERVOTHREAD_H
#define SERVOTHREAD_H

// Qt Libraries
#include <QDebug>
#include <QDir>
#include <QMutex>
#include <QStatusBar>
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
    
    /// Enum containing all the save file versions
    enum Version 
    {
        v_1_0
    };
    
public:
    
    /// Struct for the AX12 servos
    struct Servo 
    {
        int ID;         ///< Contains the servo ID
        double load;    ///< Contains the servo load
        double pos;     ///< Contains the servo position
        
        /// Default constructor
        Servo(int ID = -1, double load = -1, double pos = -1) 
            : ID(ID), load(load), pos(pos) {}
        
        /// Copy constructor
        Servo(const Servo &s) : ID(s.ID), load(s.load), pos(s.pos) {}
    };
    
    /// Contains the working mode
    enum Mode 
    {
        controlled,
        manual
    };
    
    /// Default constructor
    ServoThread();
    
    /// Default destructor
    ~ServoThread();
    
    /// Ends the execution
    inline void end()
    {
        _mutex.lock();
        _end = true;
        _cond.wakeOne();
        _mutex.unlock();
        
        wait();
    }
    
    /// Loads the data from the selected file
    void load(QString &file);
    
    /// Pauses the execution
    inline void pause()
    {
        _mutex.lock();
        _pause = true;
        _mutex.unlock();
    }
    
    /// Returns the current servo Baud rate
    inline int getServoBaud()
    {
        QMutexLocker mL(&_mutex);
        return _sBaud;
    }
    
    /// Returns the current servo Port
    inline QString getServoPort()
    {
        QMutexLocker mL(&_mutex);
        return _sPort;
    }
    
    /// Returns both servo Port and baud Rate
    inline void getServoPortInfo(QString &port, int &baud)
    {
        _mutex.lock();
        baud = _sBaud;
        port = _sPort;
        _mutex.unlock();
    }
    
    
    /// Returns the servos info, with all its load and current position
    /// @param V Servo vector to store information
    inline void getServosInfo(QVector<Servo> &V)
    {
        _mutex.lock();
        V = _servos;
        _mutex.unlock();
    }
    
    /// Overloaded function to get the servo info
    inline QVector<Servo> getServosInfo()
    {
        QMutexLocker mL(&_mutex);
        return _servos;
    }
    
    /// Returns the mutex used in the thread
    inline QMutex* mutex() { return &_mutex; }
    
    /// Adds the loaded data
    /// @param aV Contains the axis values
    /// @param buts Contains the buttons values
    void setData(QVector< float > &aV, QVector<bool> &buts);
    
    /// Sets the servos port baud rate
    /// @param baud Positive number containing the baud rate
    void setServoBaud(unsigned int baud)
    {
        _mutex.lock();
        _sBaud = baud;
        _mutex.unlock();
    }
    
    /// Sets the servos port
    /// @param port String containing the port name
    void setServoPort(QString &port)
    {
        _mutex.lock();
        _sPort = port;
        _mutex.unlock();
    }
    
    /// Sets the servos port info, data and selected port
    /// @param port String containing the selected port
    /// @param baud Contains the selected baud rate
    void setServoPortInfo(QString &port, unsigned int baud)
    {
        _mutex.lock();
        _sPort = port;
        _sBaud = baud;
        _mutex.unlock();
    }
    
    /// Sets the servos ID
    /// @param V Vector containing all the servos ID
    inline void setSID (QVector< int > &V) 
    {
        _mutex.lock();
        if (V.size() != _servos.size()) _servos.resize(V.size());
        
        for (int i = 0; i < V.size(); ++i) _servos[i].ID = V[i];
        _dChanged = true;
        _mutex.unlock();
    }
    
    /// Sets the status bar
    /// @param status Pointer to the status bar
    inline void setStatusBar(QStatusBar *status) 
    {
        _statusBar = status;
    }
    
    /// Continues program's execution
    inline void wakeUp()
    {
        _mutex.lock();
        _pause = false;
        _cond.wakeOne();
        _mutex.unlock();
    }
    
    /// Writes data to the selected directory
    /// @param file Path to the file
    void write(QString &file); 
    
private:
    
    const double cos60 = 0.5;       ///< Contains the cosinus of 60
    const double sin60 = sqrt(3)/2; ///< Contains the sinus of 60
    const double a = 17.233;        ///< The arm length
    const double b = 22.648;        ///< The forearm length
    const double L1 = 5.000;        ///< The base center lenght
    const double L2 = 6.000;        ///< The platform center length
    
    /// Contains the axis value
    QVector < float > _axis;
    
    /// Contains the buttons value
    QVector < bool > _buts;
    
    /// Contains the baud rate used to comunicate with the clamp
    int _cBaud;
    
    /// To start and pause the thread
    QWaitCondition _cond;
        
    /// Contains the selected com port used to comunitate with the clamp
    QString _cPort;
    
    /// True if the data changes
    bool _dChanged;
    
    /// True when we must end executino
    bool _end;
    
    /// Contains the working mode
    Mode _mod;
    
    /// To prevent memory errors
    QMutex _mutex;
    
    /// Pauses the execution of the thread
    bool _pause;
    
    /// Contains the used baud rate to comunicate with the servos
    int _sBaud;
    
    /// Contains the servos information
    QVector< Servo > _servos;
    
    /// Contains the selected com port used in the comunication with servos
    QString _sPort;
    
    /// True if the servos port changes
    bool _sPortChanged;
    
    /// Pointer to the window status Bar
    QStatusBar *_statusBar;
    
    /// Used to create another thread
    void run();
    
    /// Used to calculate the servos angles
    void setAngles(double x0, double y0, double z0, 
                   double &theta1, double &theta2, double &theta3);
    
    /// Calculates the angle of one servo in the selected position
    double singleAngle(double x0, double y0, double z0);
    
};

#endif // SERVOTHREAD_H
