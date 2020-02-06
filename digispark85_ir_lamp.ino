/*************************************
* This code is written by Andrii Valchuk
* GitHub: https://github.com/AndriiVal
* 
* 
* Microcontroller: ATtiny85
* Dev board:       Digispark
* LEDs:            WS2812
* 
*************************************/

// Connect the library Adafruit NeoPixel.
#include "Adafruit_NeoPixel.h"

// ************************** SETTINGS ***********************
#define AUTOPLAY_TIME 30    // time between mode changes in seconds
#define NUM_LEDS 8         // the number of LEDs in one piece of strip
#define LED_PIN 4           // pin strip
#define MIN_BRIGHTNESS 10  // minimum brightness with manual adjustment
#define MAX_BRIGHTNESS 250  // maximum brightness with manual adjustment
#define BRIGHTNESS 180      // initial brightness
#define MODES_AMOUNT 6    // number of modes

// Define Remote control keys
// You can get the keycodes of the remote here: https://www.instructables.com/id/DIY-USB-IR-receiver/
#define PowerOnOff 56  // 'OK'
#define WhiteOnOff 104  // '*'
#define NextMode 24  // '^'
#define BeforeMode 74  // 'v'
#define ModeAuto 176  // '#'
#define BrightUp 90  // '>'
#define BrightDown 16  // '<'

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

class Lamp_timer
{
  public:
  Lamp_timer(){};  // timer announcement (MILLISECOND)
  Lamp_timer(uint32_t interval){
    _interval = interval;  // interval timer announcement
    _timer = millis();
  }
  void setInterval(uint32_t interval){
    _interval = interval;  // setting the timer interval
    reset();
  }
  void setMode(boolean mode){
    _mode = mode;  // setting the type of work: AUTO or MANUAL (MANUAL must be manually reset)
  }
  boolean isReady(){
    if (!_state) return false;  // returns true when the time is right. It is reset to false by itself (AUTO) or manually (MANUAL)
    if ((long)millis()-_timer >= _interval){
      if (_mode) _timer = millis();
      return true;
    } else {
      return false;
    }
  }
  void reset(){
    _timer - millis();  // manual reset of the timer to the set interval
  }
  void stop(){
    _state = false;  // stop timer
  }
  void start(){
    _state = true;  // proceed
  }
  private:
  uint32_t _timer = 0;
  uint32_t _interval = 0;
  boolean _mode = true;
  boolean _state = true;
};

#define MANUAL 0
#define AUTO 1

Lamp_timer effectTimer(60);
Lamp_timer autoplayTimer((long)AUTOPLAY_TIME * 1000);
Lamp_timer brightTimer(40);
Lamp_timer whiteTimer(80); 

int brightness = BRIGHTNESS;
int tempBrightness;
byte thisMode;
byte StepBrightCount = 10;  // brightness step

boolean loadingFlag = true;
boolean autoplay = true;
boolean powerDirection = true;
boolean powerActive = false;
boolean powerState = true;
boolean whiteMode = false;
boolean whiteModeColor = false;  // true white by default, in white mode

uint8_t situation = 0;
uint8_t START = 0;
uint8_t x = 0;
uint8_t BIT = 0;
uint8_t Id = 0;
uint8_t Id_inv = 0;
uint8_t Data = 0;
uint8_t Data_back = 0;
uint8_t Data_inv = 0;
uint8_t Repeat = 0;
uint8_t sended = 0;

uint16_t Time_old = 0;
uint16_t Time = 0;
uint16_t TimeDelta = 0;
 
// fill everything
void fillAll(int newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, newcolor));
  }
}

void setup()
{
  attachInterrupt(0, IR_Read, FALLING);
  strip.setBrightness(brightness);
  strip.begin();
  strip.show(); 
  randomSeed(analogRead(0));
}

void loop() {
  if (sended == 1) {
    switch (Data_back) {
      case PowerOnOff:
        powerDirection = !powerDirection;
        powerActive = true;
        tempBrightness = brightness * !powerDirection;
        break;
      case NextMode:
        if (powerState){
          nextMode();
        }
        break;
      case BeforeMode:
        if (powerState){
          beforeMode();
        }
        break;
      case WhiteOnOff:
        if (powerState) {
          AnimSt(0, 204, 255);  //blue
          whiteMode = !whiteMode;
          if (whiteMode) {
            effectTimer.stop();
            if (autoplay) autoplayTimer.stop();
            if (whiteModeColor) fillAll(240);   //white
            else fillAll(70);   //warm
            strip.setBrightness(brightness);
            strip.show();
          } else {
            effectTimer.start();
            if (autoplay) autoplayTimer.start();
          }
        }
        break;
      case ModeAuto:
        if (!whiteMode && powerState){
          autoplay = !autoplay;
          if (autoplay) AnimSt(255, 0, 0);  //red
          else AnimSt(0, 255, 0);  //green
        }
        break;
      case BrightUp:
        if (powerState){
          if (brightness + StepBrightCount <= MAX_BRIGHTNESS) brightness += StepBrightCount;
          else {
            brightness = MAX_BRIGHTNESS;
          }
          strip.setBrightness(brightness);
          strip.show();
        }
        break;
      case BrightDown:
        if (powerState){
          if (brightness - StepBrightCount >= MIN_BRIGHTNESS) brightness -= StepBrightCount;
          else {
            brightness = MIN_BRIGHTNESS;
          }
          strip.setBrightness(brightness);
          strip.show();
        }
        break;
      default:
        break;
      }
    sended = 0;
  }
  if (effectTimer.isReady() && powerState) {
    switch (thisMode) {
      case 0:
        spark();
        break;
      case 1: fireflies();
        break;
      case 2: colors();
        break;
      case 3: rainbow();
        break;
      case 4:
        sparkles();
        break;
      case 5: fire();
        break;
    }
    strip.show();
  }
  lightTick();
}

void nextMode() {
  if (whiteMode) {
    strip.clear();
    whiteModeColor = !whiteModeColor;
    if (whiteModeColor) fillAll(240);   //white
    else fillAll(70);   //warm
    strip.show();
  } else {
      if (thisMode+1 >= MODES_AMOUNT) thisMode = 0;
      else thisMode++;
      loadingFlag = true;
      strip.clear();
  }
}

void beforeMode() {
  if (whiteMode) {
    strip.clear();
    whiteModeColor = !whiteModeColor;
    if (whiteModeColor) fillAll(240);   //white
    else fillAll(70);   //warm
    strip.show();
  } else {
      if (thisMode-1 <= -1) thisMode = MODES_AMOUNT-1;
      else thisMode--;
      loadingFlag = true;
      strip.clear();
  }
}

void lightTick() {
  if (whiteTimer.isReady() && powerState) {
    strip.setBrightness(brightness);
    if (whiteMode){
      strip.clear();
      if (whiteModeColor) fillAll(240);   //white
      else fillAll(70);   //warm
      strip.show();
    }
  }
  if (autoplayTimer.isReady() && autoplay && powerState) {
    nextMode();
  }
  if (powerActive) {
    if (brightTimer.isReady()) {
      if (powerDirection) {
        powerState = true;
        tempBrightness += 10;
        if (tempBrightness > brightness) {
          tempBrightness = brightness;
          powerActive = false;
        }
        strip.setBrightness(tempBrightness);
        strip.show();
      } else {
        tempBrightness -= 10;
        if (tempBrightness < 0) {
          tempBrightness = 0;
          powerActive = false;
          powerState = false;
        }
        strip.setBrightness(tempBrightness);
        strip.show();
      }
    }
  }
}

void IR_Read(void) {   //This part of code from here https://www.instructables.com/id/DIY-USB-IR-receiver/
  Time = micros();
  if (Time_old != 0) {
    TimeDelta = Time - Time_old;
    if ((TimeDelta > 12000)&&(TimeDelta < 14000)) {
      START = 1;
      x = 0;
      situation = 1;
      Id = 0;
      Id_inv = 0;
      Data = 0;
      Data_inv = 0;
    } else if ((TimeDelta > 10000)&&(TimeDelta < 12000)) {
      situation = 2; // repeat
    } else if ((TimeDelta > 1500)&&(TimeDelta < 2500)) {
      situation = 3; //"1"
      BIT = 1;
    } else if ((TimeDelta > 1000)&&(TimeDelta < 1500)) {
      situation = 3; //"0"
      BIT = 0;
    } else situation = 5; 
    if (situation == 3) {
      if (x < 8) {
        Id |= BIT;
        if (x < 7) Id <<= 1;  
        x++;
      } else if (x < 16) {
        Id_inv |= BIT;
        if (x < 15) Id_inv <<= 1;
        x++;
      } else if (x < 24) {
        Data |= BIT;
        if (x < 23) Data <<= 1;
        x++;
      } else if (x < 32) {
        Data_inv |= BIT;
        if (x < 31) {
          Data_inv <<= 1;
        } else {
          sended = 1;
          Data_back = Data;
          Repeat = 0;
        }
        x++;
      }
    } else if (situation == 2) {
      if(Repeat == 1) {
      } else {
        Repeat = 1;
      }
    }
  }
  Time_old = Time;
}

