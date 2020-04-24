#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include <ipMIDI.h>

// Dependency:
// https://github.com/lathoub/arduino_midi_library
// branch: static-polymorphism

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

const char ssid[] = WIFI_SSID;
const char pass[] = WIFI_PASSWD;

IPMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, ipMIDI, 21928);

unsigned long t1 = millis();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  DEBUG_BEGIN(115200);

  N_DEBUG_PRINTLN(F("Getting IP address..."));

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    N_DEBUG_PRINT(F("."));

  }

  N_DEBUG_PRINTLN(F("WiFi connected"));
  N_DEBUG_PRINT(F("IP address is "));
  N_DEBUG_PRINTLN(WiFi.localIP());

  // Listen for MIDI messages on channel 1
  MIDI.begin(1);

  MIDI.setHandleNoteOn(OnMidiNoteOn);
  MIDI.setHandleNoteOff(OnMidiNoteOff);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  MIDI.read();

  if ((millis() - t1) > 500)
  {
    t1 = millis();

    byte note = random(1, 127);
    byte velocity = 55;
    byte channel = 1;

    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
  }
}

// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// received note on
// -----------------------------------------------------------------------------
void OnMidiNoteOn(byte channel, byte note, byte velocity) {
  N_DEBUG_PRINT(F("Incoming NoteOn  from channel: "));
  N_DEBUG_PRINT(channel);
  N_DEBUG_PRINT(F(", note: "));
  N_DEBUG_PRINT(note);
  N_DEBUG_PRINT(F(", velocity: "));
  N_DEBUG_PRINTLN(velocity);
}


// -----------------------------------------------------------------------------
// received note off
// -----------------------------------------------------------------------------
void OnMidiNoteOff(byte channel, byte note, byte velocity) {
  N_DEBUG_PRINT(F("Incoming NoteOff from channel: "));
  N_DEBUG_PRINT(channel);
  N_DEBUG_PRINT(F(", note: "));
  N_DEBUG_PRINT(note);
  N_DEBUG_PRINT(F(", velocity: "));
  N_DEBUG_PRINTLN(velocity);
}
