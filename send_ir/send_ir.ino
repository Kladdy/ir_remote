/*
  Based on
  https://www.arduino.cc/en/Tutorial/InputPullupSerial
  https://www.build-electronic-circuits.com/arduino-ir-transmitter-circuit/
  https://woodsgood.ca/projects/2015/02/13/rgb-led-strip-controllers-ir-codes/
*/
#include <IRremote.hpp>

#define IR_OUT_PIN            3
#define COLOR_BUTTON_PIN      5
#define MODE_BUTTON_PIN       6
#define BRIGHTNESS_BUTTON_PIN 7

#define	IR_BPLUS  0xF700FF	// 
#define	IR_BMINUS 0xF7807F	// 
#define	IR_OFF 	  0xF740BF	// 
#define	IR_ON 	  0xF7C03F	// 
#define	IR_R 	    0xF720DF	// 
#define	IR_G 	    0xF7A05F	// 
#define	IR_B 	    0xF7609F	// 
#define	IR_W 	    0xF7E01F	// 
#define	IR_B1	    0xF710EF	// 
#define	IR_B2	    0xF7906F	// 
#define	IR_B3	    0xF750AF	// 
#define	IR_FLASH  0xF7D02F	// 
#define	IR_B4	    0xF730CF	// 
#define	IR_B5	    0xF7B04F	// 
#define	IR_B6	    0xF7708F	// 
#define	IR_STROBE 0xF7F00F	// 
#define	IR_B7	    0xF708F7	// 
#define	IR_B8	    0xF78877	// 
#define	IR_B9	    0xF748B7	// 
#define	IR_FADE   0xF7C837	// 
#define	IR_B10	  0xF728D7	// 
#define	IR_B11	  0xF7A857	// 
#define	IR_B12	  0xF76897	// 
#define	IR_SMOOTH 0xF7E817	// 

// Color
const int N_COLORS = 16;
const unsigned long colors[] = {
    IR_R,
    IR_G,
    IR_B,
    IR_W,
    IR_B1,
    IR_B2,
    IR_B3,
    IR_B4,
    IR_B5,
    IR_B6,
    IR_B7,
    IR_B8,
    IR_B9,
    IR_B10,
    IR_B11,
    IR_B12
};

// Mode
const int N_MODES = 4;
const unsigned long modes[] = {
  IR_FLASH,
  IR_STROBE,
  IR_FADE,
  IR_SMOOTH
};

// Brightness
const int N_BRIGHTNESS_STEPS = 15;

// States
bool colorButtonIsPressed = false;
int currentColor = N_COLORS - 1;
bool modeButtonIsPressed = false;
int currentMode = N_MODES - 1;
bool brightnessButtonIsPressed = false;
bool brightnessDirectionIsUp = true;
int currentBrightness = 0;

void sendOnSignalAfterDelay() {
  delay(1);
  IrSender.sendNECMSB(IR_ON, 32);
}

void setup() {
  // Start serial connection
  Serial.begin(9600);

  // Configure pins as an input and enable the internal pull-up resistor
  pinMode(COLOR_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BRIGHTNESS_BUTTON_PIN, INPUT_PULLUP);

  // Internal LED
  pinMode(13, OUTPUT);

  // Configure IRremote
  IrSender.begin(IR_OUT_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
}

void loop() {
  // Read the color pushbutton value
  int colorSensorVal = digitalRead(COLOR_BUTTON_PIN);
  int modeSensorVal = digitalRead(MODE_BUTTON_PIN);
  int brightnessSensorVal = digitalRead(BRIGHTNESS_BUTTON_PIN);

  // Keep in mind the pull-up means the pushbutton logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. 

  // Color
  if (colorSensorVal == HIGH) {
    digitalWrite(13, LOW);
    colorButtonIsPressed = false;
  } else if (colorSensorVal == LOW && colorButtonIsPressed == false){
    colorButtonIsPressed = true;
    if (currentColor + 1 >= N_COLORS) {
      currentColor = 0;
    } else {
      currentColor += 1;
    }
    digitalWrite(13, HIGH);
    IrSender.sendNECMSB(colors[currentColor], 32); 
    sendOnSignalAfterDelay();
    Serial.println("Color code sent: " + String(colors[currentColor]) + " (color index " + String(currentColor) + ")");
    delay(5);
  } else {
    Serial.println("Wanted to send color code, but button has not been released yet");
  }

  // Mode
  if (modeSensorVal == HIGH) {
    digitalWrite(13, LOW);
    modeButtonIsPressed = false;
  } else if (modeSensorVal == LOW && modeButtonIsPressed == false){
    modeButtonIsPressed = true;
    if (currentMode + 1 >= N_MODES) {
      currentMode = 0;
    } else {
      currentMode += 1;
    }
    digitalWrite(13, HIGH);
    IrSender.sendNECMSB(modes[currentMode], 32);  
    sendOnSignalAfterDelay();
    Serial.println("Mode code sent: " + String(modes[currentMode]) + " (mode index " + String(currentMode) + ")");
    delay(5);
  } else {
    Serial.println("Wanted to send mode code, but button has not been released yet");
  }

  // Brightness
  if (brightnessSensorVal == HIGH) {
    digitalWrite(13, LOW);
    brightnessButtonIsPressed = false;
  } else if (brightnessSensorVal == LOW && brightnessButtonIsPressed == false){
    brightnessButtonIsPressed = true;

    unsigned long signalToSend = 0;

    if (brightnessDirectionIsUp == true) { // Going up
      currentBrightness += 1;
      signalToSend = IR_BPLUS;
      if (currentBrightness >= N_BRIGHTNESS_STEPS) {
        brightnessDirectionIsUp = false;
      }
    } else { // Going down
      currentBrightness -= 1;
      signalToSend = IR_BMINUS;
      if (currentBrightness <= 0) {
        brightnessDirectionIsUp = true;
      }
    }
    
    digitalWrite(13, HIGH);
    IrSender.sendNECMSB(signalToSend, 32);
    sendOnSignalAfterDelay();
    Serial.println("Brightness code sent: " + String(signalToSend) + " (current brightness index " + String(currentBrightness) + ")");
    delay(5);
  } else {
    Serial.println("Wanted to send brightness code, but button has not been released yet");
  }
}