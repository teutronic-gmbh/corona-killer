/************************************************************************************/
/*! \file
  <!--------------------------------------------------------------------------------->
  \section desc Module Description

  - Project           : Luftsterilisierer
  - Subsystem         : DRV (Driver)
  - Module            : SGP30

*/
/************************************************************************************/

#ifndef _DRV_SGP30_H /* avoids multiple inclusion */
#define _DRV_SGP30_H

#include "Luftsterilisierer.h"

void setup_sgp30_f();
unsigned long get_eCO2_f(event_st *event);


#endif
