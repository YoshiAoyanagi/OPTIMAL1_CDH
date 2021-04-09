// 
// 
// 

#include "CommonSerialReceiver.h"

SERIAL_MUX	serial_mux_enable_port;

//SerialéÛêMèàóù//////////////////////////
ack Serial_DataReceive(FOWARD_PORT port, unsigned char *data, unsigned int timeout_threshold, int *len) {

	int i_ret = 0;
	int receive_size = 0;
	int packet_length = 0;
	int timeout = 0;
	int rec_sts = REC_STATUS_0;
	int header_offset = 0;

	while (rec_sts != REC_STATUS_ETX_2)
	{
		switch (port)
		{
			case PORT_DATA:		i_ret = DATA_Serial.available(); break;
			case PORT_STRX:		i_ret = STRX_Serial.available(); break;
			case PORT_ADCS:		i_ret = ADCS_Serial.available(); break;
			case PORT_MSN:		i_ret = MSN_Serial.available(); break;
			case PORT_DBG:		i_ret = DEBUG_Serial.available(); break;
			default:			break;
		}
		timeout++;

		if (timeout > timeout_threshold) //0xFFFF)
		{
#ifdef PRINT_DEBUG
			Serial.println("serial receive timeout");
#endif
			return (ValueNull);
		}

		if (i_ret > 0)
		{
			for (int i = 0; i < i_ret; i++)
			{
				switch (port)
				{
				case PORT_DATA:		data[receive_size] = DATA_Serial.read(); break;// Serial.print(data[receive_size], HEX); Serial.print(" ");; break;
				case PORT_STRX:		data[receive_size] = STRX_Serial.read(); break;
				case PORT_ADCS:		data[receive_size] = ADCS_Serial.read(); break;
				case PORT_MSN:		data[receive_size] = MSN_Serial.read();  break;// Serial.print(data[receive_size], HEX); Serial.print(" "); break;
				case PORT_DBG:		data[receive_size] = DEBUG_Serial.read(); break;
				default:			break;

				}
				if (rec_sts == REC_STATUS_0)
				{
					if (data[receive_size] == CMD_STX1)
					{
						rec_sts = REC_STATUS_STX_1;
						header_offset = receive_size;
					}
				}
				else if (rec_sts == REC_STATUS_STX_1)
				{
					if (data[receive_size] == CMD_STX2)
					{
						rec_sts = REC_STATUS_STX_2;
					}
				}
				else if (rec_sts == REC_STATUS_STX_2)		//length
				{
					packet_length = data[receive_size];
					rec_sts = REC_STATUS_LENGTH;
				}
				else if (rec_sts == REC_STATUS_LENGTH)
				{
					//SFì¡ï èàóù
					if (data[2] == 0xA8)
					{
						data[2] = 0xA1;
						packet_length = 0xA1;
					}
					//

					if (receive_size >= (packet_length + CCP_STX_ETX_LENGTH + header_offset))
					{
						rec_sts = REC_STATUS_ETX_2;
						
						if ((receive_size + 1) > CCP_MAX_PACKET_LENGTH)
						{
#ifdef PRINT_DEBUG
							Serial.println("serial receive overflow");
#endif
							return (ValueNull);
						}

						*len = receive_size + 1;
						return (Success);
					}
				}
				receive_size++;
				if (receive_size > CCP_MAX_PACKET_LENGTH)
				{
#ifdef PRINT_DEBUG
					Serial.println("serial receive overflow");
#endif
					return (ValueNull);
				}
			}
		}
		if (rec_sts == REC_STATUS_0)
			break;
		else if (rec_sts == REC_STATUS_LENGTH)//SFì¡ï èàóù
		{
			//LoRa Cmd routing process
			if ((port == PORT_MSN) && (serial_mux_enable_port == MUX_SF))
			{
				if (data[1] == 0xFF || data[1] == 0xFA || data[1] == 0xFB)
				{
					if(data[2] == 0x0A || data[2] == 0x0B)
						return (OtherProcess);
				}
			}
		}
	}

	//LoRa Cmd routing process
	if (rec_sts == REC_STATUS_0 && (port == PORT_MSN) && (serial_mux_enable_port == MUX_SF))
	{
		if (data[1] == 0xFF || data[1] == 0xFA || data[1] == 0xFB)
		{
			if (data[2] == 0x0A || data[2] == 0x0B)
				return (OtherProcess);
		}
	}
	return (ValueNull);
}


void Serial_MUX_init(void)
{
	//Serial_Mux
	pinMode(PORT_SERIAL_MUX_A0, OUTPUT);
	pinMode(PORT_SERIAL_MUX_A1, OUTPUT);
	pinMode(PORT_SERIAL_MUX_EN, OUTPUT);
	digitalWrite(PORT_SERIAL_MUX_EN, HIGH);

	unsigned char port = MUX_SF;
	ack i_ret = FRAM_Read_serial_mux(&port);
	if (Serial_MUX_change((SERIAL_MUX)port) != Success)
	{
		serial_mux_enable_port = MUX_SF;
		Serial_MUX_change(serial_mux_enable_port);
	}
}

ack Serial_MUX_change(SERIAL_MUX port)
{
	if (port == MUX_RPI)
	{
		MSN_Serial.end();
		MSN_Serial.begin(BITRATE_MSN);
		digitalWrite(PORT_SERIAL_MUX_A0, LOW);
		digitalWrite(PORT_SERIAL_MUX_A1, LOW);
		serial_mux_enable_port = MUX_RPI;
		FRAM_Write_serial_mux((SERIAL_MUX)port);//FRAMÇ…ï€ë∂
		return Success;
	}
	else if (port == MUX_SF)
	{
		MSN_Serial.end();
		MSN_Serial.begin(BITRATE_MSN);
		digitalWrite(PORT_SERIAL_MUX_A0, HIGH);
		digitalWrite(PORT_SERIAL_MUX_A1, LOW);
		serial_mux_enable_port = MUX_SF;
		FRAM_Write_serial_mux((SERIAL_MUX)port);//FRAMÇ…ï€ë∂
		return Success;
	}
	else if (port == MUX_AQU)
	{
		MSN_Serial.end();
		MSN_Serial.begin(115200);
		digitalWrite(PORT_SERIAL_MUX_A0, LOW);
		digitalWrite(PORT_SERIAL_MUX_A1, HIGH);
		serial_mux_enable_port = MUX_AQU;
		FRAM_Write_serial_mux((SERIAL_MUX)port);//FRAMÇ…ï€ë∂
		return Success;
	}
	else if (port == MUX_OVCO)
	{
		MSN_Serial.end();
		MSN_Serial.begin(57600);
		digitalWrite(PORT_SERIAL_MUX_A0, HIGH);
		digitalWrite(PORT_SERIAL_MUX_A1, HIGH);
		serial_mux_enable_port = MUX_OVCO;

		FRAM_Write_serial_mux((SERIAL_MUX)port);//FRAMÇ…ï€ë∂
		return Success;
	}
	else
		return Error;

}