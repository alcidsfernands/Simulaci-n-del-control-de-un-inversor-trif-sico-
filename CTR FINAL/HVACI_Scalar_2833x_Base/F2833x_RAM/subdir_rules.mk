################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DSP2833x_ADC_cal.obj: ../DSP2833x_ADC_cal.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/Drivers/f2833x_v2.0" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_common/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/developement_kits/F2833x_headers" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_headers/include" -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLOATING_MATH" --quiet --diag_warning=225 --preproc_with_compile --preproc_dependency="DSP2833x_ADC_cal.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

DSP2833x_GlobalVariableDefs.obj: ../DSP2833x_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/Drivers/f2833x_v2.0" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_common/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/developement_kits/F2833x_headers" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_headers/include" -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLOATING_MATH" --quiet --diag_warning=225 --preproc_with_compile --preproc_dependency="DSP2833x_GlobalVariableDefs.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

DSP2833x_usDelay.obj: ../DSP2833x_usDelay.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/Drivers/f2833x_v2.0" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_common/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/developement_kits/F2833x_headers" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_headers/include" -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLOATING_MATH" --quiet --diag_warning=225 --preproc_with_compile --preproc_dependency="DSP2833x_usDelay.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HVACI_Scalar-DevInit_F2833x.obj: ../HVACI_Scalar-DevInit_F2833x.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/Drivers/f2833x_v2.0" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_common/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/developement_kits/F2833x_headers" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_headers/include" -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLOATING_MATH" --quiet --diag_warning=225 --preproc_with_compile --preproc_dependency="HVACI_Scalar-DevInit_F2833x.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HVACI_Scalar.obj: ../HVACI_Scalar.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.6.LTS/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/Drivers/f2833x_v2.0" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_common/include" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/developement_kits/F2833x_headers" --include_path="C:/Users/Estudiante/Desktop/SLT_IMN_AF/LibreriaTI/device_support/DSP2833x_headers/include" -g --define="_DEBUG" --define="LARGE_MODEL" --define="FLOATING_MATH" --quiet --diag_warning=225 --preproc_with_compile --preproc_dependency="HVACI_Scalar.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


