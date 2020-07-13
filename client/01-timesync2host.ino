

/*
  Arduino Wireless Network - Multiple NRF24L01 Tutorial
  == Example 01 - Servo Control / Node 01 - Servo motor ==
*/
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

unsigned long tickCounter = 0; // incremental counter since clock-start-event
uint16_t tickWidth = 8000;     // time distance between 2 ticks [microseconds]
bool clockRunning = true;

int32_t timeDeltaToHost = 0;

RF24 radio(10, 9);             // nRF24L01 (CE,CSN)
RF24Network network(radio);    // Include the radio in the network
const uint16_t this_node = 01; // Address of our node in Octal format ( 04,031, etc)

unsigned long lastIncomingData = 0;

// Structure of our payload
struct payload_t
{
  unsigned long hostTime;
  bool clockRunning;
  uint16_t tickWidth;
  unsigned long currentTick;
};

void setupRF24Network()
{
  SPI.begin();
  radio.begin();
  network.begin(90, this_node); //(channel, node address)
}

void loopRF24Network()
{
  network.update();
  while (network.available())
  { // Is there any incoming data?
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    reconfigureFakeMidiEventGenerator(payload);
    lastIncomingData = millis();
  }
}

// only needed for host. but due to shared "05-midiEvents.ino" (host + client) we need this defined for compiling
void pushToAllConnectedClients()
{
}

uint32_t getCurrentHostMillis()
{
  return millis() + timeDeltaToHost;
}
