#ifndef LoRa_A28_h
#define LoRa_A28_h

/*
 * check processor,Teensy4.0 is cortex-M7,not other board
 * In many special cases, software serial ports are used, usually hardware serial ports are used
*/

#include "./includes/statesNaming.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MAX_SIZE_TX_PACKET 120	//Maximum number of bytes per data packet

// Define where debug output will be printed.
#define DEBUG_PRINTER Serial

// Setup debug printing macros.
#ifdef LoRa_A28_DEBUG
	#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
	#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
	#define DEBUG_PRINT(...) {}
	#define DEBUG_PRINTLN(...) {}
#endif

//4 modes
enum MODE_TYPE
{
	MODE_0_HALF_DUPLEX			    =0,
	MODE_1_FULL_DUPLEX			    =1,
	MODE_2_RESERVED				    =2,   //FULL_DUPLEX
	MODE_3_SLEEP_CONFIGURATION		=3,
	MODE_3_PROGRAM				    =3,
	//MODE_INIT					=0xFF    //whether to use
};

//command for A28
enum PROGRAM_COMMAND
{
	WRITE_CFG_PWR_DWN_SAVE 		= 0xC0,
	READ_CONFIGURATION 	   		= 0xC1,
	WRITE_CFG_PWR_DWN_NO_SAVE	= 0xC2,
	READ_HARDWARE_VERSION	    = 0xC3,
	GENERATE_RESET				= 0xC4,
    RESTORE_DEFAULT_CFG         = 0xC9,
    READ_SOFTWARE_VERSION       = 0xF3,

    /*read RSSI Current data/environment*/
    READ_RSSI_0_1_4_COMMON      = 0xAF,
    READ_RSSI_2_CURRENT_DATA    = 0x73,
    READ_RSSI_2_ENVIRONMENT     = 0x74,
    READ_RSSI_3_COMMON          = 0x00,
    READ_RSSI_5_CURRENT_DATA    = 0xF3,
    READ_RSSI_5_ENVIRONMENT     = 0xf4
};

/*
 * Register address
*/
enum REGISTER_ADDRESS
{
	REG_ADDRESS_CFG 			= 0x00,
	REG_ADDRESS_SPED			= 0x02,
	REG_ADDRESS_CHANNEL			= 0x03,
	REG_ADDRESS_OPTION			= 0x04
};

enum PACKET_LENGHT
{
	PL_CONFIGURATION			= 0x06,

	PL_SPED 					= 0X01,
	PL_CHANNEL					= 0x01,
	PL_OPTION					= 0x01,
};

typedef struct fixedStransmission
{
	byte ADDH = 0;
	byte ADDL = 0;
	byte CHAN = 0;
	unsigned char message[];
}FixedStransmission;

#pragma pack(push, 1)
/*
 * High bit to low bit in one mode, Low bit to High bit in one register, Low byte to High byte in many register 
*/
struct Speed 
{
	uint8_t airDataRate :3; //bit 2,1,0
	String getAirDataRateDescription() {
		return getAirDataRateDescriptionByParams(this->airDataRate);
	}

	uint8_t uartBaudRate :3; //bit 5,4,3
	String getUARTBaudRateDescription() {
		return getUARTBaudRateDescriptionByParams(this->uartBaudRate);
	}

	uint8_t uartParity :2; //bit 7,6
	String getUARTParityDescription() {
		return getUARTParityDescriptionByParams(this->uartParity);
	}
	
};

struct Option
{
	uint8_t transmissionPower :2; //bit 1,0
	String getTRANSMISSION_POWERDescription() {
		return getTRANSMISSION_POWERDescriptionByParams(this->transmissionPower);
	}

	uint8_t option_5_2 :4; //bit 5,4,3,2

	uint8_t transmission_modes :1; //bit 6
	String getTRANSMISSION_MODEDescription() {
		return getTRANSMISSION_MODEDescriptionByParams(this->transmission_modes);
	}

	uint8_t modulation_mode :1;  //bit 7
    String getModulation_ModeDescription() {
		return getModulation_ModeDescriptionByParams(this->modulation_mode);
	}
};
// set register is end



//how to read   00H-04H
struct Configuration {
	byte ADDH = 0;
	byte ADDL = 0;
	
	struct Speed SPED;

	byte CHAN = 0;
	String getChannelDescription() {
		return String(this->CHAN + OPERATING_FREQUENCY) + F("MHz");
	}

	struct Option OPTION;
};



struct ResponseStatus {
	Status code;
	String getResponseDescription() {
		return getResponseDescriptionByParams(this->code);
	}
};



struct ResponseStructContainer {
	void *data;
	byte rssi;
	ResponseStatus status;
	void close() {
		free(this->data);
	}
};

struct ResponseContainer {
	String data;
	byte rssi;
	ResponseStatus status;
};
#pragma pack(pop)




class LoRa_A28
{
	public:
		LoRa_A28(HardwareSerial* Serial, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE bpsRate = UART_BPS_RATE_9600);

		bool begin();
		Status setMode(MODE_TYPE mode);
		
        MODE_TYPE getMode();

		ResponseStructContainer getConfiguration();
		ResponseStatus setConfiguration(Configuration configuration);
		ResponseStatus sendMessage(const void *message, const uint8_t size);
		ResponseStatus sendFixedMessage(byte ADDH,byte ADDL, byte CHAN, const void *message, const uint8_t size);
		ResponseStructContainer receiveMessage(const uint8_t size);
		ResponseStructContainer receiveMessageRSSI(const uint8_t size);
		ResponseStructContainer receiveMessageComplete(const uint8_t size, bool enableRSSI);

		int available();
		
	private:
		HardwareSerial* hs;

		bool isSoftwareSerial = true;

		int8_t txE32pin = -1;
        int8_t rxE32pin = -1;
        int8_t auxPin = -1;

		int8_t m0Pin = -1;
		int8_t m1Pin = -1;

		UART_BPS_RATE bpsRate = UART_BPS_RATE_9600;

		struct NeedsStream {
			template<typename T>
			void begin(T &t, uint32_t baud) {
				DEBUG_PRINTLN("Begin ");
				t.setTimeout(500);
				t.begin(baud);
				stream = &t;
			}
		
			void listen() {}

			Stream *stream;
		};

		NeedsStream serialDef;

		MODE_TYPE mode = MODE_1_FULL_DUPLEX;

		void managedDelay(unsigned long timeout);
		Status waitCompleteResponse(unsigned long timeout = 1000, unsigned int waitNoAux = 100);
		void flush();
		void cleanUARTBuffer();

		Status sendStruct(void *structureManaged, uint16_t size_);
		Status receiveStruct(void *structureManaged, uint16_t size_);
		bool writeProgram_3_Command(PROGRAM_COMMAND cmd1, PROGRAM_COMMAND cmd2, PROGRAM_COMMAND cmd3);
		bool writeProgramCommand(PROGRAM_COMMAND cmd, REGISTER_ADDRESS addr, PACKET_LENGHT pl);


		RESPONSE_STATUS checkUARTConfiguration(MODE_TYPE mode);
};

#endif
