/// @file dynamixel.cpp
/// Contains the dynamixel and dynamixel2 classes implementation

#include "dynamixel.h"

#define LATENCY_TIME		(16) //ms (USB2Dynamixel Default Latency Time)
#define PING_STATUS_LENGTH  (14)



/////////////////// Dynamixel Protocol 1.0 ///////////////////
////////////// device control method //////////////
dynamixel::dynamixel()
{
    
}

dynamixel::dynamixel(QString port_num, int baud_rate)
{
    initialize(port_num, baud_rate);
}

int dynamixel::initialize( QString port_num, int baud_rate )
{
	unsigned int idx = 0;

	if( baud_rate < 1900 )
		return 0;

	if( dH.open(port_num, baud_rate) == 0 )
		return 0;

	gdByteTransTime = 1000.0 / (double)baud_rate * 10.0; // 1000/baudrate(bit per msec) * 10(start bit + data bit + stop bit)


	for(idx = 1; idx <= MAX_ID; idx++)
	{
		gSyncData[idx].iID		  = idx;
		gSyncData[idx].iStartAddr = 1;
		gSyncData[idx].iLength	  = 1;
		gSyncData[idx].iError	  = 0;
		gSyncData[idx].pucTable	  = 0;

		gBulkData[idx].iID		  = idx;
		gBulkData[idx].iStartAddr = 1;
		gBulkData[idx].iLength	  = 1;
		gBulkData[idx].iError	  = 0;
		gBulkData[idx].pucTable	  = 0;

		gPingData[idx].iID = idx;
		gPingData[idx].iFirmVer = -1;
		gPingData[idx].iModelNo = -1;
	}

	gbCommStatus = COMM_RXSUCCESS;
	giBusUsing = 0;

	return 1;
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
	int id = 0;
	for(id = 0; id <= MAX_ID; id++)
	{
		if(gBulkData[id].pucTable != 0)
			free((gBulkData[id].pucTable));

		if(gSyncData[id].pucTable != 0)
			free((gBulkData[id].pucTable));
	}
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

int dynamixel::is_packet_timeout(void)
{
    if(this->get_packet_time() > gdRcvWaitTime)
        return 1;
    return 0;
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

int  dynamixel::get_rxpacket_error(int error)
{
	if( gbStatusPacket[PRT1_PKT_ERRBIT] & (unsigned char)error )
		return 1;

	return 0;
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


/////////////////// Dynamixel Protocol 1.0 ///////////////////
////////////// device control method //////////////
int dynamixel2::initialize( QString port_num, int baud_rate )
{
	unsigned int idx = 0;

	if( baud_rate < 1900 )
		return 0;

	if( dH.open(port_num, baud_rate) == 0 )
		return 0;

	gdByteTransTime = 1000.0 / (double)baud_rate * 10.0; // 1000/baudrate(bit per msec) * 10(start bit + data bit + stop bit)


	for(idx = 1; idx <= MAX_ID; idx++)
	{
		gSyncData[idx].iID		  = idx;
		gSyncData[idx].iStartAddr = 1;
		gSyncData[idx].iLength	  = 1;
		gSyncData[idx].iError	  = 0;
		gSyncData[idx].pucTable	  = 0;

		gBulkData[idx].iID		  = idx;
		gBulkData[idx].iStartAddr = 1;
		gBulkData[idx].iLength	  = 1;
		gBulkData[idx].iError	  = 0;
		gBulkData[idx].pucTable	  = 0;

		gPingData[idx].iID = idx;
		gPingData[idx].iFirmVer = -1;
		gPingData[idx].iModelNo = -1;
	}

	gbCommStatus = COMM_RXSUCCESS;
	giBusUsing = 0;

	return 1;
}

int dynamixel2::change_baudrate(int baud_rate )
{
    int result = 0;
    float baudrate = (float)baud_rate;
    
    result = dH.change_baudrate(baudrate);
    if(result == 1)
        gdByteTransTime = 1000.0f / baudrate * 10.0; // 1000/baudrate(bit per msec) * 10(start bit + data bit + stop bit)

    return result;
}

int dynamixel2::terminate(void)
{
	int id = 0;
	for(id = 0; id <= MAX_ID; id++)
	{
		if(gBulkData[id].pucTable != 0)
			free((gBulkData[id].pucTable));

		if(gSyncData[id].pucTable != 0)
			free((gBulkData[id].pucTable));
	}
	dH.close();
	return 0;
}

////////////// methods for timeout //////////////
double dynamixel2::get_packet_time(void)
{
	double elapsed_time;

    elapsed_time = (double)(dH.get_curr_time() - gdPacketStartTime);

    // Overflow
    if(elapsed_time < 0)
        gdPacketStartTime = dH.get_curr_time();
	
	return elapsed_time;
}

int dynamixel2::is_packet_timeout(void)
{
    if(this->get_packet_time() > gdRcvWaitTime)
        return 1;
    return 0;
}

void dynamixel2::set_packet_timeout(int NumRcvByte)
{
	gdPacketStartTime = dH.get_curr_time();
	gdRcvWaitTime = (gdByteTransTime*(double)NumRcvByte + 2.0*LATENCY_TIME + 2.0);
}

void dynamixel2::set_packet_timeout_ms(int msec)
{
	gdPacketStartTime = dH.get_curr_time();
	gdRcvWaitTime = (double)msec;
}


/////////////////// Dynamixel Protocol 2.0 ///////////////////
unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size)
{
	unsigned short i, j;
	unsigned short crc_table[256] = {0x0000,
	                                0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
	                                0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027,
	                                0x0022, 0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D,
	                                0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B,
	                                0x004E, 0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9,
	                                0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF,
	                                0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 0x00A0, 0x80A5,
	                                0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093,
	                                0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
	                                0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197,
	                                0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE,
	                                0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB,
	                                0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9,
	                                0x01C8, 0x81CD, 0x81C7, 0x01C2, 0x0140, 0x8145, 0x814F,
	                                0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176,
	                                0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 0x8123,
	                                0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
	                                0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104,
	                                0x8101, 0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D,
	                                0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B,
	                                0x032E, 0x0324, 0x8321, 0x0360, 0x8365, 0x836F, 0x036A,
	                                0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C,
	                                0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5,
	                                0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3,
	                                0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
	                                0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7,
	                                0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E,
	                                0x0384, 0x8381, 0x0280, 0x8285, 0x828F, 0x028A, 0x829B,
	                                0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9,
	                                0x02A8, 0x82AD, 0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC,
	                                0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5,
	                                0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243,
	                                0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
	                                0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264,
	                                0x8261, 0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E,
	                                0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208,
	                                0x820D, 0x8207, 0x0202 };

	for(j = 0; j < data_blk_size; j++)
	{
		i = ((unsigned short)(crc_accum >> 8) ^ *data_blk_ptr++) & 0xFF;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
	}

	return crc_accum;
}

void dynamixel2::add_stuffing()
{
    int i = 0, index = 0;
	int packet_length_in = MAKEWORD(gbInstructionPacket[PRT2_PKT_LENGTH_L], gbInstructionPacket[PRT2_PKT_LENGTH_H]);
    int packet_length_out = packet_length_in;
    unsigned char temp[MAXNUM_TXPACKET] = {0};

	memcpy(temp, gbInstructionPacket, PRT2_PKT_LENGTH_H+1);    // FF FF FD XX ID LEN_L LEN_H
	index = PRT2_PKT_INSTRUCTION;
	for( i = 0; i < packet_length_in - 2; i++)  // except CRC
	{
		if((index - 1) == MAXNUM_TXPACKET) {
			gbCommStatus = COMM_TXERROR;
			return;
		}
		temp[index++] = gbInstructionPacket[i+PRT2_PKT_INSTRUCTION];

		if(gbInstructionPacket[i+PRT2_PKT_INSTRUCTION] == 0xFD && gbInstructionPacket[i+PRT2_PKT_INSTRUCTION-1] == 0xFF && gbInstructionPacket[i+PRT2_PKT_INSTRUCTION-2] == 0xFF)
		{   
			if((index - 1) == MAXNUM_TXPACKET) {
				gbCommStatus = COMM_TXERROR;
				return;
			}
			// FF FF FD
			temp[index++] = 0xFD;

			packet_length_out++;
		}
	}

	if((index - 1) == MAXNUM_TXPACKET) {
		gbCommStatus = COMM_TXERROR;
		return;
	}
	temp[index++] = gbInstructionPacket[PRT2_PKT_INSTRUCTION+packet_length_in-2];

	if((index - 1) == MAXNUM_TXPACKET) {
		gbCommStatus = COMM_TXERROR;
		return;
	}
	temp[index++] = gbInstructionPacket[PRT2_PKT_INSTRUCTION+packet_length_in-1];

	memcpy(gbInstructionPacket, temp, index);
	gbInstructionPacket[PRT2_PKT_LENGTH_L] = LOBYTE(packet_length_out);
	gbInstructionPacket[PRT2_PKT_LENGTH_H] = HIBYTE(packet_length_out);
}

void dynamixel2::remove_stuffing()
{
    int i = 0, index = 0;
    int packet_length_in = MAKEWORD(gbInstructionPacket[PRT2_PKT_LENGTH_L], gbInstructionPacket[PRT2_PKT_LENGTH_H]);
    int packet_length_out = packet_length_in;

    index = PRT2_PKT_INSTRUCTION;
    for( i = 0; i < packet_length_in - 2; i++)  // except CRC
    {
		if(gbInstructionPacket[i+PRT2_PKT_INSTRUCTION] == 0xFD && gbInstructionPacket[i+PRT2_PKT_INSTRUCTION+1] == 0xFD && gbInstructionPacket[i+PRT2_PKT_INSTRUCTION-1] == 0xFF && gbInstructionPacket[i+PRT2_PKT_INSTRUCTION-2] == 0xFF)
        {   // FF FF FD FD
            packet_length_out--;
            i++;
        }
        gbInstructionPacket[index++] = gbInstructionPacket[i+PRT2_PKT_INSTRUCTION];
    }
    gbInstructionPacket[index++] = gbInstructionPacket[PRT2_PKT_INSTRUCTION+packet_length_in-2];
    gbInstructionPacket[index++] = gbInstructionPacket[PRT2_PKT_INSTRUCTION+packet_length_in-1];

    gbInstructionPacket[PRT2_PKT_LENGTH_L] = LOBYTE(packet_length_out);
    gbInstructionPacket[PRT2_PKT_LENGTH_H] = HIBYTE(packet_length_out);
}

///////// 2.0 packet communocation method /////////
void dynamixel2::tx_packet(void)
{
	int packet_tx_len, real_tx_len;
	int length;
	unsigned short crc = 0;


	// Check Bus Using
	if(giBusUsing == 1)
	{
		gbCommStatus = COMM_TXFAIL;
		return;
	}
	giBusUsing = 1;

	// Character stuffing
	add_stuffing();
	if(gbCommStatus == COMM_TXERROR)
		return;

	length = MAKEWORD(gbInstructionPacket[PRT2_PKT_LENGTH_L], gbInstructionPacket[PRT2_PKT_LENGTH_H]);

    // Check MAX packet length
    if(length > (MAXNUM_TXPACKET))
    {
        giBusUsing = 0;
		gbCommStatus = COMM_TXERROR;
		return;
    }

    // Packet Header
    gbInstructionPacket[PRT2_PKT_HEADER0]   = 0xFF;
    gbInstructionPacket[PRT2_PKT_HEADER1]   = 0xFF;
    gbInstructionPacket[PRT2_PKT_HEADER2]   = 0xFD;
    gbInstructionPacket[PRT2_PKT_RESERVED]  = 0x00; // RESERVED

    // Add CRC16
    crc = update_crc(0, gbInstructionPacket, length+PRT2_PKT_LENGTH_H+1-2);  // -2 : except CRC16
    gbInstructionPacket[length+PRT2_PKT_LENGTH_H-1] = LOBYTE(crc);     // last - 1
    gbInstructionPacket[length+PRT2_PKT_LENGTH_H-0] = HIBYTE(crc);     // last - 0

    // Tx Packet
    dH.clear();
    packet_tx_len = length + PRT2_PKT_LENGTH_H + 1;
    real_tx_len = dH.write(gbInstructionPacket, packet_tx_len );
    if( packet_tx_len != real_tx_len )
    {
        giBusUsing = 0;
		gbCommStatus = COMM_TXFAIL;
		return;
    }
	
	gbCommStatus = COMM_TXSUCCESS;
}

void dynamixel2::rx_packet(void)
{
    //int rx_length = 0, wait_length = PRT2_PKT_LENGTH_H + 4 + 1;    // 4 : INST ERROR CHKSUM_L CHKSUM_H
	unsigned int i;
    unsigned short crc = 0;

	gbRxGetLength = 0; gbRxPacketLength = PRT2_PKT_LENGTH_H + 4 + 1;

    // Check Bus Using
    //if(bus_using == 0)
    //    return 0;

    while(1)
    {
		gbRxGetLength += dH.read( &gbStatusPacket[gbRxGetLength], gbRxPacketLength - gbRxGetLength);
        if(gbRxGetLength >= gbRxPacketLength)    // wait_length minimum : 11
        {
            // Find packet header
            for(i = 0; i < (gbRxGetLength - 2); i++)
            {
                if(gbStatusPacket[i] == 0xFF && gbStatusPacket[i+1] == 0xFF && gbStatusPacket[i+2] == 0xFD)
                    break;
            }

            if(i == 0)
            {
                // Check length
                gbRxPacketLength = MAKEWORD(gbStatusPacket[PRT2_PKT_LENGTH_L], gbStatusPacket[PRT2_PKT_LENGTH_H]) + PRT2_PKT_LENGTH_H + 1;
                if(gbRxGetLength < gbRxPacketLength)
                {
                    // Check timeout
                    if(is_packet_timeout() == 1)
                    {
                        if(gbRxGetLength == 0)
                            gbCommStatus = COMM_RXTIMEOUT;
                        else
                            gbCommStatus = COMM_RXCORRUPT;
                        giBusUsing = 0;
                        break;
                    }
                    continue;
                }

                // Check CRC16
                crc = MAKEWORD(gbStatusPacket[gbRxPacketLength-2], gbStatusPacket[gbRxPacketLength-1]);
                if(update_crc(0, gbStatusPacket, gbRxPacketLength-2) == crc) // -2 : except CRC16
                    gbCommStatus = COMM_RXSUCCESS;
                else
                    gbCommStatus = COMM_RXCORRUPT;
                giBusUsing = 0;
                break;
            }
            else
            {
                // Remove unnecessary packets
                memmove(&gbStatusPacket[0], &gbStatusPacket[i], gbRxGetLength - i);
                gbRxGetLength -= i;
            }
        }
        else
        {
            // Check timeout
            if(is_packet_timeout() == 1)
            {
                if(gbRxGetLength == 0)
                    gbCommStatus = COMM_RXTIMEOUT;
                else
					gbCommStatus = COMM_RXCORRUPT;
                giBusUsing = 0;
                break;
            }
        }
    }

    // Character stuffing
    if(gbCommStatus == COMM_RXSUCCESS)
        remove_stuffing();

    giBusUsing = 0;
}

void dynamixel2::txrx_packet(void)
{
	int n = 0, num = 0;
	int id = 0;
	int wait_length = 0;
	int data_length = 0;
	gbCommStatus = COMM_TXFAIL;
	
    // Wait for Bus Idle
    while(giBusUsing == 1) 
    {
        //Sleep(0);
    }
																		
	if( ( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_PING		 )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_READ		 )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_WRITE		 )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_REG_WRITE	 )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_ACTION		 )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_RESET		 )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_SYNC_WRITE	 )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_BULK_READ    )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_REBOOT       )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_STATUS       )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_SYNC_READ    )  &&
		( gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_BULK_WRITE   ) )
	{
		gbCommStatus = COMM_TXERROR;
		return;
	}

    

	//if( (gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_SYNC_READ) && (gbInstructionPacket[PRT2_PKT_INSTRUCTION] != INST_BULK_READ) )
	if( (gbInstructionPacket[PRT2_PKT_ID] != BROADCAST_ID) )
	{
		tx_packet();
		// Check Tx packet result
		if( gbCommStatus != COMM_TXSUCCESS )
			return;

		// Set Rx Timeout
		if(gbInstructionPacket[PRT2_PKT_INSTRUCTION] == INST_READ)
			set_packet_timeout(MAKEWORD(gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+2], gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+3]) + 11);
		else
			set_packet_timeout(PRT2_PKT_LENGTH_H+4+1);    // 4 : INST ERROR CHKSUM_L CHKSUM_H

		//// BroadCast ID && !BulkRead = There is no need to wait for a rxpacket
		//if(gbInstructionPacket[PRT2_PKT_ID] == BROADCAST_ID)
		//{
		//	giBusUsing = 0;
		//	gbCommStatus = COMM_RXSUCCESS;
		//	return;
		//}

		rx_packet();
		if((gbCommStatus == COMM_RXSUCCESS) && (gbStatusPacket[PRT2_PKT_ID] != BROADCAST_ID) && (gbInstructionPacket[PRT2_PKT_ID] != gbStatusPacket[PRT2_PKT_ID]))
			rx_packet();
	}
	else
	{
		if(gbInstructionPacket[PRT2_PKT_INSTRUCTION] == INST_BULK_READ )
		{
			num = (MAKEWORD(gbInstructionPacket[PRT2_PKT_LENGTH_L], gbInstructionPacket[PRT2_PKT_LENGTH_H]) - 3 )/5;
			for(n = 0; n < num; n++)
			{
				id = gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + n*5];
				gBulkData[id].iError = -1;
				gBulkData[id].iStartAddr = MAKEWORD(gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + n*5 + 1], 
														gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + n*5 + 2]);
				gBulkData[id].iLength	 = MAKEWORD(gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + n*5 + 3], 
													    gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + n*5 + 4]);

				if(gBulkData[id].pucTable != 0)
					free((gBulkData[id].pucTable));

				gBulkData[id].pucTable = (unsigned char *)calloc(gBulkData[id].iLength, sizeof(unsigned char));
				wait_length += gBulkData[id].iLength + 11;
			}

			while(giBusUsing == 1) 
			{
				//Sleep(0);
			}
			tx_packet();
			if( gbCommStatus != COMM_TXSUCCESS )
				return;
			
			set_packet_timeout(wait_length);

			for(n = 0; n < num; n++)
			{
				id = gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + n*5];
				// Rx packet
				rx_packet();
				if(gbCommStatus == COMM_RXSUCCESS)
					gBulkData[id].iError = gbStatusPacket[PRT2_PKT_ERRBIT];
				// rxpacket to rxdata[id]->pucTable
				memcpy(gBulkData[id].pucTable, &gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0], gBulkData[id].iLength);
			}
		}
		else if(gbInstructionPacket[PRT2_PKT_INSTRUCTION] == INST_SYNC_READ)
		{
			num = (MAKEWORD(gbInstructionPacket[PRT2_PKT_LENGTH_L], gbInstructionPacket[PRT2_PKT_LENGTH_H]) - 3 - 4); //3 : INST  CRC_L CRC H, 4 : param0->addr_l param0->addr_h param0->length_l param0->length_h 
			data_length = MAKEWORD(gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+2], 
				gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+3]);


			for(n = 0; n < num; n++)
			{
				id = gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + 4 + n];
				gSyncData[id].iID = id;
				gSyncData[id].iStartAddr = MAKEWORD(gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + 0], 
														gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + 1]);
				gSyncData[id].iError = -1;
				if(gSyncData[id].pucTable != 0)
					free((gSyncData[id].pucTable));

				gSyncData[id].pucTable = (unsigned char*)calloc(data_length, sizeof(unsigned char));
			}


			wait_length  = 11 + data_length;
			wait_length *= num;

			while(giBusUsing == 1);

			tx_packet();

			// Check Tx packet result
			if( gbCommStatus != COMM_TXSUCCESS )
				return;

			// Set Rx Timeout (SYNK_READ)
			set_packet_timeout(wait_length);

			for(n = 0; n < num; n++)
			{
				id = gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0 + 4 + n];
				// Rx packet
				rx_packet();
				if(gbCommStatus == COMM_RXSUCCESS)
					gSyncData[id].iError = gbStatusPacket[PRT2_PKT_ERRBIT];
				// rxpacket to rxdata[id]->pucTable
				memcpy(gSyncData[id].pucTable, &gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0], data_length);
			}

			return;
		}
		else if(gbInstructionPacket[PRT2_PKT_INSTRUCTION] == INST_PING)
		{
			int rx_length = 0;
			tx_packet();
			if(gbCommStatus != COMM_TXSUCCESS)
			{
				giBusUsing = 0;
				return;
			}

			wait_length = PING_STATUS_LENGTH * MAX_ID;
			set_packet_timeout_ms((int)((gdByteTransTime * wait_length) + (3 * MAX_ID) + 2 * LATENCY_TIME));

			while(1)
			{
				int _cnt = dH.read(&gbStatusPacket[rx_length], wait_length - rx_length);
				if(_cnt > 0)
				{
					rx_length += _cnt;
					//printf("cnt : %d, Interval : %f / Wait time : %f\n", _cnt,  get_packet_time(), gdPacketWaitTime);
				}
				if(is_packet_timeout() == 1 || rx_length >= wait_length)
					break;
			}
			giBusUsing = 0;

			if(rx_length== 0)
			{
				gbCommStatus = COMM_RXTIMEOUT;
				return;
			}

			while(1)
			{
				int idx = 0;

				if(rx_length < PING_STATUS_LENGTH)
				{
					gbCommStatus = COMM_RXCORRUPT;
					return;
				}

				// find packet header
				while( idx < (rx_length - 2) )
				{
					if(gbStatusPacket[idx] == 0xFF && gbStatusPacket[idx + 1] == 0xFF && gbStatusPacket[idx + 2] == 0xFD)
						break;
					else
						idx++;
				}

				if(idx == 0)
				{
					// check CRC16
					int crc = MAKEWORD(gbStatusPacket[PING_STATUS_LENGTH - 2], gbStatusPacket[PING_STATUS_LENGTH - 1]);
					if(update_crc(0, gbStatusPacket, PING_STATUS_LENGTH - 2) == crc) // - 2 : except CRC16
					{
						gPingData[gbStatusPacket[PRT2_PKT_ID]].iID = gbStatusPacket[PRT2_PKT_ID];
						gPingData[gbStatusPacket[PRT2_PKT_ID]].iModelNo = MAKEWORD(gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0], gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0+1]);
						gPingData[gbStatusPacket[PRT2_PKT_ID]].iFirmVer = gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0+2];

						memcpy(&gbStatusPacket[0], &gbStatusPacket[PING_STATUS_LENGTH], rx_length - PING_STATUS_LENGTH);
						rx_length -= PING_STATUS_LENGTH;
					}
					else
					{
						gbCommStatus = COMM_RXCORRUPT;

						// remove header (0xFF 0xFF 0xFD)
						memcpy(&gbStatusPacket[0], &gbStatusPacket[3], rx_length - 3); 
						rx_length -= 3;
					}

					if(rx_length < PING_STATUS_LENGTH)
						break;
				}
				else
				{
					// remove unnecessary packets
					memcpy(&gbStatusPacket[0], &gbStatusPacket[idx], rx_length - idx);
					rx_length -= idx;
				}
			}
		}
		else // Sync_Write ans Bulk Write
		{
			tx_packet();
			giBusUsing = 0;
			if(gbCommStatus == COMM_TXSUCCESS)
				gbCommStatus = COMM_RXSUCCESS;
			return;
		}
	}
}



////////////// get/set packet methods /////////////
void dynamixel2::set_txpacket_id(unsigned char id)
{
	gbInstructionPacket[PRT2_PKT_ID] = id;
}

void dynamixel2::set_txpacket_instruction(unsigned char instruction)
{
	gbInstructionPacket[PRT2_PKT_INSTRUCTION] = (unsigned char)instruction;
}

void dynamixel2::set_txpacket_parameter(unsigned short index, unsigned char value)
{
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+index] = value;
}

void dynamixel2::set_txpacket_length(unsigned short length)
{
	gbInstructionPacket[PRT2_PKT_LENGTH_L] = LOBYTE(length);
	gbInstructionPacket[PRT2_PKT_LENGTH_H] = HIBYTE(length);
}

int dynamixel2::get_rxpacket_error_byte(void)
{
	return gbStatusPacket[PRT2_PKT_ERRBIT];
}

int  dynamixel2::get_rxpacket_parameter( int index )
{
	return (int)gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0+index];
}

int  dynamixel2::get_rxpacket_length()
{
	return (int)MAKEWORD(gbStatusPacket[PRT2_PKT_LENGTH_L], gbStatusPacket[PRT2_PKT_LENGTH_H]);
}

void dynamixel2::ping(unsigned char id)
{
	gbCommStatus = COMM_TXFAIL;

	gbInstructionPacket[PRT2_PKT_ID]            = (unsigned char)id;
    gbInstructionPacket[PRT2_PKT_LENGTH_L]      = 0x03;
    gbInstructionPacket[PRT2_PKT_LENGTH_H]      = 0x00;
    gbInstructionPacket[PRT2_PKT_INSTRUCTION]   = INST_PING;

	gPingData[id].iModelNo = -1;
	gPingData[id].iFirmVer = -1;

    txrx_packet();

	if( (id != BROADCAST_ID) && (gbCommStatus == COMM_RXSUCCESS) )
	{
		gPingData[id].iID      = id;
		gPingData[id].iModelNo = MAKEWORD( gbStatusPacket[PRT1_PKT_PARAMETER0+1], gbStatusPacket[PRT1_PKT_PARAMETER0+2] );
		gPingData[id].iFirmVer = gbStatusPacket[PRT1_PKT_PARAMETER0+3];
	}
}

int  dynamixel2::get_ping_result(unsigned char id, int info_num)
{
	if(id <= MAX_ID && gPingData[id].iModelNo != -1 && gPingData[id].iFirmVer != -1)
	{
		if(info_num == PING_INFO_MODEL_NUM )
			return gPingData[id].iModelNo;
		else if(info_num == PING_INFO_FIRM_VER)
			return gPingData[id].iFirmVer;
	}

	return 0;
}

void dynamixel2::broadcast_ping()
{
	int idx = 0;

	gbCommStatus = COMM_TXFAIL;

	gbInstructionPacket[PRT2_PKT_ID]            = BROADCAST_ID;
    gbInstructionPacket[PRT2_PKT_LENGTH_L]      = 0x03;
    gbInstructionPacket[PRT2_PKT_LENGTH_H]      = 0x00;
    gbInstructionPacket[PRT2_PKT_INSTRUCTION]   = INST_PING;

	for(idx = 1; idx <= MAX_ID; idx++)
	{
		gPingData[idx].iID = idx;
		gPingData[idx].iFirmVer = -1;
		gPingData[idx].iModelNo = -1;
	}

	txrx_packet();
}

void dynamixel2::reboot(unsigned char id)
{
	if(id == BROADCAST_ID)
	{
		gbCommStatus = COMM_TXERROR;
		return;
	}

	gbInstructionPacket[PRT2_PKT_ID]            = (unsigned char)id;
    gbInstructionPacket[PRT2_PKT_LENGTH_L]      = 0x03;
    gbInstructionPacket[PRT2_PKT_LENGTH_H]      = 0x00;
    gbInstructionPacket[PRT2_PKT_INSTRUCTION]   = INST_REBOOT;

    txrx_packet();
}

void dynamixel2::factory_reset(unsigned char id, int option)
{
	if(id == BROADCAST_ID)
	{
		gbCommStatus = COMM_TXERROR;
		return;
	}

	gbCommStatus = COMM_TXFAIL;

	gbInstructionPacket[PRT2_PKT_ID]            = (unsigned char)id;
    gbInstructionPacket[PRT2_PKT_LENGTH_L]      = 0x04;
    gbInstructionPacket[PRT2_PKT_LENGTH_H]      = 0x00;
    gbInstructionPacket[PRT2_PKT_INSTRUCTION]   = INST_RESET;
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0] = (unsigned char)option;

	txrx_packet();
}

unsigned char  dynamixel2::read_byte(unsigned char id, int address)
{
	unsigned short length = 1;
	gbCommStatus = COMM_TXFAIL; 

	gbInstructionPacket[PRT2_PKT_ID]						= id;
	gbInstructionPacket[PRT2_PKT_LENGTH_L]					= 0x07;
	gbInstructionPacket[PRT2_PKT_LENGTH_H]					= 0x00;
	gbInstructionPacket[PRT2_PKT_INSTRUCTION]				= INST_READ;
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+0]	= (unsigned char)LOBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+1]	= (unsigned char)HIBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+2]	= (unsigned char)LOBYTE(length);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+3]	= (unsigned char)HIBYTE(length);

	txrx_packet();
	//if(gbCommStatus == COMM_RXSUCCESS && id != BROADCAST_ID)
	//	memmove(data, &rxpacket[PKT_PARAMETER+1], length);

	return gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0];
}

void dynamixel2::write_byte(unsigned char id, int address, unsigned char value)
{
	unsigned short length = 1;
	gbInstructionPacket[PRT2_PKT_ID]						= id;
	gbInstructionPacket[PRT2_PKT_LENGTH_L]					= LOBYTE(length+5);
	gbInstructionPacket[PRT2_PKT_LENGTH_H]					= HIBYTE(length+5);
	gbInstructionPacket[PRT2_PKT_INSTRUCTION]				= INST_WRITE;
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+0]	= LOBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+1]	= HIBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+2]	= (unsigned char)value;
	
	txrx_packet();
}

unsigned short dynamixel2::read_word(unsigned char id, int address)
{
	unsigned short length = 2;
	gbCommStatus = COMM_TXFAIL; 

	gbInstructionPacket[PRT2_PKT_ID]						= id;
	gbInstructionPacket[PRT2_PKT_LENGTH_L]					= 0x07;
	gbInstructionPacket[PRT2_PKT_LENGTH_H]					= 0x00;
	gbInstructionPacket[PRT2_PKT_INSTRUCTION]				= INST_READ;
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+0]	= LOBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+1]	= HIBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+2]	= LOBYTE(length);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+3]	= HIBYTE(length);

	txrx_packet();
	//if(gbCommStatus == COMM_RXSUCCESS && id != BROADCAST_ID)
	//	memmove(data, &rxpacket[PKT_PARAMETER+1], length);

	return MAKEWORD ( gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0], gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0+1]);
}

void dynamixel2::write_word(unsigned char id, int address, unsigned short value)
{
	unsigned short length = 2;
	gbInstructionPacket[PRT2_PKT_ID]						= id;
	gbInstructionPacket[PRT2_PKT_LENGTH_L]					= LOBYTE(length+5);
	gbInstructionPacket[PRT2_PKT_LENGTH_H]					= HIBYTE(length+5);
	gbInstructionPacket[PRT2_PKT_INSTRUCTION]				= INST_WRITE;
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+0]	= LOBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+1]	= HIBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+2]	= LOBYTE( value );
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+3]	= HIBYTE( value );
	
	txrx_packet();
}

unsigned long dynamixel2::read_dword(unsigned char id, int address)
{
	unsigned short length = 4;
	gbCommStatus = COMM_TXFAIL; 

	gbInstructionPacket[PRT2_PKT_ID]						= id;
	gbInstructionPacket[PRT2_PKT_LENGTH_L]					= 0x07;
	gbInstructionPacket[PRT2_PKT_LENGTH_H]					= 0x00;
	gbInstructionPacket[PRT2_PKT_INSTRUCTION]				= INST_READ;
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+0]	= LOBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+1]	= HIBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+2]	= LOBYTE(length);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+3]	= HIBYTE(length);

	txrx_packet();
	//if(gbCommStatus == COMM_RXSUCCESS && id != BROADCAST_ID)
	//	memmove(data, &rxpacket[PKT_PARAMETER+1], length);

	return MAKEDWORD(MAKEWORD ( gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0], gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0+1]),
		MAKEWORD ( gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0+2], gbStatusPacket[PRT2_STATUS_PKT_PARAMETER0+3]));
}

void dynamixel2::write_dword(unsigned char id, int address, unsigned long value)
{
	unsigned short length = 4;
	gbInstructionPacket[PRT2_PKT_ID]						= id;
	gbInstructionPacket[PRT2_PKT_LENGTH_L]					= LOBYTE(length+5);
	gbInstructionPacket[PRT2_PKT_LENGTH_H]					= HIBYTE(length+5);
	gbInstructionPacket[PRT2_PKT_INSTRUCTION]				= INST_WRITE;
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+0]	= LOBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+1]	= HIBYTE(address);
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+2]	= LOBYTE(LOWORD( value ));
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+3]	= HIBYTE(LOWORD( value ));
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+4]	= LOBYTE(HIWORD( value ));
	gbInstructionPacket[PRT2_INSTRUCTION_PKT_PARAMETER0+5]	= HIBYTE(HIWORD( value ));
	
	txrx_packet();
}

unsigned char  dynamixel2::get_bulk_read_data_byte(unsigned char id, unsigned int start_address)
{
    if((start_address < gBulkData[id].iStartAddr) || ((gBulkData[id].iStartAddr + gBulkData[id].iLength-1) < start_address))
        return 0;
    return gBulkData[id].pucTable[(start_address-gBulkData[id].iStartAddr)];
}

unsigned short dynamixel2::get_bulk_read_data_word(unsigned char id, unsigned int start_address)
{
    if( (start_address < gBulkData[id].iStartAddr) || ((gBulkData[id].iStartAddr + gBulkData[id].iLength-1) < start_address))
        return 0;
	return MAKEWORD(gBulkData[id].pucTable[(start_address-gBulkData[id].iStartAddr)],
						gBulkData[id].pucTable[(start_address-gBulkData[id].iStartAddr+1)]);
}

unsigned long  dynamixel2::get_bulk_read_data_dword(unsigned char id, unsigned int start_address)
{
    if((start_address < gBulkData[id].iStartAddr) || ((gBulkData[id].iStartAddr + gBulkData[id].iLength-1) < start_address))
        return 0;
    return MAKEDWORD(MAKEWORD(gBulkData[id].pucTable[(start_address-gBulkData[id].iStartAddr)],
                                      gBulkData[id].pucTable[(start_address-gBulkData[id].iStartAddr+1)]),
                         MAKEWORD(gBulkData[id].pucTable[(start_address-gBulkData[id].iStartAddr+2)],
                                      gBulkData[id].pucTable[(start_address-gBulkData[id].iStartAddr+3)]));
}

unsigned char  dynamixel2::get_sync_read_data_byte(unsigned char id, unsigned int start_address)
{
	if((start_address < gSyncData[id].iStartAddr) || ((gSyncData[id].iStartAddr + gSyncData[id].iLength-1) < start_address))
		return 0;
	return gBulkData[id].pucTable[(start_address-gSyncData[id].iStartAddr)];
}

unsigned short dynamixel2::get_sync_read_data_word(unsigned char id, unsigned int start_address)
{
	if((start_address < gSyncData[id].iStartAddr) || ((gSyncData[id].iStartAddr + gSyncData[id].iLength-1) < start_address))
		return 0;
	return MAKEWORD(gSyncData[id].pucTable[(start_address-gSyncData[id].iStartAddr)],
						gSyncData[id].pucTable[(start_address-gSyncData[id].iStartAddr+1)]);
}

unsigned long  dynamixel2::get_sync_read_data_dword(unsigned char id, unsigned int start_address)
{
	if((start_address < gSyncData[id].iStartAddr) || ((gSyncData[id].iStartAddr + gSyncData[id].iLength-1) < start_address))
		return 0;
	return MAKEDWORD(MAKEWORD(gSyncData[id].pucTable[(start_address-gSyncData[id].iStartAddr)+0],
									  gSyncData[id].pucTable[(start_address-gSyncData[id].iStartAddr)+1]),
						 MAKEWORD(gSyncData[id].pucTable[(start_address-gSyncData[id].iStartAddr)+2],
									  gSyncData[id].pucTable[(start_address-gSyncData[id].iStartAddr)+3]));
}
