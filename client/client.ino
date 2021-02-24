/*

*/

#define DEBUG 0

// incoming data on serial takes priority. after X seconds the RFHOST takes over
#define DATASOURCE_NONE 0
#define DATASOURCE_IS_SERIAL 1
#define DATASOURCE_IS_RFHOST 2

uint8_t currentDataSource = DATASOURCE_NONE;

unsigned long lastMidiEventFromSerial = 0;
unsigned long lastIncomingDataFromRfHost = 0;

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
  midiStuffLoop();
  loopCurrentDataSource();
  loopRF24Network();
  fakeMidiEventsLoop();
}

void loopCurrentDataSource()
{
  // serial has highest priority
  if (millis() - lastMidiEventFromSerial < 5000) {
    currentDataSource = DATASOURCE_IS_SERIAL;
    return;
  }
  if (millis() - lastIncomingDataFromRfHost < 20000) {
    currentDataSource = DATASOURCE_IS_RFHOST;
    return;
  }
  if (currentDataSource != DATASOURCE_NONE) {
    currentDataSource = DATASOURCE_NONE;
    handleMidiEventStop();
  }
  currentDataSource = DATASOURCE_NONE;
}

void debug(String Msg)
{
  if (DEBUG)
  {
    Serial.println(Msg);
  }
}
