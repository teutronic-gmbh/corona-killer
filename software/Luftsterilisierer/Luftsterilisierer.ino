
/************************************************************************************/
/*! \file
  <!--------------------------------------------------------------------------------->
  \section desc Module Description

  - Project           : Luftsterilisierer
  - Subsystem         : Main
  - Module            :

*/
/************************************************************************************/

/*** Imported external Objects ******************************************************/
#include "Arduino.h"
#include <Adafruit_SGP30.h>
#include <Adafruit_NeoPixel.h>
#include "drv_sgp30.h"
#include "LEDstripe.h"


#include "Luftsterilisierer.h"

/*** Types **************************************************************************/


/*** Variables **********************************************************************/


/*** Defines and Constants **********************************************************/
#define TIO_DEBUG   1
#include "drv_tio.h"

// zwischen THRESHOLD_ON und THRESHOLD_OFF muss ein Hysterese-Abstand sein, damit es nicht zu einem ständigen Ein-Aus-Schalten kommt
#define THRESHOLD_ON 520
#define THRESHOLD_OFF 490   
#define THRESHOLD_YELLOW 750
#define THRESHOLD_RED 1000
#define THRESHOLD_ALARM 2500

//#define IAMALIVE  600 // 10*60 Sekunden = 10 Minuten
#define IAMALIVE 7200 // 2*60*60 Sekunden = 2 Stunden

/*** Modul Function Prototypes ******************************************************/

//state machine
void state_idle_f(event_st event);
void state_check_f(event_st event);


//other
int  check_portpins(void);
state_et eCO2check_f(event_st *event);

/*** Globals  ***********************************************************************/

state_et state;

/************************************************************************************/
/*                                                                                  */
/*                                                                                  */
/*      Arduino Hauptroutinen setup() und loop()                                    */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


void setup()
{
	tio_init_f();
	setup_sgp30_f();
  setup_LEDstripe_f();
  pinMode(RELAIS_PIN, OUTPUT);
  digitalWrite(RELAIS_PIN, RELAIS_OFF);
  LEDset_f(COLOR_NONE);
}

/************************************************************************************/

void loop()
{
	  static event_st event = EVENT_ST_DEFAULT;
	  const  long interval = 1000; /* 1 secound */
	  static long intervalLED = 300; /* 0,3 secounds */
	  static unsigned long int currentMillis;
	  static unsigned long int preMillis = 1000;
	  static unsigned long int preMillisLED = 1000;
	  int lastpinstate = 0;


	  // Events ermitteln
	  event.tio_cmd = tio_input_f();

	  currentMillis = millis();

	  if (currentMillis >= (preMillis + interval) )
	  {
	    event.tick = true;
	    preMillis = currentMillis;
	  } else {
	    event.tick = false;
	  }
	  if (currentMillis >= (preMillisLED + intervalLED) )
	  {
	    event.tickLED = true;
	    preMillisLED = currentMillis;
	  } else {
	    event.tickLED = false;
	  }

	  event.pins = check_portpins();


	  if (event.tio_cmd != "")
	  {
	    tio_cmdinterpreter_f(& event);

	  }



	  // Events abarbeiten
	  if ( (event.tio_cmd != "")        ||
	       event.error != ERROR_NONE    ||
         event.tickLED                ||
	       event.tick                   ||
	       (event.pins != lastpinstate)  )
	  {
	    // mindestens ein Event vorhanden
	    //    tio_printf_f("event.tio_cmd=%s , event.error=%d event.tick=%d  event.pins=%d\r\n", event.tio_cmd.c_str(), event.error, event.tick , event.pins);
	    lastpinstate = event.pins;

	    switch (state)
	    {

        case STATE_CALIBRIER:
          state_calibrier_f(& event);
          break; /* STATE_CALIBRIER */

	      case STATE_IDLE:
	        state_idle_f(& event);
	        break; /* STATE_IDLE */

	      case STATE_CHECK:
	        state_check_f(& event);
	        break; /* STATE_CHECK */

        case STATE_GREEN:
          state_green_f(& event);
          break; /* STATE_GREEN */

        case STATE_YELLOW:
          state_yellow_f(& event);
          break; /* STATE_YELLOW */

        case STATE_RED:
          state_red_f(& event);
          break; /* STATE_RED*/

        case STATE_ALARM:
          state_alarm_f(& event);
          break; /* STATE_ALARM */

	    }
      if (event.error != ERROR_NONE) {
        tio_printf_f(F2("unhandeled error %d \r\n"), event.error);
        state = STATE_IDLE;
        LEDerrorset_f(event.error);
      }
	    //Event sollte nun behandelt sein
	    event = EVENT_ST_DEFAULT;

	}

}

/************************************************************************************/
/*                                                                                  */
/*                                                                                  */
/*      state machine                                                               */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/

void state_calibrier_f(event_st *event)
{
  static byte cnt = 0;
  int eCO2;

  if ((event)->tick) {
    eCO2= get_eCO2_f(event); // die ersten Messwerte taugen noch nichts
    LEDrainbow_f(5); //optischer Pausenfüller
    cnt += 1;
    if (cnt > 1) {
      tio_printf_f(F2("state = STATE_IDLE\r\n"));
      state = STATE_IDLE;
      LEDset_f(COLOR_NONE);        
      cnt = 0;
      }
  }
}

/************************************************************************************/

void state_idle_f(event_st *event)
{
  static byte cnt = 0;
  static long longtime = IAMALIVE;
  unsigned long eCO2;
  static bool toggle=true;
  
  if ((event)->tick) {    
    eCO2= get_eCO2_f(event);
    if ( eCO2 > THRESHOLD_ON) {
      cnt += 1;
      if (cnt > STATEDELAY) {
        tio_printf_f(F2("state = STATE_CHECK\r\n"));
        state = STATE_CHECK;
        cnt = 0;
        /* alle LEDS einschalten*/
        LEDset_f(COLOR_WHITE);
      }      
    } else {
      if (cnt > 1) {
        cnt -= 1;
      }
    }
    longtime -= 1;
    if ( ! longtime ) {
      longtime = IAMALIVE;
      LEDrainbow_f(20); //signalisiert: ich bin eingeschaltet
      LEDset_f(COLOR_NONE); 
    }
    /* LED ausschalten */
    if ((event->error == ERROR_NONE) && (state == STATE_IDLE)) LEDset_f(COLOR_NONE);
  }
}

/************************************************************************************/

void state_check_f(event_st *event)
{
  state_et check;

  if ((event)->tick) {
    /* alle LEDS weiß schalten*/
    LEDset_f(COLOR_WHITE);
    check = eCO2check_f(event);
    if (check == STATE_IDLE) {
     /* UV-Lampe und Lüfter ausschalten */
     digitalWrite(RELAIS_PIN, RELAIS_OFF);
     /* LED ausschalten */
      LEDset_f(COLOR_NONE);
    } else if (state != check) {
      /* UV-Lampe und Lüfter einschalten */
      digitalWrite(RELAIS_PIN, RELAIS_ON);
      LEDset_f(COLOR_GREEN);
    }
    state = check;
  }
}

/************************************************************************************/

void state_green_f(event_st *event)
{
  if ((event)->tick) {
    state = eCO2check_f(event);
    if (state == STATE_IDLE) {
      /* UV-Lampe und Lüfter ausschalten */
      digitalWrite(RELAIS_PIN, RELAIS_OFF);
      /* LED ausschalten */
      LEDset_f(COLOR_NONE);
    }
  }

  if ((event)->tickLED) {
    wipeLED_f(COLOR_GREEN);
  }
}


/************************************************************************************/

void state_yellow_f(event_st *event)
{
  if ((event)->tick) {
    state = eCO2check_f(event);
  }
  if ((event)->tickLED) {
    wipeLED_f(COLOR_YELLOW);
  }
}

/************************************************************************************/

void state_red_f(event_st *event)
{
  if ((event)->tick) {
    state = eCO2check_f(event);
  }
  if ((event)->tickLED) {
    wipeLED_f(COLOR_RED);
  }
}

/************************************************************************************/

void state_alarm_f(event_st *event)
{
  static bool toogle=false;
  
  if ((event)->tick) {
    if (toogle)
    {
      LEDset_f(COLOR_BLUE);
    } else {
      LEDset_f(COLOR_NONE);
    }
    toogle = ! toogle;
  }

  state = eCO2check_f(event);
  if (state == STATE_ALARM) {
   // massiv erhoeter CO2-Wert deutet auf Brand hin, daher:
   /* UV-Lampe und Lüfter ausschalten */
   digitalWrite(RELAIS_PIN, RELAIS_OFF);
  } else if (state == STATE_CHECK) {
    /* von Alarm direkt zu Check ?? ... na, dann muss jetzt auch nichts mehr eingeschaltet werden  */
  } else {
    /* UV-Lampe und Lüfter einschalten */
    digitalWrite(RELAIS_PIN, RELAIS_ON);
  }

}

/************************************************************************************/








/************************************************************************************/
/*                                                                                  */
/*                                                                                  */
/*      Hilfsroutinen                                                               */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


int check_portpins(void)
{
  int ret_val = 0;
  int i, j;
  bool pinstate;

#if 0
  //EVENT_COVEROPEN
  for (i = 0, j = 0; i < 10; i++)
  {
    pinstate = digitalRead(COVER_PIN) ;
    if (pinstate == true) j++;
  }
  if ( j <= 5 ) ret_val |= EVENT_COVEROPEN;
#endif

  return ret_val;
}


/************************************************************************************/

state_et eCO2check_f(event_st *event)
{
  static byte cnt = 0;
  static unsigned long eCO2sum = 0;
  int t;
  state_et newstate = state;
  
  cnt += 1;
  eCO2sum += get_eCO2_f(event);
  tio_printf_f(F2("eCO2sum  / %d = %d \r\n"), cnt, (eCO2sum / cnt));

  if ( cnt >= STATEDELAY) {
    t = eCO2sum / STATEDELAY;
    if ( t > THRESHOLD_ALARM) {
      tio_printf_f(F2("state = STATE_ALARM\r\n"));
      newstate = STATE_ALARM;
    } else if ( t > THRESHOLD_RED) {
      tio_printf_f(F2("state = STATE_RED\r\n"));
      newstate = STATE_RED;
    } else if ( t > THRESHOLD_YELLOW)  {
      tio_printf_f(F2("state = STATE_YELLOW\r\n"));
      newstate = STATE_YELLOW;
    } else if ( t > THRESHOLD_OFF)  {
      tio_printf_f(F2("state = STATE_GREEN\r\n"));
      newstate = STATE_GREEN;
    } else {
      if (( state == STATE_CHECK ) || ( state == STATE_GREEN )) {
        tio_printf_f(F2("state = STATE_IDLE\r\n"));
        newstate = STATE_IDLE;        
      } else {
        tio_printf_f(F2("state = STATE_CHECK\r\n"));
        newstate = STATE_CHECK;        
      }
    }  
    cnt = 0;
    eCO2sum = 0;
  }
  return newstate;
}

/************************************************************************************/

void wipeLED_f(color_et color) {
  static int  cntLED = 0;
  static bool toogle=false;

  if (toogle)
  {
    LEDwipe_f(cntLED, color);
  } else {
    LEDwipe_f(cntLED, COLOR_NONE);
  }
  cntLED += 1;
  if (cntLED == LED_COUNT ) {
    cntLED = 0;
    toogle = ! toogle;
  }
}


/************************************************************************************/





/************************************************************************************/


