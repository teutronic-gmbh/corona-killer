/************************************************************************************/
/*! \file
  <!--------------------------------------------------------------------------------->
  \section desc Module Description

  - Project           : Luftsterilisierer
  - Subsystem         : DRV (Driver)
  - Module            : LEDstripe
*/
/************************************************************************************/

#ifndef _LEDSTRIPE_H /* avoids multiple inclusion */
#define _LEDSTRIPE_H

//#define MAXCOLORS 7
typedef enum
{
  COLOR_NONE,
  COLOR_GREEN,
  COLOR_YELLOW,
  COLOR_RED,
  COLOR_WHITE,
  COLOR_BLUE,
  COLOR_RAINBOW
} color_et;

typedef struct
{
  int r;
  int g;
  int b;
} color_st;



void setup_LEDstripe_f();
void LEDset_f(color_et color); 
void LEDerrorset_f(int err_no);
void LEDwipe_f(int i, color_et color);
void LEDrainbow_f(int wait);

#endif
