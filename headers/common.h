#ifndef COMMON_H_
#define  COMMON_H_
#include <EEPROM.h>
/*
	Hardware assignments
*/
#define ENCODERPINA 3
#define ENCODERPINB 48
#define BUTTONPIN 46
#define INTERRUPT_PIN 2	

#define DELAY_VALUE 200
#define COOLDOWN 5
#define BUFFER_SIZE 32
#define PEAK_SENS 30			//samples to watch for peak
/*
	Manual Memory Map
	bytes 0-511 are for system variables such as weight size, last exercise, whether the device is initialized, etc.
	bytes 512-1023 store curl stats
	bytes 1024-1535 store benchpress stats
	bytes 1536-2048 store squat stats


	all defines for addresses must end in _ADDR
*/
//SYSTEM VARIABLES
#define INITIALIZED_ADDR  0	//(8 bit)
#define LASTEXERCISE_ADDR 2 //(8 bit)
#define WEIGHT_CURLS_ADDR       4	//(16 bit)
#define WEIGHT_BENCHPRESS_ADDR  6	//(16 bit)
#define WEIGHT_SQUATS_ADDR      8	//(16 bit)
#define STAT_POINTER_ADDR	32		//current active memory position
#define APP_POINTER_ADDR	34
//STATS
#define STAT_BASEMEM_ADDR 64	

#define MAXMEM_ADDR	(EEPROM.length()-21)	//4096 real, 4084 to be safe

/*
	CURL PARAMETERS
*/
#define CURLS_UP_THRESH 60		//max angle that must be surpassed
#define CURLS_DOWN_THRESH 35	//minumum angle after which a curl is registered
#define ACC_MAG -250			//acceleration detection sensitivity
#define ACC_MSEC 12				//number of msecs that acceleration must be present
/*
	BENCHPRESS PARAMETERS
*/
#define BENCHPRESS_MIN 120
#define VELOCITY_CAP 1000
#define HEIGHT_CAP 800
#define MAX_ALLOWABLE_WEIGHT 500

#define BTNDELAYMS 100
#define ENCDELAYMS 5

#define UART_ON true
#define DEBUG_A false
#define DEBUG_H true
#define DEBUG_T false
#define DEBUG_MAT true

/*
	Screen
*/
#define TFT_CS    53 //uno8
#define TFT_RST   45 //uno9  
#define TFT_DC     47//uno10
#define REFRESH	  0
enum states
{
	//menus
	mainMenu,
	personalRecords,	//tbd
	wod,
	settings,

	chooseWeight,
	warmup,
	cooldown,

	curls,
	benchpress,
	squats,
	overhead,
	deadlift
};

//exercises
/*
enum exercises {
	curls,
	benchpress,
	squats,
	overhead,
	deadlift
};
*/

enum workouts
{
	fiveByFive
};



struct exercise_strings {
	static const int ex_size = 4;
	char* ex_strings[ex_size] = {"Curls", "Benchpress", "Squats", "Go Back"};
};


#endif  /* UTILITIES_H */


