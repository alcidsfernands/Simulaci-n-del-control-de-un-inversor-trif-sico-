/* ==============================================================================
System Name:  	HVACI_Scalar
 
File Name:		HVACI_Scalar

Description:	Primary system header file for the Real Implementation of Scalar  
          		Field Orientation Control for Induction Motor
=================================================================================  */



/*-------------------------------------------------------------------------------
Next, Include project specific include files.
-------------------------------------------------------------------------------*/
typedef float _iq;

#if (DSP2833x_DEVICE_H==1)
#include "f2833xpwm.h"			// Include header for the PWMGEN object
#include "f2833xileg_vdc.h" 	// Include header for the ILEG2DCBUSMEAS object
#include "DSP2833x_EPwm_defines.h" // Include header for PWM defines
#endif


//===========================================================================
// No more.
//===========================================================================



//===========================================================================
// My own structures
//===========================================================================
/*
 * Estructura para la modulación escalar. Completar según necesidades
 */
typedef struct SINMODUL
{

}SINMODUL;


/*
 * Estructura para el perfil V/f. A modo de ejemplo, completar según necesidades
 */
typedef struct 	{
	float	SpFreq;	//Consigna de velocidad, INPUT de usuario.

} PERFIL_VF;
