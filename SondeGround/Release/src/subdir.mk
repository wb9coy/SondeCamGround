################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SX1278FSK.c \
../src/SondeGround.c \
../src/batt.c \
../src/berlekamp.c \
../src/callSign.c \
../src/crc16.c \
../src/dictionary.c \
../src/ecc.c \
../src/galois.c \
../src/gps.c \
../src/habInfo.c \
../src/imageFileManager.c \
../src/iniparser.c \
../src/logger.c \
../src/modem.c \
../src/queue.c \
../src/rs.c \
../src/rssi.c \
../src/rxThread.c \
../src/sensor.c \
../src/statusThread.c \
../src/timers.c \
../src/utils.c \
../src/webClientThread.c 

OBJS += \
./src/SX1278FSK.o \
./src/SondeGround.o \
./src/batt.o \
./src/berlekamp.o \
./src/callSign.o \
./src/crc16.o \
./src/dictionary.o \
./src/ecc.o \
./src/galois.o \
./src/gps.o \
./src/habInfo.o \
./src/imageFileManager.o \
./src/iniparser.o \
./src/logger.o \
./src/modem.o \
./src/queue.o \
./src/rs.o \
./src/rssi.o \
./src/rxThread.o \
./src/sensor.o \
./src/statusThread.o \
./src/timers.o \
./src/utils.o \
./src/webClientThread.o 

C_DEPS += \
./src/SX1278FSK.d \
./src/SondeGround.d \
./src/batt.d \
./src/berlekamp.d \
./src/callSign.d \
./src/crc16.d \
./src/dictionary.d \
./src/ecc.d \
./src/galois.d \
./src/gps.d \
./src/habInfo.d \
./src/imageFileManager.d \
./src/iniparser.d \
./src/logger.d \
./src/modem.d \
./src/queue.d \
./src/rs.d \
./src/rssi.d \
./src/rxThread.d \
./src/sensor.d \
./src/statusThread.d \
./src/timers.d \
./src/utils.d \
./src/webClientThread.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I../inc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


