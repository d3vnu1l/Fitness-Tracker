#ifndef COMMON_H_
#define  COMMON_H_
/*
	Hardware assignments
*/
#define ENCODERPINA 50
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
#define WEIGHT_ADDR       2	//(16 bit)
#define LASTEXERCISE_ADDR 4 //(8 bit)
//CURL STATS
#define CURLS_BASEMEM 64	//base memory address for curl data
#define CAT_ADDR (CURLS_BASEMEM)	//curls average time
#define CAA_ADDR (CURLS_BASEMEM+2)	//curls average acceleration
#define CAS_ADDR (CURLS_BASEMEM+4)	//curls average intensity

//BENCHPRESS STATS
#define BENCHPRESS_BASEMEM 64	//base memory address for curl data
#define BAT_ADDR (BENCHPRESS_BASEMEM)	//curls average time
#define BAA_ADDR (BENCHPRESS_BASEMEM+2)	//curls average acceleration
#define BAS_ADDR (BENCHPRESS_BASEMEM+4)	//curls average ecceleration
//SQUAT STATS
#define SQUATS_BASEMEM 64	//base memory address for curl data
#define SAT_ADDR (SQUATS_BASEMEM)	//curls average time
#define SAA_ADDR (SQUATS_BASEMEM+2)	//curls average acceleration
#define SAS_ADDR (SQUATS_BASEMEM+4)	//curls average ecceleration

/*
	CURL PARAMETERS
*/
#define CURLS_UP_THRESH 60		//max angle that must be surpassed
#define CURLS_DOWN_THRESH 35	//minumum angle after which a curl is registered
#define ACC_MAG -310			//acceleration detection sensitivity
#define ACC_MSEC 22				//number of msecs that acceleration must be present
/*
	BENCHPRESS PARAMETERS
*/
#define BENCHPRESS_MIN 120

#define MAX_ALLOWABLE_WEIGHT 500

#define BTNDELAYMS 35
#define ENCDELAYMS 6

#define DEBUG_A false
#define DEBUG_H false
#define DEBUG_T false

/*
	Screen
*/
#define TFT_CS    53 //uno8
#define TFT_RST   45 //uno9  
#define TFT_DC     47//uno10

enum states
{
	//menus
	mainMenu,
		personalRecords,	//tbd
		wod,
		settings,

	chooseWeight,
	warmup,

	//exercises
	curls,
	benchpress,
	squats,
	overhead,
	deadlift,

	cooldown,
};


#endif  /* UTILITIES_H */


