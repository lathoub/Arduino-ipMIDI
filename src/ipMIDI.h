#pragma once

#include <Ethernet.h>

#include <MIDI.h>
#include <midi_RingBuffer.h>

BEGIN_MIDI_NAMESPACE

struct ipMIDIDefaultSettings : public DefaultSettings
{
	static constexpr uint16_t BufferSize = 16;

	static constexpr uint16_t Port = 21928;
};

END_MIDI_NAMESPACE

BEGIN_MIDI_NAMESPACE

static const IPAddress ipMIDI_multicast(225, 0, 0, 37);

template<class UdpClass>
class ipMidiTransport 
{
public:
	ipMidiTransport()
		:	port_(ipMIDIDefaultSettings::Port)
	{
		mRxBuffer.clear();
	};

public:
	inline void begin(const int port = ipMIDIDefaultSettings::Port, const Channel inChannel = 1)
	{
		port_ = port;
		dataPort_.beginMulticast(ipMIDI_multicast, port_);
	}

	inline bool beginTransmission()
	{
		dataPort_.beginPacket(ipMIDI_multicast, port_);

		return true;
	};

	inline void write(byte byte)
	{
		dataPort_.write(byte);
	};

	inline void endTransmission()
	{
		dataPort_.endPacket();
	};

	inline byte read()
	{
		return mRxBuffer.read();
	};

	inline unsigned available()
	{
		Ethernet.maintain();

		auto packetSize = dataPort_.parsePacket();
		if (packetSize > 0)
			while (packetSize-- > 0)
				mRxBuffer.write(dataPort_.read());

		return mRxBuffer.getLength();
	};

private:
	UdpClass dataPort_;

    typedef RingBuffer<byte, ipMIDIDefaultSettings::BufferSize> Buffer;
    Buffer mRxBuffer;

	uint16_t port_;
};

#define IPMIDI_CREATE_INSTANCE(Type, Name)             \
	typedef midi::ipMidiTransport<Type> __st;   \
	__st st;                                           \
	midi::MidiInterface<__st> Name((__st&)st);

#define IPMIDI_CREATE_DEFAULT_INSTANCE()               \
	IPMIDI_CREATE_INSTANCE(EthernetUDP, ipMIDI);

END_MIDI_NAMESPACE