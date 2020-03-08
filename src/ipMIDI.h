#pragma once

#include "utility/Logging.h"

#include <MIDI.h>
using namespace MIDI_NAMESPACE;

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
        // if we were called very soon after the board was booted, we need to give the
        // EthernetShield (WIZnet) some time to come up. Hence, we delay until millis() is at
        // least 3000. This is necessary, so that if we need to add a service record directly
        // after begin, the announce packet does not get lost in the bowels of the WIZnet chip.
        while (millis() < 3000) delay(100);

        auto success = dataPort_.beginMulticast(ipMIDIMulticastAddr, port_);
		if (!success)
			E_DEBUG_PRINTLN("beginMulticast failed");
	}

	bool beginTransmission(MidiType)
	{
        auto success = dataPort_.beginPacket(ipMIDIMulticastAddr, port_);
		if (!success)
			E_DEBUG_PRINTLN("beginPacket failed");

		return success;
	};

	void write(byte byte)
	{
		auto success = dataPort_.write(byte);
        if (!success)
            E_DEBUG_PRINTLN("write failed");
	};

	void endTransmission()
	{
        auto success = dataPort_.endPacket();
        if (!success)
            E_DEBUG_PRINTLN("endPacket failed");
	};

	byte read()
	{
        return 0;//dataPort_.read();
	};

	unsigned available()
	{
        return 0;//dataPort_.parsePacket();
	};

private:
	UdpClass dataPort_;

	uint16_t port_;
};

#define IPMIDI_CREATE_INSTANCE(Type, midiName, ipMidiName, portNr)  \
    typedef IPMIDI_NAMESPACE::ipMidiTransport<Type> __amt; \
    __amt ipMidiName(portNr); \
    MIDI_NAMESPACE::MidiInterface<__amt> midiName((__amt &)ipMidiName);

#define IPMIDI_CREATE_DEFAULT_INSTANCE()  \
    IPMIDI_CREATE_INSTANCE(EthernetUDP, MIDI, ipMIDI, 21928);

END_IPMIDI_NAMESPACE
