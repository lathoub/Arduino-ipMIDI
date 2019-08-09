#pragma once

#include "utility/MIDI.h"

BEGIN_MIDI_NAMESPACE

struct ipMIDIDefaultSettings : AbstractDefaultSettings
{
	static const long Port = 21928;
};

END_MIDI_NAMESPACE

BEGIN_MIDI_NAMESPACE

template<class UdpClass, class _Settings = ipMIDIDefaultSettings>
class ipMidiInterface : public AbstractMidiInterface<ipMIDIDefaultSettings>
{
public:
	typedef _Settings Settings;

public:
	ipMidiInterface()
		:	packetBufferIndex_(0),
			packetAmountRead_(0)
	{
		// fixed, part of the protocol
		multiIP_ = IPAddress(225, 0, 0, 37);
	};

	inline bool begin(const int port = Settings::Port, const Channel inChannel = 1)
	{
		AbstractMidiInterface<Settings>::begin(inChannel);

		port_ = port;
		return (dataPort_.beginMulticast(multiIP_, port_));
	}

protected:
	inline void __beginWrite()
	{
		dataPort_.beginPacket(multiIP_, port_);
	};

	inline void __write(byte byte)
	{
		dataPort_.write(byte);
	};

	inline void __endWrite()
	{
		dataPort_.endPacket();
	};

	inline byte __read()
	{
		return packetBuffer_[packetAmountRead_ - packetBufferIndex_];
	};

	inline int __available()
	{
		if (packetBufferIndex_ > 0)
			packetBufferIndex_--; // data is still in the packetBuffer_, empty it first
		else
		{
			auto packetSize = dataPort_.parsePacket();
			if (0 == packetSize) return 0; // if nothing is available, leave here

			// data is ready to be read, do not read more than what we have memory for
			packetBufferIndex_ = packetAmountRead_ = dataPort_.read(packetBuffer_, sizeof(packetBuffer_));

			Serial.print("Incoming: ");
			for (int i = 0; i < packetAmountRead_; i++)
			{
				Serial.print(packetBuffer_[i], HEX);
				Serial.print(" ");
			}
			Serial.println("");
		}

		return packetBufferIndex_;
	};

private:
	UdpClass dataPort_;

	uint16_t port_ = Settings::Port;
	IPAddress multiIP_;

	byte		packetBuffer_[UDP_TX_PACKET_MAX_SIZE];
	uint16_t	packetBufferIndex_;
	uint16_t	packetAmountRead_;
};

#define IPMIDI_CREATE_INSTANCE(Type, Name)                            \
	midi::ipMidiInterface<Type> Name;

#define IPMIDI_CREATE_DEFAULT_INSTANCE()                                      \
	IPMIDI_CREATE_INSTANCE(EthernetUDP, ipMIDI);

END_MIDI_NAMESPACE