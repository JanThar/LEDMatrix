//#include "colorPalette.h"

//void setmatrixPixel(int x, int y, int color) {
//  matrixpixel[x][y] = color;
//  if (x%2) { // even or odd
//    leds[x*FIELD_WIDTH+y] = color;
//  } else {
//    leds[(x+1)*FIELD_WIDTH-y-1] = color;
//  }
//}

void FillLEDsFromPaletteColors( uint8_t colorIndex) {
  uint8_t brightness = 255;
  for(int x = 0; x < FIELD_WIDTH; x++) {
    for(int y = 0; y < FIELD_HEIGHT; y++) {
      CRGB color = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
      int r = color.r << 16;
      int g = color.g << 8;
      setTablePixel(x,y,r+g+color.b);
      colorIndex += 1;
    }
  }
}

static uint8_t rainbowIndex = 0;

void runRainbowPalette(){
  FillLEDsFromPaletteColors(rainbowIndex);
  rainbowIndex = (rainbowIndex+1)%256;
}

// Sprites

uint16_t PlasmaTime = 0;
uint16_t PlasmaShift = (random8(0, 5) * 32) + 64;
uint8_t plasmaLowHue = 100;
uint8_t plasmaHighHue = 150;
uint8_t plasmaBrightness = 200;

void runPlasma() {
  // Fill background with dim plasma
  #define PLASMA_X_FACTOR  24
  #define PLASMA_Y_FACTOR  24
  for (int16_t x=0; x<FIELD_WIDTH; x++) {
    for (int16_t y=0; y<FIELD_HEIGHT; y++) {
      int16_t r = sin16(PlasmaTime) / 256;
      int16_t h = sin16(x * r * PLASMA_X_FACTOR + PlasmaTime) + cos16(y * (-r) * PLASMA_Y_FACTOR + PlasmaTime) + sin16(y * x * (cos16(-PlasmaTime) / 256) / 2);
      CRGB color = CHSV((uint8_t)((h / 256) + 128), 255, plasmaBrightness);
      
      int rc = color.r << 16;
      int gc = color.g << 8;
      setTablePixel(x,y,rc+gc+color.b);
    }
  }
  uint16_t OldPlasmaTime = PlasmaTime;
  PlasmaTime += PlasmaShift;
  if (OldPlasmaTime > PlasmaTime) PlasmaShift = (random8(0, 5) * 32) + 64;
}
