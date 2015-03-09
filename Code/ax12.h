#ifndef AX12_H
#define AX12_H

#include <QObject>
#include <algorithm>
#include "dynamixel.h"

#define P_TORQUE_ENABLE 24
#define P_GOAL_POSITION 30
#define P_PRESENT_POS	36

/// @brief The AX12 class is used to control AX-12 motors from Dynamixel
class AX12 : public QObject
{
    Q_OBJECT
private:
    enum ROM {
        ModelNumber         = 0,
        VersionFirmware     = 2,
        ID                  = 3,
        BaudRate            = 4,
        ReturnDelayTime     = 5,
        CWAngleLimit        = 6,
        CCWAngleLimit       = 8,
        HighestLimitTemp    = 11,
        LowestLimitVoltage  = 12,
        HighestLimitVoltage = 13,
        MaxTorque           = 14,
        StatusReturnLevel   = 16,
        AlarmLED            = 17,
        AlarmShutdown       = 18
    };
    
    enum RAM {
        TorqueEnable        = 24,
        LED                 = 25,
        CWComplianceMargin  = 26,
        CCWComplianceMargin = 27,
        CWComplianceSlope   = 28,
        CCWComplianceSlope  = 29,
        GoalPosition        = 30,
        MovingSpeed         = 32,
        TorqueLimit         = 34,
        PresentPosition     = 36,
        PresentSpeed        = 38,
        PresentLoad         = 40,
        PresentVoltage      = 42,
        PresentTemperature  = 43,
        Registered          = 44,
        Moving              = 46,
        Lock                = 47,
        Punch               = 48
        
    };    
    
    /// Refrence to a USB2Dynamixel controler
    dynamixel &_dxl;
    
    /// Stores the current ID
    int _ID;
    
    /// True if we use the joint mode
    bool _mode;
    
    /// True if all instructions must be done at the moment
    bool _writeInstant;
    
public:
    
    /// Default constructor must pass an initialized dynamixel object
    /// if ID == -1 no action is done
    explicit AX12(dynamixel &dxl, int ID = -1, QObject *parent = 0);
    
    /// Default destructor
    ~AX12();
    
    /// Returns the current load from -100% to 100%, 100% is ClockWise 
    /// and -100% is CounterClockWise
    double getCurrentLoad();
    
    /// Returns the current position from 0º to 300º
    double getCurrentPos();
    
    /// Returns the current Temperature in Celsius
    int getCurrentTemp();
    
    /// Returns the current speed from -100% to 100%, 100% is ClockWise
    /// and -100% is CounterClockWise
    double getCurrentSpeed();
    
    /// Returns the current voltage in Volts
    double getCurrentVoltage();
    
    /// To get the current ID
    inline int getID() { return _ID; }
    
    /// Sets the Goal's position (in degrees) or speed depending on the mode
    void setGoalPosition(double goal);
    
    /// To set a new ID
    void setID(int ID);
    
    /// To set Joint/Wheel mode, true if Joint
    void setJointMode(bool mode);
    
    /// To set the minimum and maximum angle from 0 to 300º
    void setMinMax(double min, double max);
    
    /// To set if a instruction must be done at the moment or later
    /// <em>TRUE</em> to do instantly and <em>FALSE</em> to do it later.
    inline void setWriteInstant(bool inst) { _writeInstant = inst; }
    
    
signals:
    
public slots:
};

#endif // AX12_H
