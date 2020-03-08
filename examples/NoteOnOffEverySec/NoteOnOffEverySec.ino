#include <Ethernet.h>

#define DEBUG 7
#include <ipMIDI.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPMIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t1 = millis();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  DEBUG_BEGIN(115200);

  N_DEBUG_PRINTLN(F("Getting IP address..."));

  if (Ethernet.begin(mac) == 0) {
    F_DEBUG_PRINTLN(F("Failed DHCP, check network cable & reboot"));
    for (;;);
  }

  N_DEBUG_PRINT(F("IP address is "));
  N_DEBUG_PRINTLN(Ethernet.localIP());

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
  // Listen to incoming notes
  MIDI.read();

  // send note on/off every second
  // (dont cáll delay(1000) as it will stall the pipeline)
  if ((millis() - t1) > 500)
  {
    t1 = millis();
    //   Serial.print(F(".");

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
  Serial.print(F("Incoming NoteOn from channel:"));
  Serial.print(channel);
  Serial.print(F(" note:"));
  Serial.print(note);
  Serial.print(F(" velocity:"));
  Serial.print(velocity);
  Serial.println();
}


// -----------------------------------------------------------------------------
// received note off
// -----------------------------------------------------------------------------
void OnMidiNoteOff(byte channel, byte note, byte velocity) {
  Serial.print(F("Incoming NoteOff from channel:"));
  Serial.print(channel);
  Serial.print(F(" note:"));
  Serial.print(note);
  Serial.print(F(" velocity:"));
  Serial.print(velocity);
  Serial.println();
}
