#include <TeensyTouch.h>

int touch_pins[2] = {30, 23};
int touch_pins_size = sizeof(touch_pins)/sizeof(touch_pins[0]);
int* ptr_touch_pins = touch_pins;

int touch_values[2] = {0};
int touch_values_size = sizeof(touch_values)/sizeof(touch_values[0]);
int* ptr_touch_values = touch_values;

TeensyTouch tt(touch_pins, touch_pins_size, touch_values, touch_values_size);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
}

void loop() {
  tt.readNonblocking(ptr_touch_values, ptr_touch_pins);
  for (int i=0; i<2; i++) {
    Serial.print(touch_values[i]);
    Serial.print(" ");
  }
  Serial.println("");

}
