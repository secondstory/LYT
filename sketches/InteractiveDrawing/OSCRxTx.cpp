/*

Copyright (c) 2013 - Philippe Laulheret, Second Story [http://www.secondstory.com]

This code is protected under MIT license. 
For more information visit  : https://github.com/secondstory/LYT

*/

#include "OSCRxTx.h"

#include "Global.h"

OSCRxTx::OSCRxTx(IPAddress ip,  int portOut, int portIn)
{

     _udpOut.begin(8888);
     
     _udpIn.begin(portIn);
     _udpIn.listen();
     _ip = ip;
     _portOut = portOut;
     
     
     
}

 void OSCRxTx::sendPixelBuffer(uint8_t *buffer, int nLine, int nCol)
 {
 
  for (int i=0; i < nLine; i++)
  {
    
    OSCMessage msg("/line/");
    msg.add(i);
     for (int j=0; j < 3*nCol; j++) 
     {
        msg.add((int) *buffer);
        buffer++;
     }
     _udpOut.beginPacket(_ip, _portOut);
     msg.send(_udpOut); // send the bytes to the SLIP stream
     _udpOut.endPacket(); // mark the end of the OSC Packet
     msg.empty(); // free space occupied by message
   }
 }
 
void OSCRxTx::sendCurrentColor()
{
    OSCMessage msg("/color");
    uint8_t r,g,b;
    getRGBFromHSB(r,g,b,current_hue,default_sat,default_brightness);
    uint32_t val = ((uint32_t )r << 16) | ((uint16_t) g <<  8) | b ;

    msg.add(boardID);
    msg.add((int) val);
     
    _udpOut.beginPacket(touchServerIp, _portOut);
    msg.send(_udpOut); // send the bytes to the SLIP stream  
    _udpOut.endPacket(); 
}

int OSCRxTx::getNextMessage(OSCBundle &b)
{
  
  int bytes  = _udpIn.parsePacket();
  if (bytes)
  {
    unsigned char *buffer = new unsigned char[bytes];
    bytes = _udpIn.read(buffer,bytes);
    if (bytes > 1024) { 
      Serial.println("Overload warning !");
      Serial.println(bytes);
    }
    
    if (bytes >0) 
    {
      if (buffer[0] == '#') b.fill(buffer,bytes);
      else 
      {
        OSCMessage *m = new OSCMessage(); 
        //Serial.print(bytes);
        m->fill(buffer,bytes);
        if (!m->hasError()) b.add(*m);
        else { 
          Serial.println("OSC Message seems to have Error ");
          delete m;
          delete[] buffer;
          return 0;
        }
        
      }
    }
    
    
   
    //Serial.println((const char*) buffer);
   

   
    
    delete[] buffer;
    delay(10);
    if (!b.hasError()) 
    { 
       //Serial.println("Buffer read yo");
       
      return 1;
      }
      
    Serial.println("OSC Message seems to have Error ");
    //m.empty();
    return 0;
    
  }
  return 0;
}
