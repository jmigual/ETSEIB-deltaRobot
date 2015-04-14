/// @file dynamixel.h Contains the dynamixel and dynamixel2 classes declaration
// Dynamixel SDK platform independent header
#ifndef _DYNAMIXEL_HEADER
#define _DYNAMIXEL_HEADER

#include "dxl_hal.h"

#define MAX_ID				(252)
#define BROADCAST_ID		(254)  //BroadCast ID

/* Communication Errorrobotis1	1 List */
#define	COMM_TXSUCCESS		(0)  //Succeed transmit instruction packet
#define COMM_RXSUCCESS		(1)  //Succeed get status packet
#define COMM_TXFAIL			(2)  //Failed transmit instruction packet
#define COMM_RXFAIL			(3)  //Failed get status packet
#define COMM_TXERROR		(4)  //Incorrect instruction packet
#define COMM_RXWAITING		(5)  //Now recieving status packet
#define COMM_RXTIMEOUT		(6)  //There is no status packet
#define COMM_RXCORRUPT		(7)  //Incorrect status packet

/* Error Status List */
#define ERRBIT_ALERT		(128)//When the device has a problem, it is et as 1. Check "Device Status Check" value.

#define ERR_RESULT_FAIL		(1)  //Failed to process the instruction packet.
#define ERR_INSTRUCTION		(2)  //Instruction error
#define ERR_CRC				(3)  //CRC check error
#define ERR_DATA_RANGE		(4)  //Data range error
#define ERR_DATA_LENGTH		(5)  //Data length error
#define ERR_DATA_LIMIT		(6)  //Data limit error
#define ERR_ACCESS			(7)  //Access error


////////// for Protocol 1.0
#define PRT1_PKT_ID					(2)
#define PRT1_PKT_LENGTH				(3)
#define PRT1_PKT_INSTRUCTION		(4)
#define PRT1_PKT_ERRBIT				(4)
#define PRT1_PKT_PARAMETER0			(5)

////////// for Protocol 2.0
#define PRT2_PKT_HEADER0				(0)
#define PRT2_PKT_HEADER1				(1)
#define PRT2_PKT_HEADER2				(2)
#define PRT2_PKT_RESERVED				(3)
#define PRT2_PKT_ID						(4)
#define PRT2_PKT_LENGTH_L				(5)
#define PRT2_PKT_LENGTH_H				(6)
#define PRT2_PKT_INSTRUCTION			(7)
#define PRT2_INSTRUCTION_PKT_PARAMETER0	(8)
#define PRT2_PKT_ERRBIT					(8)
#define PRT2_STATUS_PKT_PARAMETER0		(9)
//////// Instruction for Dynamixel Protocol ////////
/////// Common Instruction for 1.0 and 2.0
#define INST_PING			(1)
#define INST_READ			(2)
#define INST_WRITE			(3)
#define INST_REG_WRITE		(4)
#define INST_ACTION			(5)
#define INST_RESET			(6)
#define INST_SYNC_WRITE		(131)
#define	INST_BULK_READ      (146)  // 0x92
/////// Added Instruction for 2.0
#define INST_REBOOT         (8)
#define INST_STATUS         (85)   // 0x55
#define INST_SYNC_READ      (130)  // 0x82
#define INST_BULK_WRITE     (147)  // 0x93


#define PING_INFO_MODEL_NUM   (1)
#define PING_INFO_FIRM_VER	  (2)
	
///////////////// utility for value ///////////////////////////
#define MAKEWORD(a, b)      ((unsigned short)(((unsigned char)(((unsigned long)(a)) & 0xff)) | ((unsigned short)((unsigned char)(((unsigned long)(b)) & 0xff))) << 8))
#define MAKEDWORD(a, b)     ((unsigned int)(((unsigned short)(((unsigned long)(a)) & 0xffff)) | ((unsigned int)((unsigned short)(((unsigned long)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((unsigned short)(((unsigned long)(l)) & 0xffff))
#define HIWORD(l)           ((unsigned short)((((unsigned long)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((unsigned char)(((unsigned long)(w)) & 0xff))
#define HIBYTE(w)           ((unsigned char)((((unsigned long)(w)) >> 8) & 0xff))

/// Dynamixel 1.0 protocol class
class dynamixel {
private:
    dxl_hal dH;
    
    unsigned char gbInstructionPacket[MAXNUM_TXPACKET] = {0};
    unsigned char gbStatusPacket[MAXNUM_RXPACKET] = {0};
    unsigned int gbRxPacketLength = 0;
    unsigned int gbRxGetLength = 0;
    
    double gdPacketStartTime = 0.0;
    double gdByteTransTime = 0.0;
    double gdRcvWaitTime = 0.0;
    
    int gbCommStatus = COMM_RXSUCCESS;
    int giBusUsing = 0; 
    
public:
    
    dynamixel();
    
    dynamixel(QString port_num, int baud_rate = 1000000);
    
    inline bool isOpen() { return dH.isOpen(); }
    
    /////////////////// Common Method for 1.0 & 2.0 ///////////////////	
    ////////////// device control method //////////////
    int initialize(QString port_num, int baud_rate);
    int change_baudrate(int baud_rate);
    int terminate(void);
    
    ///////// get communication result method /////////
    inline int get_comm_result() { return gbCommStatus; }
    
    
    
    /////////////////// Dynamixel Protocol 1.0 ///////////////////	
    ///////// 1.0 packet communocation method /////////
    void tx_packet(void);
    void rx_packet(void);
    void txrx_packet(void);
    
    ////////////// get/set packet methods /////////////
    void set_txpacket_id(int id);
    void set_txpacket_instruction(int instruction);
    void set_txpacket_parameter(int index, int value);
    void set_txpacket_length(int length);
    int  get_rxpacket_error(int error);
    int  get_rxpacket_error_byte(void);
    
    int  get_rxpacket_parameter( int index );
    int  get_rxpacket_length();
    
    ///////// high communication method /////////
    void ping(int id);
    int  read_byte(int id, int address);
    void write_byte(int id, int address, int value);
    int  read_word(int id, int address);
    void write_word(int id, int address, int value);
    //////////////////////////////////////////////////////////////
    
    
    
    
    double get_packet_time();
    void set_packet_timeout(int NumRcvByte);
    void set_packet_timeout_ms(int msec);
    int is_packet_timeout();
};

/// Dynamixel 2.0 protocol class
class dynamixel2 {
    
private:
    
    /// Struct used to do a ping
    typedef struct ping_data {
        int iID;
        int iModelNo;
        int iFirmVer;
    } PingData;    
    /// Struct used to handle dynamixel data
    typedef struct data {
        unsigned char   iID;
        unsigned int    iStartAddr;
        unsigned short	iLength;
        unsigned char   iError;
        unsigned char*  pucTable;
    } SyncBulkData;
    
    unsigned char gbInstructionPacket[MAXNUM_TXPACKET] = {0};
    unsigned char gbStatusPacket[MAXNUM_RXPACKET] = {0};
    unsigned int gbRxPacketLength = 0;
    unsigned int gbRxGetLength = 0;
    
    double gdPacketStartTime = 0.0;
    double gdByteTransTime = 0.0;
    double gdRcvWaitTime = 0.0;
    
    int gbCommStatus = COMM_RXSUCCESS;
    int giBusUsing = 0;
    dxl_hal dH; 
    
    PingData		gPingData[MAX_ID + 1];
    SyncBulkData gSyncData[MAX_ID + 1];
    SyncBulkData gBulkData[MAX_ID + 1];
    
public:
    
    inline bool isOpen() { return dH.isOpen(); }
    
    /////////////////// Common Method for 1.0 & 2.0 ///////////////////	
    ////////////// device control method //////////////
    int initialize(QString port_num, int baud_rate);
    int change_baudrate(int baud_rate);
    int terminate(void);
    
    ///////// get communication result method /////////
    int get_comm_result(void);
    
    /////////////////// Dynamixel Protocol 2.0 ///////////////////
    ///////// 1.0 packet communocation method /////////
    void tx_packet(void);
    void rx_packet(void);
    void txrx_packet(void);
    
    ///////// get/set packet methods /////////
    void set_txpacket_id(unsigned char id);
    void set_txpacket_instruction(unsigned char instruction);
    void set_txpacket_parameter(unsigned short index, unsigned char value);
    void set_txpacket_length(unsigned short length);
    int  get_rxpacket_error_byte(void);
    
    int  get_rxpacket_parameter( int index );
    int  get_rxpacket_length();
    
    ///////////// high communication method /////////
    void ping(unsigned char id);
    int  get_ping_result(unsigned char id, int info_num);
    void broadcast_ping();
    
    void reboot(unsigned char id);
    void factory_reset(unsigned char id, int option);
    
    unsigned char  read_byte(unsigned char id, int address);
    void           write_byte(unsigned char id, int address, unsigned char value);
    unsigned short read_word(unsigned char id, int address);
    void           write_word(unsigned char id, int address, unsigned short value);
    unsigned long  read_dword(unsigned char id, int address);
    void           write_dword(unsigned char id, int address, unsigned long value);
    
    //////////////// method for sync/bulk read ////////////////
    unsigned char  get_bulk_read_data_byte(unsigned char id, unsigned int start_address);
    unsigned short get_bulk_read_data_word(unsigned char id, unsigned int start_address);
    unsigned long  get_bulk_read_data_dword(unsigned char id, unsigned int start_address);
                                
    unsigned char  get_sync_read_data_byte(unsigned char id, unsigned int start_address);
    unsigned short get_sync_read_data_word(unsigned char id, unsigned int start_address);
    unsigned long  get_sync_read_data_dword(unsigned char id, unsigned int start_address);
    void add_stuffing();
    void remove_stuffing();
    double get_packet_time();
    int is_packet_timeout();
    void set_packet_timeout(int NumRcvByte);
    void set_packet_timeout_ms(int msec);
};

#endif
