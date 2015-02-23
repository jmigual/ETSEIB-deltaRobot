// Dynamixel SDK platform independent header
#ifndef _DYNAMIXEL_HEADER
#define _DYNAMIXEL_HEADER

#include <QString>
#include "dxl_hal.h"


#define BROADCAST_ID		(254)

#define INST_PING			(1)
#define INST_READ			(2)
#define INST_WRITE			(3)
#define INST_REG_WRITE		(4)
#define INST_ACTION			(5)
#define INST_RESET			(6)
#define INST_SYNC_WRITE		(131)

#define MAXNUM_TXPARAM		(150)

#define ERRBIT_VOLTAGE		(1)
#define ERRBIT_ANGLE		(2)
#define ERRBIT_OVERHEAT		(4)
#define ERRBIT_RANGE		(8)
#define ERRBIT_CHECKSUM		(16)
#define ERRBIT_OVERLOAD		(32)
#define ERRBIT_INSTRUCTION	(64)

#define MAXNUM_RXPARAM		(60)

#define	COMM_TXSUCCESS		(0)
#define COMM_RXSUCCESS		(1)
#define COMM_TXFAIL			(2)
#define COMM_RXFAIL			(3)
#define COMM_TXERROR		(4)
#define COMM_RXWAITING		(5)
#define COMM_RXTIMEOUT		(6)
#define COMM_RXCORRUPT		(7)

class dxl {
private:
    
    unsigned char gbInstructionPacket[MAXNUM_TXPARAM+10] = {0};
    unsigned char gbStatusPacket[MAXNUM_RXPARAM+10] = {0};
    unsigned char gbRxPacketLength = 0;
    unsigned char gbRxGetLength = 0;
    int gbCommStatus = COMM_RXSUCCESS;
    int giBusUsing = 0;
    dxl_hal dH;    
    
public:
    ///////////// device control methods ////////////////////////
    int initialize(QString &devName, int baudrate );
    void terminate(void);
    
    
    ///////////// set/get packet methods //////////////////////////
    void set_txpacket_id( int id );
    
    
    void set_txpacket_instruction( int instruction );
    
    
    
    void set_txpacket_parameter( int index, int value );
    void set_txpacket_length( int length );
    
    int get_rxpacket_error( int errbit );

    int get_rxpacket_parameter( int index );
    int get_rxpacket_length(void);
    
    // utility for value
    int makeword( int lowbyte, int highbyte );
    int get_lowbyte( int word );
    int get_highbyte( int word );
    
    
    ////////// packet communication methods ///////////////////////
    void tx_packet(void);
    void rx_packet(void);
    void txrx_packet(void);
    
    int get_result(void);
    
    
    //////////// high communication methods ///////////////////////
    void ping( int id );
    int read_byte( int id, int address );
    void write_byte( int id, int address, int value );
    int read_word( int id, int address );
    void write_word( int id, int address, int value );
};

extern "C" {





}

#endif
