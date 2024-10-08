# SpiShiftRegisterChain

 A simple Arduino library for controlling any length of chained 595 style serial to parallel shift registers over the built in SPI bus.
 
## Description

`SpiShiftRegisterChain` enables you to manage a chain of 595-style shift registers. Utilizing the hardware SPI bus, this library allows you to control large numbers of shift registers connected in series with high performance, making it ideal for projects requiring a high number of digital outputs.

## Features

- Simple and efficient control of 595-style shift registers
- Supports chaining of any length with any amount of bits per register
- Allows toggling individual bits, setting bits on or off, and clearing data
- Control multiple chains by using multiple `SpiShiftRegisterChain` objects
- Adjustable SPI clock speed for compatibility and performance tuning

## Installation

### Option 1: Using the Arduino IDE Library Manager

1. Open the Arduino IDE and go to **Sketch > Include Library > Manage Libraries**.
2. In the Library Manager, type "SpiShiftRegister" into the search bar.
3. When you see the library in the results, click **Install**.

### Option 2: Manually from GitHub

1. Download the latest release from this [GitHub repository](https://github.com/bsch2734/SpiShiftRegisterChain).
2. Move the downloaded folder to your Arduino `libraries` directory (usually located at `Documents/Arduino/libraries`).
3. Open the Arduino IDE, go to **Sketch > Include Library > Manage Libraries**, and ensure `SpiShiftRegisterChain` appears in the list.

## Usage

### Initial Setup

To begin using the `SpiShiftRegisterChain` library after installing it, include it in your sketch and create an instance of `SpiShiftRegisterChain` with the appropriate parameters. This object will be used to manage the chain:

```cpp
#include <SPI.h>
#include <SpiShiftRegisterChain.h>

// Define latch pin and number of bytes for shift register chain
const int latchPin = 10;
const int dataLengthBytes = 4;  // Example: 4 bytes for 32 output bits (4 * 8)

SpiShiftRegisterChain shiftRegister(latchPin, dataLengthBytes);

void setup() {
  SPI.begin();
}
```

The entire shift register chain is treated as one large shift register with bits numbered from `0` to `n-1` for a register chain with `n` total output bits. Each bit is addressed by its zero indexed number no matter which physical register it lies on.

When setting the number of bytes it does not matter what your physical configuration is, only how many outputs it has. For example, 3 8-bit registers would be handled the same as 2 12-bit registers since both have 24 bits or 3 bytes.

Before using the shift register you must call `SPI.begin()` or the SPI bus will not be initialized for the registers. This is typically done in `setup()`, as above, but can be done anywhere as required.

### Basic Functions

1. **Setting Individual Bits On/Off**:
   
   The library keeps an internal representation of the data which you manipulate before writing it out to the hardware. By default, data is written immediately when you call a data function such as `setBitOn()`. If you want to batch changes, set the second parameter of the data function to `false`. Then, later, call `writeData()` manually when you're ready to apply the changes to the hardware. This allows you to change any number of bits at exactly the same time.

   ```cpp
   shiftRegister.setBitOn(3);         // Turns on bit 3 and writes the data to the hardware
   shiftRegister.setBitOff(4, false); // Turns off bit 4 internally but does not write out to the hardware
   shiftRegister.setBitOn(8, false);  // Turns on bit 8 internally but does not write out to the hardware
   shiftRegister.toggleBit(6, false); // Toggles bit 6 internally but does not write out to the hardware
   
   shiftRegister.writeData();         // Now, manually apply the internal changes to the hardware, in this case to bits 4, 8, and 6
   ```
   
   Also note that any internal changes will be written on the next write even if it not from a call to `writeData()`. So the following are equivalent:
      ```cpp
    //example 1: call writeData()
   shiftRegister.setBitOn(8, false);
   shiftRegister.toggleBit(6, false);
   shiftRegister.writeData();
   
    //example 2: implicit write
   shiftRegister.setBitOn(8, false);
   shiftRegister.toggleBit(6);
   ```

2. **Toggling a Bit**:

   Toggling will change a bit's state from false to true or true to false as required. The toggle function works the same way as the on/ off functions, automatically writing data unless `false` is specified as the second parameter.

   ```cpp
   shiftRegister.toggleBit(3);   // Toggles the state of the 4th bit and writes the data
   ```

3. **Writing Data to All Shift Registers**:

   Since writing happens by default, use `writeData()` if you've opted to delay writing data for any changes by setting the write parameter to `false`.

   ```cpp
   shiftRegister.writeData();   // Manually applies any batched internal changes
   ```

### Example Sketch

Here’s a full example demonstrating how to set up the `SpiShiftRegisterChain` library and control individual bits:

```cpp
#include <SPI.h>
#include <SpiShiftRegisterChain.h>

const int latchPin = 10;
const int dataLengthBytes = 4;

SpiShiftRegisterChain shiftRegister(latchPin, dataLengthBytes);

void setup() {
  SPI.begin();
  shiftRegister.setBitOn(0);    // Turn on the 1st bit and write data
  shiftRegister.setBitOff(1, false);   // Turn off the 2nd bit without writing
  shiftRegister.writeData();    // Apply all changes at once
  delay(1000);

  shiftRegister.toggleBit(0);   // Toggle the 1st bit and write data
  delay(1000);
}

void loop() {
  // Your main code here
}
```

## Wiring Connections

To use `SpiShiftRegisterChain`, connect your Arduino's SPI and control pins to the 595 shift registers as follows:

- **Latch Pin**: Connect the specified `latchPin` from the Arduino (defined in your code) to the **Latch** pin on the first shift register (usually labeled **ST_CP** or **RCLK**). You may set `latchPin` to any Arduino pin. You may also create multiple instances of `SpiShiftRegisterChain` with different latch pins to control independent chains.
- **SPI MOSI Pin**: Connect the Arduino's **MOSI** pin (usually **D11** on most Arduino boards) to the **Data** pin on the first shift register (usually labeled **DS** or **SER**).
- **SPI Clock Pin**: Connect the Arduino's **SCK** pin (usually **D13**) to the **Clock** pin on the shift register (usually labeled **SH_CP** or **SRCLK**).
- **Ground**: Connect the **GND** pins on the shift register and Arduino.

For each additional shift register in the chain:
- Connect the data chaining pin, labeled **Q7’** or **Q7’ Out** on most 8 bit registers, of the current register to the **Data** pin of the next register in the chain.
- **Ground**: Connect the **GND** pins on the shift register and Arduino.

## API Reference

- **Constructor**: `SpiShiftRegisterChain(unsigned int latchPin, unsigned int dataLengthBytes, unsigned long maxClockSpeed = 4000000);`
  - `latchPin`: The digital pin used to tell the shift register new data is coming.
  - `dataLengthBytes`: The number of bytes (8-bit groups) in the shift register chain.
  - `maxClockSpeed`: Optional. Sets the maximum SPI clock speed (default is 4 MHz).
- **Destructor**: `~SpiShiftRegisterChain()`
  - Deallocates any memory used by the library.
- **setBitOn(unsigned int bitNumber, bool writeData = true)**: Turns on a specific bit in the shift register chain.
- **setBitOff(unsigned int bitNumber, bool writeData = true)**: Turns off a specific bit in the shift register chain.
- **toggleBit(unsigned int bitNumber, bool writeData = true)**: Toggles the state of a specific bit.
- **setDataToZeroes()** Sets the internal data to false for every bit but does not write it to the hardware.
- **setActiveState()** Changes between latch pin being active low and active high. Defaults to active low (false).
- **writeData()**: Transfers the current state of all bits to the shift register chain to the hardware.

## License

This project is licensed under the Unlicense - see the [license](license.txt) file for details.

## Contributing

Contributions are welcome! Please fork this repository and submit a pull request with your improvements. For major changes, please open an issue first to discuss what you would like to change.
