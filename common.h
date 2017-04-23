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
#define SENSITIVITY 100

/*
	CURL PARAMETERS
*/
#define CURLS_UP_THRESH 60		//max angle that must be surpassed
#define CURLS_DOWN_THRESH 35	//minumum angle after which a curl is registered
#define ACC_MAG -340			//acceleration detection sensitivity
#define ACC_MSEC 50				//number of msecs that acceleration must be present
/*
	BENCHPRESS PARAMETERS
*/
#define BENCHPRESS_MAX 300

enum states
{
	start,
	chooseExercise,
	chooseWeight,
	warmup,
	curls,
	benchpress,
	squats,
	cooldown
};

#endif  /* UTILITIES_H */


