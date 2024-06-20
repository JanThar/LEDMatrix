/* LedTable

   Written by: Klaas De Craemer, Ing. David Hrbaty


   Main file with common methods and defines, such as button reading from Bluetooth controller
   or setting pixels on the LED area
*/

// #define FAST_LED_DATA_PIN  14

// #define BUTTON_UP     27
// #define BUTTON_DOWN   33
// #define BUTTON_LEFT   25
// #define BUTTON_RIGHT  26
// //#define BUTTON_CENTER 13
// #define BUTTON_RB     32
// //#define BUTTON_CB     11
// #define BUTTON_LB     13

//#define FAST_LED_DATA_PIN  23
//
//#define BUTTON_UP     5
//#define BUTTON_DOWN   24
//#define BUTTON_LEFT   22
//#define BUTTON_RIGHT  25
//#define BUTTON_RB     6
//#define BUTTON_LB     18

#define FAST_LED_DATA_PIN  D5

#define BUTTON_UP     D1
#define BUTTON_DOWN   D2
#define BUTTON_LEFT   D3
#define BUTTON_RIGHT  D4
#define BUTTON_RB     D10
#define BUTTON_LB     D6

// Audio D0/A0

int buttons[] = {BUTTON_UP,BUTTON_DOWN,BUTTON_LEFT,BUTTON_RIGHT,BUTTON_RB,BUTTON_LB};

//LED field size
#define FIELD_WIDTH       14
#define FIELD_HEIGHT      21
#define MATRIX_TILE_H       1  // number of matrices arranged horizontally
#define MATRIX_TILE_V       1  // number of matrices arranged vertically
#define COLOR_ORDER       GRB
#define BRIGHTNESS   255

#include "FastLED.h"
#include <LEDMatrix.h>


//#define USE_FAST_LED   // FAST_LED as library to control the LED strips

/*
   Some defines used by the FAST_LED library
*/
#define FAST_LED_CHIPSET WS2812

#define  NUM_PIXELS    FIELD_WIDTH*FIELD_HEIGHT

/* *** LED color table *** */
#define  GREEN  0x00FF00
#define  RED    0xFF00FF
#define  BLUE   0x0000FF
#define  YELLOW 0xFFFF00
#define  LBLUE  0x00FFFF
#define  PURPLE 0xFF00FF
#define  WHITE  0XFFFFFF
unsigned int colorLib[6] = {YELLOW, BLUE, RED, GREEN, LBLUE, PURPLE};
/* *** Game commonly used defines ** */
#define  DIR_UP    1
#define  DIR_DOWN  2
#define  DIR_LEFT  3
#define  DIR_RIGHT 4

/* *** Bluetooth controller button defines and input method *** */
#define  BTN_NONE   0
#define  BTN_UP     1
#define  BTN_DOWN   2
#define  BTN_LEFT   3
#define  BTN_RIGHT  4
#define  BTN_CENTER 5
#define  BTN_CB     6
#define  BTN_RB     7
#define  BTN_LB     8

boolean buttonTimeout = false;
long buttonTimeoutTimer = millis();
#define BUTTONTIME 200

uint8_t curControl = BTN_NONE;

void readInput() {
  curControl = BTN_NONE;

  if (!buttonTimeout) {
    if (!digitalRead(BUTTON_UP)) curControl = BTN_UP;
    if (!digitalRead(BUTTON_DOWN)) curControl = BTN_DOWN;
    if (!digitalRead(BUTTON_LEFT)) curControl = BTN_LEFT;
    if (!digitalRead(BUTTON_RIGHT)) curControl = BTN_RIGHT;

    //if (!digitalRead(BUTTON_RB)) curControl = BTN_RB;
    //if (!digitalRead(BUTTON_LB)) curControl = BTN_LB;
    //if (!digitalRead(BUTTON_CENTER)) curControl = BTN_CENTER;
    //if (!digitalRead(BUTTON_CB)) curControl = BTN_CB;

    if(curControl != BTN_NONE) {
      buttonTimeout = true;
      buttonTimeoutTimer = millis();
    }
  } 
  if (millis() > buttonTimeoutTimer + BUTTONTIME) buttonTimeout = false; 
}

void initInput() {
  for(int i=0; i<8; i++) {
    pinMode(buttons[i],INPUT_PULLUP);
  }
}

/*
   FAST_LED implementation
*/


int leds[FIELD_WIDTH][FIELD_HEIGHT];
cLEDMatrix < FIELD_WIDTH / MATRIX_TILE_H, FIELD_HEIGHT / MATRIX_TILE_V, VERTICAL_ZIGZAG_MATRIX, MATRIX_TILE_H, MATRIX_TILE_V, HORIZONTAL_BLOCKS > leds1;


void initPixels() {
  FastLED.addLeds<FAST_LED_CHIPSET, FAST_LED_DATA_PIN,  COLOR_ORDER>(leds1[0], 0, leds1.Size()).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void setDelay(int duration) {
  FastLED.delay(duration);
}

void showPixels() {
  FastLED.show();
}

void setTablePixel(int x, int y, int color) {
  leds[x][y] = color;
  //leds1.DrawPixel((FIELD_WIDTH - 1) - x, y, CRGB(color));
  leds1.DrawPixel(x, (FIELD_HEIGHT-1) - y, CRGB(color));
}


void setTablePixelRGB(int x, int y, int r, int g, int b) {
  r = r << 16;
  g = g << 8;
  int color = r + g + b;
  setTablePixel(x, y, color);
}

void clearTablePixels() {
  for (int x = 0; x < FIELD_WIDTH; x++)
    for (int y = 0; y < FIELD_HEIGHT; y++)
      setTablePixel(x, y, 0);
}

/* *** text helper methods *** */
#include "font.h"
uint8_t charBuffer[8][8];

void printText(char* text, unsigned int textLength, int xoffset, int yoffset, int color, int font) {
  uint8_t curLetterWidth = 0;
  int curX = xoffset;
  //clearTablePixels();
  uint8_t charWidth, charHeight;
  switch (font) {
    case FONT_8X8:
      charWidth = 8;
      charHeight = 8;
      break;
    case FONT_6X8:
      charWidth = 6;
      charHeight = 8;
      break;
  }

  //Loop over all the letters in the string
  for (int i = 0; i < textLength; i++) {
    //Determine width of current letter and load its pixels in a buffer
    loadCharInBuffer(text[i], font);
    //Loop until width of letter is reached
    for (int lx = 0; lx < charWidth; lx++) {
      //Now copy column per column to field (as long as within the field
      if (curX >= FIELD_WIDTH) { //If we are to far to the right, stop loop entirely
        break;
      } else if (curX >= 0) { //Draw pixels as soon as we are "inside" the drawing area
        for (int ly = 0; ly < charHeight; ly++) { //Finally copy column
          setTablePixel(curX, yoffset + ly, charBuffer[lx][ly]*color);
        }
      }
      curX++;
    }
  }
  //showPixels();
}

//Load char in buffer and return width in pixels
void loadCharInBuffer(char letter, int font) {
  int letterIdx;
  int x = 0; int y = 0;

  switch (font) {
    case FONT_8X8:
      letterIdx = (letter - 32) * 8;
      for (int idx = letterIdx; idx < letterIdx + 8; idx++) {
        for (int x = 0; x < 8; x++) {
          charBuffer[x][y] = ((font_8x8[idx]) & (1 << (7 - x))) > 0;
        }
        y++;
      }
      break;
    case FONT_6X8:
      letterIdx = (letter - 32);
      for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 8; y++) {
          charBuffer[x][y] = ((font_6x8[letterIdx][x]) & (1 << (7 - y))) > 0;
        }
      }
      for (int x = 5; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
          charBuffer[x][y] = 0;
        }
      }
      break;
    case FONT_3X5:
      for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 8; y++) {
          charBuffer[x][y] = ((font_6x8[letter][x]) & (1 << (7 - y))) > 0;
        }
      }
      for (int x = 3; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
          charBuffer[x][y] = 0;
        }
      }
      break;
  }
}


/* *********************************** */

void fadeOut() {
  //Select random fadeout animation
  int selection = random(3);

  switch (selection) {
    case 0:
    case 1: {
        //Fade out by dimming all pixels
        for (int i = 0; i < 100; i++) {
          dimLeds(0.97);
          showPixels();
          delay(5);
        }
        break;
      }
    case 2: {
        //Fade out by swiping from left to right with ruler
        const int ColumnDelay = 10;
        int curColumn = 0;
        for (int i = 0; i < FIELD_WIDTH * ColumnDelay; i++) {
          dimLeds(0.97);
          if (i % ColumnDelay == 0) {
            //Draw vertical line
            for (int y = 0; y < FIELD_HEIGHT; y++) {
              setTablePixel(curColumn, y, GREEN);
            }
            curColumn++;
          }
          showPixels();
          delay(2);
        }
        //Sweep complete, keep dimming leds for short time
        for (int i = 0; i < 100; i++) {
          dimLeds(0.9);
          showPixels();
          delay(2);
        }
        break;
      }
  }
}

void dimLeds(float factor) {
  //Reduce brightness of all LEDs, typical factor is 0.97
  for (int x = 0; x < FIELD_WIDTH; x++) {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
      //Derive the tree colors
      int r = ((leds[x][y] & 0xFF0000) >> 16);
      int g = ((leds[x][y] & 0x00FF00) >> 8);
      int b = ((leds[x][y] & 0x0000FF));
      //Reduce brightness
      r = r * factor;
      g = g * factor;
      b = b * factor;
      //Set led again
      setTablePixelRGB(x, y, r, g, b);
    }
  }
}

#include "colorPalette.h"
#include "Sprites.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  //Initialise display
  initPixels();
  clearTablePixels();
  showPixels();
  Serial.println("Init Display");
  
  initInput();
  Serial.println("Init Input");
  delay(100);
  //Init random number generator
  randomSeed(millis());
  Serial.println("Start sequence done"); 
}

void loop() {
  runPlasma();
  readInput(); 
  FastLED.show();
  delay(50);
  if (curControl != BTN_NONE) {
    runTetris();
  }
}
