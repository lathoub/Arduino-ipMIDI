#pragma once

#include <MIDI.h>
#include <midi_RingBuffer.h>

BEGIN_MIDI_NAMESPACE

struct ipMIDIDefaultSettings : public DefaultSettings
{
	static constexpr uint16_t Port = 21928;
};

END_MIDI_NAMESPACE

BEGIN_MIDI_NAMESPACE

template<class UdpClass>
class ipMidiTransport 
{
public:
	ipMidiTransport()
	{
		// fixed, part of the protocol
		multiIP_ = IPAddress(225, 0, 0, 37);

		mRxBuffer.clear();
	};

public:
	inline void begin(const int port = ipMIDIDefaultSettings::Port, const Channel inChannel = 1)
	{
		port_ = 21928;
		dataPort_.beginMulticast(multiIP_, port_);
	}

	inline void beginTransmission()
	{
		dataPort_.beginPacket(multiIP_, port_);
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
		auto packetSize = dataPort_.parsePacket();
		if (packetSize > 0)
		{
			while (packetSize-- > 0)
				mRxBuffer.write(dataPort_.read());
		}

		return mRxBuffer.getLength();
	};

private:
	UdpClass dataPort_;

    typedef RingBuffer<byte, 256> Buffer;
    Buffer mRxBuffer;

	uint16_t port_;
	IPAddress multiIP_;
};

#define IPMIDI_CREATE_INSTANCE(Type, Name)             \
	typedef midi::ipMidiTransport<EthernetUDP> __st;   \
	__st st;                                           \
	midi::MidiInterface<__st> Name((__st&)st);

#define IPMIDI_CREATE_DEFAULT_INSTANCE()               \
	IPMIDI_CREATE_INSTANCE(EthernetUDP, ipMIDI);

END_MIDI_NAMESPACE