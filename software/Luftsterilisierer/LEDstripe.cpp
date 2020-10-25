
/************************************************************************************/
/*! \file 
  <!--------------------------------------------------------------------------------->
  \section desc Module Description
  
  - Project           : Luftsterilisierer
  - Subsystem         : DRV (Driver)
  - Module            : LEDstripe

*/
/************************************************************************************/

/*** Imported external Objects ******************************************************/
#include "Luftsterilisierer.h"

/*** Types **************************************************************************/
#include "LEDstripe.h"
#include <Adafruit_NeoPixel.h>


/*** Imported external Objects ******************************************************/


/*** Variables **********************************************************************/

/*** Defines and Constants **********************************************************/
#if 0
color_st color_array[] ={
  /* COLOR_NONE */
  {   0,   0,   0 },
  /* COLOR_GREEN */
  {   0, 127,   0 },
  /* COLOR_YELLOW */
  {  64,  45,   0 },
  /* COLOR_RED */
  { 127,   0,   0 },
  /* COLOR_WHITE */
  {  64,  64,  64 },
  /* COLOR_BLUE */
  {   0,   0, 255 },
  /* COLOR_RAINBOW */
  {   0,   0,   0 }
};
#else
color_st color_array[] ={
  /* COLOR_NONE */
  {   0,   0,   0 },
  /* COLOR_GREEN */
  {   0, 255,   0 },
  /* COLOR_YELLOW */
  { 255, 160,   0 },
  /* COLOR_RED */
  { 255,   0,   0 },
  /* COLOR_WHITE */
  {  64,  64,  64 },
  /* COLOR_BLUE */
  {   0,   0, 255 },
  /* COLOR_RAINBOW */
  {   0,   0,   0 }
};
#endif


/*** Modal Function Prototypes ******************************************************/
void colorWipe(uint32_t color, int wait);
void theaterChase(uint32_t color, int wait);
void rainbow(int wait);
void theaterChaseRainbow(int wait);


/*** Globals                  ******************************************************/
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)



/************************************************************************************/

void setup_LEDstripe_f() {

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}


/************************************************************************************/


void LEDset_f(color_et color) {
  color_st c ;

  c=color_array[color];


  for(int i=0; i<strip.numPixels(); i++) {       // For each pixel in strip...
    strip.setPixelColor(i, strip.Color( c.r,c.g,c.b)); //  Set pixel's color (in RAM)
    strip.show();                                //  Update strip to match
  }
}

/************************************************************************************/

void LEDwipe_f(int i, color_et color) {
  color_st c ;
  
  c=color_array[color];

  strip.setPixelColor(i, strip.Color( c.r,c.g,c.b)); //  Set pixel's color (in RAM)
  strip.show();                                //  Update strip to match
}

/************************************************************************************/

void LEDerrorset_f(int err_no) {
  color_st c ;
  int i, errleds;

  c=color_array[COLOR_RED];    
  for(i=0; i<2; i++) {       // For each pixel in strip...
    strip.setPixelColor(i, strip.Color( c.r,c.g,c.b)); //  Set pixel's color (in RAM)
    strip.show();                                //  Update strip to match
  }


  c=color_array[COLOR_WHITE];
  errleds = err_no;
  if (errleds > strip.numPixels() - 4 ) errleds = strip.numPixels() - 4;
  for(; i<errleds +2  ; i++) {       // For each pixel in strip...
    strip.setPixelColor(i, strip.Color( c.r,c.g,c.b)); //  Set pixel's color (in RAM)
    strip.show();                                //  Update strip to match
  }

  c=color_array[COLOR_NONE];
  for(; i<strip.numPixels() - 2; i++) {       // For each pixel in strip...
    strip.setPixelColor(i, strip.Color( c.r,c.g,c.b)); //  Set pixel's color (in RAM)
    strip.show();                                //  Update strip to match
  }

  c=color_array[COLOR_RED];    
  for(; i<strip.numPixels(); i++) {       // For each pixel in strip...
    strip.setPixelColor(i, strip.Color( c.r,c.g,c.b)); //  Set pixel's color (in RAM)
    strip.show();                                //  Update strip to match
  }


}

/************************************************************************************/

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void LEDrainbow_f(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

/************************************************************************************/
/*                                                                                  */
/*                                                                                  */
/*      Hilfsroutinen                                                               */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


