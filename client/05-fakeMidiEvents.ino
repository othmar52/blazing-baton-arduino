

/**
 * until there 
 */

unsigned long lastFakeTrigger = 0;
uint32_t lastVerifiedHostTickNumber = 0;
unsigned long lastVerifiedHostTickMilliSecond = 0;

void reconfigureFakeMidiEventGenerator(struct payload_t payload)
{

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

  timeDeltaToHost = payload.hostTime - millis() + 25;

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
  //int ticksDelta = 0;
  //
  // if(tickCounter)
}