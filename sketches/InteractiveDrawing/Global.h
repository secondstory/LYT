#pragma once
#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#include <Adafruit_WS2801.h>




#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

#include <SPI.h>  

#include <algorithm> //for min max


#include "OSCRxTx.h"

extern int count;


//----------------------------------------------------------
// Layout Setting
//-----------------------------------------------------------

#define N_COL 55
#define N_LINE 6
#define N_PANNEL 3

#define MAX_OBJECT 1000


//----------------------------------------------------------
// Network Setting
//-----------------------------------------------------------

//IPAddress ip(192,168,4,17);
extern IPAddress ip;  //Is the IP of the debug computer !
extern IPAddress touchServerIp;
extern IPAddress boardsIP[3]; //used for lookup
extern OSCRxTx osc;

extern int boardID;

//----------------------------------------------------------
// Animation setting
//-----------------------------------------------------------

extern float default_brightness;
extern float default_sat;
extern float current_hue;
extern float decaySpeed;

extern float hueSpeed;

extern float my_time[N_COL*N_LINE];



//----------------------------------------------------------
// Global Variables
//-----------------------------------------------------------



extern float bright[N_LINE*N_COL];
extern uint8_t pixelBuffer[N_COL*N_LINE*3];


extern uint8_t _r;
extern uint8_t _g;
extern uint8_t _b;


extern Adafruit_WS2801 strip;



bool getIndex(int &ix, int x, int y);


//-----------------------------------------
//  Drawing functions
//------------------------------------------

void drawCircle(int x, int y, int radius);
void drawRectangle(int x, int y, int w, int h);
void drawPixel(int x, int y);
void setColor(uint8_t r, uint8_t g,uint8_t b);
void setColor(float hue, float saturation, float brightness);

void getRGBFromHSB(uint8_t &r, uint8_t  &g, uint8_t &b, float hue, float saturation, float brightness );

#endif
