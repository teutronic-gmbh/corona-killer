/************************************************************************************/
/*! \file 
  <!--------------------------------------------------------------------------------->
  \section desc Module Description
  
  - Project           : Luftsterilisierer
  - Subsystem         : DRV (Driver)
  - Module            : Test input output

*/
/************************************************************************************/


#ifndef _DRV_TIO_H /* avoids multiple inclusion */
#define _DRV_TIO_H

#include "Arduino.h"

/** Macros **************************************************************************/

#if (TIO_DEBUG)
#ifndef tio_printf_f
  #define tio_printf_f(...) \
    _tio_printf_f(__VA_ARGS__)
#endif
#else
  #define tio_printf_f(...)
#endif


#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define F2(s)   ((const char PROGMEM *)s)

/** Prototypes **********************************************************************/


#include "Luftsterilisierer.h"

extern bool simulate_cut;


/************************************************************************************/
/*!
  \brief     Init the TIO module.

  \param     
*/
/************************************************************************************/
void tio_init_f(void);


/************************************************************************************/
/*!
  \brief      Print test output.

  \param      fmt_p     : format string (mandatory)
              arg, ...  : optional arguments according to the format string
*/
/************************************************************************************/
void _tio_printf_f(const char *fmt_p, ...);

/************************************************************************************/
/*!
  \brief      .

  \param      .
              .
*/
/************************************************************************************/
void serialEvent_tio(void);

/************************************************************************************/
/*!
  \brief      .

  \param      .
              .
*/
/************************************************************************************/
String tio_input_f(void);

/************************************************************************************/
/*!
  \brief      .

  \param      .
              .
*/
/************************************************************************************/
void tio_cmdinterpreter_f(event_st *event);

#endif

