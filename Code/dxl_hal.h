// Dynamixel SDK platform dependent header
#ifndef _DYNAMIXEL_HAL_HEADER
#define _DYNAMIXEL_HAL_HEADER
#include <QSerialPort>
#include <QString>

class dxl_hal {
private:
    QSerialPort _serial;
    
    int _time = 30;
    bool _timed = false;
    
public:
    int open(QString &devName, int baudrate );
    void close(void);
    void clear(void);
    int tx( unsigned char *pPacket, int numPacket );
    int rx( unsigned char *pPacket, int numPacket );
    void set_timeout( int NumRcvByte );
    int timeout(void);
};

extern "C" {





}
#endif
