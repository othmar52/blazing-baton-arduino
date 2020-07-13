/**
 * interpolate midi events (ticks) between real MIDI status pushed by host
 */

unsigned long lastFakeTrigger = 0;
uint32_t lastVerifiedHostTickNumber = 0;
unsigned long lastVerifiedHostTickMilliSecond = 0;

void reconfigureFakeMidiEventGenerator(struct payload_t payload)
{
  // assume our RF latency is 25 milliseconds
  timeDeltaToHost = payload.hostTime - millis() + 25;
  if (clockRunning == true && payload.clockRunning == false)
  {
    handleMidiEventStop();
  }
  if (clockRunning == false && payload.clockRunning == true)
  {
    handleMidiEventStart();
  }

  tickWidth = payload.tickWidth;
  lastVerifiedHostTickNumber = payload.currentTick;
  lastVerifiedHostTickMilliSecond = payload.hostTime;

  recalculateCurrentTickNumber();
}

void recalculateCurrentTickNumber()
{
  unsigned long myTickShoudNowBe = lastVerifiedHostTickNumber + (((getCurrentHostMillis() - lastVerifiedHostTickMilliSecond) * 1000) / tickWidth);
  if (myTickShoudNowBe == tickCounter)
  {
    // we are still in sync
    return;
  }
  tickCounter = myTickShoudNowBe - 1;
  handleMidiEventTick();
}

void triggerFakeEventTick()
{
  recalculateCurrentTickNumber();
}

void fakeMidiEventsLoop()
{
  recalculateCurrentTickNumber();
}
