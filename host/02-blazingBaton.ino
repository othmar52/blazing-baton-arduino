/**
 * it seems to be better to use an 6N138 instead of 4N35 opto coupler
 * @see: http://sandsoftwaresound.net/wp-content/uploads/2016/05/schematic_midi_in.jpg
 *
 */
#include <Adafruit_NeoPixel.h>

#define PINLEDSTRIP 6 /** input pin Neopixel is attached to */
#define NUMPIXELS 16  /** number of neopixels in unit */

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PINLEDSTRIP, NEO_GRB + NEO_KHZ800);

// those variables are for "05-midiEvents.ino"
////unsigned long tickCounter = 0;      // incremental counter since clock-start-event
unsigned long lastClockStartMs = 0;       // millisecond of last clock-start-event
unsigned long lastBarStartMicros = 0;     // microsecond of last bar first tick (@24 ppqm 1, 97, 193, ) to calculate tempo or tickWidth
unsigned long lastBarStartTickNumber = 0; // the most recent start tick of bar whichs millisecond is stored in lastClockStartMs
/////uint16_t tickWidth = 8000;          // microseconds between 2 ticks
/////bool clockRunning = true;

const int countDownToQuarterNote = 64; /** end of countdown [quarter note] (should be a multiple of NUMPIXELS) */

/** TODO: remove unused color vars */
const uint32_t colorWhite = pixels.Color(255, 255, 255);
const uint32_t colorRed = pixels.Color(255, 0, 0);
const uint32_t colorGreen = pixels.Color(0, 255, 0);
const uint32_t colorBlue = pixels.Color(0, 0, 255);
const uint32_t colorCyan = pixels.Color(0, 255, 255);
const uint32_t colorMagenta = pixels.Color(255, 0, 255);
const uint32_t colorYellow = pixels.Color(255, 255, 0);

const uint32_t colorDefault = colorMagenta;
const uint32_t colorCountDown = colorBlue;
const uint32_t colorSectionIndex = colorCyan;

/** some helper vars that does not change during runtime */
const int ppqn = 24;                                /** pulses per quarter note */
const int maxTicks = ppqn * countDownToQuarterNote; /** when to reset tick counter to zero */
const int countDownStartTick = ppqn * (countDownToQuarterNote - 4);
const int NUMSTATES = NUMPIXELS + 1;
const int numSections = countDownToQuarterNote / NUMPIXELS; /** amount of loops before restart */

/** some helper vars that do change during runtime */

uint32_t tickCounterBatonLoop = 0; // incremental counter since clock-start-event
int insideQuarterNoteCounter = 0;  /** on quarter note start */
int currentSection = 1;            /** loop index (starting from 1) */
int currentStepLedIndex = 0;       /** index of last highlighted led (starting from 0) */

uint32_t lastState[NUMSTATES];
uint32_t newState[NUMSTATES];

/**
 * those data that is been sent to the client is only blazing baon/midiclock relevant
 * replace this with your own application date and make sure to modify clients "neededDelimiterAmount"
 */
String appendApplicationData()
{
  return ";" + String((clockRunning) ? "1" : "0") + ";" + String(tickCounter) + ";" + String(tickWidth);
}

void handleMidiEventStartBaton()
{
  tickCounterBatonLoop = tickCounter % maxTicks;
  insideQuarterNoteCounter = (tickCounterBatonLoop / ppqn) + 1;
}

void handleMidiEventStopBaton()
{
  tickCounterBatonLoop = tickCounter % maxTicks;
  insideQuarterNoteCounter = (tickCounterBatonLoop / ppqn) + 1;
}

// we cant use incremental operator as this script is used by host AND client
// sometims we (client) get the tickCounter set from outside
// so always refer to the tickCounter!
void handleMidiEventTickBaton()
{
  tickCounterBatonLoop = tickCounter % maxTicks;
  insideQuarterNoteCounter = (tickCounterBatonLoop / ppqn) + 1;
  currentSection = (tickCounterBatonLoop / (ppqn * NUMPIXELS)) + 1;
  //debug("inside quarter note " + String(insideQuarterNoteCounter));

  //debug("insideQuarterNoteCounter" + String(insideQuarterNoteCounter));
  //debug("currentSection" + String(currentSection));
  //if (tickCounterBatonLoop > maxTicks) {
  //  tickCounterBatonLoop = 1;
  //}
  //if (tickCounterBatonLoop%ppqn == 0) {
  //  insideQuarterNoteCounter++;
  //  if (insideQuarterNoteCounter > countDownToQuarterNote) {
  //    insideQuarterNoteCounter = 1;
  //  }
  //  currentSection = insideQuarterNoteCounter/NUMPIXELS;
  //  if (insideQuarterNoteCounter%NUMPIXELS != 0) {
  //    currentSection++;
  //  }
  //}
  prepareNewLedStates();
  checkLedChange();
}

/** this method does not trigger any led change but sets the stats in the newArray variable */
void prepareNewLedStates()
{
  if (clockRunning == false)
  {
    idleLedStates();
    return;
  }
  if (tickCounterBatonLoop < countDownStartTick)
  {
    standardLedStates();
    return;
  }
  //debug("inside quarter note " + String(insideQuarterNoteCounter));
  countdownLedStates();
}

/**
 * increment colored leds by one on each quarter note change
 * and start from 1 again when we reach the last led
 */
void standardLedStates()
{
  currentStepLedIndex = 0;
  for (int i = 0; i < NUMPIXELS; i++)
  {
    newState[i] = 0;
    if (i + 1 <= insideQuarterNoteCounter % NUMPIXELS || insideQuarterNoteCounter % NUMPIXELS == 0)
    {
      newState[i] = colorDefault;
      currentStepLedIndex = i;
    }
  }
  sectionIndexLedStates();
}

/**
 * perma lightning the last few leds depending on loop index before starting over again
 * but current step has a higher priority
 */
void sectionIndexLedStates()
{
  int ledIndexOfSection;
  for (int i = 0; i < currentSection; i++)
  {
    ledIndexOfSection = NUMPIXELS - numSections + i;
    if (ledIndexOfSection == currentStepLedIndex)
    {
      continue;
    }
    newState[ledIndexOfSection] = colorSectionIndex;
  }
}

/**
 * clock is not running
 * alternating blink of 1st and 2nd led in quarter note interval
 * TODO: this does not work in case we do not receive clock ticks
 */
void idleLedStates()
{
  clearLedStates();
  if (insideQuarterNoteCounter % 2 == 0)
  {
    newState[0] = colorDefault;
    return;
  }
  newState[1] = colorDefault;
}

void checkLedChange()
{
  if (hasStateChanged() == false)
  {
    /** nothing to do without any changes within leds */
    return;
  }
  pixels.clear();
  for (int i = 0; i < NUMPIXELS; i++)
  {
    if (newState[i] == 0)
    {
      continue;
    }
    pixels.setPixelColor(i, newState[i]);
  }
  pixels.setBrightness(newState[NUMPIXELS]);
  pixels.show();

  /** persist current led states in lastState array */
  rememberLastState();
}

bool hasStateChanged()
{
  for (int i = 0; i < NUMSTATES; i++)
  {
    if (lastState[i] != newState[i])
    {
      return true;
    }
  }
  return false;
}

/**
 * helper array to check if we need to apply any changes
 * too bad arduino does not support std::array<uint32_t,17> lastState = newState;
 */
void rememberLastState()
{
  for (int i = 0; i < NUMSTATES; i++)
  {
    lastState[i] = newState[i];
  }
}

/**
 * NOTE: led index arguments are starting from 1 (but array indices starts from zero)
 */
void batchColor(int ledIndexFrom, int ledIndexTo, uint32_t color)
{
  for (int i = ledIndexFrom; i <= ledIndexTo; i++)
  {
    newState[(i - 1)] = color;
  }
}

void clearLedStates()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    newState[i] = 0;
  }
}

/**
 * first <NUMPIXELS> values are led colors; last value is brightness
 */
void initStateArrays()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    newState[i] = 0;
  }
  newState[NUMPIXELS] = 10;
  rememberLastState();
}

void setupBaton()
{
  initStateArrays();
  pixels.begin(); /** Initializes the NeoPixel library. */
}