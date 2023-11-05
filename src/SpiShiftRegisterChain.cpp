#include "SpiShiftRegisterChain.h"


// 
// public constructor/ destructor
// 
SpiShiftRegisterChain::SpiShiftRegisterChain(unsigned int latchPin, unsigned int dataLengthBytes, unsigned int maxClockSpeed /*= 4000000*/) :
	_latchPin(latchPin),
	_dataLengthBytes(dataLengthBytes),
	_spiSettings(maxClockSpeed, _bitOrder, _dataMode),
	_dataArray(0)
{
	initPins();
	allocateMemory();
	setDataToZeros();
}

SpiShiftRegisterChain::~SpiShiftRegisterChain() {
	if(_dataArray)
		delete[] _dataArray;
}


//
// public functions
//
void SpiShiftRegisterChain::maxClockSpeed(unsigned int maxClockSpeed) {
	_spiSettings = SPISettings(maxClockSpeed, _bitOrder, _dataMode);
}

void SpiShiftRegisterChain::setBitOff(unsigned int bitNumber, bool writeData/*= true*/){
	_dataArray[bitNumber / 8] &= ~(0b10000000 >> (bitNumber % 8));
	if (writeData)
		this->writeData();
}

void SpiShiftRegisterChain::setBitOn(unsigned int bitNumber, bool writeData/*= true*/) {
	_dataArray[bitNumber / 8] |= 0b10000000 >> (bitNumber % 8);
	if (writeData)
		this->writeData();
}

void SpiShiftRegisterChain::setDataToZeros() {
	for (int i = 0; i < _dataLengthBytes; i++)
		_dataArray[i] = 0b00000000;
}

void SpiShiftRegisterChain::toggleBit(unsigned int bitNumber, bool writeData/*= true*/) {
	_dataArray[bitNumber / 8] ^= 0b10000000 >> (bitNumber % 8);
	if (writeData)
		this->writeData();
}

void SpiShiftRegisterChain::writeData() const {
	SPI.beginTransaction(_spiSettings);
	digitalWrite(_latchPin, LOW);

	for (int i = _dataLengthBytes - 1; i >= 0; i--)
		SPI.transfer(_dataArray[i]);

	digitalWrite(_latchPin, HIGH);
	SPI.endTransaction();
}


//
// private functions
//
void SpiShiftRegisterChain::allocateMemory() {
	if (_dataArray)
		delete[] _dataArray;

	_dataArray = new byte[max(4, _dataLengthBytes)];
}

void SpiShiftRegisterChain::initPins() const {
	pinMode(_latchPin, OUTPUT);
	digitalWrite(_latchPin, HIGH);
}
