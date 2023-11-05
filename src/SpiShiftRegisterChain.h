#ifndef _SpiShiftRegisterChain_h
#define _SpiShiftRegisterChain_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SPI.h"

class SpiShiftRegisterChain {
public: // constructor/ destructor
	SpiShiftRegisterChain(unsigned int latchPin, unsigned int dataLengthBytes, unsigned int maxClockSpeed = 4000000);
	~SpiShiftRegisterChain();

public: // functions
	void maxClockSpeed(unsigned int maxClockSpeed);
	void setBitOff(unsigned int bitNumber, bool writeData = true);
	void setBitOn(unsigned int bitNumber, bool writeData = true);
	void setDataToZeros();
	void toggleBit(unsigned int bitNumber, bool writeData = true);
	void writeData() const;

private: // functions
	void allocateMemory();
	void initPins() const;

private: // variables
	uint8_t _bitOrder = LSBFIRST;
	byte* _dataArray;
	uint8_t _dataMode = SPI_MODE0;
	unsigned int _dataLengthBytes;
	unsigned int _latchPin;
	SPISettings _spiSettings;
};


#endif