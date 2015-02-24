#ifndef AX12_H
#define AX12_H

#include <QObject>
#include "dynamixel.h"

#define P_TORQUE_ENABLE 24
#define P_GOAL_POSITION 30
#define P_PRESENT_POS	36


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
    
    int _ID;
    
public:
    explicit AX12(int ID = -1, QObject *parent = 0);
    ~AX12();
    
signals:
    
public slots:
};

#endif // AX12_H
