#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN 4
#define BUTTON_PIN 0
#define STRIP_PIN 2

#define LONG_HOLD_TIME 1000

Adafruit_NeoPixel strip = Adafruit_NeoPixel(18, STRIP_PIN, NEO_GRB + NEO_KHZ800);
byte currentState;
byte LED_val;

unsigned long buttonPressTime;
int lastButtonState;

void setup()
{
  randomSeed(analogRead(3)); 
  pinMode(BUTTON_PIN, INPUT);
  strip.begin();
  strip.show();

  currentState = 1;
  fadeInButton();
  colorWipe(strip.Color(127, 127, 127), 100); // White
}

void loop()
{

  int buttonState = digitalRead(BUTTON_PIN);

  // Button Started Press
  if ((buttonState == HIGH) && (buttonState != lastButtonState)){
    buttonPressTime = millis();
  // Button Ended Press
  }else if ((buttonState == LOW) && (buttonState != lastButtonState)){

    unsigned long timeDiff = millis() - buttonPressTime;
    buttonPressTime = 0;

    if (currentState == 0){
        // Wake from sleep
        currentState = 1;
        fadeInButton();
        colorWipe(strip.Color(127, 127, 127), 50); // White
    }else if (timeDiff > LONG_HOLD_TIME){
      // Long hold - Go to sleep
      currentState = 0;
      //fadeOutButton();
      colorWipe(strip.Color(10, 10, 10), 100); // Black
    }else{
      //Regular press - Do LED Animation
      fadeOutButton();
      playRandomAnimation();
      fadeInButton();
    }
    
  }

  lastButtonState = buttonState;
  delay(2);

  if (currentState == 0){
    sleepMode();
  }
  
}

void sleepMode(){

  LED_val = (exp(sin(millis()/3000.0*PI)) - 0.36787944)*108.0;
  analogWrite(LED_PIN, LED_val);
}

void fadeInButton(){

  for (int x = LED_val; x < 255; x++){
    LED_val++;
    analogWrite(LED_PIN, LED_val);
    delay(4);
  }
  
}

void fadeOutButton(){
  
  for (int x = LED_val; x > 0; x--){
    LED_val--;
    analogWrite(LED_PIN, LED_val);
    delay(4);
  }
}

void playRandomAnimation(){

  byte index = random(6);

  switch (index) {
  case 0:
    theaterChase(strip.Color(0, 127, 0), 100); // Green
    break;
  case 1:
    theaterChase(strip.Color(127, 0, 0), 100); // Red
    break;
  case 2:
    theaterChase(strip.Color(0, 0, 127), 100); // Blue
    break;
  case 3:
    rainbow(50);
    break;
  case 4:
    rainbowCycle(50);
    break;
  case 5:
    theaterChaseRainbow(100);
    break;
  default:
    break;
}
  
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

