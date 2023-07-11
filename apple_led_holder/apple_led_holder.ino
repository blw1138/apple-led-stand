#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define LED_PIN 4
#define BUTTON_PIN 0
#define STRIP_PIN 2

#define STATE_SLEEP 0
#define STATE_ANIM_1 1
#define STATE_ANIM_2 2

#define LONG_HOLD_TIME 700
#define SLEEP_BRIGHTNESS 30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(18, STRIP_PIN, NEO_GRB + NEO_KHZ800);
volatile byte currentState;
volatile byte lastState;
byte LED_val;

volatile unsigned long buttonPressTime;
volatile int lastButtonState;

volatile bool stopCurrentAnim;

void setup()
{
//  randomSeed(analogRead(3));
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(BUTTON_PIN, button_change, CHANGE);
  strip.begin();

  currentState = STATE_ANIM_1;
  stopCurrentAnim = false;
  fadeInButton();
  fadeInRainbowCycle();
  interrupts();
}

void loop()
{

  if (currentState == STATE_SLEEP) {
    //Start Sleep
    if (stopCurrentAnim == true) {
      stopCurrentAnim = false;
      fadeOutButton();
    }
    sleepMode();
  } else if (currentState == STATE_ANIM_2) {
    //Start Anim 1
    analogWrite(LED_PIN, 255);
    if (stopCurrentAnim == true) {
      stopCurrentAnim = false;
      if (lastState == STATE_SLEEP) {
        fadeInButton();
      }
      fadeInRainbow();
    }
    rainbow(150);
  } else {
    //Start Anim 1
    if (stopCurrentAnim == true) {
      stopCurrentAnim = false;
      if (lastState == STATE_SLEEP) {
        fadeInButton();
      }
      fadeInRainbowCycle();
    }
    analogWrite(LED_PIN, 255);
    rainbowCycle(30);
  }

}

void button_change()
{
  int buttonState = digitalRead(BUTTON_PIN);

  // Button Started Press
  if ((buttonState == HIGH) && (buttonState != lastButtonState)) {
    buttonPressTime = millis();
    // Button Ended Press
  } else if ((buttonState == LOW) && (buttonState != lastButtonState)) {

    unsigned long timeDiff = millis() - buttonPressTime;
    buttonPressTime = 0;
    lastState = currentState;

    if (currentState == STATE_SLEEP) {
      // Wake from sleep
      currentState = STATE_ANIM_1;
    } else if (timeDiff > LONG_HOLD_TIME) {
      // Long hold - Go to sleep
      currentState = STATE_SLEEP;
    } else {
      //Regular press - Do LED Animation
      if (currentState == STATE_ANIM_1) {
        currentState = STATE_ANIM_2; //anim 2
      } else {
        currentState = STATE_ANIM_1; //anim 1
      }
    }
    stopCurrentAnim = true;
  }

  lastButtonState = buttonState;
}

void fadeInRainbowCycle() {
  noInterrupts();
  for (byte i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels())) & 255));
    strip.show();
    delay(100);
  }
  interrupts();
}

void fadeInRainbow() {
  noInterrupts();
  for (byte i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i) & 255));
    strip.show();
    delay(100);
  }
  interrupts();
}

void sleepMode() {

    LED_val = (exp(sin(millis() / 4000.0 * PI)) - 0.36787944) * 108.0;
    analogWrite(LED_PIN, map(LED_val, 0, 255, 1, 255));
    int sleep_val = map(LED_val, 0, 255, 12, SLEEP_BRIGHTNESS);

    if (currentState != STATE_SLEEP) {
      return;
    }

    colorWipe(strip.Color(sleep_val, sleep_val, sleep_val), 10);

}

void fadeInButton() {
  noInterrupts();
  for (int x = LED_val; x < 255; x++) {
    LED_val++;
    analogWrite(LED_PIN, LED_val);
    delay(4);
  }
  interrupts();
}

void fadeOutButton() {
  noInterrupts();
  for (int x = LED_val; x > 0; x--) {
    LED_val--;
    analogWrite(LED_PIN, LED_val);
    delay(4);
  }
  interrupts();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }

    if (stopCurrentAnim == true) {
      return;
    }

    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }

    if (stopCurrentAnim == true) {
      return;
    }

    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
