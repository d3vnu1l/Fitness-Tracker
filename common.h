#ifndef COMMON_H_
#define  COMMON_H_
/*
	Hardware assignments
*/
#define ENCODERPINA 50
#define ENCODERPINB 48
#define BUTTONPIN 52
#define SLED 44
#define INTERRUPT_PIN 2	

#define DELAY_VALUE 200
#define COOLDOWN 5
#define BUFFER_SIZE 64
#define PEAK_SENS 30			//samples to watch for peak
/*
	Manual Memory Map

*/
#define INITIALIZED_ADDR  0	//(8 bit)
#define WEIGHT_ADDR       2	//(16 bit)
#define LASTEXERCISE_ADDR 4 //(8 bit)

#define CURLS_BASEMEM 64	//base memory address for curl data
#define CAT_ADDR (CURLS_BASEMEM)	//curls average time
#define CAA_ADDR (CURLS_BASEMEM+2)	//curls average acceleration
#define CAS_ADDR (CURLS_BASEMEM+4)	//curls average ecceleration
/*
	CURL PARAMETERS
*/
#define CURLS_UP_THRESH 60		//max angle that must be surpassed
#define CURLS_DOWN_THRESH 35	//minumum angle after which a curl is registered
#define ACC_MAG -340			//acceleration detection sensitivity
#define ACC_MSEC 52				//number of msecs that acceleration must be present
/*
	BENCHPRESS PARAMETERS
*/
#define BENCHPRESS_MAX 200

enum states
{
	//menus
	mainMenu,
		fitnessTest,	//tbd
		wod,
		settings,
		statistics,

	chooseWeight,
	//exercises
	warmup,
	curls,
	benchpress,
	squats,

	cooldown,
};

#endif  /* UTILITIES_H */


