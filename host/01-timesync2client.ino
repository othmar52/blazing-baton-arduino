/*
  Arduino Wireless Network - Multiple NRF24L01 Tutorial
  == Example 01 - Servo Control / Node 00 - Potentiometer ==
  by Dejan, www.HowToMechatronics.com
  Libraries:
  nRF24/RF24, https://github.com/nRF24/RF24
  nRF24/RF24Network, https://github.com/nRF24/RF24Network
*/
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

unsigned long lastDistrubution = 0;
const uint16_t distributionInterval = 5000;

unsigned long tickCounter = 0; // incremental counter since clock-start-event
uint16_t tickWidth = 8000;     // time distance between 2 ticks [microseconds]
bool clockRunning = true;

RF24 radio(10, 9);          // nRF24L01 (CE,CSN)
RF24Network network(radio); // Include the radio in the network

// Address of nodes in Octal format ( 04,031, etc)
const uint16_t this_node = 00;
const uint16_t clientNodes[5] = {01, 02, 03, 04, 05};

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
  network.begin(90, this_node); // (channel, node address)
  // TODO: check if we have to care about non existing clients
  //network.txTimeout = 10;
  //radio.setRetries(1,15);
}

void loopRF24Network()
{
  network.update();

  if (millis() - lastDistrubution < distributionInterval)
  {
    return;
  }
  pushToAllConnectedClients();
}

void pushToAllConnectedClients()
{

  payload_t payload = {
      millis(),
      clockRunning,
      tickWidth,
      tickCounter};

  for (uint8_t i = 0; i < 5; i++)
  {
    RF24NetworkHeader header(clientNodes[i]);                   // (Address where the data is going)
    bool ok = network.write(header, &payload, sizeof(payload)); // Send the data
  }
  lastDistrubution = millis();
}
