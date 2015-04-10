// Dynamixel SDK platform dependent header
#ifndef _DYNAMIXEL_HAL_HEADER
#define _DYNAMIXEL_HAL_HEADER

#include <QDebug>
#include <QSerialPort>
#include <QString>
#include <QTime>

#define MAXNUM_TXPACKET  (10000)
#define MAXNUM_RXPACKET  (10000)

class dxl_hal {
private:
    QSerialPort _serial;
    
    int _time = 30;
    bool _timed = false;
    bool _open = false;
    
public:
    int open(QString &devName, int baudrate );
    void close(void);
    void clear(void);
    int change_baudrate(float baudrate);
    int write( unsigned char *pPacket, int numPacket );
    int read( unsigned char *pPacket, int numPacket );
    double get_curr_time();
    inline bool isOpen() { return _open; }
};
#endif
