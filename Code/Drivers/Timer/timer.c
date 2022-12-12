#include "timer.h"


static TypeDef_Timer_Handler * timer_handler;


//init the list of callback function
static const TypeDef_Timer_Callback function_list[] = {
		{MUSIC, &play_music, 69},
		{SEGMENT, &callback_display, 499},
		//TODO
};

/**
 * If the timer is running, call the interrupt function
 */
void timer_interrupt(void) {

	if (timer_handler->timer_is_running == 1) {
		timer_handler->chosen_function.interrupt_function();
	}
}

/**
 * This function initializes the timer and sets the timer interrupt frequency to 100Hz
 * 
 * @param _timer_handler pointer to the timer handler
 * 
 * @return HAL_OK
 */
HAL_StatusTypeDef timer_init(TypeDef_Timer_Handler * _timer_handler) {

	timer_handler = _timer_handler;

	//start timer
	HAL_TIM_Base_Start_IT(timer_handler->htim);

	//init interrupt frequence
	timer_handler->htim->Instance->ARR = 100;

	timer_handler->callback_function = function_list;
	timer_handler->timer_frequence = 0;
	timer_handler->timer_is_running = 0;

	return HAL_OK;
}


/**
 * It sets the interrupt handler to the chosen function
 * 
 * @param _chosen_function the function you want to call
 */
void set_interrupt_launcher(TIMER_Enum _chosen_function) {
	timer_handler->chosen_function = timer_handler->callback_function[_chosen_function];
	timer_handler->htim->Instance->ARR = timer_handler->callback_function[_chosen_function].frequence;
}

/**
 * It sets the timer_is_running flag to 1
 */
void start_timer(void) { timer_handler->timer_is_running = 1; }

/**
 * > The function `stop_timer()` sets the `timer_is_running` flag to 0
 */
void stop_timer(void) { timer_handler->timer_is_running = 0; }



