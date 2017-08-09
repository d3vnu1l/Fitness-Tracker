#include "headers\Display.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
//#include <SPI.h>
#include <Fonts/FreeMonoBoldOblique18pt7b.h>
#include <Fonts/FreeSerif9pt7b.h> //Each filename starts with the face name (“FreeMono”, “FreeSerif”, etc.) followed by the style (“Bold”, “Oblique”, none, etc.), font size in points (currently 9, 12, 18 and 24 point sizes are provided) and “7b” to indicate that these contain 7-bit characters (ASCII codes “ ” through “~”); 8-bit fonts (supporting symbols and/or international characters) are not yet provided but may come later.
#include "resources\bitmaps.h"
#include <avr/pgmspace.h>
#include "headers\dmp.h"




// Color definitions
#define BLACK    0x0000
#define BLUE     0x0C3A
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xF640
#define WHITE    0xFFFF
#define GREY     0xD6BA


#define TFT_CS    53 //uno8
#define TFT_RST   45 //uno9  
#define TFT_DC     47//uno10


const PROGMEM String BTU = "BLUETOOTH";
const PROGMEM String SETT = "SETTINGS";
const PROGMEM String Y = "YES";
const PROGMEM String N = "NO";
const PROGMEM String CO = "CONNECT";
const PROGMEM String TA = "DISCONNECT";
const PROGMEM String CLD = "CLEAR DATA";
const PROGMEM String GB = "GO BACK";
const PROGMEM String GO = "GO!";
const PROGMEM String RE = "REST";
const PROGMEM String WRK = "WORKOUT";
const PROGMEM String PRG = "PROGRESS";

const String cu = "CURLS";
const String sq = "SQUATS";
const String dl = "DEADLIFTS";
const String ro = "ROWS";
const String bench = "BENCH";
const String benchp = "BENCHPRESS";
const String over = "OVERHEAD";
const String mr = "MAXREP";

static int settings_locations[][2] = { { 16, 58 },{ 64, 58 },{ 48, 88 },{ 70, 88 },{ 44, 108 } };
static String settings_redrawables[] = { CO, TA, Y, N, GB };

static int mm_locations[][2] = { { 25, 46 },{ 18, 68 },{ 18, 92 } };
static String mm_redrawables[] = { WRK, PRG, SETT};

static int wod_locations[][2] = { {35,46},{5,68},{30,89}, {44,111} };
static String wod_redrawables[] = { cu,benchp,sq, GB};

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

extern int state, laststate;
extern bool bluetoothConnected;
const int reps = 0;
static bool init_ed = false;



void setupScreen() {
	pinMode(TFT_CS, OUTPUT);
	pinMode(TFT_RST, OUTPUT);
	pinMode(TFT_DC, OUTPUT);
	tft.initR(INITR_REDTAB);   // initialize a ST7735S chip, red tab

	tft.fillScreen(WHITE);
	tft.fillCircle(64, 64, 64, BLACK);
	//initScreen();
	//initSettings();
	//initScreen();
	initMainMenu();

	init_ed = true;
}

//handles inits screens
void initScreen(void) {
	if (init_ed == true) { //draw main menu
		destroyScreen();
		switch (state) {
		case mainMenu:
			initMainMenu();
			break;
		case settings:
			initSettings();
			break;
		case wod:
			initWOD();
			break;
		case curls:
			initCurls();
			break;
		case benchpress:
			initBenchpress();
			break;
		case personalRecords:
			initPersonalRecords();
			break;
		case chooseWeight:
			initChooseWeight();
			break;
		}
		init_ed = true;
	}
}

void drawScreen(int reps, int tier) {

	if (init_ed == true) {
		switch (state) {
		case mainMenu:
			updateMainMenu(reps);
			break;
		case wod:
			updateWOD(reps);
			break;
		case curls:
			updateExercise(reps);
			break;
		case benchpress:
			updateExercise(reps);
			break;
		case personalRecords:
			updatePersonalRecords(reps);
			break;
		case chooseWeight:
			updateChooseWeight(reps, tier);
			break;
		}
	}
}

void initCurls(void)
{
	tft.setTextSize(2);
	tft.setTextColor(BLUE);
	tft.setCursor(24, 96);
	tft.print(GB);
	tft.setTextColor(RED);
	tft.setCursor(16, 50);
	tft.print(GO);
	tft.setTextColor(WHITE);
	tft.setCursor(37, 24);
	tft.print(cu);
	tft.setCursor(68, 50);
	tft.print(RE);
	tft.setCursor(76, 73);
	tft.print("0");
	tft.setTextSize(1);
	tft.setCursor(44, 76);
	tft.print("REPS:");
}

void initBenchpress(void)
{
	tft.setTextSize(2);
	tft.setTextColor(BLUE);
	tft.setCursor(24, 96);
	tft.print(GB);
	tft.setTextColor(RED);
	tft.setCursor(16, 50);
	tft.print(GO);
	tft.setTextColor(WHITE);
	tft.setCursor(37, 24);
	tft.print(bench);
	tft.setCursor(68, 50);
	tft.print(RE);
	tft.setCursor(76, 73);
	tft.print("0");
	tft.setTextSize(1);
	tft.setCursor(44, 76);
	tft.print("REPS:");
}

void updateMainMenu(int place) {
	resetF();
	static int place_l = place;
	tft.setTextSize(2);
	if (place_l!=place) {
		tft.setTextColor(BLUE);
		tft.setCursor(mm_locations[place][0], mm_locations[place][1]);
		tft.print(mm_redrawables[place]);

		tft.setTextColor(WHITE);
		tft.setCursor(mm_locations[place_l][0], mm_locations[place_l][1]);
		tft.print(mm_redrawables[place_l]);
	}
	else {
		tft.setTextColor(BLUE);
		tft.setCursor(mm_locations[place][0], mm_locations[place][1]);
		tft.print(mm_redrawables[place]);
	}
	place_l = place;
	enableF();
}

void initMainMenu(void)
{
	tft.setTextSize(2);
	tft.setTextColor(WHITE);
	tft.setCursor(18, 92);
	tft.print("SETTINGS");
	tft.setCursor(18, 68);
	tft.print("PROGRESS");
	tft.setCursor(31, 21);
	tft.print(mr);
	tft.drawLine(28, 37, 100, 37, WHITE);
	tft.setCursor(25, 46);
	tft.print("WORKOUT");
}

void initWOD(void)
{
	tft.setTextSize(2);
	tft.setCursor(35, 46);
	tft.setTextColor(BLUE);
	tft.print("CURLS");
	tft.setTextColor(WHITE);
	tft.setCursor(30, 89);
	tft.print("SQUATS");
	tft.setCursor(5, 68);
	tft.print("BENCHPRESS");
	tft.setCursor(31, 21);
	tft.print(mr);
	tft.drawLine(28, 37, 100, 37, WHITE);

	tft.setTextSize(1);
	tft.setCursor(44, 111);
	tft.print("GO BACK");
}

void updateWOD(int place) {
	resetF();
	static int place_l = place;
	tft.setTextSize(2);


	if (place != place_l) {
		if (place == 3 && place_l != 3)
			tft.setTextSize(1);

		tft.setTextColor(BLUE);
		tft.setCursor(wod_locations[place][0], wod_locations[place][1]);
		tft.print(wod_redrawables[place]);

		if (place == 3 && place_l != 3)
			tft.setTextSize(2);

		if (place != 3 && place_l == 3)
			tft.setTextSize(1);

		tft.setTextColor(WHITE);
		tft.setCursor(wod_locations[place_l][0], wod_locations[place_l][1]);
		tft.print(wod_redrawables[place_l]);
	}

	place_l = place;
	enableF();
}

void updateExercise(int reps) {
	static int rep_last = 0;
	if (reps != rep_last) {
		tft.setTextSize(2);
		tft.setTextColor(BLACK);
		tft.setCursor(76, 73);
		tft.print((String)rep_last);
		rep_last = reps;
		tft.setCursor(76, 73);
		tft.setTextColor(WHITE);
		tft.print(reps);
	}
}

void initSettings(void) {

	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.setCursor(17, 26);
	tft.print(SETT);

	tft.setTextSize(1);
	tft.setCursor(38, 48);
	tft.print(BTU);


	tft.setCursor(16, 58);
	tft.print(CO);


	tft.setCursor(64, 58);
	tft.print(TA);

	tft.setCursor(35, 78);
	tft.print(CLD);

	tft.setCursor(48, 88);
	tft.print(Y);

	tft.setCursor(70, 88);
	tft.print(N);

	tft.setCursor(44, 108);
	tft.print(GB);
}

void updateSettings(int place)
{
	resetF();
	static int place_l = place;
	tft.setTextSize(1);

	
	if (place != place_l) {
		tft.setTextColor(BLUE);
		tft.setCursor(settings_locations[place][0], settings_locations[place][1]);
		tft.print(settings_redrawables[place]);

		tft.setTextColor(WHITE);
		tft.setCursor(settings_locations[place_l][0], settings_locations[place_l][1]);
		tft.print(settings_redrawables[place_l]);
	}
	else{
		tft.setTextColor(BLUE);
		tft.setCursor(settings_locations[place][0], settings_locations[place][1]);
		tft.print(settings_redrawables[place]);
	}

	place_l = place;
	enableF();
}

void updateBT(void) {
	if (bluetoothConnected == true) {
		tft.drawTriangle(36, 13, 32, 10, 32, 16, WHITE);
		tft.drawTriangle(36, 19, 32, 16, 32, 22, WHITE);
		tft.drawLine(32, 16, 28, 13, WHITE);
		tft.drawLine(32, 16, 28, 19, WHITE);
	}
	else {
		tft.drawTriangle(36, 13, 32, 10, 32, 16, BLACK);
		tft.drawTriangle(36, 19, 32, 16, 32, 22, BLACK);
		tft.drawLine(32, 16, 28, 13, BLACK);
		tft.drawLine(32, 16, 28, 19, BLACK);
	}
}

void initPersonalRecords(void) {
	tft.setTextSize(2);
	tft.setCursor(35, 46);
	tft.setTextColor(BLUE);
	tft.print("CURLS");
	tft.setTextColor(WHITE);
	tft.setCursor(30, 89);
	tft.print("SQUATS");
	tft.setCursor(5, 68);
	tft.print("BENCHPRESS");
	tft.setCursor(22, 21);
	tft.print("RECORDS");
	tft.drawLine(20, 37, 100, 37, WHITE);

	tft.setTextSize(1);
	tft.setCursor(44, 111);
	tft.print("GO BACK");
}

void updatePersonalRecords(int place) {
	resetF();
	static int place_l = place;
	tft.setTextSize(2);


	if (place != place_l) {
		if (place == 3 && place_l != 3)
			tft.setTextSize(1);

		tft.setTextColor(BLUE);
		tft.setCursor(wod_locations[place][0], wod_locations[place][1]);
		tft.print(wod_redrawables[place]);

		if (place == 3 && place_l != 3)
			tft.setTextSize(2);

		if (place != 3 && place_l == 3)
			tft.setTextSize(1);

		tft.setTextColor(WHITE);
		tft.setCursor(wod_locations[place_l][0], wod_locations[place_l][1]);
		tft.print(wod_redrawables[place_l]);
	}

	place_l = place;
	enableF();
}

void initChooseWeight(void) {
	tft.setTextSize(2);
	tft.setCursor(5, 57);
	tft.setTextColor(WHITE);
	tft.print("WEIGHT:");
	tft.setCursor(31, 21);
	tft.print(mr);
	tft.drawLine(28, 37, 100, 37, WHITE);

	tft.setTextSize(1);
	tft.setCursor(50, 95);
	tft.print("START");
	tft.setCursor(44, 111);
	tft.print("GO BACK");
}

void updateChooseWeight(int reps, int tier) {
	static int rep_last = 0;
	resetF();

	if (tier == 0) {
		if (reps != rep_last) {
			tft.setTextSize(2);
			tft.setTextColor(BLACK);
			tft.setCursor(86, 57);
			tft.print(rep_last);
			rep_last = reps;
			tft.setCursor(86, 57);
			tft.setTextColor(BLUE);
			tft.print(reps);
		}
	}
	else if (tier == 1) {
		tft.setTextSize(2);
		tft.setTextColor(WHITE);
		tft.setCursor(86, 57);
		tft.print(rep_last);
		if (reps == 0) {
			tft.setTextColor(BLUE);
			tft.setTextSize(1);
			tft.setCursor(50, 95);
			tft.print("START");
			tft.setTextColor(WHITE);
			tft.setCursor(44, 111);
			tft.print("GO BACK");
		}
		else if (reps==1){
			tft.setTextColor(WHITE);
			tft.setTextSize(1);
			tft.setCursor(50, 95);
			tft.print("START");
			tft.setTextColor(BLUE);
			tft.setCursor(44, 111);
			tft.print("GO BACK");
		}

	}
	enableF();
}


void destroyScreen(void) {
	init_ed = false;
	tft.fillCircle(64, 64, 64, BLACK);
}

