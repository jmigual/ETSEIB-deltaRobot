/// @file dynamixel.cpp Contains the dynamixel and dynamixel2 classes 
/// implementation

#include "dynamixel.h"

#define LATENCY_TIME		(16) //ms (USB2Dynamixel Default Latency Time)
#define PING_STATUS_LENGTH  (14)


dynamixel::dynamixel(QString port_num, int baud_rate)
{
    initialize(port_num, baud_rate);
}

int dynamixel::initialize( QString port_num, int baud_rate )
{
	if( baud_rate < 1900 ) return 0;

	if( not dH.open(port_num, baud_rate) ) return false;
    
    // 1000/baudrate(bit per msec) * 10(start bit + data bit + stop bit)
	gdByteTransTime = 1000.0 / (double)baud_rate * 10.0; 

	gbCommStatus = COMM_RXSUCCESS;
	giBusUsing = 0;

	return true;
}

int dynamixel::change_baudrate(int baud_rate )
{
    int result = 0;
    float baudrate = (float)baud_rate;
    
    result = dH.change_baudrate(baudrate);
    if(result == 1)
        gdByteTransTime = 1000.0f / baudrate * 10.0; // 1000/baudrate(bit per msec) * 10(start bit + data bit + stop bit)

    return result;
}

int dynamixel::terminate(void)
{
	dH.close();
	return 0;
}

////////////// methods for timeout //////////////
double dynamixel::get_packet_time(void)
{
	double elapsed_time;

    elapsed_time = (double)(dH.get_curr_time() - gdPacketStartTime);

    // Overflow
    if(elapsed_time < 0) gdPacketStartTime = dH.get_curr_time();
	
	return elapsed_time;
}

void dynamixel::set_packet_timeout(int NumRcvByte)
{
	gdPacketStartTime = dH.get_curr_time();
	gdRcvWaitTime = (gdByteTransTime*(double)NumRcvByte + 2.0*LATENCY_TIME + 2.0);
}

void dynamixel::set_packet_timeout_ms(int msec)
{
	gdPacketStartTime = dH.get_curr_time();
	gdRcvWaitTime = (double)msec;
}

bool dynamixel::is_packet_timeout(void)
{
    if(this->get_packet_time() > gdRcvWaitTime)
        return true;
    return false;
}

///////// 1.0 packet communocation method /////////
void dynamixel::tx_packet(void)
{
	unsigned char pkt_idx = 0;
	unsigned char TxNumByte, RealTxNumByte;
	unsigned char checksum = 0;

	if( giBusUsing == 1 )
	{
		gbCommStatus = COMM_TXFAIL;
		return;
	}
	
	giBusUsing = 1;
	
	if( gbInstructionPacket[PRT1_PKT_INSTRUCTION] != INST_PING
		&& gbInstructionPacket[PRT1_PKT_INSTRUCTION] != INST_READ
		&& gbInstructionPacket[PRT1_PKT_INSTRUCTION] != INST_WRITE
		&& gbInstructionPacket[PRT1_PKT_INSTRUCTION] != INST_REG_WRITE
		&& gbInstructionPacket[PRT1_PKT_INSTRUCTION] != INST_ACTION
		&& gbInstructionPacket[PRT1_PKT_INSTRUCTION] != INST_RESET
		&& gbInstructionPacket[PRT1_PKT_INSTRUCTION] != INST_SYNC_WRITE )
	{
		gbCommStatus = COMM_TXERROR;
		giBusUsing = 0;
		return;
	}
	
	gbInstructionPacket[0] = 0xff;
	gbInstructionPacket[1] = 0xff;
	for( pkt_idx = 0; pkt_idx < (gbInstructionPacket[PRT1_PKT_LENGTH]+1); pkt_idx++ )
		checksum += gbInstructionPacket[pkt_idx+2];
	gbInstructionPacket[gbInstructionPacket[PRT1_PKT_LENGTH]+3] = ~checksum;
	
	//if( gbCommStatus == COMM_RXTIMEOUT || gbCommStatus == COMM_RXCORRUPT )
	//	dH.clear();

	dH.clear();

	TxNumByte = gbInstructionPacket[PRT1_PKT_LENGTH] + 4;
	RealTxNumByte = dH.write( gbInstructionPacket, TxNumByte );

	if( TxNumByte != RealTxNumByte )
	{
		gbCommStatus = COMM_TXFAIL;
		giBusUsing = 0;
		return;
	}

	if( gbInstructionPacket[PRT1_PKT_INSTRUCTION] == INST_READ )
	{
		gbRxPacketLength =  gbInstructionPacket[PRT1_PKT_PARAMETER0+1] + 6;
		set_packet_timeout( gbInstructionPacket[PRT1_PKT_PARAMETER0+1] + 6 );
	}
	else
	{
		gbRxPacketLength = 6;
		set_packet_timeout( 6 );
	}

	gbCommStatus = COMM_TXSUCCESS;
}

void dynamixel::rx_packet(void)
{
	unsigned char i = 0, j = 0, nRead = 0;
	unsigned char checksum = 0;

	if( giBusUsing == 0 )
		return;

	if( gbInstructionPacket[PRT1_PKT_ID] == BROADCAST_ID )
	{
		gbCommStatus = COMM_RXSUCCESS;
		giBusUsing = 0;
		return;
	}
	
	if( gbCommStatus == COMM_TXSUCCESS )
	{
		gbRxGetLength = 0;
		//gbRxPacketLength = 6; //minimum wait length
	}
	
	while(1)
	{
		nRead = dH.read( &gbStatusPacket[gbRxGetLength], gbRxPacketLength - gbRxGetLength );
		gbRxGetLength += nRead;

		if(gbRxGetLength > 4)
			gbRxPacketLength = gbStatusPacket[PRT1_PKT_LENGTH] + 4;

		if( gbRxGetLength < gbRxPacketLength )
		{
			if( is_packet_timeout() == 1 )
			{
				if(gbRxGetLength == 0)
					gbCommStatus = COMM_RXTIMEOUT;
				else
					gbCommStatus = COMM_RXCORRUPT;
				giBusUsing = 0;
				return;
			}
			gbCommStatus = COMM_RXWAITING;
			//return;			
		}
		else
		{
			break;
		}
	}

	// Find packet header
	for( i=0; i<(gbRxGetLength-1); i++ )
	{
		if( gbStatusPacket[i] == 0xff && gbStatusPacket[i+1] == 0xff )
			break;
		else if( i == gbRxGetLength-2 && gbStatusPacket[gbRxGetLength-1] == 0xff )
			break;
		else {
			gbCommStatus = COMM_RXCORRUPT;
			return;
		}
	}

	if( i > 0 )
	{
		for( j=0; j<(gbRxGetLength-i); j++ )
			gbStatusPacket[j] = gbStatusPacket[j + i];
			
		gbRxGetLength -= i;		
	}

	// Check id pairing
	if( gbInstructionPacket[PRT1_PKT_ID] != gbStatusPacket[PRT1_PKT_ID])
	{
		gbCommStatus = COMM_RXCORRUPT;
		giBusUsing = 0;
		return;
	}
	
	// Check checksum
	for( i=0; i<(gbStatusPacket[PRT1_PKT_LENGTH]+1); i++ )
		checksum += gbStatusPacket[i+2];
	checksum = ~checksum;

	if( gbStatusPacket[gbStatusPacket[PRT1_PKT_LENGTH]+3] != checksum )
	{
		gbCommStatus = COMM_RXCORRUPT;
		giBusUsing = 0;
		return;
	}
	
	gbCommStatus = COMM_RXSUCCESS;
	giBusUsing = 0;
}

void dynamixel::txrx_packet(void)
{
	tx_packet();

	if( gbCommStatus != COMM_TXSUCCESS )
		return;	
	
	
	rx_packet();
}

////////////// get/set packet methods /////////////
void dynamixel::set_txpacket_id(int id)
{
	gbInstructionPacket[PRT1_PKT_ID] = (unsigned char)id;
}

void dynamixel::set_txpacket_instruction(int instruction)
{
	gbInstructionPacket[PRT1_PKT_INSTRUCTION] = (unsigned char)instruction;
}

void dynamixel::set_txpacket_parameter(int index, int value)
{
	gbInstructionPacket[PRT1_PKT_PARAMETER0+index] = (unsigned char)value;

}

void dynamixel::set_txpacket_length(int length)
{
	gbInstructionPacket[PRT1_PKT_LENGTH] = (unsigned char)length;
}

bool  dynamixel::get_rxpacket_error(int error)
{
	if( gbStatusPacket[PRT1_PKT_ERRBIT] & (unsigned char)error )
		return true;

	return false;
}

int dynamixel::get_rxpacket_error_byte(void)
{
	return gbStatusPacket[PRT1_PKT_ERRBIT];
}

int  dynamixel::get_rxpacket_parameter( int index )
{
	return (int)gbStatusPacket[PRT1_PKT_PARAMETER0+index];
}

int  dynamixel::get_rxpacket_length()
{
	return (int)gbStatusPacket[PRT1_PKT_LENGTH];
}

void dynamixel::ping( int id )
{
	while(giBusUsing);

	gbInstructionPacket[PRT1_PKT_ID] = (unsigned char)id;
	gbInstructionPacket[PRT1_PKT_INSTRUCTION] = INST_PING;
	gbInstructionPacket[PRT1_PKT_LENGTH] = 2;
	
	txrx_packet();
}

int dynamixel::read_byte( int id, int address )
{
	while(giBusUsing);

	gbInstructionPacket[PRT1_PKT_ID] = (unsigned char)id;
	gbInstructionPacket[PRT1_PKT_INSTRUCTION] = INST_READ;
	gbInstructionPacket[PRT1_PKT_PARAMETER0+0] = (unsigned char)address;
	gbInstructionPacket[PRT1_PKT_PARAMETER0+1] = 1;
	gbInstructionPacket[PRT1_PKT_LENGTH] = 4;
	
	txrx_packet();

	return (int)gbStatusPacket[PRT1_PKT_PARAMETER0];
}

void dynamixel::write_byte( int id, int address, int value )
{
	while(giBusUsing);

	gbInstructionPacket[PRT1_PKT_ID] = (unsigned char)id;
	gbInstructionPacket[PRT1_PKT_INSTRUCTION] = INST_WRITE;
	gbInstructionPacket[PRT1_PKT_PARAMETER0+0] = (unsigned char)address;
	gbInstructionPacket[PRT1_PKT_PARAMETER0+1] = (unsigned char)value;
	gbInstructionPacket[PRT1_PKT_LENGTH] = 4;
	
	txrx_packet();
}

int dynamixel::read_word( int id, int address )
{
	while(giBusUsing);

	gbInstructionPacket[PRT1_PKT_ID] = (unsigned char)id;
	gbInstructionPacket[PRT1_PKT_INSTRUCTION] = INST_READ;
	gbInstructionPacket[PRT1_PKT_PARAMETER0+0] = (unsigned char)address;
	gbInstructionPacket[PRT1_PKT_PARAMETER0+1] = 2;
	gbInstructionPacket[PRT1_PKT_LENGTH] = 4;
	
	txrx_packet();

	return MAKEWORD((int)gbStatusPacket[PRT1_PKT_PARAMETER0+0], (int)gbStatusPacket[PRT1_PKT_PARAMETER0+1]);
}

void dynamixel::write_word( int id, int address, int value )
{
	while(giBusUsing);

	gbInstructionPacket[PRT1_PKT_ID] = (unsigned char)id;
	gbInstructionPacket[PRT1_PKT_INSTRUCTION] = INST_WRITE;
	gbInstructionPacket[PRT1_PKT_PARAMETER0+0] = (unsigned char)address;
	gbInstructionPacket[PRT1_PKT_PARAMETER0+1] = (unsigned char)LOBYTE(value);
	gbInstructionPacket[PRT1_PKT_PARAMETER0+2] = (unsigned char)HIBYTE(value);
	gbInstructionPacket[PRT1_PKT_LENGTH] = 5;
	
	txrx_packet();
}
