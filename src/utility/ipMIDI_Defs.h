#pragma once

#include "ipMIDI_Namespace.h"

#if ARDUINO
	#include <Arduino.h>
	#include <Ethernet.h>
#else
	#include <inttypes.h>
	typedef uint8_t byte;
#endif

BEGIN_IPMIDI_NAMESPACE

/*! \brief Create an instance of the library
 */
#define IPMIDI_CREATE_INSTANCE(Type, Name)                            \
    IPMIDI_NAMESPACE::ipMIDIinterface<Type> Name;

/*! \brief
*/
#define IPMIDI_CREATE_DEFAULT_INSTANCE()                                      \
    IPMIDI_CREATE_INSTANCE(EthernetUDP, ipm);

END_IPMIDI_NAMESPACE
