/************************************************************************************/
/*! \file
  <!--------------------------------------------------------------------------------->
  \section desc Module Description

  - Project           : Luftsterilisierer
  - Subsystem         : DRV (Driver)
  - Module            : Test input output

*/
/************************************************************************************/




/*** Imported external Objects ******************************************************/
#include "Arduino.h"
#include "Adafruit_SGP30.h"
#include "LEDstripe.h"

/*** Types **************************************************************************/

#include "Luftsterilisierer.h"

/*** Imported external Objects ******************************************************/

extern void show_allcutdata_f(void);

/*** Variables **********************************************************************/

extern Adafruit_SGP30 sgp;
extern state_et state;

/*** Defines and Constants **********************************************************/

#define TIO_DEBUG   1
#include "drv_tio.h"

//#define TIO_CLRSCR    "\x1B[2J"
#define TIO_CLRSCR    "\r\n\r\n\r\n\r\n\r\n"

#define BUFFERSIZE 127



char buffer1[BUFFERSIZE];

/*** Modal Function Prototypes ******************************************************/


/*** Globals                  ******************************************************/

String inputString = "";         // a string to hold incoming data
boolean stringComplete_tio = false;  // whether the string is complete

bool simulate_cut = false;


/************************************************************************************/

void tio_init_f(void)
{

  Serial.begin(9600);
  inputString.reserve(200);

#if TIO_DEBUG
  while (!Serial) {
    ; // wait for serial port to connect.
  }
#endif
  delay(5);
  Serial.print(TIO_CLRSCR);

}



/************************************************************************************/
void _tio_printf_f(const char *fmt_p, ...)
{
  /*-- Local Variables ---------------------------------------------------------------*/

  va_list args;

  /*-- Code Statements ---------------------------------------------------------------*/


  va_start (args, fmt_p);
  vsnprintf (buffer1, BUFFERSIZE - 1 , fmt_p, args);

  Serial.print(buffer1);

  va_end (args);
} /* _tio_printf_f */
/************************************************************************************/


/************************************************************************************/

void serialEvent(void) {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete_tio = true;
    }
  }
}

/************************************************************************************/


String tio_input_f(void) {
  String ret_val = "";

  if (stringComplete_tio) {
    ret_val = inputString;
    // clear the string:
    inputString = "";
    stringComplete_tio = false;
  }
  return ret_val;
}


/************************************************************************************/

void tio_cmdinterpreter_f(event_st *event)
{
  String cmd, param;
  int pos, l , c;

  pos = (event)->tio_cmd.indexOf(':');
  if (pos != -1 ) {
    cmd   = (event)->tio_cmd.substring(0, pos);
    param = (event)->tio_cmd.substring(pos + 1, (event)->tio_cmd.length() - 2);
  } else {
    cmd   = (event)->tio_cmd.substring(0, (event)->tio_cmd.length() - 2);;
  }

  if (cmd == "help")
  {
    tio_printf_f(F2("\n\r\n\rvalid commands: \n\r"));
    tio_printf_f(F2("set:color   switch LED color (numerical) \n\r"));
    tio_printf_f(F2("set:state   switch to state no  \n\r"));
    tio_printf_f(F2("uvon   turn UV lamp and fan on \n\r"));
    tio_printf_f(F2("uvoff  turn UV lamp and fan off \n\r"));
    tio_printf_f(F2("sgp30       serial number SPG30 \n\r"));
    tio_printf_f(F2("\n\r\n\r"));
  }
  else if (cmd == "set")
  {
     if (param.substring(0, 5) == "color" ) {
      c = atoi(param.substring(6,param.length()).c_str()) ;
      LEDset_f( (color_et) c );
      tio_printf_f(F2("setting color %d \r\n"),c);
     }
     else if (param.substring(0, 5) == "state" ) {
      c = atoi(param.substring(6,param.length()).c_str()) ;
      state = (state_et) c ;
      tio_printf_f(F2("setting state %d \r\n"),c);
     }
  }
  else if (cmd == "uvon")
  {
     digitalWrite(RELAIS_PIN, RELAIS_ON);
     tio_printf_f(F2("turn UV lamp and fan on \r\n"));
  }
  else if (cmd == "uvoff")
  {
     digitalWrite(RELAIS_PIN, RELAIS_OFF);
     tio_printf_f(F2("turn UV lamp and fan off \r\n"));
    
  }
  else if (cmd == "sgp30")
  {
    tio_printf_f(F2("Found SGP30 serial #%02x%02x%02x\r\n"),sgp.serialnumber[0],sgp.serialnumber[1],sgp.serialnumber[2]);
  }
  else
  {
    tio_printf_f(F2("no command found, try 'help' (input:%s)\n\r"), cmd.c_str());
  }

}



