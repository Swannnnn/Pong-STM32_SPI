/*
 * max7219.c
 *
 *  Created on: Oct 11, 2022
 *      Author: mathi
 */

#include "max7219.h"

#define RESET_MAX7219_PARAMS() \
	do                         \
	{                          \
		max7219_handle = NULL; \
	} while (0)

#define CHECK_MAX7219_PARAMS()      \
	do                              \
	{                               \
		if (max7219_handle == NULL) \
		{                           \
			RESET_MAX7219_PARAMS(); \
			return HAL_ERROR;       \
		}                           \
	} while (0)

/* Static variables used to store MAX7219 related objects */
static MAX7219_Handle_TypeDef *max7219_handle = NULL;

static uint8_t digits_registers[] = {
	DIGIT_0_REG_BASE,
	DIGIT_1_REG_BASE,
	DIGIT_2_REG_BASE,
	DIGIT_3_REG_BASE,
	DIGIT_4_REG_BASE,
	DIGIT_5_REG_BASE,
	DIGIT_6_REG_BASE,
	DIGIT_7_REG_BASE,
};

/*
 * @brief Send data to address
 * @param _address Address on 8 bits
 * @param _data Data on 8 bits
 */
static HAL_StatusTypeDef max7219_transmit(uint8_t _address, uint8_t _data)
{
	uint8_t data[] = {_address, _data};				 // SPI transmit buffer
	size_t data_sz = sizeof(data) / sizeof(uint8_t); // Size of SPI transmit buffer
	HAL_StatusTypeDef max7219_status = HAL_OK;		 // Return value

	// Select MAX7219, send data, de-select MAX7219
	HAL_GPIO_WritePin(max7219_handle->spi_ncs_port, max7219_handle->spi_ncs_pin, GPIO_PIN_RESET);
	max7219_status = HAL_SPI_Transmit(max7219_handle->hspi, data, data_sz, 100);
	HAL_GPIO_WritePin(max7219_handle->spi_ncs_port, max7219_handle->spi_ncs_pin, GPIO_PIN_SET);

	// Return transmit status
	return max7219_status;
}

/**
 * @brief Init function. Pass hardware handles and constants.
 * also initializes basic functions of MAX7219
 * @param _max_7219_handle Pointer to MAX7219 handle
 *
 */
HAL_StatusTypeDef max7219_init(MAX7219_Handle_TypeDef *_max_7219_handle)
{
	/* Attribute parameters to static variables */
	max7219_handle = _max_7219_handle;

	CHECK_MAX7219_PARAMS();

	/* Initialize MAX7219 following datasheet */
	HAL_StatusTypeDef max7219_status = HAL_OK;

	// Shutdown MAX7219 to reset configuration
	max7219_status = max7219_transmit(SHUTDOWN_REG_BASE, SHUTDOWN_REG_SHUTDOWN_MODE);
	if (max7219_status != HAL_OK)
		return max7219_status;

	// Enable MAX7219
	max7219_status = max7219_transmit(SHUTDOWN_REG_BASE, SHUTDOWN_REG_NORMAL_MODE);
	if (max7219_status != HAL_OK)
		return max7219_status;

	// Set scan limit to number of digits
	max7219_status = max7219_transmit(SCAN_LIMIT_REGG_BASE, max7219_handle->digits_count - 1);
	if (max7219_status != HAL_OK)
		return max7219_status;

	// Set decode mode to 'no decode'
	max7219_status = max7219_transmit(DECODE_MODE_REG_BASE, 0x00);
	if (max7219_status != HAL_OK)
		return max7219_status;

	// Set brightness to middle value
	max7219_status = max7219_transmit(INTENSITY_REG_BASE, 0x08);
	if (max7219_status != HAL_OK)
		return max7219_status;

	// Erase all digits
	max7219_status = max7219_erase_no_decode();
	if (max7219_status != HAL_OK)
		return max7219_status;

	// Return on success
	return HAL_OK;
}

/**
 * @brief Display value without code B decoding.
 * @param _digit_index 7 segment digit index (starts at 0)
 * @param _digit_value Desired digit value to be written
 * @retval HAL_OK on success
 */
HAL_StatusTypeDef max7219_display_no_decode(uint8_t _digit_index, uint8_t _digit_value)
{
	HAL_StatusTypeDef max7219_status = HAL_OK;

	// Check if init has been called
	CHECK_MAX7219_PARAMS();

	// Set decode mode to 'no decode'
	max7219_status = max7219_transmit(DECODE_MODE_REG_BASE, 0x00);
	if (max7219_status != HAL_OK)
		return max7219_status;

	/* Check if digit index does not overflow actual hardware setup */
	if (_digit_index > max7219_handle->digits_count)
		return HAL_ERROR;

	// Display value
	max7219_status = max7219_transmit(digits_registers[_digit_index], _digit_value);
	if (max7219_status != HAL_OK)
		return max7219_status;

	return max7219_status;
}

/**
 * @brief Display value with code B decoding.
 * @param _digit_index 7 segment digit index (starts at 0)
 * @param _digit_value Desired digit value to be written
 * @retval HAL_OK on success
 */
HAL_StatusTypeDef max7219_display_decode(uint8_t _digit_index, uint8_t _digit_value){
	HAL_StatusTypeDef max7219_status = HAL_OK;

	// Check if init has been called
	CHECK_MAX7219_PARAMS();

	/* Check if digit index does not overflow actual hardware setup */
	if (_digit_index > max7219_handle->digits_count)
		return HAL_ERROR;

	// Set decode mode to 'decode'
	max7219_status = max7219_transmit(DECODE_MODE_REG_BASE, 0xFF);
	if (max7219_status != HAL_OK)
		return max7219_status;

	// Display value
	max7219_status = max7219_transmit(digits_registers[_digit_index], _digit_value);
	if (max7219_status != HAL_OK)
		return max7219_status;

	return max7219_status;
}

/**
 * @brief Erase display
 * @retval HAL_OK on success
 */
HAL_StatusTypeDef max7219_erase_no_decode(void)
{
	HAL_StatusTypeDef max7219_status = HAL_OK;

	// Check if init has been called
	CHECK_MAX7219_PARAMS();

	// Set decode mode to 'no decode'
	max7219_status = max7219_transmit(DECODE_MODE_REG_BASE, 0x00);
	if (max7219_status != HAL_OK)
		return max7219_status;

	for (int i = 0; i < max7219_handle->digits_count; i++)
	{
		max7219_status = max7219_transmit(digits_registers[i], DIGIT_OFF);
		if (max7219_status != HAL_OK)
			return max7219_status;
	}

	return max7219_status;
}

/**
 * @brief Erase display
 * @retval HAL_OK on success
 */
HAL_StatusTypeDef max7219_erase_decode(void)
{
	HAL_StatusTypeDef max7219_status = HAL_OK;

	// Check if init has been called
	CHECK_MAX7219_PARAMS();

	// Set decode mode to 'decode'
	max7219_status = max7219_transmit(DECODE_MODE_REG_BASE, 0xFF);
	if (max7219_status != HAL_OK)
		return max7219_status;

	for (int i = 0; i < max7219_handle->digits_count; i++)
	{
		max7219_status = max7219_transmit(digits_registers[i], DIGIT_OFF_DECODE);
		if (max7219_status != HAL_OK)
			return max7219_status;
	}

	return max7219_status;
}

/**
 * If the message is longer than 4 characters or the blinking value is greater than 2, return an error.
 * Otherwise, set the message and blinking values in the max7219_handle struct.
 * 
 * @param _message The message to be displayed on the 7-segment display.
 * @param _is_blinking 0 = no blinking, 1 = blinking, 2 = blinking with a dot
 * 
 * @return HAL_OK
 */
HAL_StatusTypeDef set_7segment(char * _message, uint8_t _is_blinking) {

	if (sizeof(_message)/sizeof(char*) > 4 || _is_blinking > 2)
		return HAL_ERROR;

	max7219_handle->message = _message;
	max7219_handle->is_blinking = _is_blinking;

	return HAL_OK;
}

/**
 * If the display is not blinking, display the message. If the display is blinking, display nothing
 * this function is called by the interrupt function
 */
void callback_display(void) {
	static uint8_t display_state = 1;

	if (display_state == 1) {
		display_on_7segments(max7219_handle->message);

		if (max7219_handle->is_blinking == 1)
			display_state =0;
	}
	else if (display_state == 0) {
		max7219_erase_no_decode();
		display_state = 1;
	}
}

/**
 * It takes a string and displays it on the 7-segment display
 * 
 * @param _message The message to display.
 * 
 * @return The HAL_StatusTypeDef is a variable that is returned by the function.
 */
HAL_StatusTypeDef display_on_7segments(char * _message) {
	if (sizeof(_message)/sizeof(char*) > 4)
			return HAL_ERROR;

	max7219_erase_no_decode();

	for (int i=0;i<max7219_handle->digits_count;i++) {
		switch ((int) _message[i]) {
		case 48: max7219_display_no_decode(i, 0b1111110); break; //0
		case 49: max7219_display_no_decode(i, 0b0110000); break; //1
		case 50: max7219_display_no_decode(i, 0b1101101); break; //2
		case 51: max7219_display_no_decode(i, 0b1111001); break; //3
		case 52: max7219_display_no_decode(i, 0b0110011); break; //4
		case 53: max7219_display_no_decode(i, 0b1011011); break; //5
		case 54: max7219_display_no_decode(i, 0b1011111); break; //6
		case 55: max7219_display_no_decode(i, 0b1110000); break; //7
		case 56: max7219_display_no_decode(i, 0b1111111); break; //8
		case 57: max7219_display_no_decode(i, 0b1111011); break; //9
		case 65: case 97: max7219_display_no_decode(i, 0b1110111); break; //a
		case 66: case 98: max7219_display_no_decode(i, 0b1111111); break; //b
		case 67: case 99: max7219_display_no_decode(i, 0b1001110); break; //c
		case 69: case 101: max7219_display_no_decode(i, 0b1001111); break; //e
		case 70: case 102: max7219_display_no_decode(i, 0b1000111); break; //f
		case 71: case 103: max7219_display_no_decode(i, 0b1011111); break; //g
		case 72: case 104: max7219_display_no_decode(i, 0b0110111); break; //h
		case 73: case 105: max7219_display_no_decode(i, 0b0110000); break; //i
		case 74: case 106: max7219_display_no_decode(i, 0b1111101); break; //j
		case 76: case 108: max7219_display_no_decode(i, 0b0001110); break; //l
		case 78: case 110: max7219_display_no_decode(i, 0b0010101); break; //n
		case 79: case 111: max7219_display_no_decode(i, 0b1111110); break; //o
		case 80: case 112: max7219_display_no_decode(i, 0b1100111); break; //p
		case 81: case 113: max7219_display_no_decode(i, 0b1110011); break; //q
		case 82: case 114: max7219_display_no_decode(i, 0b0000101); break; //r
		case 83: case 115: max7219_display_no_decode(i, 0b1011011); break; //s
		case 84: case 116: max7219_display_no_decode(i, 0b0001111); break; //t
		case 85: case 117: max7219_display_no_decode(i, 0b0111110); break; //u
		case 89: case 121: max7219_display_no_decode(i, 0b0100111); break; //y
		default : max7219_display_no_decode(i, 0b0);
		/**
		 * you can't display these folowing letters
		 * D, K, M, V, W, X, Z
		 * Sorry :/
		 */
		}
	}
	return HAL_OK;
}
