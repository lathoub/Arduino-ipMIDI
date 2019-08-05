/*!
 *  @file		ipMIDI.h
 */

#pragma once

#include "utility/ipMIDI_Settings.h"
#include "utility/ipMIDI_Defs.h"

#include "interface/AbstractMidiInterface.h"
using namespace Midi;

BEGIN_IPMIDI_NAMESPACE

template<class UdpClass>
class ipMIDIinterface : public AbstractMidiInterface
{
protected:
	UdpClass _dataPort;

	uint16_t _port = 21928;
	IPAddress _multiIP;

	byte _packetBuffer[UDP_TX_PACKET_MAX_SIZE];

protected:
	void write(DataByte b1)
	{
		Serial.println(F("write(1) "));

		_dataPort.beginPacket(_multiIP, _port);
		_dataPort.write(b1);
		_dataPort.endPacket();
	};

	void write(DataByte b1, DataByte b2)
	{
		const uint8_t data[2] = { b1, b2 };

		Serial.println(F("write(2) "));

		_dataPort.beginPacket(_multiIP, _port);
		_dataPort.write(data, 2);
		_dataPort.endPacket();
	};

	void write(DataByte b1, DataByte b2, DataByte b3)
	{
		const uint8_t data[3] = { b1, b2, b3 };

		Serial.println(F("write(3) "));
		Serial.println(b1, HEX);
		Serial.println(b2, HEX);
		Serial.println(b3, HEX);

		_dataPort.beginPacket(_multiIP, _port);
		_dataPort.write(data, 3);
		_dataPort.endPacket();
	};

	void sendMIDI(StatusByte, DataByte data1 = 0, DataByte data2 = 0);
	void receive(uint8_t *buffer, uint8_t bufferSize);

public:
	ipMIDIinterface()
	{
		_multiIP = IPAddress{ 225, 0, 0, 37 };
		_port = 21928;
	}

	~ipMIDIinterface()
	{
	}

	// TODO why must these functions be inline??

	inline bool begin(const char* deviceName);

	inline void read()
	{
		auto packetSize = _dataPort.parsePacket();

		Serial.println();
		Serial.print(F("parsePacket "));
		Serial.println(packetSize);


		if (packetSize) {
			packetSize = _dataPort.read(_packetBuffer, sizeof(_packetBuffer));

			Serial.println();
			Serial.print(F("read "));
			Serial.println(packetSize);

			receive(_packetBuffer, packetSize);
		}
	}
};

template<class UdpClass>
bool ipMIDIinterface<UdpClass>::begin(const char* deviceName)
{
	auto tt = _dataPort.beginMulticast(_multiIP, _port);
	Serial.print (F("beginMulticast "));
	Serial.println(tt);

	return true;
}

template<class UdpClass>
void ipMIDIinterface<UdpClass>::sendMIDI(StatusByte status, DataByte data1, DataByte data2)
{
	auto type = getTypeFromStatusByte(status);
	auto channel = getChannelFromStatusByte(status);

	switch (type) {
	case NoteOff:
		if (_noteOffCallback) _noteOffCallback(channel, data1, data2);
		break;
	case NoteOn:
		if (_noteOnCallback) _noteOnCallback(channel, data1, data2);
		break;
	case AfterTouchPoly:
		if (_afterTouchPolyCallback) _afterTouchPolyCallback(channel, data1, data2);
		break;
	case ControlChange:
		if (_controlChangeCallback) _controlChangeCallback(channel, data1, data2);
		break;
	case ProgramChange:
		if (_programChangeCallback) _programChangeCallback(channel, data1);
		break;
	case AfterTouchChannel:
		if (_afterTouchChannelCallback) _afterTouchChannelCallback(channel, data1);
		break;
	case PitchBend:
		if (_pitchBendCallback) {
			int value = (int)((data1 & 0x7f) | ((data2 & 0x7f) << 7)) + MIDI_PITCHBEND_MIN;
			_pitchBendCallback(channel, value);
		}
		break;

	case SystemExclusive:
		break;

	case TimeCodeQuarterFrame:
		if (_timeCodeQuarterFrameCallback) _timeCodeQuarterFrameCallback(data1);
		break;
	case SongPosition:
		if (_songPositionCallback) {
			unsigned short value = unsigned((data1 & 0x7f) | ((data2 & 0x7f) << 7));
			_songPositionCallback(value);
		}
		break;
	case SongSelect:
		if (_songSelectCallback) _songSelectCallback(data1);
		break;
	case TuneRequest:
		if (_tuneRequestCallback) _tuneRequestCallback();
		break;

	case Clock:
		if (_clockCallback) _clockCallback();
		break;
	case Tick:
		break;
	case Start:
		if (_startCallback) _startCallback();
		break;
	case Continue:
		if (_continueCallback) _continueCallback();
		break;
	case Stop:
		if (_stopCallback) _stopCallback();
		break;
	case ActiveSensing:
		if (_activeSensingCallback) _activeSensingCallback();
		break;
	case SystemReset:
		if (_resetCallback) _resetCallback();
		break;
	}
}

template<class UdpClass>
void ipMIDIinterface<UdpClass>::receive(uint8_t *buffer, uint8_t bufferSize)
{
	//Pointers used to search through payload.
	uint8_t lPtr = 0;
	uint8_t rPtr = 0;

	//lastStatus used to capture runningStatus
	uint8_t lastStatus;

	//Decode first packet -- SHALL be "Full MIDI message"
	lPtr = 2; //Start at first MIDI status -- SHALL be "MIDI status"

	//While statement contains incrementing pointers and breaks when buffer size exceeded.
	while (1) {
		lastStatus = buffer[lPtr];
		if ((buffer[lPtr] < 0x80)) {
			//Status message not present, bail
			return;
		}
		//Point to next non-data byte
		rPtr = lPtr;
		while ((buffer[rPtr + 1] < 0x80) && (rPtr < (bufferSize - 1))) {
			rPtr++;
		}
		//look at l and r pointers and decode by size.
		if (rPtr - lPtr < 1) {
			//Time code or system
			sendMIDI(lastStatus);
		}
		else if (rPtr - lPtr < 2) {
			sendMIDI(lastStatus, buffer[lPtr + 1]);
		}
		else if (rPtr - lPtr < 3) {
			sendMIDI(lastStatus, buffer[lPtr + 1], buffer[lPtr + 2]);
		}
		else {
			//Too much data
			//If not System Common or System Real-Time, send it as running status
			switch (buffer[lPtr] & 0xF0)
			{
			case NoteOff:
			case NoteOn:
			case AfterTouchPoly:
			case ControlChange:
			case PitchBend:
				for (int i = lPtr; i < rPtr; i = i + 2)
					sendMIDI(lastStatus, buffer[i + 1], buffer[i + 2]);
				break;
			case ProgramChange:
			case AfterTouchChannel:
				for (int i = lPtr; i < rPtr; i = i + 1)
					sendMIDI(lastStatus, buffer[i + 1]);
				break;
			default:
				break;
			}
		}
		//Point to next status
		lPtr = rPtr + 2;
		if (lPtr >= bufferSize) {
			//end of packet
			return;
		}
	}

}

END_IPMIDI_NAMESPACE
