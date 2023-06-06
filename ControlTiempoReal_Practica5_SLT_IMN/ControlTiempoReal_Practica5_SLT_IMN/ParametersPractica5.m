clear all;
clc;
mex Moduladora.c;

%% Parametros Simulacion
StepTime = 1.6e-6;
SimulationTime = 2;
ConstanteTiempo = 159e-6;

%% Motor
P = 186.5;
Vff = 220;
FrecFundamental =  60;
VelocidadMotor = 1800;
ParPolos = 2;
Rs = 11.05;
Ls = 3.164e-3;
Rr = 6.11;
Lr = Ls;
Lm = 293.94e-3;
Inercia = 0.0005;
Tm = 0.05;

%% Consigna Velocidad
velocidadInicial=600/VelocidadMotor;
velocidadStepTime = 1;
velocidadFinal = 850/VelocidadMotor;

%% Inversor
Vdc = 311;
Tconmut = 200e-6;
FrecConmut=1/Tconmut;
RampaAceleracion = 500/0.2/VelocidadMotor;
TiempoMuerto = 4e-6;
Kc=1/6;

%% DSP
NBits = 12;
OffSet = 0.1;
PWMSelect = -1; % Pot <= 0, DSP > 0

%Control
Kp = 5;
Ki = 150;
Kd = 0;
Control = -1; % Control PI <= 0, Control Step > 0
Activar3Armonico = 1; % 3Armonico Activo > 0, 3Armonico Desativado <= 0
OverCurrentLimit = 10;
VffPU = Vff/Vdc/0.6124;
if (Activar3Armonico > 0)
    limiteSuperior = 1.154;
    if (VffPU > limiteSuperior)
        VffPU = limiteSuperior;
    end
else
    limiteSuperior = 1;
    if (VffPU > limiteSuperior)
        VffPU = limiteSuperior;
    end
end
RelacionVF = (VffPU-0)/(1-0);
PI_Num = 3.14159265359;