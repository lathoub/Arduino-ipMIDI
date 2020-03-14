#pragma once

#include "utility/Logging.h"

#include <MIDI.h>
using namespace MIDI_NAMESPACE;

#include "ipMIDI_Namespace.h"

#ifndef UDP_TX_PACKET_MAX_SIZE
#define UDP_TX_PACKET_MAX_SIZE 24
#endif

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
		if (success == 0)
			E_DEBUG_PRINTLN("beginPacket failed");

		return success;
	};

	void write(byte byte)
	{
		auto bytesWritten = dataPort_.write(byte);
        if (bytesWritten != 1)
            E_DEBUG_PRINTLN("write failed");
	};

	void endTransmission()
	{
        auto success = dataPort_.endPacket();
        if (success == 0)
            E_DEBUG_PRINTLN("endPacket failed");
	};

	byte read()
	{
        return packetBuffer_[packetAmountRead_ - packetBufferIndex_];
	};

	unsigned available()
	{
        if (packetBufferIndex_ > 0) // empty buffer first, before reading new data
            packetBufferIndex_--;
        else
        {
            auto packetSize = dataPort_.parsePacket();
            if (0 == packetSize) return 0; // if nothing is available, leave here
            // data is ready to be read, do not read more than what we have memory for
            packetBufferIndex_ = packetAmountRead_ = dataPort_.read(packetBuffer_, packetSize);
        }
        return packetBufferIndex_;
	};

private:
    byte        packetBuffer_[UDP_TX_PACKET_MAX_SIZE];
    uint16_t    packetBufferIndex_;
    uint16_t    packetAmountRead_;

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
