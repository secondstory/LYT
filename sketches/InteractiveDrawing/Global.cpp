

#include <Adafruit_WS2801.h>




#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

#include <SPI.h>  

#include <algorithm> //for min max


#include "OSCRxTx.h"

int count=0;



#include "Global.h"


//----------------------------------------------------------
// Network Setting
//-----------------------------------------------------------

//IPAddress ip(10,0,8,101);
IPAddress ip(192,168,0,14);  //Is the IP of the debug computer !
IPAddress touchServerIp(192,168,0,10);

IPAddress boardsIP[] = { IPAddress(192,168,0,11), IPAddress(192,168,0,12), IPAddress(192,168,0,13) } ;

int boardID=0;

OSCRxTx osc(ip,3333,3333);

//----------------------------------------------------------
// Animation setting
//-----------------------------------------------------------

float default_brightness=50;
float default_sat=255;
float current_hue=0;
float decaySpeed = 0.05;

float hueSpeed = 0.1;

float my_time[N_COL*N_LINE];



//----------------------------------------------------------
// Global Variables
//-----------------------------------------------------------



float bright[N_LINE*N_COL];
uint8_t pixelBuffer[N_COL*N_LINE*3];

Adafruit_WS2801 strip = Adafruit_WS2801(N_LINE*N_COL);


uint8_t _r;
uint8_t _g;
uint8_t _b;


bool getIndex(int &ix, int x, int y)
{
    if ((x < 0 ) || x >= N_COL) return false;
    if ((y < 0 ) || y >= N_LINE) return false;
    
    {
      ix = 3*(N_COL*y + x);
    }
    return true;

}


void drawPixel(int x, int y)
{
  int ix;
  if(getIndex(ix,x,y)) 
  {
    pixelBuffer[ix++] = _r;
    pixelBuffer[ix++] = _g;
    pixelBuffer[ix++] = _b;
  }
  else 
  {
    //Serial.println("Pixel not drawn: out of range");
  }
  
}

void drawCircle(int x, int y, int radius)
{
  int startX = std::max(x-radius,-1);
  int endX = std::min(x+radius,N_COL);
  int startY = std::max(y-radius,-1);
  int endY = std::min(y+radius,N_LINE-1);
  
  int r2= radius*radius;
  for (int i=startY; i < endY; i++)
  {
    int y2 = (i-y)*(i-y);
    for (int j=startX; j < endX; j++)   //could use trigo to define startX,endX, but lazzy
   {
     if ((j-x)*(j-x)*y2 < r2)
    
    {
         /* if ((y2 == 0) && (j != x)) continue;
          if ((j==x)  && (i != y)) continue;*/
          int n = N_COL*i + j;    
          //int n = N_LINE*j + i; 
          pixelBuffer[3*n] = 255; //r 
          pixelBuffer[3*n+1] = 255; //g
          pixelBuffer[3*n+2] = 255; //b
          //strip.setPixelColor(n,_r,_g,_b);
    } 
   } 
  }
}


void drawRectangle(int x, int y, int w, int h)
{
  int startX = std::max(x-w/2,-1);
  int endX = std::min(x+w/2,N_COL+1);
  int startY = std::max(y-h/2,-1);
  int endY = std::min(y+h/2,N_LINE+1);
  

     
  for (int i=startY+1; i < endY+1; i++)
  {
    for (int j=startX+1; j < endX; j++)   //could use trigo to define startX,endX, but lazzy
   {
    

     
    {
         /* if ((y2 == 0) && (j != x)) continue;
          if ((j==x)  && (i != y)) continue;*/
          unsigned int n = 3*(N_COL*(i-1) + j -1);    
          //int n = N_LINE*j + i; 
          pixelBuffer[n] = 255; //r 
         // pixelBuffer[n+1] = 0; //g
          //pixelBuffer[n+2] = 0; //b
          //strip.setPixelColor(n,_r,_g,_b);
           //Serial.print(n);
           //Serial.print(" ");
    } 
    
   }
  Serial.println(""); 
   }
  
}
