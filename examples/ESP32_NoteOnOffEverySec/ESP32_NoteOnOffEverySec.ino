#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include "ipMIDI.h"

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

char ssid[] = "The Hefty Network"; //  your network SSID (name)
char pass[] = "0208196700";    // your network password (use for WPA, or use as key for WEP)

IPMIDI_CREATE_INSTANCE(WiFiUDP, ipMIDI); 

unsigned long startTime = millis();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  // Serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Booting");

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F(""));
  Serial.println(F("WiFi connected"));


  Serial.println();
  Serial.print(F("IP address is "));
  Serial.println(WiFi.localIP());

  ipMIDI.begin();

  ipMIDI.setHandleNoteOn(OnMidiNoteOn);
  ipMIDI.setHandleNoteOff(OnMidiNoteOff);

  Serial.println(F("looping"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  ipMIDI.read();

  auto now = millis();
  if (now - startTime >= 1000)
  {
    Serial.println(F("note"));
    ipMIDI.sendNoteOn(60, 127, 1);
    startTime = now;
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