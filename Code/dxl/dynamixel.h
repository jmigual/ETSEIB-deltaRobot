/// @file dynamixel.h Contains the dynamixel class declaration
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


#define PRT1_PKT_ID					(2)
#define PRT1_PKT_LENGTH				(3)
#define PRT1_PKT_INSTRUCTION		(4)
#define PRT1_PKT_ERRBIT				(4)
#define PRT1_PKT_PARAMETER0			(5)


#define INST_PING			(1)
#define INST_READ			(2)
#define INST_WRITE			(3)
#define INST_REG_WRITE		(4)
#define INST_ACTION			(5)
#define INST_RESET			(6)
#define INST_SYNC_WRITE		(131)
#define	INST_BULK_READ      (146)  // 0x92

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
    
    /// Conains the serial port comunication
    dxl_hal dH;
    
    /// Contains all the instructions
    unsigned char gbInstructionPacket[MAXNUM_TXPACKET] = {0};
    
    /// Contains the status
    unsigned char gbStatusPacket[MAXNUM_RXPACKET] = {0};
    
    /// Received packet length
    unsigned int gbRxPacketLength = 0;
    
    /// Temporal length from the received packet
    unsigned int gbRxGetLength = 0;
    
    /// Packet start time
    double gdPacketStartTime = 0.0;
    
    /// Byte transmission time
    double gdByteTransTime = 0.0;
    
    /// Receive wait time
    double gdRcvWaitTime = 0.0;
    
    /// Current communication status
    int gbCommStatus = COMM_RXSUCCESS;
    
    /// True if the bus if being used
    int giBusUsing = 0; 
    
public:
    
    /// Default constructor
    dynamixel() {}
    
    /// Initialization constructor
    dynamixel(QString port_num, int baud_rate = 1000000);
    
    /// Default destructor
    ~dynamixel() { dH.close(); }
    
    /// True if the port is open
    inline bool isOpen() { return dH.isOpen(); }

    /// Initializates the port
    int initialize(QString port_num, int baud_rate);
    
    /// Changes the current baud rate
    int change_baudrate(int baud_rate);
    
    /// Closes the comunication
    int terminate(void);
    
    /// Returns the current com status
    inline int get_comm_result() { return gbCommStatus; }
    
    
    
    /// Sends a packet
    void tx_packet(void);
    
    /// Receives a packet
    void rx_packet(void);
    
    /// Sends and receives a packet
    void txrx_packet(void);
    
    /// Sets the sending packet ID
    void set_txpacket_id(int id);
    
    /// Sets the sending packet instruction
    void set_txpacket_instruction(int instruction);
    
    /// Sets the sending packet parameter
    void set_txpacket_parameter(int index, int value);
    
    /// Sets the sending packet length
    void set_txpacket_length(int length);
    
    /// Returns false if no receive error and true if there's an error
    /// @param error Selects the error to check
    bool get_rxpacket_error(int error);
    
    /// Returns the error byte
    int  get_rxpacket_error_byte(void);
    
    /// Returns the received parameter
    int  get_rxpacket_parameter( int index );
    
    /// Returns the received packet length
    int  get_rxpacket_length();
    
    /// Ping to the selected id, check com status for the ping result
    /// @param id ID where the ping is done
    void ping(int id);
    
    /// Reads a byte from the selected ID at the selected address
    /// @param id Selects the ID to read the byte
    /// @param address Selects the address to read the byte
    int  read_byte(int id, int address);
    
    /// Writes a byte to the selected ID at the selected address
    /// @param id Selects the ID to write the byte
    /// @param address Selects the address to write the byte
    /// @param value Value to set at the selected location
    void write_byte(int id, int address, int value);
    
    /// Reads a word to the selected ID at the selected address
    /// @param id Selects the ID to read the word
    /// @param address Selects the address to read the word
    int  read_word(int id, int address);
    
    /// Writes a word to the selected ID at the selected address
    /// @param id Selects the ID to write the word
    /// @param address Selects the address to write the word
    /// @param value Value to set at the selected location
    void write_word(int id, int address, int value);    
    
    /// Returns the packet time
    double get_packet_time();
    
    /// Sets the timeout in number of received bytes
    /// @param NumRcvByte Number of received bytes to do a timeout
    void set_packet_timeout(int NumRcvByte);
    
    /// Sets the timeout in ms
    /// @param msec Miliseconds for the timeout
    void set_packet_timeout_ms(int msec);
    
    /// Returns true if the packet is timeout
    /// @return True if the packet is timeout
    bool is_packet_timeout();
};

#endif
