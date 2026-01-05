#include "LoRa_A28.h"

LoRa_A28::LoRa_A28(HardwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE bpsRate){ //, uint32_t serialConfig
    this->txE32pin = txE32pin;
    this->rxE32pin = rxE32pin;

    this->auxPin = auxPin;

    this->m0Pin = m0Pin;
    this->m1Pin = m1Pin;

    this->hs = serial;

    this->bpsRate = bpsRate;
}

bool LoRa_A28::begin()
{
    DEBUG_PRINT("RX MIC ---> ");
	DEBUG_PRINTLN(this->txE32pin);
	DEBUG_PRINT("TX MIC ---> ");
	DEBUG_PRINTLN(this->rxE32pin);
	DEBUG_PRINT("AUX ---> ");
	DEBUG_PRINTLN(this->auxPin);
	DEBUG_PRINT("M0 ---> ");
	DEBUG_PRINTLN(this->m0Pin);
	DEBUG_PRINT("M1 ---> ");
	DEBUG_PRINTLN(this->m1Pin);
	
	/*
	 * Set mode
	*/
	if (this->auxPin != -1) {
		pinMode(this->auxPin, INPUT);
		DEBUG_PRINTLN("Init AUX pin!");
	}
	if (this->m0Pin != -1) {
		pinMode(this->m0Pin, OUTPUT);
		DEBUG_PRINTLN("Init M0 pin!");
		digitalWrite(this->m0Pin, HIGH);

	}
	if (this->m1Pin != -1) {
		pinMode(this->m1Pin, OUTPUT);
		DEBUG_PRINTLN("Init M1 pin!");
		digitalWrite(this->m1Pin, HIGH);

	}

    DEBUG_PRINTLN("Begin ex");
    if (this->hs){
        DEBUG_PRINTLN("Begin Hardware Serial");

#ifndef HARDWARE_SERIAL_SELECTABLE_PIN
        this->serialDef.begin(*this->hs, this->bpsRate);
#endif
        while (!this->hs) {
          ; // wait for serial port to connect. Needed for native USB
        }
	}

    this->serialDef.stream->setTimeout(100);    //占用100ms
    Status status = setMode(MODE_1_FULL_DUPLEX);
    return status==A28_SUCCESS;
}

/*

Utility method to wait until module is doen tranmitting
a timeout is provided to avoid an infinite loop

*/

Status LoRa_A28::waitCompleteResponse(unsigned long timeout, unsigned int waitNoAux) {

	Status result = A28_SUCCESS;

	unsigned long t = millis();

	// make darn sure millis() is not about to reach max data type limit and start over
	if (((unsigned long) (t + timeout)) == 0){
		t = 0;
	}

	// if AUX pin was supplied and look for HIGH state
	// note you can omit using AUX if no pins are available, but you will have to use delay() to let module finish
	if (this->auxPin != -1) {
		while (digitalRead(this->auxPin) == LOW) {
			if ((millis() - t) > timeout){
				result = ERR_A28_TIMEOUT;
				DEBUG_PRINTLN("Timeout error!");
				return result;
			}
		}
		DEBUG_PRINTLN("AUX HIGH!");
	}
	else {
		// if you can't use aux pin, use 4K7 pullup with Arduino
		// you may need to adjust this value if transmissions fail
		this->managedDelay(waitNoAux);
		DEBUG_PRINTLN(F("Wait no AUX pin!"));
	}


	// per data sheet control after aux goes high is 2ms so delay for at least that long)
	DEBUG_PRINTLN(F("Complete!"));
	return result;
}


/*

delay() in a library is not a good idea as it can stop interrupts
just poll internal time until timeout is reached

*/


void LoRa_A28::managedDelay(unsigned long timeout) {

	unsigned long t = millis();

	// make darn sure millis() is not about to reach max data type limit and start over
	if (((unsigned long) (t + timeout)) == 0){
		t = 0;
	}

	while ((millis() - t) < timeout) 	{ 	}

}


/*

Method to indicate availability

*/

//int LoRa_E22::available(unsigned long timeout) {
int LoRa_A28::available() {
//	unsigned long t = millis();
//
//	// make darn sure millis() is not about to reach max data type limit and start over
//	if (((unsigned long) (t + timeout)) == 0){
//		t = 0;
//	}
//
//	if (this->auxPin != -1) {
//		if (digitalRead(this->auxPin) == HIGH){
//			return 0;
//		}else{
//			while (digitalRead(this->auxPin) == LOW) {
//				if ((millis() - t) > timeout){
//					DEBUG_PRINTLN("Timeout error!");
//					return 0;
//				}
//			}
//			DEBUG_PRINTLN("AUX HIGH!");
//			return 2;
//		}
//	}else{
		return this->serialDef.stream->available();
//	}
}

/*

Method to indicate availability

*/

void LoRa_A28::flush() {
	this->serialDef.stream->flush();
}


void LoRa_A28::cleanUARTBuffer()
{
//  bool IsNull = true;

  while (this->available())
  {
//    IsNull = false;

    this->serialDef.stream->read();
  }
}


/*

Method to send a chunk of data provided data is in a struct--my personal favorite as you
need not parse or worry about sprintf() inability to handle floats

TTP: put your structure definition into a .h file and include in both the sender and reciever
sketches

NOTE: of your sender and receiver MCU's are different (Teensy and Arduino) caution on the data
types each handle ints floats differently

*/

Status LoRa_A28::sendStruct(void *structureManaged, uint16_t size_) {
		if (size_ > MAX_SIZE_TX_PACKET + 2){
			return ERR_A28_PACKET_TOO_BIG;
		}

		Status result = A28_SUCCESS;

		uint8_t len = this->serialDef.stream->write((uint8_t *) structureManaged, size_);
		if (len!=size_){
			DEBUG_PRINT(F("Send... len:"))
			DEBUG_PRINT(len);
			DEBUG_PRINT(F(" size:"))
			DEBUG_PRINT(size_);
			if (len==0){
				result = ERR_A28_NO_RESPONSE_FROM_DEVICE;
			}else{
				result = ERR_A28_DATA_SIZE_NOT_MATCH;
			}
		}
		if (result != A28_SUCCESS) return result;

		result = this->waitCompleteResponse(1000);
		if (result != A28_SUCCESS) return result;
		DEBUG_PRINT(F("Clear buffer..."))
		this->cleanUARTBuffer();

		DEBUG_PRINTLN(F("ok!"))

		return result;
}


/*

Method to get a chunk of data provided data is in a struct--my personal favorite as you
need not parse or worry about sprintf() inability to handle floats

TTP: put your structure definition into a .h file and include in both the sender and reciever
sketches

NOTE: of your sender and receiver MCU's are different (Teensy and Arduino) caution on the data
types each handle ints floats differently

*/


Status LoRa_A28::receiveStruct(void *structureManaged, uint16_t size_) {
	Status result = A28_SUCCESS;

	uint8_t len = this->serialDef.stream->readBytes((uint8_t *) structureManaged, size_);

	DEBUG_PRINT("Available buffer: ");
	DEBUG_PRINT(len);
	DEBUG_PRINT(" structure size: ");
	DEBUG_PRINTLN(size_);

	if (len!=size_){
		if (len==0){
			result = ERR_A28_NO_RESPONSE_FROM_DEVICE;
		}else{
			result = ERR_A28_DATA_SIZE_NOT_MATCH;
		}
	}
	if (result != A28_SUCCESS) return result;

	result = this->waitCompleteResponse(1000);
	if (result != A28_SUCCESS) return result;

	return result;
}

/*

method to set the mode (program, normal, etc.)

*/

Status LoRa_A28::setMode(MODE_TYPE mode) {

	// data sheet claims module needs some extra time after mode setting (2ms)
	// most of my projects uses 10 ms, but 40ms is safer

	this->managedDelay(40);

	if (this->m0Pin == -1 && this->m1Pin == -1) {
		DEBUG_PRINTLN(F("The M0 and M1 pins is not set, this mean that you are connect directly the pins as you need!"))
	}else{
		switch (mode)
		{
		  case MODE_0_HALF_DUPLEX:
			// Mode 0 | normal operation
			digitalWrite(this->m0Pin, LOW);
			digitalWrite(this->m1Pin, LOW);
			DEBUG_PRINTLN("MODE NORMAL!");
			break;
		  case MODE_1_FULL_DUPLEX:
			digitalWrite(this->m0Pin, LOW);
			digitalWrite(this->m1Pin, HIGH);
			DEBUG_PRINTLN("MODE WOR!");
			break;
		  case MODE_2_RESERVED:
//		  case MODE_2_PROGRAM:
			digitalWrite(this->m0Pin, HIGH);
			digitalWrite(this->m1Pin, LOW);
			DEBUG_PRINTLN("MODE CONFIGURATION!");
			break;
		  case MODE_3_SLEEP_CONFIGURATION:
			// Mode 3 | Setting operation
			digitalWrite(this->m0Pin, HIGH);
			digitalWrite(this->m1Pin, HIGH);
			DEBUG_PRINTLN("MODE SLEEP!");
			break;

		  default:
			return ERR_A28_INVALID_PARAM;
		}
	}
	// data sheet says 2ms later control is returned, let's give just a bit more time
	// these modules can take time to activate pins
	this->managedDelay(40);

	// wait until aux pin goes back low
	Status res = this->waitCompleteResponse(1000);

	if (res == A28_SUCCESS){
		this->mode = mode;
	}

	return res;
}

MODE_TYPE LoRa_A28::getMode(){
	return this->mode;
}


bool LoRa_A28::writeProgram_3_Command(PROGRAM_COMMAND cmd1, PROGRAM_COMMAND cmd2, PROGRAM_COMMAND cmd3){
	  uint8_t CMD[3] = {cmd1, cmd2, cmd3};
	  uint8_t size = this->serialDef.stream->write(CMD, 3);

	  DEBUG_PRINTLN(size);

	  this->managedDelay(50);  //need ti check

	  return size!=3;
}

bool LoRa_A28::writeProgramCommand(PROGRAM_COMMAND cmd,  REGISTER_ADDRESS addr, PACKET_LENGHT pl)
{
	uint8_t CMD[3] = {cmd, addr, pl};
	uint8_t size = this->serialDef.stream->write(CMD, 3);

	DEBUG_PRINTLN(size);

	this->managedDelay(50);  //need ti check

	return size!=3;
}



ResponseStructContainer LoRa_A28::getConfiguration(){
	ResponseStructContainer rc;

	rc.status.code = checkUARTConfiguration(MODE_3_PROGRAM);
	if (rc.status.code!=A28_SUCCESS) return rc;

	MODE_TYPE prevMode = this->mode;

	rc.status.code = this->setMode(MODE_3_PROGRAM);
	if (rc.status.code!=A28_SUCCESS) return rc;

	this->writeProgram_3_Command(READ_CONFIGURATION, READ_CONFIGURATION, READ_CONFIGURATION);

	rc.data = malloc(sizeof(Configuration));
	rc.status.code = this->receiveStruct((uint8_t *)rc.data, sizeof(Configuration));

#ifdef LoRa_A28_DEBUG
	 this->printParameters((Configuration *)rc.data);
#endif

	if (rc.status.code!=A28_SUCCESS) {
		this->setMode(prevMode);
		return rc;
	}

	rc.status.code = this->setMode(prevMode);
	if (rc.status.code!=A28_SUCCESS) return rc;


	return rc;
}



ResponseStatus LoRa_A28::setConfiguration(Configuration configuration)
{
	ResponseStatus rc;
	rc.code = checkUARTConfiguration(MODE_3_PROGRAM);
	if (rc.code!=A28_SUCCESS) return rc;
	
	MODE_TYPE prevMode = this->mode;

	rc.code = this->setMode(MODE_3_PROGRAM);
	if (rc.code!=A28_SUCCESS) return rc;
	//	this->writeProgramCommand(saveType, REG_ADDRESS_CFG);

	rc.code = this->sendStruct((uint8_t *)&configuration, sizeof(Configuration));
	if (rc.code!=A28_SUCCESS) {
		this->setMode(prevMode);
		return rc;
	}

	rc.code = this->receiveStruct((uint8_t *)&configuration, sizeof(Configuration));

	#ifdef LoRa_E22_DEBUG
		 this->printParameters((Configuration *)&configuration);
	#endif


	rc.code = this->setMode(prevMode);
	if (rc.code!=A28_SUCCESS) return rc;

	return rc;
}


RESPONSE_STATUS LoRa_A28::checkUARTConfiguration(MODE_TYPE mode){
	if (mode==MODE_3_PROGRAM && this->bpsRate!=UART_BPS_RATE_9600){
		return ERR_A28_WRONG_UART_CONFIG;
	}
	return A28_SUCCESS;
}



FixedStransmission *init_stack(int m){
	FixedStransmission *st = (FixedStransmission *)malloc(sizeof(FixedStransmission)+m*sizeof(int));
    return st;
}



ResponseStatus LoRa_A28::sendMessage(const void *message, const uint8_t size){
	Serial.println("mode:");
	Serial.println((MODE_TYPE)(this->mode));
	ResponseStatus status;
	status.code = this->sendStruct((uint8_t *)message, size);
	if (status.code!=A28_SUCCESS) return status;

	return status;
}

ResponseStatus LoRa_A28::sendFixedMessage(byte ADDH,byte ADDL, byte CHAN, const void *message, const uint8_t size)
{
	//	#pragma pack(push, 1)
//	struct FixedStransmission {
//		byte ADDH = 0;
//		byte ADDL = 0;
//		byte CHAN = 0;
//		unsigned char message[];
//	} fixedStransmission;
//	#pragma pack(pop)


	DEBUG_PRINT(ADDH);


	FixedStransmission *fixedStransmission = init_stack(size);

//	STACK *resize_stack(STACK *st, int m){
//	    if (m<=st->max){
//	         return st; /* Take sure do not kill old values */
//	    }
//	    STACK *st = (STACK *)realloc(sizeof(STACK)+m*sizeof(int));
//	    st->max = m;
//	    return st;
//	}

	fixedStransmission->ADDH = ADDH;
	fixedStransmission->ADDL = ADDL;
	fixedStransmission->CHAN = CHAN;
//	fixedStransmission.message = &message;

	memcpy(fixedStransmission->message,(unsigned char*)message,size);

	ResponseStatus status;
	status.code = this->sendStruct((uint8_t *)fixedStransmission, size+3);

	// fix for #8 issue
	free(fixedStransmission);

	if (status.code!=A28_SUCCESS) return status;

	return status;
}

ResponseStructContainer LoRa_A28::receiveMessage(const uint8_t size){
	return LoRa_A28::receiveMessageComplete(size, false);
}
ResponseStructContainer LoRa_A28::receiveMessageRSSI(const uint8_t size){
	return LoRa_A28::receiveMessageComplete(size, true);
}

ResponseStructContainer LoRa_A28::receiveMessageComplete(const uint8_t size, bool rssiEnabled){
	ResponseStructContainer rc;

	rc.data = malloc(size);
	rc.status.code = this->receiveStruct((uint8_t *)rc.data, size);
	if (rc.status.code!=A28_SUCCESS) {
		return rc;
	}

	if (rssiEnabled){

		char rssi[1];
		this->serialDef.stream->readBytes(rssi, 1);
		rc.rssi = rssi[0];
	}
	 this->cleanUARTBuffer();

	return rc;
}
