/*

*/

#define DEBUG 0

void setup()
{
  Serial.begin(31250); // MIDI baudrate
  //Serial.begin(115200);
  setupRF24Network();
  setupMidiStuff();
  setupBaton();
}

void loop()
{
  loopRF24Network();
  midiStuffLoop();
}

void debug(String Msg)
{
  if (DEBUG)
  {
    Serial.println(Msg);
  }
}
