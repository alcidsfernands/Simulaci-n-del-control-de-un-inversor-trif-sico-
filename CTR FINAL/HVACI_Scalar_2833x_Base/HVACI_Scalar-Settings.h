/* =================================================================================
File name:  HVACI_Scalar-Settings.H                     
 
Description: Incremental Build Level control file.
=================================================================================  */

#ifndef PROJ_SETTINGS_H

#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#define PI 3.14159265358979

// Define the system frequency (MHz)
#define SYSTEM_FREQUENCY 150

// Define the ISR frequency (kHz)
#define ISR_FREQUENCY 5

// Define the electrical motor parametes (1/4 hp Marathon Motor)
#define RS 		11.05		        // Stator resistance (ohm) 
#define RR   	6.11		        // Rotor resistance (ohm) 
#define LS   	0.316423    	  	// Stator inductance (H) 
#define LR   	0.316423	  		// Rotor inductance (H) 	
#define LM   	0.293939	   		// Magnatizing inductance (H)
#define POLES  	4					// Number of poles

// Define the base quantites for PU system conversion
#define BASE_VOLTAGE    236.174     // Base peak phase voltage (volt)
#define BASE_CURRENT    10          // Base peak phase current (amp)
#define BASE_TORQUE         		// Base torque (N.m)
#define BASE_FLUX       		    // Base flux linkage (volt.sec/rad)
#define BASE_FREQ      	60          // Base electrical frequency (Hz) 
									// Note that 0.5 pu (1800 rpm) is max for Marathon motor 
									// Above 1800 rpm, field weakening is needed.
#endif

