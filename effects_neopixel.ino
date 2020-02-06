
// ****************************** SPARK ******************************
int16_t position;
boolean direction;

void spark() {
  strip.clear();
  if (direction) {
    position++;
    if (position > NUM_LEDS - 2) {
      direction = false;
    }
  } else {
    position--;
    if (position < 1) {
      direction = true;
    }
  }
  strip.setPixelColor(position, Wheel(uint16_t(random(0, 9)*28) & 255));
}

// ****************************** FIREFLIES ******************************
#define MAX_SPEED 15
#define BUGS_AMOUNT 3
int8_t speed[BUGS_AMOUNT];
int16_t pos[BUGS_AMOUNT];
int bugColors[BUGS_AMOUNT];

void fireflies() {
  if (loadingFlag) {
    loadingFlag = false;
    for (int i = 0; i < BUGS_AMOUNT; i++) {
      pos[i] = random(0, NUM_LEDS);
      speed[i] += random(-5, 6);
    }
  }
  strip.clear();
  for (int i = 0; i < BUGS_AMOUNT; i++) {
    speed[i] += random(-5, 6);
    if (speed[i] == 0) speed[i] += (-5, 6);

    if (abs(speed[i]) > MAX_SPEED) speed[i] = 0;
    pos[i] += speed[i] / 10;
    if (pos[i] < 0) {
      pos[i] = 0;
      speed[i] = -speed[i];
    }
    if (pos[i] > NUM_LEDS - 1) {
      pos[i] = NUM_LEDS - 1;
      speed[i] = -speed[i];
    }
    strip.setPixelColor(pos[i],Wheel(uint16_t(random(0, 9)*28) & 255));
  }
}

// ****************************** COLORS ******************************
void colors() {
    static uint16_t j=0;
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    j++;
    if(j >= 256) j=0;
}

// ****************************** RAINBOW ******************************
void rainbow() {
  static uint16_t j=0;
    for(int i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    j++;
    if(j >= 256*5) j=0;
}

// ****************************** SPARKLES ******************************
void sparkles() {
  static uint8_t j=0, q = 0;
     static boolean on = true;
     if(on){
            for (int i=0; i < strip.numPixels(); i=i+3) {
                strip.setPixelColor(i+q, Wheel(((i * 256 / strip.numPixels()) + j) & 255));    //turn every third pixel on
            }
     }
     else {
            for (int i=0; i < strip.numPixels(); i=i+3) {
               strip.setPixelColor(i+q, 0);
            }
     }
     on = !on;
     q++;
     if(q >=3 ){
        q=0;
        j++;
        if(j >= 256) j = 0;
     }
}

// ****************************** FIRE ******************************
void fire() {
  for(int i=0; i<strip.numPixels(); i++)
    {
      strip.setPixelColor(i, strip.Color(random(150, 254), random(0, 45), 0));
    }
  strip.setBrightness(random(151,254));
}

// ****************** SERVICE FUNCTIONS *******************
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

// Animation of the status of the inclusion of auto mode
void AnimSt(uint16_t c1, uint16_t c2, uint16_t c3) {
  strip.clear();
  strip.setBrightness(brightness);
  delayMicroseconds(100000);
  int i = NUM_LEDS / 2 - 1;
  int j = i + 1;
  for (i; i>-1; i--, j++){
    strip.setPixelColor(i, strip.Color(c1, c2, c3));
    strip.setPixelColor(j, strip.Color(c1, c2, c3));
    strip.show();
    delayMicroseconds(100000);
  }
  delayMicroseconds(300000);
  i = NUM_LEDS / 2 - 1;
  j = i + 1;
  for (i; i>-1; i--, j++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.setPixelColor(j, strip.Color(0, 0, 0));
    strip.show();
    delayMicroseconds(50000);
  }
  delayMicroseconds(200000);
}

