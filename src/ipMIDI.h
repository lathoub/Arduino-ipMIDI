#pragma once

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
		auto success = dataPort_.beginMulticast(ipMIDI_multicast, port_);
		if (!success)
			Serial.println("beginPacket failed");
	}

	inline bool beginTransmission()
	{
		auto success = dataPort_.beginPacket(ipMIDI_multicast, port_);
		if (!success)
			Serial.println("beginPacket failed");

		return success;
	};

	inline void write(byte byte)
	{
		auto bytesWritten = dataPort_.write(byte);
		if (1 != bytesWritten)
			Serial.println("write 1 != bytesWritten");
	};

	inline void endTransmission()
	{
		auto success = dataPort_.endPacket();
		if (!success)
			Serial.println("endPacket failed");
	};

	inline byte read()
	{
		return mRxBuffer.read();
	};

	inline unsigned available()
	{
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