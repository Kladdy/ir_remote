/*
  Based on
  https://www.arduino.cc/en/Tutorial/InputPullupSerial
  https://www.build-electronic-circuits.com/arduino-ir-transmitter-circuit/
  https://woodsgood.ca/projects/2015/02/13/rgb-led-strip-controllers-ir-codes/
*/
#include <IRremote.h>

void setup() {
  // Start serial connection
  Serial.begin(9600);

  // Configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(2, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  // Configure IRremote
  IrSender.begin(3, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
}

void loop() {
  // Read the pushbutton value into a variable
  int sensorVal = digitalRead(2);

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:

  if (sensorVal == HIGH) {
    digitalWrite(13, LOW);
  } else {
    digitalWrite(13, HIGH);
    IrSender.sendNEC(0xF720DF, 32);  // Replace with your own unique code
    Serial.println("Code sent!");
    delay(30);
  }
}