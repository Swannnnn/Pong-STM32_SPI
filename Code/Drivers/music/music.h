#ifndef HEADER_MUSIC_H
#define HEADER_MUSIC_H

#include "stm32l1xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//enum of the diffrents musics initialised in music_init
typedef enum {

	PACMAN = 0,
	AU_CLAIR_DE_LA_LUNE = 1,
	P1_REFLEXE = 2,
	P2_REFLEXE = 3,
	WIN = 4,
}MUSIC_Enum;

//structure note
typedef struct {
	const char * name;
	double frequency;
	uint16_t arr;
}TypeDef_Note;

//structure partition
typedef struct {
	const char ** partition;
	size_t array_sz;
}TypeDef_Partition;

typedef struct {
	TIM_HandleTypeDef * htim;
	TypeDef_Note * notes;
	size_t notes_sz;
	TypeDef_Partition * partitions;
	MUSIC_Enum chosen_music;
	uint8_t music_running;
}TypeDef_Music_Handler;

#define TIMER_FREQ 32000000
#define CRR 6400 //max 6400
#define MUTE (char *)"-"
#define NoteFrequency 100

HAL_StatusTypeDef init_music(TypeDef_Music_Handler * _music_handler);
void buzzer_play_note(TypeDef_Note * _note);
void buzzer_mute(void);
void buzzer_play_note_by_name(uint16_t _index, MUSIC_Enum choice);
uint16_t get_partition_sz(MUSIC_Enum name);
void play_music(void);
void set_music(MUSIC_Enum music_name);


#endif
