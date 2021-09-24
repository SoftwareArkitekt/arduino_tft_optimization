#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <ST7789_t3.h>    // Hardware-specific library

#define TFT_SCLK   14  // 13 or 14
#define TFT_MOSI   11  // 7 or 11
#define TFT_CS     15  // 2, 6, 9, 10, 15, 20, 21, 22, 23
#define TFT_DC     9   // 2, 6, 9, 10, 15, 20, 21, 22, 23 | but not 2+10, 6+9, 20+23, 21+22
#define TFT_RST    26  // Any pin. you can also connect this to the Arduino reset in which case, set this #define pin to -1!
#define SD_CS      25  // Any pin.

SPISettings settingsA(25000000, MSBFIRST, SPI_MODE1);
volatile int segment_number = 1;
static uint16_t segment[45][240]; // 4 segments make up one camera image, 240x180
static uint16_t image[240][240]; // For benchmarking
ST7789_t3 tft = ST7789_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup() {
  pinMode(SD_CS, INPUT_PULLUP);  // keep SD CS high when not using SD card
  Wire.begin();
  Serial.begin(115200);
  SPI.begin();
  tft.useFrameBuffer(true);
  tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
  tft.fillScreen(ST77XX_BLACK);
}

void fullscreen_test() {
  for (int y=0; y<240; y++) {
    for (int x=0; x<240; x++) {
      image[y][x] = random(65535);
    }
  }
  unsigned long m1 = micros();
  tft.writeRect(0,0,240,240,*image);
  unsigned long m2 = micros();
  unsigned long delta = m2 - m1;
  // Averages to 50ms.
  Serial.print("Delta: ");
  Serial.println(delta);
}

// Mimics the SPI camera I am using.
void segment_test() {
  // Generate some noise
  for (int y=0; y<45; y++) {
    for (int x=0; x<240; x++) {
      segment[y][x] = random(65535);
    }
  }
  unsigned long m1 = micros();
  int16_t x = 0;
  int16_t y = 30+45*(segment_number-1);
  int16_t w = 240;
  int16_t h = 45;
  // Display the noise on the screen
  tft.writeRect(x,y,w,h,*segment);
  // Cycle through each quarter of the screen
  if (segment_number < 4) {
    segment_number++;
  } else {
    segment_number = 1;
  }
  // Calculate how long it took to display this 240x45px segment
  // 4 segments = 1 full 240x180 camera image. 
  // delta averages to 9389 microseconds for an individual 1/4 image. (9.39ms)
  unsigned long m2 = micros();
  unsigned long delta = m2 - m1;
  Serial.print("Delta: ");
  Serial.println(delta);
}

void loop() {
  fullscreen_test();
  //segment_test();
}
