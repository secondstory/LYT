/*

Copyright (c) 2013 - Philippe Laulheret, Second Story [http://www.secondstory.com]

This code is protected under MIT license. 
For more information visit  : https://github.com/secondstory/LYT

*/

#pragma once
#include "Global.h"


class InteractiveObject
{
  protected:
    int _defaultLife;
  public:
    InteractiveObject();
    int life;
    
    int x,y;   //0,0 is bottom left
    virtual void draw();
    virtual void update();
    virtual void onUpdate() { }
//    virtual void onDraw() { }
    void reset(int x, int y);
    virtual void onReset();
    void setDefaultLife(int defaultVal) ;
};


class PlusObject : public InteractiveObject
{
  private:
    float brightness;
    
    float hueAdjust = 10;
    float satAdjust = -5;

  public:
  PlusObject();
  virtual void draw();
  virtual void onUpdate();
  virtual void onReset();
};
