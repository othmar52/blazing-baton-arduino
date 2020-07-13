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
uint16_t tickWidth = 8000;     // microseconds between 2 ticks
bool clockRunning = true;

RF24 radio(10, 9);          // nRF24L01 (CE,CSN)
RF24Network network(radio); // Include the radio in the network

const uint16_t this_node = 00; // Address of this node in Octal format ( 04,031, etc)
const uint16_t node01 = 01;
const uint16_t node02 = 02;
const uint16_t node03 = 03;
const uint16_t node04 = 04;
const uint16_t node05 = 05;

const uint16_t clientNodes[5] = {01, 02, 03, 04, 05};
struct payload_t
{ // Structure of our payload
  unsigned long hostTime;
  bool clockRunning;
  uint16_t tickWidth;
  unsigned long currentTick;
};

void setupRF24Network()
{
  SPI.begin();
  radio.begin();
  //radio.setRetries(1,15);
  network.begin(90, this_node); // (channel, node address)
  //network.txTimeout = 10;
}

void loopRF24Network()
{
  network.update();

  if (millis() - lastDistrubution > distributionInterval)
  {
    pushToAllConnectedClients();
  }
}

void pushToAllConnectedClients()
{

  payload_t payload = {
      millis(),
      clockRunning,
      tickWidth,
      tickCounter};

  //bool ok = network.write(header, &payload, sizeof(payload)); // Send the data

  for (uint8_t i = 0; i < 5; i++)
  {
    RF24NetworkHeader header(clientNodes[i]);                   // (Address where the data is going)
    bool ok = network.write(header, &payload, sizeof(payload)); // Send the data
  }
  lastDistrubution = millis();
}