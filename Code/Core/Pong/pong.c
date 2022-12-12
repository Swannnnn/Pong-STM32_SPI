#include "pong.h"

// Stores hardware peripherals
static Pong_Handle_TypeDef *pong_handle = NULL;

// Stores FSM
static FSM_Handle_TypeDef *fsm_handle = NULL;

// FSM enum and callbacks
static FSM_State_TypeDef states_list[] = {
	{STATE_START, &state_start},
	{STATE_WPP1, &state_wpp1},
	{STATE_WPP2, &state_wpp2},
	{STATE_GTP1, &state_gtp1},
	{STATE_GTP2, &state_gtp2},
	{STATE_RPP1, &state_rpp1},
	{STATE_RPP2, &state_rpp2},
	{STATE_IP1S, &state_ip1s},
	{STATE_IP2S, &state_ip2s},
	{STATE_P1WN, &state_p1wn},
	{STATE_P2WN, &state_p2wn},
};

/**
 * @brief Set new FSM state
 * @param _new_state Enum member representing desired state.
 * It will check if desired state is contained in states list.
 */
static void set_new_state(FSM_State_Enum _new_state)
{
	// Check if desired state is contained in states array
	if ((_new_state >= 0) && (_new_state < fsm_handle->states_list_sz))
	{
		// Set new FSM state UID & callback
		fsm_handle->state = fsm_handle->states_list[_new_state];

		// Reset execution count for variables initializations
		fsm_handle->controllers.state_execution_count = 0;

		// Reset animation state
		fsm_handle->controllers.animation_state = ANIMATION_RUNNING;

		// Reset base time for animations
		fsm_handle->controllers.state_base_time = HAL_GetTick();

		//Reset button pushed counter
		fsm_handle->inputs.nb_press_btn1 = 0;
		fsm_handle->inputs.nb_press_btn2 = 0;

		//stop the timer loop
		stop_timer();
	}
}

/**
 * @brief Initialize pong game
 * @param _pong_handle Handle to pong peripherals
 * @param _fsm_handle Handle to Pong FSM
 * @retval HAL status
 */
HAL_StatusTypeDef pong_init(Pong_Handle_TypeDef *_pong_handle, FSM_Handle_TypeDef *_fsm_handle)
{
	HAL_StatusTypeDef max7219_status = HAL_OK;
	HAL_StatusTypeDef led_arrray_status = HAL_OK;
	HAL_StatusTypeDef music_status = HAL_OK;
	HAL_StatusTypeDef timer_status = HAL_OK;

	/* Attribute input parameters */
	pong_handle = _pong_handle;
	fsm_handle = _fsm_handle;

	/* Check input parameters */
	CHECK_PONG_PARAMS();

	/* Init hardware peripherals */

	led_arrray_status = led_array_init(&pong_handle->led_array);

	max7219_status = max7219_init(&pong_handle->max7219_handle);

	music_status = init_music(&pong_handle->music_handler);

	timer_status = timer_init(&pong_handle->timer_handler);

	/* CHECK HARDWARE INIT BEGIN  ----------------------------------------------------------------------------------*/

	if (max7219_status != HAL_OK)
		return max7219_status;

	else if(led_arrray_status != HAL_OK)
		return led_arrray_status;

	else if(music_status != HAL_OK)
		return music_status;

	else if(timer_status != HAL_OK)
			return timer_status;

	/* CHECK HARDWARE INIT END  ----------------------------------------------------------------------------------*/

	/* Init FSM */
	fsm_handle->states_list = states_list;
	fsm_handle->states_list_sz = sizeof(states_list) / sizeof(FSM_State_TypeDef);
	set_new_state(STATE_START);


	return HAL_OK;
}

/**
 * @brief Run pong game, execute FSM callback and check for transition
 * @retval HAL status
 */
HAL_StatusTypeDef pong_run(void)
{
	CHECK_PONG_PARAMS();

	/* RUN STATE */
	// Call associated callback
	fsm_handle->state.state_callback();

	// Increase execution count
	fsm_handle->controllers.state_execution_count += 1;

	/* CHECK TRANSITION */
	switch (fsm_handle->state.state)
	{

	/* Start state */
	case STATE_START:

		//change state at the end of the music animation
		if (fsm_handle->controllers.animation_state == ANIMATION_ENDED)
			set_new_state(STATE_WPP1);
		break;

	/* "Wait for P1 to press PB1" state */
	case STATE_WPP1:

		//change state when player 1 push his button
		if (fsm_handle->inputs.nb_press_btn1 >= 1)
			set_new_state(STATE_GTP2);

		break;

	/* "Wait for P2 to press PB2" state */
	case STATE_WPP2:

		//change state when player 1 push his button
		if (fsm_handle->inputs.nb_press_btn2 >= 1)
			set_new_state(STATE_GTP1);

		break;

	/* "Go to P1" state */
	case STATE_GTP1:

		//change state if player 2 push the button before the led went tho the right border
		if (fsm_handle->inputs.nb_press_btn1 >=1 && fsm_handle->controllers.led_index < 7)
			set_new_state(STATE_IP2S);
		//change state if led touch the right border
		else if (fsm_handle->controllers.led_index > 7)
			set_new_state(STATE_RPP1);

		break;

	/* "Go to P2" state */
	case STATE_GTP2:

		//change state if player 2 push the button before the led went tho the right border
		if (fsm_handle->inputs.nb_press_btn2 >=1 && fsm_handle->controllers.led_index > 0)
			set_new_state(STATE_IP1S);
		//change state if led touch the right border
		else if (fsm_handle->controllers.led_index < 0)
			set_new_state(STATE_RPP2);

		break;

	/* "Reflex press P1" state */
	case STATE_RPP1:
		//wait the led_shift_period before to test if the player pushed the button
		if (fsm_handle->controllers.state_execution_count == fsm_handle->controllers.led_shift_period) {
			//check if the player pushed his button
			if (fsm_handle->inputs.nb_press_btn1 >= 1)
				set_new_state(STATE_GTP2);
			else
				set_new_state(STATE_IP2S);
		}
		break;

	/* "Reflex press P2" state */
	case STATE_RPP2:
		//wait the led_shift_period before to test if the player pushed the button
		if (fsm_handle->controllers.state_execution_count == fsm_handle->controllers.led_shift_period) {
			//check if the player pushed his button
			if (fsm_handle->inputs.nb_press_btn2 >= 1)
				set_new_state(STATE_GTP1);
			else
				set_new_state(STATE_IP1S);
		}
		break;

	/* "Increase P1 score" state */
	case STATE_IP1S:

		//wait the duration of the score display
		if (fsm_handle->controllers.state_execution_count == 500000) {
			//check if the player won
			if (fsm_handle->controllers.p1_score >= MAX_SCORE)
				set_new_state(STATE_P1WN);
			else
				set_new_state(STATE_WPP2);
		}

		break;

	/* "Increase P2 score" state */
	case STATE_IP2S:

		//wait the duration of the score display
		if (fsm_handle->controllers.state_execution_count == 500000) {
			//check if the player won
			if (fsm_handle->controllers.p2_score >= MAX_SCORE)
				set_new_state(STATE_P2WN);
			else
				set_new_state(STATE_WPP1);
		}

		break;

	/* "P1 Wins !" state */
	case STATE_P1WN:

		//wait until any button is pressed
		if (fsm_handle->inputs.nb_press_btn1 >= 1 || fsm_handle->inputs.nb_press_btn2 >= 1)
			set_new_state(STATE_WPP2);

		break;

	/* "P1 Wins !" state */
	case STATE_P2WN:

		//wait until any button is pressed
		if (fsm_handle->inputs.nb_press_btn1 >= 1 || fsm_handle->inputs.nb_press_btn2 >= 1)
			set_new_state(STATE_WPP1);

		break;

	default:
		set_new_state(STATE_START);
		break;
	}

	return HAL_OK;
}

void state_start(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init variable declaration
	static uint32_t buffer7, cnt;

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();

		//init the timer buffer7
		buffer7 = fsm_handle->controllers.state_execution_count;

		//init temp counter of the animation
		cnt = 0;

		//reset scores
		fsm_handle->controllers.p1_score = 0;
		fsm_handle->controllers.p2_score = 0;

		//reset pass count
		fsm_handle->controllers.pass_count = 0;

		//set music
		set_music(PACMAN);
		//set_7segment(" P1 ", 1);

		set_interrupt_launcher(MUSIC);
		start_timer();
	}

	/* INIT END  ----------------------------------------------------------------------------------*/

	/* ANIMATION BEGIN  ----------------------------------------------------------------------------------*/

	/**
	 * I would like to use stdarg.h and set set_interrupt_launcher as a multiple args
	 *  function to be able to call it with one or several callback functions.
	 *  But I didn't find the solves to my issues during the code writing
	 */

	//check if the animation state is on
	if (fsm_handle->controllers.animation_state == ANIMATION_RUNNING) {

		/* 7SEGMENT BEGIN  ----------------------------------------------------------------------------------*/

		//check if the last animation update happened over than 20000 cycles
		if (fsm_handle->controllers.state_execution_count-buffer7>80000) {

			//display the message with alternating between nothing and the display
			static uint8_t display_state = 0;

			/* Checking if the counter is equal to 6. If it is, it sets the animation state to ANIMATION_ENDED. */
			if (cnt == 6) {
				fsm_handle->controllers.animation_state = ANIMATION_ENDED;
			}


			/* Displaying the word "HOLA" on the 7-segment display. */
			if (display_state == 0) {
				display_on_7segments("HOLA");
				display_state=1;
				cnt++;
			}
			else {
				max7219_erase_no_decode();
				display_state=0;
			}


			//the buffer7 is actualised
			buffer7=fsm_handle->controllers.state_execution_count;
		}

		/* 7SEGMENT END  ----------------------------------------------------------------------------------*/

	}

	/* ANIMATION END  ----------------------------------------------------------------------------------*/

}

void state_wpp1(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init variables
	///static uint32_t buffer7;

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();

		//set 7segment display
		set_7segment(" P1 ", 1);

		//set the callback function of the timer
		set_interrupt_launcher(SEGMENT);

		//start the timer
		start_timer();

	}

	/* INIT END  ----------------------------------------------------------------------------------*/
}

void state_wpp2(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init variables
	///static uint32_t buffer7;

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();


		//set the 7segment display
		set_7segment(" P2 ", 1);

		//set the callback function of the timer
		set_interrupt_launcher(SEGMENT);

		//start the timer
		start_timer();

	}

	/* INIT END  ----------------------------------------------------------------------------------*/
}

void state_gtp1(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

		//init variables
		static uint32_t bufferL;

		//init functions of the state
		if (fsm_handle->controllers.state_execution_count == 0) {

			//clean the 7segments
			max7219_erase_no_decode();

			//init the timer bufferof the leds
			bufferL = fsm_handle->controllers.state_execution_count;

			//clear the leds
			clear_array();
			write_array(1, 1);

			//set the led shift period to a variable which increase on each pass
			fsm_handle->controllers.led_shift_period = 80000 - (fsm_handle->controllers.pass_count * 5000);

			//set the start led on the right border
			fsm_handle->controllers.led_index = 2;

			/*
			//set music and start the timer
			set_music(P1_REFLEXE);
			set_interrupt_launcher(MUSIC);
			start_timer();
*/
		}

		/* INIT END  ----------------------------------------------------------------------------------*/

		/* ANIMATION BEGIN  ----------------------------------------------------------------------------------*/

		//check if the animation state is on
		if (fsm_handle->controllers.animation_state == ANIMATION_RUNNING) {

			/* LED BEGIN  ----------------------------------------------------------------------------------*/

			//check if the last animation update happened over than a certain amount of cycles
			if (fsm_handle->controllers.state_execution_count-bufferL > fsm_handle->controllers.led_shift_period) {

				/* Incrementing the led_index by 1. */
				clear_array();
				write_array(fsm_handle->controllers.led_index, 1);
				fsm_handle->controllers.led_index++;

				bufferL=fsm_handle->controllers.state_execution_count;
			}

			/* LED END  ----------------------------------------------------------------------------------*/

		}

		/* ANIMATION END  ----------------------------------------------------------------------------------*/
}

void state_gtp2(void)
{

	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init variables
	static uint32_t bufferL;

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();

		//reinit the timer bufferof the leds
		bufferL = fsm_handle->controllers.state_execution_count;

		//clear the leds
		clear_array();
		write_array(6, 1);

		//set the led shift period to a variable which increase on each pass
		fsm_handle->controllers.led_shift_period = 80000 - (fsm_handle->controllers.pass_count * 5000);

		//set the start led on the left border
		fsm_handle->controllers.led_index = 5;

		/*
		//set music and start the timer
		set_music(P2_REFLEXE);
		set_interrupt_launcher(MUSIC);
		start_timer();
		*/
	}

	/* INIT END  ----------------------------------------------------------------------------------*/

	/* ANIMATION BEGIN  ----------------------------------------------------------------------------------*/

	//check if the animation state is on
	if (fsm_handle->controllers.animation_state == ANIMATION_RUNNING) {

		/* LED BEGIN  ----------------------------------------------------------------------------------*/

		//check if the last animation update happened over than a certain amount of cycles
		if (fsm_handle->controllers.state_execution_count-bufferL > fsm_handle->controllers.led_shift_period) {

			clear_array();
			write_array(fsm_handle->controllers.led_index, 1);
			fsm_handle->controllers.led_index--;

			bufferL=fsm_handle->controllers.state_execution_count;
		}

		/* LED END  ----------------------------------------------------------------------------------*/

	}

	/* ANIMATION END  ----------------------------------------------------------------------------------*/

}

void state_rpp1(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();

		//clear the leds
		clear_array();

		//switch on the led border
		write_array(7, 1);

		//increment the speed
		fsm_handle->controllers.pass_count++;
	}

	/* INIT END  ----------------------------------------------------------------------------------*/
}

void state_rpp2(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();

		//clear the leds
		clear_array();

		//switch on the led border
		write_array(0, 1);

		//increment the speed
		fsm_handle->controllers.pass_count++;
	}

	/* INIT END  ----------------------------------------------------------------------------------*/
}

void state_ip1s(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();

		//clear the leds
		clear_array();

		//increment player's score
		fsm_handle->controllers.p1_score++;

		//reset pass count
		fsm_handle->controllers.pass_count = 0;

		//display the new score of the winner
		max7219_display_no_decode(0, 0b1100111);
		max7219_display_no_decode(1, 0b0110000);
		max7219_display_no_decode(2, 0b1001);
		switch (fsm_handle->controllers.p1_score) {
		case 1: max7219_display_no_decode(3, 0b0110000); break;
		case 2: max7219_display_no_decode(3, 0b1101101); break;
		case 3: max7219_display_no_decode(3, 0b1111001); break;
		case 4: max7219_display_no_decode(3, 0b0110011); break;
		case 5: max7219_display_no_decode(3, 0b1011011); break;
		}

	}

	/* INIT END  ----------------------------------------------------------------------------------*/
}

void state_ip2s(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();
		max7219_erase_decode();

		//clear the leds
		clear_array();

		//increment player's score
		fsm_handle->controllers.p2_score++;

		//reset pass count
		fsm_handle->controllers.pass_count = 0;


		//display the new score of the winner
		max7219_display_no_decode(0, 0b1100111);
		max7219_display_no_decode(1, 0b1101101);
		max7219_display_no_decode(2, 0b1001);
		switch (fsm_handle->controllers.p2_score) {
		case 1: max7219_display_no_decode(3, 0b0110000); break;
		case 2: max7219_display_no_decode(3, 0b1101101); break;
		case 3: max7219_display_no_decode(3, 0b1111001); break;
		case 4: max7219_display_no_decode(3, 0b0110011); break;
		case 5: max7219_display_no_decode(3, 0b1011011); break;
		}

	}

	/* INIT END  ----------------------------------------------------------------------------------*/
}

void state_p1wn(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init variables
	static uint32_t buffer7;

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();

		//clear the leds
		clear_array();

		//init the timer buffer7
		buffer7 = fsm_handle->controllers.state_execution_count;

		//reset scores
		fsm_handle->controllers.p1_score = 0;
		fsm_handle->controllers.p2_score = 0;

		//reset pass count
		fsm_handle->controllers.pass_count = 0;

		/* Setting the music to play, and then it is starting the timer. */
		set_music(WIN);
		set_interrupt_launcher(MUSIC);
		start_timer();
	}

	/* INIT END  ----------------------------------------------------------------------------------*/

	/* ANIMATION BEGIN  ----------------------------------------------------------------------------------*/

	//check if the animation state is on
	if (fsm_handle->controllers.animation_state == ANIMATION_RUNNING) {

		/* 7SEGMENT BEGIN  ----------------------------------------------------------------------------------*/

		//check if the last animation update happened over than 20000 cycles
		if (fsm_handle->controllers.state_execution_count-buffer7>80000) {

			max7219_erase_no_decode();

			static uint8_t shift_state = 0;

			//display a message with shifting the letter in order to see the overall message
			for (int i=0;i<4;i++) {
				switch (i+shift_state) {
				case 0: max7219_display_no_decode(i, 0b0); break;
				case 1: max7219_display_no_decode(i, 0b0); break;
				case 2: max7219_display_no_decode(i, 0b0); break;
				case 3: max7219_display_no_decode(i, 0b0); break;
				case 4: max7219_display_no_decode(i, 0b0010101); break; 	//n
				case 5: max7219_display_no_decode(i, 0b0010000); break; 	//i
				case 6: max7219_display_no_decode(i, 0b0001101); break; 	//c
				case 7: max7219_display_no_decode(i, 0b1001111); break; 	//E
				case 8: max7219_display_no_decode(i, 0b0); break; 			//
				case 9: max7219_display_no_decode(i, 0b1100111); break; 	//P
				case 10: max7219_display_no_decode(i, 0b0001110); break;	//L
				case 11: max7219_display_no_decode(i, 0b1110111); break;	//A
				case 12: max7219_display_no_decode(i, 0b0100111); break;	//Y
				case 13: max7219_display_no_decode(i, 0b1001111); break;	//E
				case 14: max7219_display_no_decode(i, 0b0000101); break;	//r
				case 15: max7219_display_no_decode(i, 0b0); break;			//
				case 16: max7219_display_no_decode(i, 0b0110000); break;	//1
				case 17: max7219_display_no_decode(i, 0b0); break;			//
				case 18: max7219_display_no_decode(i, 0b0100111); break;	//Y
				case 19: max7219_display_no_decode(i, 0b0000101); break;	//r
				case 20: max7219_display_no_decode(i, 0b0); break;			//
				case 21: max7219_display_no_decode(i, 0b1001110); break;	//C
				case 22: max7219_display_no_decode(i, 0b1111110); break;	//O
				case 23: max7219_display_no_decode(i, 0b1111110); break;	//O
				case 24: max7219_display_no_decode(i, 0b0001110); break;	//L
				case 25: max7219_display_no_decode(i, 0b0); break;
				case 26: max7219_display_no_decode(i, 0b0); break;
				case 27: max7219_display_no_decode(i, 0b0); break;
				case 28: max7219_display_no_decode(i, 0b0); break;
				}
			}
			shift_state++;

			if (shift_state+3 > 28)
				shift_state = 0;

			//the buffer7 is actualised
			buffer7=fsm_handle->controllers.state_execution_count;
		}

		/* 7SEGMENT END  ----------------------------------------------------------------------------------*/

	}

	/* ANIMATION END  ----------------------------------------------------------------------------------*/
}

void state_p2wn(void)
{
	/* INIT BEGIN  ----------------------------------------------------------------------------------*/

	//init variables
	static uint32_t buffer7;

	//init functions of the state
	if (fsm_handle->controllers.state_execution_count == 0) {

		//clean the 7segments
		max7219_erase_no_decode();
		max7219_erase_decode();

		//clear the leds
		clear_array();

		//init the timer buffer7
		buffer7 = fsm_handle->controllers.state_execution_count;

		//reset scores
		fsm_handle->controllers.p1_score = 0;
		fsm_handle->controllers.p2_score = 0;

		//reset pass count
		fsm_handle->controllers.pass_count = 0;

		set_music(WIN);
		set_interrupt_launcher(MUSIC);
		start_timer();
	}

	/* INIT END  ----------------------------------------------------------------------------------*/

	/* ANIMATION BEGIN  ----------------------------------------------------------------------------------*/

	//check if the animation state is on
	if (fsm_handle->controllers.animation_state == ANIMATION_RUNNING) {

		/* 7SEGMENT BEGIN  ----------------------------------------------------------------------------------*/

		//check if the last animation update happened over than 20000 cycles
		if (fsm_handle->controllers.state_execution_count-buffer7>80000) {


			max7219_erase_no_decode();

			static uint8_t shift_state = 0;

			//display a message with shifting the letter in order to see the overall message
			for (int i=0;i<4;i++) {
				switch (i+shift_state) {
				case 0: max7219_display_no_decode(i, 0b0); break;
				case 1: max7219_display_no_decode(i, 0b0); break;
				case 2: max7219_display_no_decode(i, 0b0); break;
				case 3: max7219_display_no_decode(i, 0b0); break;
				case 4: max7219_display_no_decode(i, 0b0010101); break; 	//n
				case 5: max7219_display_no_decode(i, 0b0010000); break; 	//i
				case 6: max7219_display_no_decode(i, 0b0001101); break; 	//c
				case 7: max7219_display_no_decode(i, 0b1001111); break; 	//E
				case 8: max7219_display_no_decode(i, 0b0); break; 			//
				case 9: max7219_display_no_decode(i, 0b1100111); break; 	//P
				case 10: max7219_display_no_decode(i, 0b0001110); break;	//L
				case 11: max7219_display_no_decode(i, 0b1110111); break;	//A
				case 12: max7219_display_no_decode(i, 0b0100111); break;	//Y
				case 13: max7219_display_no_decode(i, 0b1001111); break;	//E
				case 14: max7219_display_no_decode(i, 0b0000101); break;	//r
				case 15: max7219_display_no_decode(i, 0b0); break;			//
				case 16: max7219_display_no_decode(i, 0b0110000); break;	//1
				case 17: max7219_display_no_decode(i, 0b0); break;			//
				case 18: max7219_display_no_decode(i, 0b0100111); break;	//Y
				case 19: max7219_display_no_decode(i, 0b0000101); break;	//r
				case 20: max7219_display_no_decode(i, 0b0); break;			//
				case 21: max7219_display_no_decode(i, 0b1001110); break;	//C
				case 22: max7219_display_no_decode(i, 0b1111110); break;	//O
				case 23: max7219_display_no_decode(i, 0b1111110); break;	//O
				case 24: max7219_display_no_decode(i, 0b0001110); break;	//L
				case 25: max7219_display_no_decode(i, 0b0); break;
				case 26: max7219_display_no_decode(i, 0b0); break;
				case 27: max7219_display_no_decode(i, 0b0); break;
				case 28: max7219_display_no_decode(i, 0b0); break;
				}
			}
			shift_state++;

			if (shift_state+3 > 28)
				shift_state = 0;

			//the buffer7 is actualised
			buffer7=fsm_handle->controllers.state_execution_count;
		}

		/* 7SEGMENT END  ----------------------------------------------------------------------------------*/

	}

	/* ANIMATION END  ----------------------------------------------------------------------------------*/
}

//button callback function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	//check the button pushed
	if (GPIO_Pin == BTN1_Pin) {

		//increment the push_nb of this button
		fsm_handle->inputs.nb_press_btn1++;
	}
	else if (GPIO_Pin == BTN2_Pin) {

		//increment the push_nb of this button
		fsm_handle->inputs.nb_press_btn2++;
	}
}
