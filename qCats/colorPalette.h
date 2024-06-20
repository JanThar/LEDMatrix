/* LedTable
 *
 * Written by: Klaas De Craemer, Ing. David Hrbaty
 * 
 * Basic animation with fading stars
 */

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
  for( int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV( random8(), 255, random8());
  }
}

// black and white stripes,

void SetupBlackAndWhiteStripedPalette() {
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  
  currentPalette = CRGBPalette16(
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black );
}


// This example shows how to set up a static color palette
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
  CRGB::Red,
  CRGB::Gray, // 'white' is too bright compared to red and blue
  CRGB::Blue,
  CRGB::Black,
  
  CRGB::Red,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Black,
  
  CRGB::Red,
  CRGB::Red,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Blue,
  CRGB::Black,
  CRGB::Black
};

DEFINE_GRADIENT_PALETTE( purple_gp ) {
  0,   0, 212, 255,   //blue
255, 179,   0, 255 }; //purple
DEFINE_GRADIENT_PALETTE( outrun_gp ) {
  0, 141,   0, 100,   //purple
127, 255, 192,   0,   //yellow
255,   0,   5, 255 };  //blue
DEFINE_GRADIENT_PALETTE( greenblue_gp ) {
  0,   0, 255,  60,   //green
 64,   0, 236, 255,   //cyan
128,   0,   5, 255,   //blue
192,   0, 236, 255,   //cyan
255,   0, 255,  60 }; //green
DEFINE_GRADIENT_PALETTE( redyellow_gp ) {
  0,   200, 200,  200,   //white
 64,   255, 218,    0,   //yellow
128,   231,   0,    0,   //red
192,   255, 218,    0,   //yellow
255,   200, 200,  200 }; //white
CRGBPalette16 purplePal = purple_gp;
CRGBPalette16 outrunPal = outrun_gp;
CRGBPalette16 greenbluePal = greenblue_gp;
CRGBPalette16 heatPal = redyellow_gp;
