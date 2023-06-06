/*
 * sfuntmpl_basic.c: Basic 'C' template for a level 2 S-function.
 *
 * Copyright 1990-2018 The MathWorks, Inc.
 */
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * You must specify the S_FUNCTION_NAME as the name of your S-function
 * (i.e. replace sfuntmpl_basic with the name of your S-function).
 */

#define S_FUNCTION_NAME  Moduladora
#define S_FUNCTION_LEVEL 2

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */ 
#include "simstruc.h"

double PI_Num = 3.14159265359;
double TSAMPLE = 0;
double time = 0;
double integral = 0;
double NBITS = 1;
double FREQFUNDAMENTAL = 60;
double FREQCOMUTACION = 1;
double LIMITESUPERIOR = 1.154;
double RAMPA = 0;
double OFFSET = 0;
double ACTIVAR3ARMONICO = 1;
double OVERCURRENTLIMIT = 10;
double KC = 1/6;
double VDC = 311;
double VFF = 220;
double RelacionVF = 1;
double pendiente = (1-0)/(1-0);
double pendiente3armonico = (1.154-0)/(1-0);
double q=1;
double qtension=1;
double fondo_pos_corriente = 9.09;
double fondo_neg_corriente = -9.09;
double fondo_pos_tension = 372.64;
double fondo_neg_tension = 0;
double overcurrent_limit = 500;
double velocidad=0;
double stage=0;
double corrienteMax=0;
double tensionMax=0;
double deltaAngulo=0;
bool trip = false;


/* Error handling
 * --------------
 *
 * You should use the following technique to report errors encountered within
 * an S-function:
 *
 *       ssSetErrorStatus(S,"Error encountered due to ...");
 *       return;
 *
 * Note that the 2nd argument to ssSetErrorStatus must be persistent memory.
 * It cannot be a local variable. For example the following will cause
 * unpredictable errors:
 *
 *      mdlOutputs()
 *      {
 *         char msg[256];         {ILLEGAL: to fix use "static char msg[256];"}
 *         sprintf(msg,"Error due to %s", string);
 *         ssSetErrorStatus(S,msg);
 *         return;
 *      }
 *
 */

/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 12);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        /* Return if number of expected != number of actual parameters */
        return;
    }
    TSAMPLE = mxGetPr(ssGetSFcnParam(S,0))[0]; /*nos devuelve un puntero a un array de “doubles”, cogiendo el primer elemento del array*/
    NBITS = mxGetPr(ssGetSFcnParam(S,1))[0]; /*nos devuelve un puntero a un array de “doubles”, cogiendo el primer elemento del array*/
    OFFSET = mxGetPr(ssGetSFcnParam(S,2))[0]; /*nos devuelve un puntero a un array de “doubles”, cogiendo el primer elemento del array*/    
    RAMPA = mxGetPr(ssGetSFcnParam(S,3))[0]; /*nos devuelve un puntero a un array de “doubles”, cogiendo el primer elemento del array*/   
    FREQFUNDAMENTAL = mxGetPr(ssGetSFcnParam(S,4))[0];  
    FREQCOMUTACION = mxGetPr(ssGetSFcnParam(S,5))[0];   
    KC = mxGetPr(ssGetSFcnParam(S,6))[0];   
    LIMITESUPERIOR = mxGetPr(ssGetSFcnParam(S,7))[0];  
    ACTIVAR3ARMONICO = mxGetPr(ssGetSFcnParam(S,8))[0];   
    OVERCURRENTLIMIT = mxGetPr(ssGetSFcnParam(S,9))[0]; 
    VDC = mxGetPr(ssGetSFcnParam(S,10))[0]; 
    VFF = mxGetPr(ssGetSFcnParam(S,11))[0]; 
    RelacionVF = (LIMITESUPERIOR-0)/(1-0);
    
    if (!ssSetNumInputPorts(S, 3)) return;
    
    ssSetInputPortWidth(S, 0, 1); //la anchura del puerto de entrada 0 es 1 - Consigna Velocidad
    ssSetInputPortRequiredContiguous(S, 0, true); /*direct input signal access*/
    
    ssSetInputPortWidth(S, 1, 3); //la anchura del puerto de entrada 1 es 3 - Corrientes r-s-t
    ssSetInputPortRequiredContiguous(S, 1, true); /*direct input signal access*/
    
    ssSetInputPortWidth(S, 2, 3); //la anchura del puerto de entrada 2 es 3 - Tensiones ff
    ssSetInputPortRequiredContiguous(S, 2, true); /*direct input signal access*/
    
    /*
     * Set direct feedthrough flag (1=yes, 0=no).
     * A port has direct feedthrough if the input is used in either
     * the mdlOutputs or mdlGetTimeOfNextVarHit functions.
     */
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 1, 1);
    ssSetInputPortDirectFeedThrough(S, 2, 1);

    if (!ssSetNumOutputPorts(S, 7)) return;
    ssSetOutputPortWidth(S, 0, 3); /* Señal Senoidal */
    ssSetOutputPortWidth(S, 1, 1); /* Overcurrent */    
    ssSetOutputPortWidth(S, 2, 3); /* Corrientes cuantificadas */
    ssSetOutputPortWidth(S, 3, 3); /* Señal de error de cuantificación de las corrientes */
    ssSetOutputPortWidth(S, 4, 3); /* Potencia calculada */
    ssSetOutputPortWidth(S, 5, 3); /* Tensiones cuantificadas */
    ssSetOutputPortWidth(S, 6, 3); /* Señal de error de cuantificación de las tensiones */

    ssSetNumSampleTimes(S, 1); // cada cuanto tiempo la S-function llama a la function1

}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, TSAMPLE); // defino el tiempo de muestreo
    ssSetOffsetTime(S, 0, 0.0);

}



/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block.
 */
// Este es nuestro Main
static void mdlOutputs(SimStruct *S, int_T tid)
{
    //definimos variables de entrada
    const real_T *Input0Consigna  = (const real_T*) ssGetInputPortSignal(S,0);
    const real_T *Input1Corriente = (const real_T*) ssGetInputPortSignal(S,1);
    const real_T *Input2Tension = (const real_T*) ssGetInputPortSignal(S,2);
    
    //definimos variables de salida
    real_T *OutputSenalSenoidal = ssGetOutputPortSignal(S,0);
    real_T *OutputOvercurrent   = ssGetOutputPortSignal(S,1);
    real_T *OutputCorrientesCuantificadas = ssGetOutputPortSignal(S,2);
    real_T *OutputErrorcuantificacionIntensidad = ssGetOutputPortSignal(S,3);
    real_T *OutputPotencia = ssGetOutputPortSignal(S,4);
    real_T *OutputTensionesCuantificadas = ssGetOutputPortSignal(S,5);
    real_T *OutputErrorcuantificacionTension = ssGetOutputPortSignal(S,6);
    
    double cosigna = Input0Consigna[0];
    
    if (abs(Input1Corriente[0]) > OVERCURRENTLIMIT ||
        abs(Input1Corriente[1]) > OVERCURRENTLIMIT ||
        abs(Input1Corriente[2]) > OVERCURRENTLIMIT ||
        trip) { 
        cosigna = 0;
        trip = true;
        OutputOvercurrent[0] = 1;
    }
    
    if (cosigna < OFFSET) {
        cosigna = OFFSET;
    } else if (cosigna > 1) {
        cosigna = 1;
    } 
    
    double incremento = RAMPA * TSAMPLE;
    double diferencia = cosigna - velocidad;
    if (diferencia > incremento) {
        velocidad = velocidad + incremento;
    } else if (diferencia < -incremento) {
        velocidad = velocidad - incremento;
    }

    integral = 2*PI_Num*velocidad*FREQFUNDAMENTAL*TSAMPLE + integral;
    if (integral > 2*PI_Num) {
        integral = integral-2*PI_Num;
	}
    if (ACTIVAR3ARMONICO > 0) {
        double terceraArmonica = velocidad * KC * sin (3*integral);
        OutputSenalSenoidal[0] = RelacionVF * (velocidad * sin(integral + 0) + terceraArmonica);
        OutputSenalSenoidal[1] = RelacionVF * (velocidad * sin(integral - 2*PI_Num/3) + terceraArmonica);
        OutputSenalSenoidal[2] = RelacionVF * (velocidad * sin(integral + 2*PI_Num/3) + terceraArmonica); 
    } else {
        OutputSenalSenoidal[0] = RelacionVF * velocidad * sin(integral + 0);
        OutputSenalSenoidal[1] = RelacionVF * velocidad * sin(integral - 2*PI_Num/3);
        OutputSenalSenoidal[2] = RelacionVF * velocidad * sin(integral + 2*PI_Num/3); 
    } 
    
    q = (fondo_pos_corriente-fondo_neg_corriente)/(pow(2,NBITS)-1);
    OutputCorrientesCuantificadas[0] = round((Input1Corriente[0])/q)*q;
    OutputCorrientesCuantificadas[1] = round((Input1Corriente[1])/q)*q;
    OutputCorrientesCuantificadas[2] = round((Input1Corriente[2])/q)*q;
    
    qtension = (fondo_pos_tension-fondo_neg_tension)/(pow(2,NBITS)-1);
    OutputTensionesCuantificadas[0] = round((Input2Tension[0])/qtension)*qtension;
    OutputTensionesCuantificadas[1] = round((Input2Tension[1])/qtension)*qtension;
    OutputTensionesCuantificadas[2] = round((Input2Tension[2])/qtension)*qtension;
    
    OutputErrorcuantificacionIntensidad[0] = Input1Corriente[0] - OutputCorrientesCuantificadas[0];
    OutputErrorcuantificacionIntensidad[1] = Input1Corriente[1] - OutputCorrientesCuantificadas[1];  
    OutputErrorcuantificacionIntensidad[2] = Input1Corriente[2] - OutputCorrientesCuantificadas[2]; 
    
    OutputErrorcuantificacionTension[0] = Input2Tension[0] - OutputTensionesCuantificadas[0];
    OutputErrorcuantificacionTension[1] = Input2Tension[1] - OutputTensionesCuantificadas[1];  
    OutputErrorcuantificacionTension[2] = Input2Tension[2] - OutputTensionesCuantificadas[2];
        
    OutputPotencia[2] = Input1Corriente[0]*Input2Tension[0] + Input1Corriente[1]*Input2Tension[1] + Input1Corriente[2]*Input2Tension[2];
    
    if (stage == 0) {
        if (corrienteMax < Input1Corriente[0]) {
            corrienteMax = Input1Corriente[0];
        }
        if (Input1Corriente[0] < 0 ) {
            deltaAngulo = integral-PI_Num;
            OutputPotencia[0] = sqrt(3)*VDC*0.612*RelacionVF*velocidad*corrienteMax/sqrt(2);
            OutputPotencia[1] = OutputPotencia[0]*cos(deltaAngulo);
            corrienteMax = 0;
            stage = 1;
        }
    } else if (stage == 1 && Input1Corriente[0] > 0.3) {
        stage = 0;
    }
    
    time = time+TSAMPLE;
}



/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
}


/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
