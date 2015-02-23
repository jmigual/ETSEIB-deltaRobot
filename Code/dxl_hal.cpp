// Dynamixel SDK platform dependent source
#include "dxl_hal.h"


int dxl_hal::open(QString &devName, int baudrate )
{
    // Opening device
	// devIndex: Device index
	// baudrate: Real baudrate (ex> 115200, 57600, 38400...)
	// Return: 0(Failed), 1(Succeed)
    
    _serial.setPortName(devName);
    _serial.setBaudRate(qint32(baudrate));
    _serial.setDataBits(QSerialPort::Data8);
    _serial.setParity(QSerialPort::NoParity);
    _serial.setStopBits(QSerialPort::OneStop);
    _serial.setFlowControl(QSerialPort::NoFlowControl);
    if(not _serial.open(QIODevice::ReadWrite)) return 0;
    

}

void dxl_hal::close()
{
	// Closing device
    _serial.close();
}

void dxl_hal::clear(void)
{
	// Clear communication buffer
    _serial.clear();

}

int dxl_hal::tx( unsigned char *pPacket, int numPacket )
{
	// Transmiting date
	// *pPacket: data array pointer
	// numPacket: number of data array
	// Return: number of data transmitted. -1 is error.
    _timed = false;
    if (_serial.isOpen()) {
        int n = _serial.write((char*)pPacket, numPacket);
        _timed = _serial.waitForBytesWritten(_time);
        _timed = not _timed;
        return n;
    }
    else return -1;

}

int dxl_hal::rx( unsigned char *pPacket, int numPacket )
{
	// Recieving date
	// *pPacket: data array pointer
	// numPacket: number of data array
	// Return: number of data recieved. -1 is error.
    _timed = false;
    if (_serial.isOpen()) {
        int n = _serial.read((char*)pPacket, numPacket);
        _timed = _serial.waitForReadyRead(_time);
        _timed = not _timed;
        return n;
    }
    else return -1;

}

void dxl_hal::set_timeout( int NumRcvByte )
{
	// Start stop watch
	// NumRcvByte: number of recieving data(to calculate maximum waiting time)
    _time = NumRcvByte;
}

int dxl_hal::timeout(void)
{
	// Check timeout
	// Return: 0 is false, 1 is true(timeout occurred)
    return int(_timed);

}
