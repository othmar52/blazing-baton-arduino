
/**
 * thanks to https://www.notesandvolts.com/2015/02/midi-and-arduino-build-midi-input.html
   TODO: read midi events (clock,start, stop) from serial
   TODO: on high tempos we dont need to recalculate tickWidth that often (currently every 4 quarter notes independent of tempo) ...
   TODO: does it make sense to add a hardware switch for choosing network client or serial (din midi in) client?
*/

const uint16_t minBpmTickWidth = 65000; // [microseconds] =~ 38 BPM
const uint16_t maxBpmTickWidth = 8000;  // [microseconds] =~ 312 BPM

// avoid sending invalid tempo to clients by storing the last
// few tick widths calculations and send value that exists most often
const uint8_t debounceLength = 8;
uint8_t currentTickWidthDebouncer = 0;
uint16_t debouncedTickWidths[debounceLength];
uint16_t indexMappingDebounce[debounceLength];
uint16_t counts[debounceLength];

#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

void setupMidiStuff()
{
  MIDI.begin(); // Launch MIDI, by default listening to channel 1.
  MIDI.setHandleClock(handleMidiEventTickFromSerial);
  MIDI.setHandleTick(handleMidiEventTickFromSerial);
  MIDI.setHandleStop(handleMidiEventStopFromSerial);
  MIDI.setHandleStart(handleMidiEventStartFromSerial);
}

void midiStuffLoop()
{
  MIDI.read();
}

void handleMidiEventTickFromSerial() {
  lastMidiEventFromSerial = millis();
  handleMidiEventTick();
}

void handleMidiEventTick()
{
  tickCounter++;
  // do stuff thats only blazing baton related
  handleMidiEventTickBaton();
  if (tickCounter == 1)
  {
    lastBarStartMicros = micros();
    // we cant reclculte tempo yet
    return;
  }
  if (this_node != 00)
  {
    // only server ("0") needs to calculate tickwidth for submitting to clients
    return;
  }
  // recalculate tempo(tickWidth) after each bar
  if (tickCounter % (4 * ppqn) == 1)
  {
    recalculateTickWidth();
  }
}

void recalculateTickWidth()
{
  uint16_t newTickWidth = (micros() - lastBarStartMicros) / 96; // microseconds
  if (newTickWidth < maxBpmTickWidth)
  {
    //debug("DANGER: calculated tickWidth " + String(newTickWidth) + " is shit! setting to " + String(maxBpmTickWidth)); // we dont need more bpm
    newTickWidth = maxBpmTickWidth;
  }
  if (newTickWidth > minBpmTickWidth)
  {
    //debug("DANGER: calculated tickWidth " + String(newTickWidth) + " is shit! setting to " + String(minBpmTickWidth)); // we dont need less bpm
    newTickWidth = minBpmTickWidth;
  }
  lastBarStartMicros = micros();
  debouncedTickWidths[currentTickWidthDebouncer] = newTickWidth;
  currentTickWidthDebouncer = (currentTickWidthDebouncer == debounceLength - 1) ? 0 : currentTickWidthDebouncer + 1;

  //debug("recalculate TickWidth to\t" + String(tickWidth) + " microsec\t" + String(tickWidthToBpm()) + " BPM. now debouncing... with debouncer index " + String(currentTickWidthDebouncer));

  setDebouncedTickWidth();
  //tickWidth = newTickWidth;
}

int getIndexOfMaximumValue(uint16_t *array, uint8_t size)
{
  uint8_t maxIndex = 0;
  uint16_t max = array[maxIndex];
  for (uint8_t i = 1; i < size; i++)
  {
    if (max < array[i])
    {
      max = array[i];
      maxIndex = i;
    }
  }
  return maxIndex;
}

int findIndexInArray(uint16_t *debouncedTickWidths, int size, uint16_t target)
{
  int i = 0;
  while ((i < size) && (debouncedTickWidths[i] != target))
    i++;

  return (i < size) ? (i) : (-1);
}

void setDebouncedTickWidth()
{
  for (int j = 0; j < debounceLength; j++)
  {
    counts[j] = 0;
    indexMappingDebounce[j] = 0;
  }
  for (int j = 0; j < debounceLength; j++)
  {
    // find index in array
    int indexInArray = findIndexInArray(indexMappingDebounce, debounceLength, debouncedTickWidths[j]);
    // if index does not exist, add value to indexMappingDebounce and create a count
    // in counts array based on index
    if (indexInArray == -1 && debouncedTickWidths[j] > 0)
    {
      indexMappingDebounce[j] = debouncedTickWidths[j];
      counts[j] = 1;
      continue;
    }

    // increase count of existing value in count array
    counts[indexInArray] = counts[indexInArray] + 1;

    // count is already bigger than half of array, no need to go any further
    if (counts[indexInArray] > ceil(debounceLength / 2))
    {
      // debug("terminate because count is already big enough");
      break;
    }
  }
  tickWidth = indexMappingDebounce[getIndexOfMaximumValue(counts, debounceLength)];
}

float tickWidthToBpm()
{
  // calculate interval of clock tick (24 ppqn)
  if (tickWidth < 1)
  {
    return 0;
  }
  return 60 / (tickWidth * 0.000001 * ppqn);
}

void handleMidiEventStartFromSerial() {
  lastMidiEventFromSerial = millis();
  handleMidiEventStart();
}

void handleMidiEventStart()
{
  clockRunning = true;
  tickCounter = 0;

  // do stuff thats only blazing baton related
  handleMidiEventStartBaton();

  // only relevant for host
  pushToAllConnectedClients();
}

void handleMidiEventStopFromSerial() {
  lastMidiEventFromSerial = millis();
  handleMidiEventStop();
}

void handleMidiEventStop()
{
  clockRunning = false;
  tickCounter = 0;

  // do stuff thats only blazing baton related
  handleMidiEventStopBaton();

  // only relevant for host
  pushToAllConnectedClients();
}
