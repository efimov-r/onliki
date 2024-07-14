################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/render/render.cpp 

CPP_DEPS += \
./src/render/render.d 

OBJS += \
./src/render/render.o 


# Each subdirectory must supply rules for building sources it contributes
src/render/%.o: ../src/render/%.cpp src/render/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-render

clean-src-2f-render:
	-$(RM) ./src/render/render.d ./src/render/render.o

.PHONY: clean-src-2f-render

