
/************************************************************************************/
/*! \file 
  <!--------------------------------------------------------------------------------->
  \section desc Module Description
  
  - Project           : Luftsterilisierer
  - Subsystem         : ---
  - Module            : Luftsterilisierer

*/
/************************************************************************************/

#ifndef _LUFTSTERILISIERER_H /* avoids multiple inclusion */
#define _LUFTSTERILISIERER_H

#include "Arduino.h"


//*********************
//*** Pinbelegungen ***
//*********************
// Relais zum Einschalten von UV-Lampe und Lüfter
#define RELAIS_PIN 8
// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    6

#define RELAIS_ON  HIGH
#define RELAIS_OFF LOW

//*********************
//*** other Defines ***
//*********************

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 10

#define STATEDELAY 60

#define EVENT_ST_DEFAULT {"", ERROR_NONE, false,0}


typedef enum
{
  ERROR_NONE,
  ERROR_BADSENSOR,
  ERROR_CASEOPEN,
  ERROR_UVFAIL,
  ERROR_UNKOWN = 6
} error_et;

typedef enum
{
  STATE_CALIBRIER,
  STATE_IDLE,
  STATE_CHECK,
  STATE_GREEN,
  STATE_YELLOW,
  STATE_RED,
  STATE_ALARM
} state_et;

typedef struct
{
  String tio_cmd;
//  String disp_info;
  error_et error;
  bool  tick;
  bool  tickLED;
  int pins;
} event_st;



#endif
