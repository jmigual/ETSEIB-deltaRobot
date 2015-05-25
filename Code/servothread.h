/// @file servothread.h Contains the ServoThread class declaration
#ifndef SERVOTHREAD_H
#define SERVOTHREAD_H

// Adding precompiled header
#include "stable.h"

// User libraries
#include "dxl/ax12.h"
#include <QVector>

#undef M_PI
#define M_PI 3.14159265358979323846264338327

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
    
    /// Conains the current status
    enum Status
    {
        waiting,
        working
    };
    
    /// Struct to handle the dominoe pieces
    struct Dominoe
    {
        double X;   ///< X position
        double Y;   ///< Y position
        double ori; ///< Orientation from X = 0 in degrees
        
        bool operator<(const Dominoe &d) const
        {
            if (this->X != d.X) return this->X < d.X;
            return this->Y < d.Y;
        }
        
        /// Initialization constructor
        Dominoe(double X, double Y, double ori) : X(X), Y(Y), ori(ori) {}
    };
    
public:
    
    /// Struct for the AX12 servos
    struct Servo 
    {
        int ID;         ///< Contains the servo ID
        double pos;     ///< Contains the servo position
        
        /// Default constructor
        Servo(int ID = -1, double pos = -1) 
            : ID(ID), pos(pos) {}
        
        /// Copy constructor
        Servo(const Servo &s) : ID(s.ID), pos(s.pos) {}
        
        /// Operator overloading
        void operator=(const Servo &s) 
        {
            this->ID = s.ID;
            this->pos = s.pos;
        }
    };
    
    /// Contains the working mode
    enum Mode 
    {
        Controlled,
        Manual,
        Reset
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
    
    /// Returns the current position
    inline QVector3D getCurrentPos()
    {
        QMutexLocker m(&_mutex);
        return _pos;
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
    
    /// Returns the number of servos to handle
    inline int getServosNum() { return _sNum; }
    
    /// Returns the current speed
    inline int getSpeed()
    {
        QMutexLocker m(&_mutex);
        return _sSpeed;
    }
    
    /// Returns true if the servos are active
    inline bool isActive()
    {
        QMutexLocker m(&_mutex);
        return not _pause;
    }
    
    /// Returns the mutex used in the thread
    inline QMutex* mutex() { return &_mutex; }
    
    /// Pauses the execution
    inline void pause()
    {
        _mutex.lock();
        _pause = true;
        _mutex.unlock();
    }
    
    /// Reads and loads the data from the selected file
    /// @param file Path to the selected file
    void read(QString file);
    
    /// Reads the path where to put the selected pieces
    /// @param file Path to the file where to read the pieces
    void readPath(QString file);
    
    /// Resets to default positions (used when the mode changes or when some
    /// data has changed
    /// @pre The thread is sleeping
    inline void reset() 
    { 
        _mutex.lock();
        _mod = Mode::Reset; 
        _mutex.unlock();
    }
    
    /// Sets the current working mode
    /// @pre The thread must be on pause
    /// @param m Contains the desired working mode
    inline void setMode(Mode m)
    {
        QMutexLocker mut(&_mutex);
        if (!_pause) return;
        _mod = m;
        _dChanged = true;
    }
    
    /// Adds the loaded data
    /// @param aV Contains the axis values
    /// @param buts Contains the buttons values
    void setData(QVector< float > &aV, QVector<bool> &buts);
    
    /// Sets the servos port baud rate
    /// @param baud Positive number containing the baud rate
    inline void setServoBaud(unsigned int baud)
    {
        _mutex.lock();
        _sBaud = baud;
        _dChanged = true;
        _mutex.unlock();
    }
    
    /// Sets the servos port
    /// @param port String containing the port name
    inline void setServoPort(QString &port)
    {
        _mutex.lock();
        _sPort = port;
        _dChanged = true;
        _mutex.unlock();
    }
    
    /// Sets the servos port info, data and selected port
    /// @param port String containing the selected port
    /// @param baud Contains the selected baud rate
    inline void setServoPortInfo(QString &port, unsigned int baud)
    {
        _mutex.lock();
        _sPort = port;
        _sBaud = baud;
        _dChanged = true;
        _mutex.unlock();
    }
    
    
    /// Sets the servos ID
    /// @param V Vector containing all the servos ID
    inline void setSID (QVector< int > &V) 
    {
        // Error passing the data
        if (V.size() != _sNum) {
            qDebug() << "Error setting servos";
            return;
        }
        
        
        _mutex.lock();        
        for (int i = 0; i < V.size(); ++i) _servos[i].ID = V[i];
        _dChanged = true;
        _mutex.unlock();
    }
    
    /// Sets the servos speed
    /// @param speed unsigned char from 0 to 100 containing the % of speed
    inline void setSpeed(unsigned char speed)
    {
        if (speed > 100) speed = 100;
        
        _mutex.lock();
        _sSpeed = speed;
        _dChanged = true;
        _mutex.unlock();
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
    void write(QString file);
    
signals:
    
    /// To show the change of a mode
    void modeChanged(Mode);
    
    /// Emmitted when the status bar must be changed
    void statusBar(QString);
    
private:
    
    const double cos60 = 0.5;       ///< Contains the cosinus of 60
    const double sin60 = sqrt(3)/2; ///< Contains the sinus of 60
    const double a = 17.233;        ///< The arm length
    const double b = 22.648;        ///< The forearm length
    const double L1 = 6.374;        ///< The base center length
    const double L2 = 6.000;        ///< The clamp support center lenght
    const double maxErr = 3.0;      ///< Max available error
    const double minAngle = 60.0;   ///< Minimum servo angle
    const double maxAngle = 240.0;  ///< Maximum servo angle
    const double workRadSq = 144.0; ///< Working radius squared
    
    const uchar ccwCS = 2;         ///< The Counter Clock Wise Compliance Slope
    const uchar cwCS = 2;          ///< The Clock Wise Compliance Slope
    
    static const int _sNum = 4;            ///< Number of servos to manage
    
    /// Contains the axis value
    QVector3D _axis;
    
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
    
    /// Contains all the dominoes information
    QVector< QVector< Dominoe > > _dominoe;
    
    /// True when we must end executino
    bool _end;
    
    /// True if the enter key is pressed
    bool _enter;
    
    /// Contains the working mode
    Mode _mod;
    
    /// To prevent memory errors between threads
    QMutex _mutex;
    
    /// Pauses the execution of the thread
    bool _pause;
    
    /// Contains the current position to show to the window
    QVector3D _pos;
    
    /// Contains the used baud rate to comunicate with the servos
    int _sBaud;
    
    /// Contains the servos information
    QVector< Servo > _servos;
    
    /// Contains the selected com port used in the comunication with servos
    QString _sPort;
    
    /// True if the servos port changes
    bool _sPortChanged;
    
    /// Speed of the robot
    unsigned int _sSpeed;
    
    /// Returns true if the position is available
    bool isPosAvailable(const QVector<Servo> &S, const QVector<double> &D, const QVector3D &newPos, double err);
    
    /// Used to create another thread
    void run();
    
    /// Used to calculate the servos angles
    void setAngles(const QVector3D &pos, 
                   QVector<double> &D);
    
    /// Calculates the angle of one servo in the selected position
    double singleAngle(double x0, double y0, double z0);
    
};

#endif // SERVOTHREAD_H

// TODO: Arreglar canvis besties
// BUG: En el moment de canviar de mode es para el Thread
