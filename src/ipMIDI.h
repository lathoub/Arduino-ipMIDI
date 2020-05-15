#pragma once

#include <MIDI.h>
using namespace MIDI_NAMESPACE;

#include "ipMIDI_Namespace.h"

#ifndef UDP_TX_PACKET_MAX_SIZE
#define UDP_TX_PACKET_MAX_SIZE 24
#endif

BEGIN_IPMIDI_NAMESPACE

static const uint8_t ipMIDIMulticastAddr[] = { 225, 0, 0, 37 };

template <class UdpClass>
class ipMidiTransport
{
    friend class MIDI_NAMESPACE::MidiInterface<ipMidiTransport<UdpClass>>;

public:
	ipMidiTransport(uint16_t port = 21928)
		:	port_(port)
	{
	};

    void begin()
	{
        dataPort_.beginMulticast(ipMIDIMulticastAddr, port_);
	}

protected:
	static const bool thruActivated = false;
		
	bool beginTransmission(MidiType)
	{
        return dataPort_.beginPacket(ipMIDIMulticastAddr, port_);
	};

	void write(byte byte)
	{
		dataPort_.write(byte);
	};

	void endTransmission()
	{
        dataPort_.endPacket();
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
            if (packetTotalSize_ == 0)
            {
                packetTotalSize_ = dataPort_.parsePacket();
                if (0 == packetTotalSize_) return 0; // if nothing is available, leave here
            }

            // data is ready to be read, do not read more than what we have memory for
            packetTotalSize_ -= packetBufferIndex_ = packetAmountRead_ = dataPort_.read(packetBuffer_, UDP_TX_PACKET_MAX_SIZE);
        }
        return packetBufferIndex_;
	};

private:
    byte        packetBuffer_[UDP_TX_PACKET_MAX_SIZE];
    uint16_t    packetBufferIndex_;
    uint16_t    packetAmountRead_;
    uint16_t    packetTotalSize_;

    UdpClass    dataPort_;
	uint16_t    port_;
};

#define IPMIDI_CREATE_INSTANCE(Type, Name, Port)  \
    IPMIDI_NAMESPACE::ipMidiTransport<Type> ip##Name(Port); \
    MIDI_NAMESPACE::MidiInterface<IPMIDI_NAMESPACE::ipMidiTransport<Type>> Name((IPMIDI_NAMESPACE::ipMidiTransport<Type> &)ip##Name);

#define IPMIDI_CREATE_DEFAULT_INSTANCE()  \
    IPMIDI_CREATE_INSTANCE(EthernetUDP, MIDI, 21928);

END_IPMIDI_NAMESPACE
