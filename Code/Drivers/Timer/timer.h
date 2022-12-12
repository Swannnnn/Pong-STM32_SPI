#ifndef TIMER_HEADER
#define TIMER_HEADER

//includes
#include "stm32l1xx_hal.h"
#include "music.h"
#include "max7219.h"

#include <stdio.h>
#include <stdlib.h>

//structures
typedef enum {
	MUSIC = 0,
	SEGMENT = 1
}TIMER_Enum;

typedef struct {
	TIMER_Enum function_name;
	void (*interrupt_function)();
	uint32_t frequence;
}TypeDef_Timer_Callback;

typedef struct {
	TypeDef_Timer_Callback chosen_function;
	uint32_t timer_frequence;
	uint8_t timer_is_running;
	const TypeDef_Timer_Callback * callback_function;
	TIM_HandleTypeDef * htim;
}TypeDef_Timer_Handler;

//functions
void timer_interrupt(void);
HAL_StatusTypeDef timer_init(TypeDef_Timer_Handler * _timer_handler);
void set_interrupt_launcher(TIMER_Enum _chosen_function);
void start_timer(void);
void stop_timer(void);

#endif
