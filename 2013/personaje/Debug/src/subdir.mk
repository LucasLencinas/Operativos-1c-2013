################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funcionesPersonaje.c \
../src/lectura_configuracion.c \
../src/mainPersonaje.c \
../src/nipc.c \
../src/serializar.c 

OBJS += \
./src/funcionesPersonaje.o \
./src/lectura_configuracion.o \
./src/mainPersonaje.o \
./src/nipc.o \
./src/serializar.o 

C_DEPS += \
./src/funcionesPersonaje.d \
./src/lectura_configuracion.d \
./src/mainPersonaje.d \
./src/nipc.d \
./src/serializar.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/so-commons-library" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


