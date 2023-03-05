/*
TeensyTouch - Non-Blocking Capacitance Touch Reading Library for Teensy boards

Author: Arjuna Deva (https://github.com/arjuna-dev)
Original idea is from PJRC forum user Lalbert (https://forum.pjrc.com/)
Based on PaulStoffregen's teensy cores touch.c library (https://github.com/PaulStoffregen/cores/blob/master/teensy3/touch.c)

MIT License

TeensyTouch is a library for Teensy boards that allows for non-blocking mode capacitance touch reading.
*/

#include "core_pins.h"
#include "Arduino.h"
#include "TeensyTouch.h"


#if defined(__MK20DX128__) || defined(__MK20DX256__)
// These settings give approx 0.02 pF sensitivity and 1200 pF range
// Lower current, higher number of scans, and higher prescaler
// increase sensitivity, but the trade-off is longer measurement
// time and decreased range.
#define CURRENT   2 // 0 to 15 - current to use, value is 2*(current+1)
#define NSCAN     9 // number of times to scan, 0 to 31, value is nscan+1
#define PRESCALE  2 // prescaler, 0 to 7 - value is 2^(prescaler+1)
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255,  12, 255, 255, 255, 255,
255, 255,  11,   5
};

#elif defined(__MK66FX1M0__)
#define NSCAN     9
#define PRESCALE  2
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255, 255, 255,  11,
 12, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

#elif defined(__MKL26Z64__)
#define NSCAN     9
#define PRESCALE  2
static const uint8_t pin2tsi[] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255,   2,   3, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255
};

#endif


TeensyTouch::TeensyTouch(int pins_array[], int pins_array_size, int touch_values_array[], int touch_values_array_size){
    _pins_array_size = pins_array_size;
    _touch_values_array_size = touch_values_array_size;

    _pins_array_first_value = pins_array;
    _touch_values_array_first_value = touch_values_array;

    teensyTouchInit(*pins_array);
}


int TeensyTouch::teensyTouchReturn() {
#if defined(HAS_KINETIS_TSI)
  return *((volatile uint16_t *)(&TSI0_CNTR1) + ch);
#elif defined(HAS_KINETIS_TSI_LITE)
  delayMicroseconds(1);
  return TSI0_DATA & 0xFFFF;
#endif
}

int TeensyTouch::teensyTouchInit(uint8_t pin) {
  uint32_t ch;

  if (pin >= NUM_DIGITAL_PINS) return -1;
  ch = pin2tsi[pin];
  if (ch == 255) return -1;

  *portConfigRegister(pin) = PORT_PCR_MUX(0);
  SIM_SCGC5 |= SIM_SCGC5_TSI;

#if defined(HAS_KINETIS_TSI)
  TSI0_GENCS = 0;
  TSI0_PEN = (1 << ch);
  TSI0_SCANC = TSI_SCANC_REFCHRG(3) | TSI_SCANC_EXTCHRG(CURRENT);
  TSI0_GENCS = TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_PS(PRESCALE) | TSI_GENCS_TSIEN | TSI_GENCS_SWTS;
  delayMicroseconds(10);
  return 0;

#elif defined(HAS_KINETIS_TSI_LITE)
  TSI0_GENCS = TSI_GENCS_REFCHRG(4) | TSI_GENCS_EXTCHRG(3) | TSI_GENCS_PS(PRESCALE)
    | TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_TSIEN | TSI_GENCS_EOSF;
  TSI0_DATA = TSI_DATA_TSICH(ch) | TSI_DATA_SWTS;
  delayMicroseconds(10);
  return 0;
#endif
}

int TeensyTouch::teensyTouchDone() {
  if (TSI0_GENCS & TSI_GENCS_SCNIP) return false;
  else return true;
}

void TeensyTouch::readNonblocking(int*& ptr_touch_values_array, int*& ptr_pin_values_array){
 
 if (teensyTouchDone()) {
  int* touch_array_end = _touch_values_array_first_value + _touch_values_array_size;
  int* pin_array_end = _pins_array_first_value + _pins_array_size;

  *ptr_touch_values_array = teensyTouchReturn();
  ptr_touch_values_array++;

  if (ptr_touch_values_array == touch_array_end) {
    ptr_touch_values_array = _touch_values_array_first_value;
  }
  ptr_pin_values_array++;
  if (ptr_pin_values_array == pin_array_end) {
    ptr_pin_values_array = _pins_array_first_value;
  }
  int current_pin = *ptr_pin_values_array;
  teensyTouchInit(current_pin);
 }
}
