/// @file ax12.h
/// Contains the AX12 class declaration
#ifndef AX12_H
#define AX12_H

// Standard variables
#define M_PI 3.14159265358979323846264338327

// Qt libraries
#include <QVector>

// User librarires
#include "dynamixel.h"



/// @brief The AX12 class is used to control AX-12 motors from Dynamixel
class AX12
{
    
private: 
    
    /// Contains the dynamixel comunication
    dynamixel *_dxl;
    
    /// Stores the current ID
    int _ID;
    
    /// True if we use the joint mode
    bool _mode;
    
    /// True if the angle is returned in radians
    bool _rads;
    
public:
    
    /// Contains all the EEPROM directions enumeration
    enum ROM 
    {
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
    
    /// Contains all the RAM directions enumerations
    enum RAM 
    {
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
    
    /// Default constructor
    AX12();
    
    /// Initializator constructor
    /// if ID == -1 no action is done
    AX12(dynamixel *_dxl, int ID = -1);
    
    /// Copy constructor
    AX12(const AX12 &a);
    
    /// Default destructor
    ~AX12();
    
    /// Returns all active servos;
    QVector< int > connectedID();
    
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
    
    /// Sets the compliance slope
    /// @param ccw Counter Clock Wise Compliance Slope
    /// @param cw Clock Wise Compliance Slope
    void setComplianceSlope(uchar ccw, uchar cw);
    
    /// Sets the dynamixel interface
    /// @param dxl Pointer to the dynamixel control class
    inline void setDxl(dynamixel *dxl) { _dxl = dxl; }
    
    /// Sets the Goal's position (in degrees) or speed depending on the mode
    /// @param goal Position (in degrees if not radian mode) or % speed if 
    /// used wheel mode
    void setGoalPosition(double goal);
    
    /// To set a new ID
    /// @param ID the new ID
    void setID(int ID);
    
    /// To set Joint/Wheel mode
    /// @param mode True if Joint and false if Wheel mode
    void setJointMode(bool mode);
    
    /// To set the minimum and maximum angle from 0 to 300º
    /// @param min Minimum value from servo
    /// @param max Maximum value from servo
    void setMinMax(double min, double max);
    
    /// Sets the radians mode
    /// @param rads True if radians mode is used
    inline void setRadians(bool rads) { _rads = rads; }
    
    /// To set the maximum speed from 0% to 100% if joint mode or from -100%
    /// to 100% if wheel mode
    void setSpeed(double speed);
    
};

#endif // AX12_H
