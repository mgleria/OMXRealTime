#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/OMXRealTime.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/OMXRealTime.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=Source/freeRTOS/Portable/MemMang/heap_1.c Source/freeRTOS/Portable/port.c Source/freeRTOS/Portable/portasm_PIC24.S Source/freeRTOS/croutine.c Source/freeRTOS/event_groups.c Source/freeRTOS/list.c Source/freeRTOS/queue.c Source/freeRTOS/tasks.c Source/freeRTOS/timers.c Source/funciones/shell.c mcc_generated_files/interrupt_manager.c mcc_generated_files/traps.c mcc_generated_files/tmr2.c mcc_generated_files/pin_manager.c mcc_generated_files/uart1.c mcc_generated_files/mcc.c mcc_generated_files/i2c1.c Source/perifericos/adc.c Source/perifericos/lcd.c Source/perifericos/lcd_driver.c Source/perifericos/led.c Source/sistema/ext_rtcc.c Source/main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o ${OBJECTDIR}/Source/freeRTOS/Portable/port.o ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o ${OBJECTDIR}/Source/freeRTOS/croutine.o ${OBJECTDIR}/Source/freeRTOS/event_groups.o ${OBJECTDIR}/Source/freeRTOS/list.o ${OBJECTDIR}/Source/freeRTOS/queue.o ${OBJECTDIR}/Source/freeRTOS/tasks.o ${OBJECTDIR}/Source/freeRTOS/timers.o ${OBJECTDIR}/Source/funciones/shell.o ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o ${OBJECTDIR}/mcc_generated_files/traps.o ${OBJECTDIR}/mcc_generated_files/tmr2.o ${OBJECTDIR}/mcc_generated_files/pin_manager.o ${OBJECTDIR}/mcc_generated_files/uart1.o ${OBJECTDIR}/mcc_generated_files/mcc.o ${OBJECTDIR}/mcc_generated_files/i2c1.o ${OBJECTDIR}/Source/perifericos/adc.o ${OBJECTDIR}/Source/perifericos/lcd.o ${OBJECTDIR}/Source/perifericos/lcd_driver.o ${OBJECTDIR}/Source/perifericos/led.o ${OBJECTDIR}/Source/sistema/ext_rtcc.o ${OBJECTDIR}/Source/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o.d ${OBJECTDIR}/Source/freeRTOS/Portable/port.o.d ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.d ${OBJECTDIR}/Source/freeRTOS/croutine.o.d ${OBJECTDIR}/Source/freeRTOS/event_groups.o.d ${OBJECTDIR}/Source/freeRTOS/list.o.d ${OBJECTDIR}/Source/freeRTOS/queue.o.d ${OBJECTDIR}/Source/freeRTOS/tasks.o.d ${OBJECTDIR}/Source/freeRTOS/timers.o.d ${OBJECTDIR}/Source/funciones/shell.o.d ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d ${OBJECTDIR}/mcc_generated_files/traps.o.d ${OBJECTDIR}/mcc_generated_files/tmr2.o.d ${OBJECTDIR}/mcc_generated_files/pin_manager.o.d ${OBJECTDIR}/mcc_generated_files/uart1.o.d ${OBJECTDIR}/mcc_generated_files/mcc.o.d ${OBJECTDIR}/mcc_generated_files/i2c1.o.d ${OBJECTDIR}/Source/perifericos/adc.o.d ${OBJECTDIR}/Source/perifericos/lcd.o.d ${OBJECTDIR}/Source/perifericos/lcd_driver.o.d ${OBJECTDIR}/Source/perifericos/led.o.d ${OBJECTDIR}/Source/sistema/ext_rtcc.o.d ${OBJECTDIR}/Source/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o ${OBJECTDIR}/Source/freeRTOS/Portable/port.o ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o ${OBJECTDIR}/Source/freeRTOS/croutine.o ${OBJECTDIR}/Source/freeRTOS/event_groups.o ${OBJECTDIR}/Source/freeRTOS/list.o ${OBJECTDIR}/Source/freeRTOS/queue.o ${OBJECTDIR}/Source/freeRTOS/tasks.o ${OBJECTDIR}/Source/freeRTOS/timers.o ${OBJECTDIR}/Source/funciones/shell.o ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o ${OBJECTDIR}/mcc_generated_files/traps.o ${OBJECTDIR}/mcc_generated_files/tmr2.o ${OBJECTDIR}/mcc_generated_files/pin_manager.o ${OBJECTDIR}/mcc_generated_files/uart1.o ${OBJECTDIR}/mcc_generated_files/mcc.o ${OBJECTDIR}/mcc_generated_files/i2c1.o ${OBJECTDIR}/Source/perifericos/adc.o ${OBJECTDIR}/Source/perifericos/lcd.o ${OBJECTDIR}/Source/perifericos/lcd_driver.o ${OBJECTDIR}/Source/perifericos/led.o ${OBJECTDIR}/Source/sistema/ext_rtcc.o ${OBJECTDIR}/Source/main.o

# Source Files
SOURCEFILES=Source/freeRTOS/Portable/MemMang/heap_1.c Source/freeRTOS/Portable/port.c Source/freeRTOS/Portable/portasm_PIC24.S Source/freeRTOS/croutine.c Source/freeRTOS/event_groups.c Source/freeRTOS/list.c Source/freeRTOS/queue.c Source/freeRTOS/tasks.c Source/freeRTOS/timers.c Source/funciones/shell.c mcc_generated_files/interrupt_manager.c mcc_generated_files/traps.c mcc_generated_files/tmr2.c mcc_generated_files/pin_manager.c mcc_generated_files/uart1.c mcc_generated_files/mcc.c mcc_generated_files/i2c1.c Source/perifericos/adc.c Source/perifericos/lcd.c Source/perifericos/lcd_driver.c Source/perifericos/led.c Source/sistema/ext_rtcc.c Source/main.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/OMXRealTime.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ1024GB610
MP_LINKER_FILE_OPTION=,--script=p24FJ1024GB610.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o: Source/freeRTOS/Portable/MemMang/heap_1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS/Portable/MemMang" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/Portable/MemMang/heap_1.c  -o ${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/Portable/port.o: Source/freeRTOS/Portable/port.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS/Portable" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/port.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/port.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/Portable/port.c  -o ${OBJECTDIR}/Source/freeRTOS/Portable/port.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/Portable/port.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/Portable/port.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/croutine.o: Source/freeRTOS/croutine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/croutine.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/croutine.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/croutine.c  -o ${OBJECTDIR}/Source/freeRTOS/croutine.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/croutine.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/croutine.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/event_groups.o: Source/freeRTOS/event_groups.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/event_groups.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/event_groups.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/event_groups.c  -o ${OBJECTDIR}/Source/freeRTOS/event_groups.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/event_groups.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/event_groups.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/list.o: Source/freeRTOS/list.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/list.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/list.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/list.c  -o ${OBJECTDIR}/Source/freeRTOS/list.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/list.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/list.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/queue.o: Source/freeRTOS/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/queue.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/queue.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/queue.c  -o ${OBJECTDIR}/Source/freeRTOS/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/queue.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/queue.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/tasks.o: Source/freeRTOS/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/tasks.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/tasks.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/tasks.c  -o ${OBJECTDIR}/Source/freeRTOS/tasks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/tasks.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/tasks.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/timers.o: Source/freeRTOS/timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/timers.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/timers.c  -o ${OBJECTDIR}/Source/freeRTOS/timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/timers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/funciones/shell.o: Source/funciones/shell.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/funciones" 
	@${RM} ${OBJECTDIR}/Source/funciones/shell.o.d 
	@${RM} ${OBJECTDIR}/Source/funciones/shell.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/funciones/shell.c  -o ${OBJECTDIR}/Source/funciones/shell.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/funciones/shell.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/funciones/shell.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/interrupt_manager.o: mcc_generated_files/interrupt_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/interrupt_manager.c  -o ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/traps.o: mcc_generated_files/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/traps.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/traps.c  -o ${OBJECTDIR}/mcc_generated_files/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/tmr2.o: mcc_generated_files/tmr2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/tmr2.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/tmr2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/tmr2.c  -o ${OBJECTDIR}/mcc_generated_files/tmr2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/tmr2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/tmr2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/pin_manager.o: mcc_generated_files/pin_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pin_manager.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pin_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/pin_manager.c  -o ${OBJECTDIR}/mcc_generated_files/pin_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/pin_manager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/pin_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/uart1.o: mcc_generated_files/uart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/uart1.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/uart1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/uart1.c  -o ${OBJECTDIR}/mcc_generated_files/uart1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/uart1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/uart1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/mcc.o: mcc_generated_files/mcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/mcc.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/mcc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/mcc.c  -o ${OBJECTDIR}/mcc_generated_files/mcc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/mcc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/mcc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/i2c1.o: mcc_generated_files/i2c1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/i2c1.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/i2c1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/i2c1.c  -o ${OBJECTDIR}/mcc_generated_files/i2c1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/i2c1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/i2c1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/perifericos/adc.o: Source/perifericos/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/perifericos" 
	@${RM} ${OBJECTDIR}/Source/perifericos/adc.o.d 
	@${RM} ${OBJECTDIR}/Source/perifericos/adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/perifericos/adc.c  -o ${OBJECTDIR}/Source/perifericos/adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/perifericos/adc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/perifericos/adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/perifericos/lcd.o: Source/perifericos/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/perifericos" 
	@${RM} ${OBJECTDIR}/Source/perifericos/lcd.o.d 
	@${RM} ${OBJECTDIR}/Source/perifericos/lcd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/perifericos/lcd.c  -o ${OBJECTDIR}/Source/perifericos/lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/perifericos/lcd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/perifericos/lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/perifericos/lcd_driver.o: Source/perifericos/lcd_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/perifericos" 
	@${RM} ${OBJECTDIR}/Source/perifericos/lcd_driver.o.d 
	@${RM} ${OBJECTDIR}/Source/perifericos/lcd_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/perifericos/lcd_driver.c  -o ${OBJECTDIR}/Source/perifericos/lcd_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/perifericos/lcd_driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/perifericos/lcd_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/perifericos/led.o: Source/perifericos/led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/perifericos" 
	@${RM} ${OBJECTDIR}/Source/perifericos/led.o.d 
	@${RM} ${OBJECTDIR}/Source/perifericos/led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/perifericos/led.c  -o ${OBJECTDIR}/Source/perifericos/led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/perifericos/led.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/perifericos/led.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/sistema/ext_rtcc.o: Source/sistema/ext_rtcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/sistema" 
	@${RM} ${OBJECTDIR}/Source/sistema/ext_rtcc.o.d 
	@${RM} ${OBJECTDIR}/Source/sistema/ext_rtcc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/sistema/ext_rtcc.c  -o ${OBJECTDIR}/Source/sistema/ext_rtcc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/sistema/ext_rtcc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/sistema/ext_rtcc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/main.o: Source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source" 
	@${RM} ${OBJECTDIR}/Source/main.o.d 
	@${RM} ${OBJECTDIR}/Source/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/main.c  -o ${OBJECTDIR}/Source/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o: Source/freeRTOS/Portable/MemMang/heap_1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS/Portable/MemMang" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/Portable/MemMang/heap_1.c  -o ${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/Portable/MemMang/heap_1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/Portable/port.o: Source/freeRTOS/Portable/port.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS/Portable" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/port.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/port.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/Portable/port.c  -o ${OBJECTDIR}/Source/freeRTOS/Portable/port.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/Portable/port.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/Portable/port.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/croutine.o: Source/freeRTOS/croutine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/croutine.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/croutine.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/croutine.c  -o ${OBJECTDIR}/Source/freeRTOS/croutine.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/croutine.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/croutine.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/event_groups.o: Source/freeRTOS/event_groups.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/event_groups.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/event_groups.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/event_groups.c  -o ${OBJECTDIR}/Source/freeRTOS/event_groups.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/event_groups.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/event_groups.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/list.o: Source/freeRTOS/list.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/list.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/list.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/list.c  -o ${OBJECTDIR}/Source/freeRTOS/list.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/list.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/list.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/queue.o: Source/freeRTOS/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/queue.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/queue.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/queue.c  -o ${OBJECTDIR}/Source/freeRTOS/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/queue.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/queue.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/tasks.o: Source/freeRTOS/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/tasks.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/tasks.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/tasks.c  -o ${OBJECTDIR}/Source/freeRTOS/tasks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/tasks.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/tasks.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/freeRTOS/timers.o: Source/freeRTOS/timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/timers.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/freeRTOS/timers.c  -o ${OBJECTDIR}/Source/freeRTOS/timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/timers.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/funciones/shell.o: Source/funciones/shell.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/funciones" 
	@${RM} ${OBJECTDIR}/Source/funciones/shell.o.d 
	@${RM} ${OBJECTDIR}/Source/funciones/shell.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/funciones/shell.c  -o ${OBJECTDIR}/Source/funciones/shell.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/funciones/shell.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/funciones/shell.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/interrupt_manager.o: mcc_generated_files/interrupt_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/interrupt_manager.c  -o ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/traps.o: mcc_generated_files/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/traps.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/traps.c  -o ${OBJECTDIR}/mcc_generated_files/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/traps.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/tmr2.o: mcc_generated_files/tmr2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/tmr2.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/tmr2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/tmr2.c  -o ${OBJECTDIR}/mcc_generated_files/tmr2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/tmr2.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/tmr2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/pin_manager.o: mcc_generated_files/pin_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pin_manager.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pin_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/pin_manager.c  -o ${OBJECTDIR}/mcc_generated_files/pin_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/pin_manager.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/pin_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/uart1.o: mcc_generated_files/uart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/uart1.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/uart1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/uart1.c  -o ${OBJECTDIR}/mcc_generated_files/uart1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/uart1.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/uart1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/mcc.o: mcc_generated_files/mcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/mcc.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/mcc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/mcc.c  -o ${OBJECTDIR}/mcc_generated_files/mcc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/mcc.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/mcc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/i2c1.o: mcc_generated_files/i2c1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/i2c1.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/i2c1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/i2c1.c  -o ${OBJECTDIR}/mcc_generated_files/i2c1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/i2c1.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/i2c1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/perifericos/adc.o: Source/perifericos/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/perifericos" 
	@${RM} ${OBJECTDIR}/Source/perifericos/adc.o.d 
	@${RM} ${OBJECTDIR}/Source/perifericos/adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/perifericos/adc.c  -o ${OBJECTDIR}/Source/perifericos/adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/perifericos/adc.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/perifericos/adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/perifericos/lcd.o: Source/perifericos/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/perifericos" 
	@${RM} ${OBJECTDIR}/Source/perifericos/lcd.o.d 
	@${RM} ${OBJECTDIR}/Source/perifericos/lcd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/perifericos/lcd.c  -o ${OBJECTDIR}/Source/perifericos/lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/perifericos/lcd.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/perifericos/lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/perifericos/lcd_driver.o: Source/perifericos/lcd_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/perifericos" 
	@${RM} ${OBJECTDIR}/Source/perifericos/lcd_driver.o.d 
	@${RM} ${OBJECTDIR}/Source/perifericos/lcd_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/perifericos/lcd_driver.c  -o ${OBJECTDIR}/Source/perifericos/lcd_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/perifericos/lcd_driver.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/perifericos/lcd_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/perifericos/led.o: Source/perifericos/led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/perifericos" 
	@${RM} ${OBJECTDIR}/Source/perifericos/led.o.d 
	@${RM} ${OBJECTDIR}/Source/perifericos/led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/perifericos/led.c  -o ${OBJECTDIR}/Source/perifericos/led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/perifericos/led.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/perifericos/led.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/sistema/ext_rtcc.o: Source/sistema/ext_rtcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/sistema" 
	@${RM} ${OBJECTDIR}/Source/sistema/ext_rtcc.o.d 
	@${RM} ${OBJECTDIR}/Source/sistema/ext_rtcc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/sistema/ext_rtcc.c  -o ${OBJECTDIR}/Source/sistema/ext_rtcc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/sistema/ext_rtcc.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/sistema/ext_rtcc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Source/main.o: Source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source" 
	@${RM} ${OBJECTDIR}/Source/main.o.d 
	@${RM} ${OBJECTDIR}/Source/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Source/main.c  -o ${OBJECTDIR}/Source/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/main.o.d"      -mno-eds-warn  -g -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Source/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o: Source/freeRTOS/Portable/portasm_PIC24.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS/Portable" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  Source/freeRTOS/Portable/portasm_PIC24.S  -o ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.d"  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -Wa,-MD,"${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.d" "${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o: Source/freeRTOS/Portable/portasm_PIC24.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Source/freeRTOS/Portable" 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.d 
	@${RM} ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  Source/freeRTOS/Portable/portasm_PIC24.S  -o ${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.d"  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -Wa,-MD,"${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.d" "${OBJECTDIR}/Source/freeRTOS/Portable/portasm_PIC24.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/OMXRealTime.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/OMXRealTime.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files"  -mreserve=data@0x800:0x81B -mreserve=data@0x81C:0x81D -mreserve=data@0x81E:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x827 -mreserve=data@0x82A:0x84F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/OMXRealTime.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/OMXRealTime.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -D__OMX_S__ -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -I"include" -I"include/freeRTOS" -I"mcc_generated_files" -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/OMXRealTime.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
