/*
 * Register bit setting and return flag bit
*/


#include "Arduino.h"

/*
 * Default operating frequency 2.4GHz
*/
#define OPERATING_FREQUENCY 2400

/*
 * All return states of lora functions or operations
*/
typedef enum RESPONSE_STATUS {
#ifndef ARDUINO_ARCH_STM32
  SUCCESS = 1,
#endif
  A28_SUCCESS = 1,  
  ERR_A28_UNKNOWN,	/* something shouldn't happened */
  ERR_A28_NOT_SUPPORT,
  ERR_A28_NOT_IMPLEMENT,
  ERR_A28_NOT_INITIAL,
  ERR_A28_INVALID_PARAM,
  ERR_A28_DATA_SIZE_NOT_MATCH,
  ERR_A28_BUF_TOO_SMALL,
  ERR_A28_TIMEOUT,
  ERR_A28_HARDWARE,
  ERR_A28_HEAD_NOT_RECOGNIZED,
  ERR_A28_NO_RESPONSE_FROM_DEVICE,
  ERR_A28_WRONG_UART_CONFIG,
  ERR_A28_WRONG_FORMAT,
  ERR_A28_PACKET_TOO_BIG
} Status;


static String getResponseDescriptionByParams(byte status){
	switch (status)
	{
	  case A28_SUCCESS:
		return F("Success");
		break;
	  case ERR_A28_UNKNOWN:
		return F("Unknown");
		break;
	  case ERR_A28_NOT_SUPPORT:
		return F("Not support!");
		break;
	  case ERR_A28_NOT_IMPLEMENT:
		return F("Not implement");
		break;
	  case ERR_A28_NOT_INITIAL:
		return F("Not initial!");
		break;
	  case ERR_A28_INVALID_PARAM:
		return F("Invalid param!");
		break;
	  case ERR_A28_DATA_SIZE_NOT_MATCH:
		return F("Data size not match!");
		break;
	  case ERR_A28_BUF_TOO_SMALL:
		return F("Buff too small!");
		break;
	  case ERR_A28_TIMEOUT:
		return F("Timeout!!");
		break;
	  case ERR_A28_HARDWARE:
		return F("Hardware error!");
		break;
	  case ERR_A28_HEAD_NOT_RECOGNIZED:
		return F("Save mode returned not recognized!");
		break;
	  case ERR_A28_NO_RESPONSE_FROM_DEVICE:
		return F("No response from device! (Check wiring)");
		break;
	  case ERR_A28_WRONG_UART_CONFIG:
		return F("Wrong UART configuration! (BPS must be 9600 for configuration)");
		break;
	  case ERR_A28_PACKET_TOO_BIG:
		return F("The device support only 240byte of data transmission!");
		break;
	  default:
		return F("Invalid status!");
	}
}


/*
 * Serial port checksum
*/
enum A28_UART_PARITY
{
  MODE_00_8N1 = 0b00,
  MODE_01_8O1 = 0b01,
  MODE_10_8E1 = 0b10,
  MODE_11_8N1 = 0b11
};


static String getUARTParityDescriptionByParams(byte uartParity){
	switch (uartParity)
	{
	  case MODE_00_8N1:
		return F("8N1 (Default)");
		break;
	  case MODE_01_8O1:
		return F("8O1");
		break;
	  case MODE_10_8E1:
		return F("8E1");
		break;
	  case MODE_11_8N1:
		return F("8N1");
		break;
	  default:
		return F("Invalid UART Parity!");
	}
}

/*
 * Serial Port Baud Rate
*/
enum UART_BPS_TYPE
{
  UART_BPS_1200 = 0b000,
  UART_BPS_2400 = 0b001,
  UART_BPS_4800 = 0b010,
  UART_BPS_9600 = 0b011,
  UART_BPS_19200 = 0b100,
  UART_BPS_38400 = 0b101,
  UART_BPS_57600 = 0b110,
  UART_BPS_115200 = 0b111
};

enum UART_BPS_RATE
{
  UART_BPS_RATE_1200 = 1200,
  UART_BPS_RATE_2400 = 2400,
  UART_BPS_RATE_4800 = 4800,
  UART_BPS_RATE_9600 = 9600,
  UART_BPS_RATE_19200 = 19200,
  UART_BPS_RATE_38400 = 38400,
  UART_BPS_RATE_57600 = 57600,
  UART_BPS_RATE_115200 = 115200
};

static String getUARTBaudRateDescriptionByParams(byte uartBaudRate)
{
	switch (uartBaudRate)
	{
	  case UART_BPS_1200:
		return F("1200bps");
		break;
	  case UART_BPS_2400:
		return F("2400bps");
		break;
	  case UART_BPS_4800:
		return F("4800bps");
		break;
	  case UART_BPS_9600:
		return F("9600bps (Default)");
		break;
	  case UART_BPS_19200:
		return F("19200bps");
		break;
	  case UART_BPS_38400:
		return F("38400bps");
		break;
	  case UART_BPS_57600:
		return F("57600bps");
		break;
	  case UART_BPS_115200:
		return F("115200bps");
		break;
	  default:
		return F("Invalid UART Baud Rate!");
	}
}

/*
 * Air rate follow by UART_BPS
*/
enum AIR_DATA_RATE
{
    AIR_DATA_RATE_000_Adapt = 0b000,
	AIR_DATA_RATE_001_1 = 0b001,
    AIR_DATA_RATE_010_2 = 0b010,
  	AIR_DATA_RATE_011_3 = 0b011,
  	AIR_DATA_RATE_100_4 = 0b100,
  	AIR_DATA_RATE_101_5 = 0b101,
  	AIR_DATA_RATE_110_6 = 0b110,
  	AIR_DATA_RATE_111_7 = 0b111
};

static String getAirDataRateDescriptionByParams(byte airDataRate)
{
    switch (airDataRate)
    {
        case AIR_DATA_RATE_000_Adapt:
        return F(" adaptive air velocity 0(continue trasnmission)");
		break;
		case AIR_DATA_RATE_001_1:
		return F("air velocity 1");
		break;
	  case AIR_DATA_RATE_010_2:
		return F("air velocity 2");
		break;
	  case AIR_DATA_RATE_011_3:
		return F("air velocity 3");
		break;
	  case AIR_DATA_RATE_100_4:
		return F("air velocity 4");
		break;
	  case AIR_DATA_RATE_101_5:
		return F("air velocity 5");
		break;
	  case AIR_DATA_RATE_110_6:
		return F("air velocity 6");
		break;
	  case AIR_DATA_RATE_111_7:
		return F("air velocity 7");
		break;
        default:
		return F("Invalid Air Data Rate!");
    }
}


enum Modulation_Mode
{
	FLRC = 0b0,
	LORA = 0b1
};

static String getModulation_ModeDescriptionByParams(byte modulation_mode)
{
	switch(modulation_mode)
	{
		case FLRC:
		  return F("FLRC Modulation Mode");
		  break;
		case LORA:
		  return F("LORA Modulation Mode");
		  break;
		default:
		  return F("Invalid Modulation_ModeD");
	}
}

enum TRANSMISSION_MODE
{
	DISCCONTINOUS_TRANSMISSION = 0b0,
	CONTINOUS_TRANSMISSION = 0b1,
};

static String getTRANSMISSION_MODEDescriptionByParams(byte transmission_modes)
{
	switch (transmission_modes)
	{
		case DISCCONTINOUS_TRANSMISSION:
		return F("transmission_modes (Default)");
		break;
		case CONTINOUS_TRANSMISSION:
		return F("9Bm");
		break;
		default:
		return F("Invalid transmission power param");
	}
}


enum TRANSMISSION_POWER
{
	POWER_12            = 0b11,
	POWER_9 			= 0b10,
	POWER_6 			= 0b01,
	POWER_3 			= 0b00		
};


static String getTRANSMISSION_POWERDescriptionByParams(byte transmissionPower)
{
	switch (transmissionPower)
	{
		case POWER_12:
		return F("12dBm (Default)");
		break;
		case POWER_9:
		return F("9dBm");
		break;
		case POWER_6:
		return F("6dBm");
		break;
		case POWER_3:
		return F("3dBm");
		break;
		default:
		return F("Invalid transmission power param");
	}
}