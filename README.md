# Arduino ipMIDI Transport 
[![arduino-library-badge](https://www.ardu-badge.com/badge/ipMIDI.svg?)](https://www.ardu-badge.com/ipMIDI)[![GitHub version](https://badge.fury.io/gh/lathoub%2FArduino-ipMIDI.svg)](https://badge.fury.io/gh/lathoub%2FArduino-ipMIDI)

This library implements the ipMIDI transport layer for the [FortySevenEffects Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library) 

## Installation
This library depends on the [Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library).

When installing this library from the Arduino IDE, the dependency be downloaded and installed in the same directory as this library. (Thanks to the `depends` clause in `library.properties`)

When manually installing this library, you have to manually download [Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library) from github and install it in the same directory as this library - without this additional install, this library will not be able to compile. 

## Usage
### Basic / Default
```cpp
#include <ipMIDI.h>
...
IPMIDI_CREATE_DEFAULT_INSTANCE();
...
void setup()
{
   MIDI.begin(1);
...
}

void loop()
{
   MIDI.read();
...
}
```
will create 2 instances: `ipMIDI` and `MIDI` and is connected to port 21928 - and listens to incoming MIDI on channel 1.

### Modified 
```cpp
#include <ipMIDI.h>
...
IPMIDI_CREATE_INSTANCE(EthernetUDP, MIDI2, 21929);
```
will create 2 instances named `ipMIDI2` and `MIDI2` and is connected to port 21929.

### Advanced
```cpp
#include <ipMIDI.h>
...
IPMIDI_NAMESPACE::ipMidiTransport<EthernetUDP> ipMIDI3(21930);
MIDI_NAMESPACE::MidiInterface<IPMIDI_NAMESPACE::ipMidiTransport<EthernetUDP>> MIDI3((IPMIDI_NAMESPACE::ipMidiTransport<EthernetUDP> &)ipMIDI3);
```
will create a instance named `ipMIDI3` (and underlaying MIDI object `MIDI3`) and is connected to port 21930.

## Tested boards/modules
- ESP32

## Other Transport protocols:
The libraries below  the same calling mechanism (API), making it easy to interchange the transport layer.
- [Arduino AppleMIDI Transport](https://github.com/lathoub/Arduino-AppleMIDI-Library)
- [Arduino USB-MIDI  Transport](https://github.com/lathoub/USB-MIDI)
- [Arduino BLE-MIDI  Transport](https://github.com/lathoub/Arduino-BLE-MIDI)
