#pragma once 

#include <OSCBundle.h>
#include <OSCMessage.h>

#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>





class OSCRxTx

{
  private:
    EthernetUDP _udpIn;
    EthernetUDP _udpOut;
    int _portOut;
    IPAddress _ip;
  public:

    OSCRxTx(IPAddress ip,  int portOut, int porIn);
    void sendPixelBuffer(uint8_t *buffer, int nLine, int nCol);
    int getNextMessage(OSCBundle &b);
    void sendCurrentColor();

};
