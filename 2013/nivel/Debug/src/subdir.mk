################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funcionesNivel.c \
../src/lectura_config.c \
../src/mainNivel.c \
../src/nipc.c \
../src/nivel.c \
../src/serializar.c \
../src/tad_items.c 

OBJS += \
./src/funcionesNivel.o \
./src/lectura_config.o \
./src/mainNivel.o \
./src/nipc.o \
./src/nivel.o \
./src/serializar.o \
./src/tad_items.o 

C_DEPS += \
./src/funcionesNivel.d \
./src/lectura_config.d \
./src/mainNivel.d \
./src/nipc.d \
./src/nivel.d \
./src/serializar.d \
./src/tad_items.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/so-commons-library" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


