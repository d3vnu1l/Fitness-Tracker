#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Fonts/FreeMonoBoldOblique18pt7b.h>
#include <Fonts/FreeSerif9pt7b.h> //Each filename starts with the face name (“FreeMono”, “FreeSerif”, etc.) followed by the style (“Bold”, “Oblique”, none, etc.), font size in points (currently 9, 12, 18 and 24 point sizes are provided) and “7b” to indicate that these contain 7-bit characters (ASCII codes “ ” through “~”); 8-bit fonts (supporting symbols and/or international characters) are not yet provided but may come later.

#include "resources\bitmaps.h"



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


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);



void initScreen() {
	pinMode(TFT_CS, OUTPUT);
	pinMode(TFT_RST, OUTPUT);
	pinMode(TFT_DC, OUTPUT);
	tft.initR(INITR_GREENTAB);   // initialize a ST7735S chip, black tab
	tft.fillScreen(WHITE);
	tft.fillCircle(64, 64, 64, BLACK);

}

void drawscreenCurls(int reps, bool bluetoothConnected)
{
	static String CU = "CURLS";
	static String BR = "BAREBELL ROWS";
	static String BP = "BENCH PRESS";
	static String SQ = "SQUAT";
	static String DL = "DEADLIFT";
	static String OH = "OVERHEAD";

	//draw bluetooth
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

	//draw everything else
	switch (reps)
	{
	case 0:
		tft.setTextSize(2);
		tft.setCursor(36, 24);
		tft.print(CU);
		tft.drawBitmap(37, 100, GB, 60, 24, WHITE);
		tft.drawBitmap(40, 80, ST, 60, 24, WHITE);
		tft.drawBitmap(16, 54, LA, 24, 24, WHITE);
		tft.drawBitmap(90, 54, RA, 24, 24, BLUE);
		break;

	case 1:
		tft.setTextSize(2);
		tft.setCursor(36, 24);
		tft.print(CU);
		tft.drawBitmap(37, 100, GB, 60, 24, WHITE);
		tft.drawBitmap(40, 80, ST, 60, 24, WHITE);
		tft.drawBitmap(16, 54, LA, 24, 24, BLUE);
		tft.drawBitmap(90, 54, RA, 24, 24, WHITE);
		break;

	case 2:
		tft.setTextSize(2);
		tft.setCursor(36, 24);
		tft.print(CU);
		tft.drawBitmap(37, 100, GB, 60, 24, WHITE);
		tft.drawBitmap(40, 80, ST, 60, 24, BLUE);
		tft.drawBitmap(16, 54, LA, 24, 24, WHITE);
		tft.drawBitmap(90, 54, RA, 24, 24, WHITE);
		break;

	case 3:
		tft.setTextSize(2);
		tft.setCursor(36, 24);
		tft.print(CU);
		tft.drawBitmap(37, 100, GB, 60, 24, BLUE);
		tft.drawBitmap(40, 80, ST, 60, 24, WHITE);
		tft.drawBitmap(16, 54, LA, 24, 24, WHITE);
		tft.drawBitmap(90, 54, RA, 24, 24, WHITE);
		break;

	}
}