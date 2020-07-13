/**
 * those functions handles the last 4 quarter notes of a loop
 * 
 */

void countdownLedStates()
{
  countdownLedStatesV2();
  /** countdownLedStatesV2(); */
}

/**
 * countdownLedStatesV1()
 **********************************************
 *   3   X X X X  - - - -  - - - -  - - - -
 *   2   - - - -  X X X X  - - - -  - - - -
 *   1   - - - -  - - - -  X X X X  - - - -
 *  and  X X X X  X X X X  X X X X  X X X X
 **********************************************
 */
void countdownLedStatesV1()
{
  const int flashDuration = 23; /** time in [ticks] before turning leds off */

  switch (tickCounterBatonLoop)
  {
  case countDownStartTick:
    clearLedStates();
    batchColor(1, 4, colorCountDown);
    break;
  case countDownStartTick + ppqn:
    clearLedStates();
    batchColor(5, 8, colorCountDown);
    break;
  case countDownStartTick + 2 * ppqn:
    clearLedStates();
    batchColor(9, 12, colorCountDown);
    break;
  case countDownStartTick + 3 * ppqn:
    uint16_t
        batchColor(1, 16, colorCountDown);
    break;
  case countDownStartTick + flashDuration:
  case countDownStartTick + flashDuration + ppqn:
  case countDownStartTick + flashDuration + 2 * ppqn:
  case maxTicks - 1:
    clearLedStates();
    break;
  }
}

/**
 * countdownLedStatesV2()
 **********************************************
 *   3   X X X X  - - X X  X X - -  X X X X
 *   2   - - X X  X X - -  - - X X  X X - -
 *   1   - - - -  - - X X  X X - -  - - - -
 *  and  X X X X  X X X X  X X X X  X X X X
 **********************************************
 */
void countdownLedStatesV2()
{
  const int flashDuration = 23; /** time in [ticks] before turning leds off */

  switch (tickCounterBatonLoop)
  {
  case countDownStartTick:
    clearLedStates();
    batchColor(1, 4, colorCountDown);
    batchColor(7, 10, colorCountDown);
    batchColor(13, 16, colorCountDown);
    break;
  case countDownStartTick + ppqn:
    clearLedStates();
    batchColor(3, 6, colorCountDown);
    batchColor(11, 14, colorCountDown);
    break;
  case countDownStartTick + 2 * ppqn:
    clearLedStates();
    batchColor(7, 10, colorCountDown);
    break;
  case countDownStartTick + 3 * ppqn:
    batchColor(1, 16, colorCountDown);
    break;
  case countDownStartTick + flashDuration:
  case countDownStartTick + flashDuration + ppqn:
  case countDownStartTick + flashDuration + 2 * ppqn:
  case maxTicks - 1:
    clearLedStates();
    break;
  }
}
