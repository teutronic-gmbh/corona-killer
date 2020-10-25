/************************************************************************************/
/*! \file
  <!--------------------------------------------------------------------------------->
  \section desc Module Description

  - Project           : Luftsterilisierer
  - Subsystem         : DRV (Driver)
  - Module            : SGP30

*/
/************************************************************************************/




/*** Imported external Objects ******************************************************/
#include <Wire.h>
#include "Adafruit_SGP30.h"
#include "Luftsterilisierer.h"

/*** Types **************************************************************************/


/*** Variables **********************************************************************/



/*** Defines and Constants **********************************************************/
#define TIO_DEBUG   1
#include "drv_tio.h"


Adafruit_SGP30 sgp;



/*** Modal Function Prototypes ******************************************************/

uint32_t getAbsoluteHumidity(float temperature, float humidity);

/*** Globals                  ******************************************************/
int counter = 0;


/************************************************************************************/


void setup_sgp30_f() {

  if (! sgp.begin()){
	  tio_printf_f(F2("CO2-Sensor not found !!\r\n"));
//	  (event)->error=ERROR_BADSENSOR;
  } else {
	  tio_printf_f(F2("Found SGP30 serial #%02x%02x%02x\r\n"),sgp.serialnumber[0],sgp.serialnumber[1],sgp.serialnumber[2]);
  }

  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  //sgp.setIAQBaseline(0x8E68, 0x8F41);  // Will vary for each sensor!
}

/************************************************************************************/

unsigned long get_eCO2_f(event_st *event) {
  unsigned long r =0 ;
  // If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
  //float temperature = 22.1; // [�C]
  //float humidity = 45.2; // [%RH]
  //sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (! sgp.IAQmeasure()) {
    tio_printf_f(F2("CO2-Measurement failed !!\r\n"));
    (event)->error=ERROR_BADSENSOR;
    return 0;
  }
  r = sgp.eCO2;
  tio_printf_f(F2("eCO2 %u ppm  (TVOC %u ppb)\r\n"),sgp.eCO2,sgp.TVOC);

  if (! sgp.IAQmeasureRaw()) {
    tio_printf_f(F2("Raw Measurement failed !!\r\n"));
    (event)->error=ERROR_BADSENSOR;
    return 0;
  }
  tio_printf_f(F2("(Raw H2 %u  Raw Ethanol %u )\r\n"),sgp.rawH2,sgp.rawEthanol);
 
  counter++;
  if (counter == 30) {
    counter = 0;

    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      tio_printf_f(F2("Failed to get baseline readings\r\n"));
      (event)->error=ERROR_BADSENSOR;
      return 0;
    }
    tio_printf_f(F2("****Baseline values: eCO2: %u & TVOC: %u )\r\n"),eCO2_base,TVOC_base);
  }
  return r;
}

/************************************************************************************/
/*                                                                                  */
/*                                                                                  */
/*      Hilfsroutinen                                                               */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/



/* return absolute humidity [mg/m^3] with approximation formula
* @param temperature [�C]
* @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

