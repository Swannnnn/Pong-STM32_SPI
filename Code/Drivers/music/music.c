#include "music.h"

static TypeDef_Music_Handler * music_handler;

/**
 * It sets the timer's ARR register to the note's ARR value, and sets the timer's CCR2 register to the
 * CRR value
 * 
 * @param _note a pointer to a note structure
 */
void buzzer_play_note(TypeDef_Note * _note)
{
	music_handler->htim->Instance->ARR = _note->arr;
	music_handler->htim->Instance->CCR2 = CRR;
}

void buzzer_mute()
{
	music_handler->htim->Instance->CCR2 = 0;
}

/**
 * It plays a note by name
 * 
 * @param _index the index of the note in the partition
 * @param _choice the song you want to play
 */
void buzzer_play_note_by_name(uint16_t _index, MUSIC_Enum _choice)
{
	if(!strcmp(MUTE, music_handler->partitions[_choice].partition[_index]))
	{
		buzzer_mute();
	}
	else
	{
		for(uint8_t i=0;i<music_handler->notes_sz;i++){
			if (!strcmp(music_handler->notes[i].name, music_handler->partitions[_choice].partition[_index])){
				buzzer_play_note(&music_handler->notes[i]);
				break;
			}
		}
	}
}

/* An array of notes. */
static TypeDef_Note notes_array[] = {
	{"C5", 1046.50, 30576},
	{"C#5", 1108.73, 28860},
	{"D5", 1174.66, 27240},
	{"Eb5", 1244.51, 25711},
	{"E5", 1318.51, 24268},
	{"F5", 1396.91, 22906},
	{"F#5", 1479.98, 21620},
	{"G5", 1567.98, 20406},
	{"G#5", 1661.22,19261},
	{"A5", 1760.00, 18180},
	{"A#5", 1864.66, 17159},
	{"B5", 1975.53, 16196}
};

/* A partition of the song Pacman. */
static const char* partition_pacman[] = {
		"C5",MUTE,"G5",MUTE,"E5",MUTE,"C5",MUTE,"G5","E5",MUTE,"C5",MUTE,MUTE,MUTE,
		"C#5",MUTE,"G#5",MUTE,"F5",MUTE,"C#5",MUTE,"G#5","F5",MUTE,"C#5",MUTE,MUTE,MUTE,
		"C5",MUTE,"G5",MUTE,"E5",MUTE,"C5",MUTE,"G5","E5",MUTE,"C5",MUTE,MUTE,MUTE,
		"G5","A5",MUTE,"G5","A5",MUTE,"G5","A5",MUTE,"G5","C5",MUTE
};

/* A partition of the song Au Clair de la Lune. */
static const char* partition_auClairDeLaLune[] = {
		"G5",MUTE,"G5",MUTE,"G5",MUTE,"A5",MUTE,"B5","B5","B5","B5","A5","A5","A5"
		,"A5","G5",MUTE,"B5",MUTE,"A5",MUTE,"A5", MUTE,"G5","G5","G5","G5",MUTE
};

static const char * partition_P1_reflexe[] = {"C5","G5",MUTE};

static const char * partition_P2_reflexe[] = {"D5","A5",MUTE};

/* A partition of the song win. */
static const char * partition_win[] = {
		"C5","C#5","D5","Eb5","E5","E5","F5","F5","F5","F5","F5",MUTE,MUTE,MUTE,
		"D5","Eb5","E5","E5","F5","F#5","G5","G5","G5","G5","G5",MUTE,MUTE,MUTE,
		"E5","E5","F5","F#5","G5","G#5","A5","A5","A5","A5","A5",MUTE,MUTE,MUTE,
		"F5","F#5","G5","G#5","A5","A#5","B5","B5","B5","B5","B5","B5","B5","B5",MUTE
};

/* An array of partitions. */
static TypeDef_Partition partition_array[] = {
	{
		partition_pacman,
		57,
	},
	{
		partition_auClairDeLaLune,
		29,
	},
	{
		partition_P1_reflexe,
		3,
	},
	{
		partition_P2_reflexe,
		3,
	},
	{
		partition_win,
		57,
	}
};

/**
 * It initializes the music handler
 * 
 * @param _music_handler a pointer to the music handler structure
 * 
 * @return HAL_OK
 */
HAL_StatusTypeDef init_music(TypeDef_Music_Handler * _music_handler) {

	music_handler = _music_handler;

	music_handler->notes = notes_array;

	music_handler->notes_sz = 12;

	music_handler->chosen_music = 0;

	music_handler->partitions = partition_array;

	music_handler->music_running = 0;

	/*
	for (uint8_t i=0; i<_music_handler.notes_sz;i++){
			_music_handler.notes[i].arr = ((TIMER_FREQ/_music_handler.notes[i].frequency) - 1);

		}
		*/

	return HAL_OK;
}

/**
 * If the music is running, play the next note in the partition
 *
 * this function is called by the interrupt function in timer
 */
void play_music(void) {
	if (music_handler->music_running == 1) {
		static uint8_t music_index = 0;
		if (music_index >= get_partition_sz(music_handler->chosen_music)) {
			music_index = 0;
			music_handler->music_running = 0;
		}
		else {
			buzzer_play_note_by_name(music_index, music_handler->chosen_music);
			music_index++;
		}
	}
}

/**
 * It sets the music to play to the music name passed in, and then sets the music to be running
 * 
 * @param _music_name The name of the music you want to play.
 */
void set_music(MUSIC_Enum _music_name) {
	music_handler->chosen_music = _music_name;
	music_handler->music_running = 1;
}

/**
 * "Get the size of the partition with the given name."
 * 
 * @param _name The name of the partition you want to get the size of.
 * 
 * @return The size of the partition.
 */
uint16_t get_partition_sz(MUSIC_Enum _name) { return music_handler->partitions[_name].array_sz; }
