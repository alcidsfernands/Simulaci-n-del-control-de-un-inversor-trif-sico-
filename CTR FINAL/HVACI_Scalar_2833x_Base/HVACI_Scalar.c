/* ==============================================================================
System Name:    HVACI_Scalar

File Name:      HVACI_Scalar.C

Description:    Primary system file for the Scalar Control of Induction Motors

================================================================================= */

// Include header files used in the main function

#include "PeripheralHeaderIncludes.h"
#include "HVACI_Scalar-Settings.h"
#include "HVACI_Scalar.h"
#include <stdbool.h>
#include <math.h>
#include "DSP2833x_Device.h"

double Vmotor = 236.174; // Motor Nominal Currrents
double Vdc = 372.64; // Bus Voltage
double fondo_corriente_positivo = 9.09;
double fondo_corriente_negativo = -9.09;
double fondo_tension_positivo = 372.64;
double fondo_tension_negativo = 0;
double fondo_ADC_positivo = 3;
double fondo_ADC_negativo = 0;
double NBITS = 12;
double KC = 1/6;
double FREQFUNDAMENTAL = 60;
double OFFSET = 0.1;

// Prototype statements for functions found within this file.
interrupt void MainISR(void);

void DeviceInit();
void HVDMC_Protection(void);
void BackgroundLoop1(void);

void InitVariables(void);
void InitPWM(void);
void InitADC(void);

void ConfigInterrupts(void);

//Custom Functions
void initCustomVariables(void);
void readAndProcessADCValues(void);
void rampaVF(void);
void SinusoidalModulation(double OutputSenal[3]);
void current_protection(double currents[3], double overcurrent_limits);

// Global variables used in this system
float SpeedRef = 0.3;           // Speed reference (pu)
float T = 0.001/ISR_FREQUENCY;  // Samping period (sec)
long freq = 150000;
long tiempoInterrupcion = 1000000;
Uint32 IsrTicker = 0;
Uint16 BackTicker = 0;
Uint16 TripFlagDMC=0;           //PWM trip status

//General
double test = 0;
double PI_Num = 3.14159265359;

// RampaVF y sinusoidal
double velocidad = 0.0;
double RAMPA = 0.1;
double integral = 0;
double RelacionVF = 1;
//double KC = 1/6;

//Protecion de sobrecorriente
double overcurrent_limit = 100000;
int overcurrent = 0;

//ADC
Uint16 ADCCONV[15]; // ADC buffer allocation
double current[3]; // ADC buffer allocation
double voltage[3]; // ADC buffer allocation
double busVoltage = 372.64; // ADC buffer allocation
double qCurrent = 1;
double qVoltage = 1;
double tensionMax = 0;
double corrienteMax = 0;
double potencia = 0;
double potenciaTotal = 0;
double potenciaReal = 0;
int stage = 0;

// Default ADC initialization 
int ChSel[16]   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int TrigSel[16] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
int ACQPS[16]   = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};

// MODIFIED BY USER:Enable Varialbe to start running the program
volatile Uint16 EnableFlag = FALSE;


//=================================================================================
// GLOBAL STRUCTURE DEFINITIONS
//=================================================================================
// Instance a PWM driver 
PWMGEN pwm1 = PWMGEN_DEFAULTS;
// Instance to user structures
PERFIL_VF vhz1 = {0};       //User-defined structure, use and name as needed.



//=================================================================================
// MAIN PROGRAM ROUTINE
//=================================================================================

void main(void)
{
    int i=0;

    DeviceInit();   // Device Life support & GPIO

   // Waiting for enable flag set.
   //Flag has to be set manually for the program to run.
   while (EnableFlag==FALSE)
    { 
      BackTicker++;
    }

// Timing sync for background loops
// Timer period definitions found in device specific PeripheralHeaderIncludes.h
    CpuTimer2Regs.PRD.all =  mSec100;   // Background task

    InitPWM();
    InitADC();

// Initialize user structures and variables
    initCustomVariables();

// Call HVDMC Protection function
    HVDMC_Protection();

//INTERRUPT CONFIGURATION
    ConfigInterrupts();


// IDLE loop. Just sit and loop forever:
    for(;;)  //infinite loop
    {
        BackgroundLoop1();

    }
} //END MAIN CODE

/*
*   InitPWM()
*   Inicialización de los tiempos de funcionamiento del PWM
*/
void InitPWM(void)
{
    // Initialize PWM modules 1, 2 and 3.

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0; // Disable ePWM modules
   EDIS;

   EPwm1Regs.TBCTL.bit.CTRMODE = 0x3;   // Time-Base Control Register - Disable counter
   EPwm1Regs.TBCTL.all = 0xC013;        // Time-Base Control Register - Free run,Sync with this register,Stop-freeze counter operation (default on reset)
   EPwm1Regs.TBCTR = 0x0000;            // Time-Base Counter Register - Reset timer counter
   EPwm1Regs.TBPRD = 15000 ;            // Time-Base Period Register - Timer = 150Mhz/5Khz/2 - PWM period = 2 x TBPRD x TTBCLK
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000; // Time-Base Phase Register - Phase=0
   EPwm1Regs.ETSEL.all = 0x0A00;        // Event-Trigger Selection Register - Enable EPWMxSOCA(Start Of Conversion de canal A) pulse, Enable event time-base counter equal to period (TBCTR = TBPRD)
   EPwm1Regs.ETPS.all = 0x0100;         // Event-Trigger Prescale Register - Generate the EPWMxSOCA pulse on the first event: ETPS[SOCACNT] = 0,1
   EPwm1Regs.CMPCTL.all = 0x0002;       // Counter-Compare Control Register - Load on either CTR = Zero or CTR = PRD
   EPwm1Regs.AQCTLA.all = 0x0090;       // Action-Qualifier Output A Control Register - Set: force EPWMxA output high, Clear: force EPWMxA output low
   EPwm1Regs.AQCTLB.all = 0x0060;       // Action-Qualifier Output B Control Register - Clear: force EPWMxB output low, Set: force EPWMxB output high
   EPwm1Regs.AQSFRC.all = 0x0000;       // Action-Qualifier Software Force Register - Disabled \ Does nothing
   EPwm1Regs.AQCSFRC.all = 0x0000;      // Action-qualifier Continuous Software Force Register - Disabled \ Does nothing
   EPwm1Regs.DBCTL.all = 0x000B;        //Dead-Band Generator Control Register - Active high complementary (AHC). EPWMxB is inverted, Dead-band is fully enabled for both rising-edge delay on output EPWMxA and falling-edge delay on output EPWMxB

   EPwm2Regs.TBCTL.bit.CTRMODE = 0x3;   // Time-Base Control Register - Disable counter
   EPwm2Regs.TBCTL.all = 0xC007;        // Time-Base Control Register - Free run,Sync with ePWM1,Stop-freeze counter operation (default on reset)
   EPwm2Regs.TBCTR = 0x0000;            // Time-Base Counter Register - Reset timer counter
   EPwm2Regs.TBPRD = 15000;             // Time-Base Period Register - Timer = 150Mhz/5Khz - PWM period = 2 x TBPRD x TTBCLK
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000; // Time-Base Phase Register - Phase=0
   EPwm2Regs.AQCTLA.all = 0x0090;       // Action-Qualifier Output A Control Register - Set: force EPWMxA output high, Clear: force EPWMxA output low
   EPwm2Regs.AQCTLB.all = 0x0060;       // Action-Qualifier Output B Control Register - Clear: force EPWMxB output low, Set: force EPWMxB output high
   EPwm2Regs.AQSFRC.all = 0x0000;       // Action-Qualifier Software Force Register - Disabled \ Does nothing
   EPwm2Regs.AQCSFRC.all = 0x0000;      // Action-qualifier Continuous Software Force Register - Disabled \ Does nothing
   EPwm2Regs.DBCTL.all = 0x000B;        //Dead-Band Generator Control Register - Active high complementary (AHC). EPWMxB is inverted, Dead-band is fully enabled for both rising-edge delay on output EPWMxA and falling-edge delay on output EPWMxB

   EPwm3Regs.TBCTL.bit.CTRMODE = 0x3;   // Time-Base Control Register - Disable counter
   EPwm3Regs.TBCTL.all = 0xC007;        // Time-Base Control Register - Free run,Sync with ePWM1,Stop-freeze counter operation (default on reset)
   EPwm3Regs.TBCTR = 0x0000;            // Time-Base Counter Register - Reset timer counter
   EPwm3Regs.TBPRD = 15000;             // Time-Base Period Register - Timer = 150Mhz/5Khz - PWM period = 2 x TBPRD x TTBCLK
   EPwm3Regs.TBPHS.half.TBPHS = 0x0000; // Time-Base Phase Register - Phase=0
   EPwm3Regs.AQCTLA.all = 0x0090;       // Action-Qualifier Output A Control Register - Set: force EPWMxA output high, Clear: force EPWMxA output low
   EPwm3Regs.AQCTLB.all = 0x0060;       // Action-Qualifier Output B Control Register - Clear: force EPWMxB output low, Set: force EPWMxB output high
   EPwm3Regs.AQSFRC.all = 0x0000;       // Action-Qualifier Software Force Register - Disabled \ Does nothing
   EPwm3Regs.AQCSFRC.all = 0x0000;      // Action-qualifier Continuous Software Force Register - Disabled \ Does nothing
   EPwm3Regs.DBCTL.all = 0x000B;        //Dead-Band Generator Control Register - Active high complementary (AHC). EPWMxB is inverted, Dead-band is fully enabled for both rising-edge delay on output EPWMxA and falling-edge delay on output EPWMxB

    /* Init Dead-Band Generator for EPWM1-EPWM3*/
    EPwm1Regs.DBFED = 200;
    EPwm1Regs.DBRED = 200;
    EPwm2Regs.DBFED = 200;
    EPwm2Regs.DBRED = 200;
    EPwm3Regs.DBFED = 200;
    EPwm3Regs.DBRED = 200;

    /* Chopper unit disabled */
    EPwm1Regs.PCCTL.bit.CHPEN = 0;     // PWM chopper unit disabled
    EPwm2Regs.PCCTL.bit.CHPEN = 0;     // PWM chopper unit disabled
    EPwm3Regs.PCCTL.bit.CHPEN = 0;     // PWM chopper unit disabled

    EPwm1Regs.TBCTL.bit.CTRMODE = 0x2; // Time-Base Control Register - Enable counter Up-down-count mode
    EPwm2Regs.TBCTL.bit.CTRMODE = 0x2; // Time-Base Control Register - Enable counter Up-down-count mode
    EPwm3Regs.TBCTL.bit.CTRMODE = 0x2; // Time-Base Control Register - Enable counter Up-down-count mode

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1; // Enable ePWM modules
   EDIS;
}

/*
*   InitADC()
*   Inicialización de los canales de captura de los ADC
*    ADC A1-> Phase A Current
*    ADC B1-> Phase B Current
*    ADC A3-> Phase C Current
*    ADC B7-> Phase A Voltage
*    ADC B6-> Phase B Voltage
*    ADC B4-> Phase C Voltage
*    ADC A7-> DC Bus  Voltage
*/
void InitADC(void)
{
    //Initizlization of the calibration data
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
    ADC_cal();
    EDIS;
    AdcRegs.ADCTRL3.all = 0x00E0;  /* Power up bandgap/reference/ADC circuits*/
    DELAY_US(ADC_usDELAY);         /* Delay before converting ADC channels*/

    // Initialize ADC module
    AdcRegs.ADCTRL1.bit.RESET = 1; // ADC Control Register 1 - Reset
    asm(" RPT #22 || NOP"); // Provides the required delay between writes to ADCTRL1

    AdcRegs.ADCREFSEL.bit.REF_SEL = 0;      // ADC Reference Select Register - Internal reference selected (default)
    AdcRegs.ADCTRL3.all = 0x00EC;         // ADC Control Register 3 - The bandgap and reference circuitry is powered up, The analog circuitry inside the core is powered up., Core Clock Driver 6 HSPCLK/[12*(ADCTRL1[7] + 1)]
    AdcRegs.ADCMAXCONV.all = 0x6;           // Maximum Conversion Channels Register - Number of conversions = 7
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x1;  // ADC Input Channel Select Sequencing Control Registers - Channels Selected ADCINA1 ( Phase A Current)
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x9;  // ADC Input Channel Select Sequencing Control Registers - Channels Selected ADCINB1 ( Phase  B  Current)
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x3;  // ADC Input Channel Select Sequencing Control Registers - Channels Selected ADCINA3 ( Phase C Current)
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0xF;  // ADC Input Channel Select Sequencing Control Registers - Channels Selected ADCINB7 (Phase A Voltage)
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0xE;  // ADC Input Channel Select Sequencing Control Registers - Channels Selected ADCINB6 (Phase B Voltage)
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0xC;  // ADC Input Channel Select Sequencing Control Registers - Channels Selected ADCINB4 (Phase C Voltage)
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x7;  // ADC Input Channel Select Sequencing Control Registers - Channels Selected ADCINA7 (DC Bus  Voltage)

    AdcRegs.ADCTRL1.all = 0x0710;           // ADC Control Register 1 - Acquisition window size = 7 + 1 times the ADCLK period,ADCCLK = Fclk/1, Cascaded mode. SEQ1 and SEQ2 operate as a single 16-state sequencer (SEQ).
    AdcRegs.ADCTRL2.all = 0x0900;           // ADC Control Register 2 - Interrupt request by INT_SEQ1 is enabled, Allows SEQ1/SEQ to be started by ePWMx SOCA trigger
}

/*
*   ConfigInterrupts()
*   Configuración de las interrupciones: Origen de la interrupcion e ISR() de destino.
*/
void    ConfigInterrupts(void)
{
    //--- Enable the MainISR interrupt with timer
    EALLOW;
    PieVectTable.ADCINT = &MainISR;     // PIE MUXed Peripheral Interrupt Vector Table - Set method for interruption
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;  // Enable the ADCINT interrupt in the PIE module
    IER |= 0x0001;                      // Interrupt Enable Register - Enable
    EINT;
    ERTM;
    test = 2;
}


//=================================================================================
//   CUSTOM FUNCTION
//=================================================================================

/*
*   initCustomVariables()
*   Configuracion de las variables iniciales.
*/
void    initCustomVariables(void)
{
    overcurrent = 0;

    //Calcular la relacion V/F
    double VmotorPU = Vmotor/(Vdc*0.6124);
    if (KC > 0) {
        if (VmotorPU > 1.154) {
            VmotorPU = 1.154;
        }
    } else {
        if (VmotorPU > 1) {
            VmotorPU = 1;
        }
    }
    RelacionVF = (VmotorPU-0)/(1-0);

    //double qADC = (fondo_ADC_positivo-fondo_ADC_negativo)/(pow(2,NBITS)-1);
    qCurrent = (fondo_corriente_positivo-fondo_corriente_negativo)/(pow(2,NBITS)-1);
    qVoltage = (fondo_tension_positivo-fondo_tension_negativo)/(pow(2,NBITS)-1);
}

/*
*   readAndProcessADCValues()
*   Configuracion de las variables iniciales.
*/
void    readAndProcessADCValues(void)
{
    ADCCONV[0] = AdcMirror.ADCRESULT0;
    ADCCONV[1] = AdcMirror.ADCRESULT1;
    ADCCONV[2] = AdcMirror.ADCRESULT2;
    ADCCONV[3] = AdcMirror.ADCRESULT3;
    ADCCONV[4] = AdcMirror.ADCRESULT4;
    ADCCONV[5] = AdcMirror.ADCRESULT5;
    ADCCONV[6] = AdcMirror.ADCRESULT6;

    current[0] = AdcMirror.ADCRESULT0 * qCurrent + fondo_corriente_negativo;
    current[1] = AdcMirror.ADCRESULT1 * qCurrent + fondo_corriente_negativo;
    current[2] = AdcMirror.ADCRESULT2 * qCurrent + fondo_corriente_negativo;

    voltage[0] = AdcMirror.ADCRESULT3 * qVoltage + fondo_tension_negativo;
    voltage[1] = AdcMirror.ADCRESULT4 * qVoltage + fondo_tension_negativo;
    voltage[2] = AdcMirror.ADCRESULT5 * qVoltage + fondo_tension_negativo;

    busVoltage = AdcMirror.ADCRESULT6 * qVoltage + fondo_tension_negativo;
    potencia = current[0]*voltage[0] + current[1]*voltage[1] + current[2]*voltage[2];

    if (stage == 0) {
        if (corrienteMax < current[0]) {
            corrienteMax = current[0];
        }
        if (current[0] < 0 ) {
            double deltaAngulo = integral-PI_Num;
            potenciaTotal = sqrt(3)*Vdc*0.612*RelacionVF*velocidad*corrienteMax/sqrt(2);
            potenciaReal = potenciaTotal*cos(deltaAngulo);
            corrienteMax = 0;
            stage = 1;
        }
    } else if (stage == 1 && current[0] > 0.3) {
        stage = 0;
    }
}

/*
*   rampaVF()
*   Calculo de la rampa y relacion tension x frecuencia.
*/
void    rampaVF(void)
{
    //Calcular la relacion V/F
    /*double VmotorPU = Vmotor/(busVoltage*0.6124);
    if (KC > 0) {
        if (VmotorPU > 1.154) {
            VmotorPU = limiteSuperior;
        }
    } else {
        if (VmotorPU > 1) {
            VmotorPU = limiteSuperior;
        }
    }
    RelacionVF = (VmotorPU-0)/(1-0);*/

    //Rampa
    if (SpeedRef < OFFSET) {
        SpeedRef = OFFSET;
    } else if (SpeedRef > 1) {
        SpeedRef = 1;
    }

    float diferencia = SpeedRef - velocidad;
    double incremento = RAMPA * T;
    if (diferencia > incremento) {
        velocidad = velocidad + incremento;
    } else if (diferencia < -incremento) {
        velocidad = velocidad - incremento;
    }
}

/*
*   SinusoidalModulation()
*   Calculo sinusoidal de modulacion.
*/
void    SinusoidalModulation(double OutputSenal[3])
{
    integral = 2*PI_Num*velocidad*FREQFUNDAMENTAL*T + integral;
    if (integral > 2*PI_Num) {
        integral = integral-2*PI_Num;
    }

    if (KC > 0) {
        double terceraArmonica = velocidad * KC * sin (3*integral);
        OutputSenal[0] = RelacionVF * (velocidad * sin(integral + 0) + terceraArmonica);
        OutputSenal[1] = RelacionVF * (velocidad * sin(integral - 2*PI_Num/3) + terceraArmonica);
        OutputSenal[2] = RelacionVF * (velocidad * sin(integral + 2*PI_Num/3) + terceraArmonica);
    } else {
        OutputSenal[0] = RelacionVF * velocidad * sin(integral + 0);
        OutputSenal[1] = RelacionVF * velocidad * sin(integral - 2*PI_Num/3);
        OutputSenal[2] = RelacionVF * velocidad * sin(integral + 2*PI_Num/3);
    }
}

/*
*   current_protection()
*   Protecion de sobrecorriente.
*/
void    current_protection(double currents[3], double overcurrent_limits)
{
    if (abs(currents[0]) > overcurrent_limits ||
        abs(currents[1]) > overcurrent_limits ||
        abs(currents[2]) > overcurrent_limits) {
        overcurrent = 1;
    }
}

//=================================================================================
//   SLOW BACKGROUND TASKS
//=================================================================================


//=================================================================================
//  Background Loop (executed in every 100 msec)
//=================================================================================

/*
*   BackgroundLoop1()
*   Función de background. Internamente, ejecuta el código únicamente cuando
*   finaliza el Timer 2 (100ms).
*   Toggle GPIO-34 (LED) para indicar que el DSP está vivo.
*/
void BackgroundLoop1(void)
//----------------------------------------
{
    if(CpuTimer2Regs.TCR.bit.TIF == 1)
    {
        CpuTimer2Regs.TCR.bit.TIF = 1;              // clear flag

        if(EPwm1Regs.TZFLG.bit.OST==0x1)            // TripZ for PWMs is low (fault trip)
        { TripFlagDMC=1;
        }

        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;     // Turn on/off LD3 on the controlCARD
    }

}


//=================================================================================
//  INTERRUPT ROUTINES
//=================================================================================

/*
*   MainISR()
*   Interrupt Service Routine principal, ejecución del control del motor
*/
interrupt void MainISR(void)
{
    int i=0;
    test=5;
// Verifying the ISR
    IsrTicker++;

    vhz1.SpFreq = SpeedRef;     //USER INPUT: Speed set-point for control function, as example.
    test = 1;
// ------------------------------------------------------------------------------
//    Call the Volt/hertz profile and ramp
// ------------------------------------------------------------------------------
    rampaVF();
// ------------------------------------------------------------------------------
//    ADC measurement calculation. Read AdcMirror registers and calculate phisical value.
// ------------------------------------------------------------------------------
    readAndProcessADCValues();
// ------------------------------------------------------------------------------
//    Call the Sinusoidal modulation
// ------------------------------------------------------------------------------
    double OutputSenalSenoidal[3];
    SinusoidalModulation(OutputSenalSenoidal);
// ------------------------------------------------------------------------------
//  Call the QEP macro (if incremental encoder used for speed sensing)
// ------------------------------------------------------------------------------
// Incluir los ficheros IQmathLib.h, IQmath.lib, f2833xqep.h
// Los ficheros de cabecera, como siempre, con los include.
// El fichero .lib: Project properties  Build  C2000 Linker  File Search Path
// El fichero f2833xqep.h facilita una estructura de datos con los inputs, parámetros y outputs de la macro que vamos a usar para obtener la velocidad.

// ------------------------------------------------------------------------------
//  Connect inputs to the PWM compare levels
// ------------------------------------------------------------------------------

    (*ePWM[1]).CMPA.half.CMPA = (OutputSenalSenoidal[0]+1)*7500; // Counter-Compare A Register - +1 = offset \ 7500 = PWMPeriod/SysClockPeriod
    (*ePWM[2]).CMPA.half.CMPA = (OutputSenalSenoidal[1]+1)*7500; // Counter-Compare A Register - +1 = offset \ 7500 = PWMPeriod/SysClockPeriod
    (*ePWM[3]).CMPA.half.CMPA = (OutputSenalSenoidal[2]+1)*7500; // Counter-Compare A Register - +1 = offset \ 7500 = PWMPeriod/SysClockPeriod
// ------------------------------------------------------------------------------
// CONTROL DE SOBRECORRIENTE - SI LA CORRIENTE > 5
// ------------------------------------------------------------------------------
    //llamada a la rutina que comprueba si hay o no sobrecorriente en cualquiera de las 3 fases
    current_protection(current, overcurrent_limit);
    //Si hay sobrecorriente abrimos todos

    if( overcurrent > 0 )
    {
        EALLOW;
        (*ePWM[1]).TZFRC.bit.OST = 1;
        (*ePWM[2]).TZFRC.bit.OST = 1;
        (*ePWM[3]).TZFRC.bit.OST = 1;
        EDIS;
    }
// ------------------------------------------------------------------------------
//    Connect inputs of the debug buffers and update buffers, for graph viewing
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//    Interrupt management
// ------------------------------------------------------------------------------

// Resetear el SEQ1 al estado CONV00
    AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
    AdcRegs.ADCTRL2.bit.RST_SEQ2=1;

// Limpiar el flag de la interrupción ADC SEQ1
    AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;
    AdcRegs.ADCST.bit.INT_SEQ2_CLR=1;

// Acknowledge interrupt to recieve more interrupts from PIE group 3
    PieCtrlRegs.PIEACK.bit.ACK6 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;


}   // ISR Ends Here




/*
*   HVDMC_Protection()
*   Función de protección para deterner los PWM ante una sobrecorriente
*   NO MODIFICAR
*/
void HVDMC_Protection(void)
{

// Configure Trip Mechanism for the Motor control software
// -Cycle by cycle trip on CPU halt
// -One shot IPM trip zone trip 
// These trips need to be repeated for EPWM1 ,2 & 3

//===========================================================================
//Motor Control Trip Config, EPwm1,2,3
//===========================================================================

      EALLOW;
// CPU Halt Trip  
      EPwm1Regs.TZSEL.bit.CBC6=0x1;
      EPwm2Regs.TZSEL.bit.CBC6=0x1;
      EPwm3Regs.TZSEL.bit.CBC6=0x1;
      
      
      EPwm1Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT  
      EPwm2Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT      
      EPwm3Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT

// What do we want the OST/CBC events to do?
// TZA events can force EPWMxA
// TZB events can force EPWMxB

      EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low 
      EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low
      
      EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low 
      EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low
      
      EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low 
      EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low
            
      EDIS;

     // Clear any spurious OV trip
      EPwm1Regs.TZCLR.bit.OST = 1;
      EPwm2Regs.TZCLR.bit.OST = 1;
      EPwm3Regs.TZCLR.bit.OST = 1;
      
}//End of protection configuration

//===========================================================================
// No more.
//===========================================================================
