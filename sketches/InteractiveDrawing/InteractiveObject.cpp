/*

Copyright (c) 2013 - Philippe Laulheret, Second Story [http://www.secondstory.com]

This code is protected under MIT license. 
For more information visit  : https://github.com/secondstory/LYT

*/

#include "InteractiveObject.h"

#include <algorithm> //for min max


 InteractiveObject::InteractiveObject()
 {
    _defaultLife = 500;
 }
 
 void InteractiveObject::setDefaultLife(int defaultVal)
 {
  _defaultLife = defaultVal;
 }
 
  void InteractiveObject::reset(int x, int y)
  {
    this->x =x ;
    this->y = y;
    life = _defaultLife;
    onReset();
  }
  
  
void  InteractiveObject::draw()
{
  
}



void InteractiveObject::update()
{
    life--;
    onUpdate();
}


//---------------------------------------------------------------------
//  PlusObject
//--------------------------------------------------------------------

PlusObject::PlusObject() : InteractiveObject()
{
  setDefaultLife(100);
  onReset();
}


void PlusObject::onUpdate()
{
  brightness = std::max( (float)(default_brightness + (255-default_brightness)*exp( (life-_defaultLife)*decaySpeed)), (float)default_brightness );
  //if (x==0) life=0;
  //else { 
  //  if (life % 60 == 0) x--;  //fall 1 led every 10 frame
  //  }
}

void PlusObject::onReset()
{
  brightness = default_brightness;
}


void PlusObject::draw()
{
    float h = current_hue + hueAdjust;
    float s = default_sat + satAdjust;
   
    if (h >= 255) h-= 255; 
    if (h <= 0 ) h+= 255;
  
    if( s >= 255 ) s-=255;
    if( s <= 0 ) s+=255;
    
   setColor(h,s,brightness);
   drawPixel(x,y);
   drawPixel(x+1,y);
   drawPixel(x-1,y);
   drawPixel(x,y+1);
   drawPixel(x,y-1);
}
