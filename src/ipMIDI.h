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
		mRxBuffer.clear();

		// fixed, part of the protocol
		multiIP_ = IPAddress(225, 0, 0, 37);
	};

public:
	inline void begin(const int port = ipMIDIDefaultSettings::Port, const Channel inChannel = 1)
	{
		port_ = port;
		auto success = dataPort_.beginMulticast(multiIP_, port_);
//		if (!success)
//			Serial.println("probleem met beginMulticast");
	}

	inline void beginTransmission()
	{
		auto success = dataPort_.beginPacket(multiIP_, port_);
		//if (!success)
		//	Serial.println("probleem met beginPacket");
	};

	inline void write(byte byte)
	{
		dataPort_.write(byte);
	};

	inline void endTransmission()
	{
		auto success = dataPort_.endPacket();
		//if (!success)
		//	Serial.println("probleem met endPacket");
	};

	inline byte read()
	{
		return mRxBuffer.read();
	};

	inline unsigned available()
	{
		return 0;

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