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

static uint16_t image[240][240]; // For benchmarking
ST7789_t3 tft = ST7789_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup() {
  pinMode(SD_CS, INPUT_PULLUP);  // keep SD CS high when not using SD card
  Wire.begin();
  Serial.begin(115200);
  SPI.begin();
  tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
  // Adding print functions to the ST7735_t3 library shows that changing this value does alter
  // the ctar value, but there is change in image print speed. Not sure what this is for.
  //tft.setBitrate(25000000); 
  // Uncommenting useFrameBuffer and setFrameBuffer brings delta measurement down from 50ms 
  // to 2ms per image, but I don't see any visual difference in speed. I'm thinking the async 
  // operation halts the timer clock, giving an incorrect output.
  tft.useFrameBuffer(true); 
  tft.setFrameBuffer(*image);
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
  tft.updateScreenAsync(); // Related to useFrameBuffer and setFrameBuffer
  unsigned long m2 = micros();
  unsigned long delta = m2 - m1;
  // 50ms @ ST7735_SPICLOCK < 30000000
  // 35ms @ ST7735_SPICLOCK >= 30000000
  Serial.print("Draw time: ");
  Serial.println(delta);
}

void loop() {
  fullscreen_test();
}
