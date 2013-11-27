#include <Dhcp.h>
#include <Dns.h>
#include <WiFi.h>


#include <Adafruit_WS2801.h>
#include <stdlib.h>
#include <list>


#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

#include <SPI.h>  

#include <algorithm> //for min max




#include "Global.h"  // check here for variable and stuff


#include "InteractiveObject.h" 
std::list<InteractiveObject*> _inactiveObjects;
std::list<InteractiveObject*> _activeObjects;

#include <trace.h>



//----------------------------------------------------------
// Drawing functions
//-----------------------------------------------------------
void setColor(uint8_t r, uint8_t g,uint8_t b)
{
  _r = r;
  _g =g;
  _b = b;
}

void setColor(float hue, float saturation, float brightness)
{
  getRGBFromHSB(_r,_g,_b,hue,saturation,brightness);
}


void getRGBFromHSB(uint8_t &r, uint8_t  &g, uint8_t &b, float hue, float saturation, float brightness ) {
      
      	if(brightness == 0) { // black
      		r =0; g = 0; b= 0;
      	} else if(saturation == 0) { // grays
      		r = brightness; g = brightness; b = brightness;
      	} else {
      		float hueSix = hue * 6. / 255;
      		float saturationNorm = saturation / 255;
      		int hueSixCategory = (int) floorf(hueSix);
      		float hueSixRemainder = hueSix - hueSixCategory;
      		uint8_t pv = (uint8_t) ((1.f - saturationNorm) * brightness);
      		uint8_t qv = (uint8_t) ((1.f - saturationNorm * hueSixRemainder) * brightness);
      		uint8_t tv = (uint8_t) ((1.f - saturationNorm * (1.f - hueSixRemainder)) * brightness);
      		switch(hueSixCategory) {
      			case 0: // r
      				r = brightness;
      				g = tv;
      				b = pv;
      				break;
      			case 1: // g
      				r = qv;
      				g = brightness;
      				b = pv;
      				break;
      			case 2:
      				r = pv;
      				g = brightness;
      				b = tv;
      				break;
      			case 3: // b
      				r = pv;
      				g = qv;
      				b = brightness;
      				break;
      			case 4:
      				r = tv;
      				g = pv;
      				b = brightness;
      				break;
      			case 5: // back to r
      				r = brightness;
      				g = pv;
      				b = qv;
      				break;
      		}
      	}


}


void hsvtorgb(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char h, unsigned char s, unsigned char v)
{
  //from http://web.mit.edu/storborg/Public/hsvtorgb.c
    unsigned char region, fpart, p, q, t;
    
    if(s == 0) {
        /* color is grayscale */
        *r = *g = *b = v;
        return;
    }
    
    /* make hue 0-5 */
    region = h / 43;
    /* find remainder part, make it from 0-255 */
    fpart = (h - (region * 43)) * 6;
    
    /* calculate temp vars, doing integer multiplication */
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * fpart) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;
        
    /* assign temp vars based on color cone region */
    switch(region) {
        case 0:
            *r = v; *g = t; *b = p; break;
        case 1:
            *r = q; *g = v; *b = p; break;
        case 2:
            *r = p; *g = v; *b = t; break;
        case 3:
            *r = p; *g = q; *b = v; break;
        case 4:
            *r = t; *g = p; *b = v; break;
        default:
            *r = v; *g = p; *b = q; break;
    }
    
    return;
}





void decay_brightness(int i)
{
  //if (i==0) Serial.println(default_brightness);
  if (bright[i] < default_brightness +1) 
  { 
    bright[i] = default_brightness;
    return;
   } 
   
   
   //default_brightness = 2;
  bright[i] = (float) default_brightness  + (255 -default_brightness) * exp(-my_time[i]*decaySpeed);
  my_time[i]++; 
}

//----------------------------------------------------------
// Animation Functions
//-----------------------------------------------------------

void updatePixelBuffer()
{
    current_hue+=hueSpeed;
    unsigned char r,g,b;
    if (current_hue > 255) current_hue = 0;
     for (int i=0; i < N_COL*N_LINE; i++)
    {
         
     decay_brightness(i);
    //hsvtorgb(&r,&g,&b,(int) current_hue,default_sat, (int) bright[i]);
     getRGBFromHSB(r,g,b,current_hue,default_sat,  bright[i]);
    pixelBuffer[3*i] = r;
    pixelBuffer[3*i+1] = g;
    pixelBuffer[3*i+2] = b;
    
    if (bright[i] > default_brightness) 
    {
      //drawCircle(i % N_COL, i/N_COL,2);
      //drawRectangle(i % N_COL, i/N_COL,6,6 );
    }
    }
}

void updateStrip()
{

  
  
  unsigned char* ptr = strip.pixels;
  unsigned char* pxBuf = pixelBuffer;
  
  bool even = 0;
  for (int i=0; i < N_COL*N_LINE; i++)
  {
    if ((i % N_COL) == 0) 
    {
      even = !even;
      if (i!=0) { 
        ptr += 3*N_COL;
        if (!even) ptr-=1;
        if (even) ptr +=1;
      }
    }
   // unsigned char r,g,b;
    //if (bright[i] > default_brightness) bright[i] = std::max((float) bright[i] - decaySpeed*exp(,(float) default_brightness);
    //decay_brightness(i);
    //hsvtorgb(&r,&g,&b,(int) current_hue,default_sat, (int) bright[i]);
    //setHsv(r,g,b,current_hue,default_sat,  bright[i]);
    //strip.setPixelColor(i,r,g,b);
    if (even)
    {
      *ptr++ =*pxBuf++;
      *ptr++ =*pxBuf++;
      *ptr++ =*pxBuf++;
    }
    else 
    {
      *ptr--=* (pxBuf +2);
      *ptr--= *(pxBuf+1) ;
      *ptr--= *(pxBuf);
       pxBuf+=3;
    }
  }
 


}


IPAddress localIP()
{
	IPAddress ret;
	struct ifaddrs *ifaddr = NULL, *ifa = NULL;
	int family, s;
	char ip_addr[NI_MAXHOST];

	// code from the manpage on getifaddrs

	if (getifaddrs(&ifaddr) == -1) {
		return ret;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr == NULL)
				continue;

			family = ifa->ifa_addr->sa_family;


			if (family == AF_INET && !strcmp(ifa->ifa_name, "wlan0")){
				s = getnameinfo(ifa->ifa_addr,
								sizeof(struct sockaddr_in),
								ip_addr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				if (s != 0) {
						
				}
				
				ret = (const uint8_t*)ip_addr;
			}
	}
	freeifaddrs(ifaddr);

	//W5100.getIPAddress(ret.raw_address());

	return ret;
}





#define MY_TRACE_PREFIX "INTERACTIVE_DRAWING"

//----------------------------------------------------------
// General Arduino functions
//-----------------------------------------------------------

void setup() {

  /* TRACE DEBUG. Listen on port udp 4661 if you want info ! */ 
  /*in_addr_t ip;
  inet_pton(AF_INET, "192.168.0.12",&ip);
  trace_enable(1);
  trace_target_enable(TRACE_TARGET_ETH);
  trace_init( TRACE_LEVEL_ERROR, ip);
 // trace_error("Failed to open SPI device\n");*/
  WiFi.begin("LYT");
  strip.begin();
  strip.show();
  Serial.begin(115200);
  
  IPAddress myIP = localIP();
  boardID = 666;
  for (int i=0; i < N_PANNEL; i++)
  {
      if (boardsIP[i] == myIP) { boardID = i; break; }
  }
  
  delay(1000);
  Serial.println(boardID);

  
  
  
  
  //memset(bright,default_brightness,N_COL*N_LINE); //not with float :(
  for (int i=0; i < N_COL*N_LINE; i++) { 
    bright[i] = default_brightness;
   my_time[i] = 0; 
  }
  
  for (int i= 0 ; i < MAX_OBJECT; i++)
  {
    _inactiveObjects.push_back(new PlusObject());
  }
}

void loop() {
  
   OSCBundle  b;
   OSCMessage *m;
   while (osc.getNextMessage(b))
  {
    m = b.getOSCMessage((int) 0);
    if (!m) break;
    
    int x=m->getInt(1);
    int y=m->getInt(2);
    
    int n = y*N_COL+ x;
    
    
    if ((!_inactiveObjects.empty() ) && n < N_COL*N_LINE)
    {
      InteractiveObject* obj = _inactiveObjects.front();
      _inactiveObjects.pop_front();
      obj->reset(x,y);
      _activeObjects.push_back(obj);
    }
    
    
    b.empty();
   } 
   
   updatePixelBuffer();
   std::list<InteractiveObject*> _toBeRemoved;
   
   for (std::list<InteractiveObject*>::const_iterator iterator = _activeObjects.begin(), end = _activeObjects.end(); iterator != end; ++iterator) {
    (*iterator)->update();
    if ((*iterator)->life <= 0) { 
      //_activeObjects.remove(*iterator);  causes segfault :'(
      _toBeRemoved.push_back(*iterator);
      _inactiveObjects.push_back(*iterator);
     } 
     else 
     {
       (*iterator)->draw();
     }
    }
   for (std::list<InteractiveObject*>::const_iterator iterator = _toBeRemoved.begin(), end = _toBeRemoved.end(); iterator != end; ++iterator) _activeObjects.remove(*iterator);
   _toBeRemoved.clear();
   
   updateStrip();
   
   
   
   count++;
   
  // osc.sendPixelBuffer(strip.pixels,N_LINE,N_COL);
   //osc.sendPixelBuffer(strip.pixels,N_LINE,N_COL);
  // osc.sendCurrentColor();
   
   if (count == 24)
   {
     count = 0;
    
     //osc.sendPixelBuffer(pixelBuffer,N_LINE,N_COL);
     
     
     osc.sendCurrentColor();
   }
   
   strip.show();
  
  
}
