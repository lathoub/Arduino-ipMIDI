#pragma once

#include "utility/Logging.h"

// this is an exported and stripped down version of the MIDI library by  47 blabla
// feat 4.4.0 summer 2019
#include "utility/midi_feat4_4_0/MIDI.h"

#include "ipMIDI_Namespace.h"

BEGIN_IPMIDI_NAMESPACE

static uint8_t ipMIDIMulticastAddr[] = {225, 0, 0, 37};

template <class UdpClass>
class ipMidiTransport
{
public:
	ipMidiTransport(uint16_t port = 21928)
		:	port_(port)
	{
	};

public:
	void begin(MIDI_NAMESPACE::Channel inChannel = 1)
	{
        auto success = dataPort_.beginMulticast(ipMIDIMulticastAddr, port_);
		if (!success)
			E_DEBUG_PRINTLN("beginPacket failed");
	}

	bool beginTransmission()
	{
        auto success = dataPort_.beginPacket(ipMIDIMulticastAddr, port_);
		if (!success)
			E_DEBUG_PRINTLN("beginPacket failed");

		return success;
	};

	void write(byte byte)
	{
		dataPort_.write(byte);
	};

	void endTransmission()
	{
        dataPort_.endPacket();
        dataPort_.flush();
	};

	byte read()
	{
		return dataPort_.read();
	};

	unsigned available()
	{
		return dataPort_.parsePacket();
	};

private:
	UdpClass dataPort_;

	uint16_t port_;
};

END_IPMIDI_NAMESPACE
