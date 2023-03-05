# TeensyTouch

TeensyTouch is a non-blocking capacitance touch reading library for Teensy boards. It allows for easy non-blocking touch sensing and is based on the touch.c library by PaulStoffregen: https://github.com/PaulStoffregen/cores/blob/a2368ad57e9470608a234d942c55a2278c6cd72b/teensy3/touch.c

Original idea is from PJRC forum user Lalbert (https://forum.pjrc.com/threads/66587-Non-blocking-(no-wait)-touch-sense-(touchRead)-routine-for-Teensy-3-2)

# Usage

###### [Also see the examples folder]

Include the library in your Arduino sketch:

```c++
#include <TeensyTouch.h>
```

Create an array to store the capacitance touch values and an array with the pin numbers to be used (must be touch capable). Additionally create a pointer for each and a variable with their size (number of items for each).

```c++
// These are the pins in your teensy board that you want to read from and are touch capable, its size and a pointer to it
int touch_pins[2] = {30, 23};
int touch_pins_size = sizeof(touch_pins)/sizeof(touch_pins[0]);
int* ptr_touch_pins = touch_pins;

// This is an array to store the values from the touch sensing, its size and a pointer to it
int touch_values[2] = {0};
int touch_values_size = sizeof(touch_values)/sizeof(touch_values[0]);
int* ptr_touch_values = touch_values;
```

Create an instance of the main class:

```c++
TeensyTouch tt(touch_pins, touch_pins_size, touch_values, touch_values_size);
```

Open a Serial connection if you want to print your values to the console:

```c++
void setup() {
  Serial.begin(9600);
  while(!Serial);

}
```

Now you can update the values of the array non-blockingly with the following method:

```c++
void loop() {
  tt.readNonblocking(ptr_touch_values, ptr_touch_pins);
}
```

To see the changes in the console you can do:

```c++
void loop() {
  tt.readNonblocking(ptr_touch_values, ptr_touch_pins);
  for (int i=0; i<touch_values_size; i++) {
    Serial.print(touch_values[i]);
    Serial.print(" ");
  }
  Serial.println("");
}
```
