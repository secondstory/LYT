#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <OSCBundle.h>
#include <OSCData.h>
#include <OSCMatch.h>
#include <OSCMessage.h>

#include <libwebsockets.h>


#define N_PANNEL 3
IPAddress ip[N_PANNEL]; 
//pthread_mutex_t mutex[N_PANNEL];
//pthread_mutex_t color_mutex;
EthernetUDP udp[N_PANNEL];

int udpPort=3333;

EthernetUDP _udpIn;
char colors[8*N_PANNEL] = "#000000,#000000,#000000";



          //-----------------------------------------------------------
          // Process recieved message here.... 
          //------------------------------------------------------------
void processTouch(char* in) {
  
 
          char* pch;
          pch = strtok((char*) in,"-");
          
          int id = 0;
          
          int x = 0;
          int y = 0;
          int pannel = 0;
          if (pch) 
          { 
            x = atoi(pch);
            pch = strtok(0,"-");
           }
           if (pch) 
          { 
            y = atoi(pch);
            pch = strtok(0,"-");
           }
           if (pch) pannel = atoi(pch);
          
          //Serial.println(x); 
          //Serial.println(y);
          //Serial.println(pannel); 
          
          if ((pannel >=0) && (pannel < N_PANNEL))
          {
            OSCMessage m("/touch");
            m.add(id);
             m.add(x);
            m.add(y);
           
            lwsl_notice("Message should be sent");
           // pthread_mutex_lock(&mutex[pannel]);
            udp[pannel].beginPacket(ip[pannel],udpPort);
            m.send(udp[pannel]);
            udp[pannel].endPacket();
            //pthread_mutex_unlock(&mutex[pannel]);
          }

}

/*
 *
 * Copyright (C) 2010-2011 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */
#ifdef CMAKE_BUILD
#include "lws_config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>


#include <syslog.h>


#include <signal.h>



int force_exit = 0;



enum demo_protocols {
	/* always first */
	PROTOCOL_HTTP = 0,

	PROTOCOL_DUMB_INCREMENT,
	

	/* always last */
	DEMO_PROTOCOL_COUNT
};


#define LOCAL_RESOURCE_PATH "/home/root/srv/"

/*
 * We take a strict whitelist approach to stop ../ attacks
 */

struct serveable {
	const char *urlpath;
	const char *mimetype;
}; 

static const struct serveable whitelist[] = {
	{ "/favicon.ico", "image/x-icon" },
	{ "/img/2s.png", "image/png" },
        { "/img/splash.png", "image/png" },
        { "/css/phone.css", "text/css" },
        { "/js/jquery-2.0.3.min.js", "application/javascript" },
        { "/js/phone.js", "application/javascript" },
        { "/js/pixel.js", "application/javascript" },
        { "/js/pixelView.js", "text/html" },
        { "/js/socketController.js", "application/javascript" },

	/* last one is the default served if no match */
	{ "/index.html", "text/html" },
};

/* this protocol server (always the first one) just knows how to do HTTP */

static int callback_http(struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason, void *user,
							   void *in, size_t len)
{
#if 0
	char client_name[128];
	char client_ip[128];
#endif
	char buf[256];
	int n;

        //lwsl_notice((char*) in);
        //sprintf(buf,"Reason:%d",reason);
        //lwsl_notice(buf);
	switch (reason) {
        
	case LWS_CALLBACK_HTTP:
              lwsl_notice("LWS_CALLBACK_HTTP");
              
		for (n = 0; n < (sizeof(whitelist) / sizeof(whitelist[0]) - 1); n++)
			if (in && strcmp((const char *)in, whitelist[n].urlpath) == 0)
				break;

		sprintf(buf, LOCAL_RESOURCE_PATH"%s", whitelist[n].urlpath);
                
		if (libwebsockets_serve_http_file(context, wsi, buf, whitelist[n].mimetype))
			return 1; /* through completion or error, close the socket */

		/*
		 * notice that the sending of the file completes asynchronously,
		 * we'll get a LWS_CALLBACK_HTTP_FILE_COMPLETION callback when
		 * it's done
		 */

		break;

	case LWS_CALLBACK_HTTP_FILE_COMPLETION:
//		lwsl_info("LWS_CALLBACK_HTTP_FILE_COMPLETION seen\n");
		/* kill the connection after we sent one file */
		lwsl_notice("LWS_FILE_COMPLETION");
              return 1;

	/*
	 * callback for confirming to continue with client IP appear in
	 * protocol 0 callback since no websocket protocol has been agreed
	 * yet.  You can just ignore this if you won't filter on client IP
	 * since the default uhandled callback return is 0 meaning let the
	 * connection continue.
	 */

	case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:

		/* if we returned non-zero from here, we kill the connection */
		break;


	default:
		break;
	}

	return 0;
}

/* dumb_increment protocol */

/*
 * one of these is auto-created for each connection and a pointer to the
 * appropriate instance is passed to the callback in the user parameter
 *
 * for this example protocol we use it to individualize the count for each
 * connection.
 */

struct per_session_data__dumb_increment {
	int number;
};

static int
callback_dumb_increment(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	int n;
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 +
						  LWS_SEND_BUFFER_POST_PADDING];
	unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	struct per_session_data__dumb_increment *pss = (struct per_session_data__dumb_increment *)user;

  
	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		lwsl_info("callback_dumb_increment: "
						 "LWS_CALLBACK_ESTABLISHED\n");
		pss->number = 0;
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
              
		n = sprintf((char *)p, "%s", colors);
		n = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
		if (n < 0) {
			lwsl_err("ERROR %d writing to socket\n", n);
			return 1;
		}
		/*if ( pss->number == 50) {
			lwsl_info("close tesing limit, closing\n");
			return -1;
		}*/
		break;

	case LWS_CALLBACK_RECEIVE:
		//fprintf(stderr, "rx %d\n", (int)len

               processTouch((char*) in); 		


		break;


	default:
		break;
	}

	return 0;
}



/* list of supported protocols and callbacks */

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */

	{
		"http-only",		/* name */
		callback_http,		/* callback */
		0,			/* per_session_data_size */
		0,			/* max frame size / rx buffer */
	},
	{
		"touchserver-protocol",
		callback_dumb_increment,
		sizeof(struct per_session_data__dumb_increment),
		128,
	},

	{ NULL, NULL, 0, 0 } /* terminator */
};

void sighandler(int sig)
{
	force_exit = 1;
}

static struct option options[] = {
	{ "help",	no_argument,		NULL, 'h' },
	{ "debug",	required_argument,	NULL, 'd' },
	{ "port",	required_argument,	NULL, 'p' },
	{ "ssl",	no_argument,		NULL, 's' },
	{ "interface",  required_argument,	NULL, 'i' },
	{ "closetest",  no_argument,		NULL, 'c' },
#ifndef LWS_NO_DAEMONIZE
	{ "daemonize", 	no_argument,		NULL, 'D' },
#endif
	{ NULL, 0, 0, 0 }
};

int initWebsocket()
{
	int n = 0;
	int use_ssl = 0;
	struct libwebsocket_context *context;
	int opts = LWS_SERVER_OPTION_SKIP_SERVER_CANONICAL_NAME;
	char interface_name[128] = "wlan0";
	const char *iface = NULL;

	unsigned int oldus = 0;
	struct lws_context_creation_info info;

	int debug_level = 7;

	int daemonize = 0;


	memset(&info, 0, sizeof info);
	info.port = 80;

	

	signal(SIGINT, sighandler);
        int syslog_options =  LOG_PID | LOG_PERROR;

	/* we will only try to log things according to our debug_level */
	setlogmask(LOG_UPTO (LOG_DEBUG));
	openlog("lwsts", syslog_options, LOG_DAEMON);


	/* tell the library what debug level to emit and to send it to syslog */
	lws_set_log_level(debug_level, lwsl_emit_syslog);




	info.iface = iface;
	info.protocols = protocols;


	info.extensions = libwebsocket_get_internal_extensions();

	
  	info.ssl_cert_filepath = NULL;
  	info.ssl_private_key_filepath = NULL;

	info.gid = -1;
	info.uid = -1;
	info.options = opts;

	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		lwsl_err("libwebsocket init failed\n");
		return -1;
	}

	n = 0;
	while (n >= 0 && !force_exit) {
		struct timeval tv;

		gettimeofday(&tv, NULL);

		/*
		 * This provokes the LWS_CALLBACK_SERVER_WRITEABLE for every
		 * live websocket connection using the DUMB_INCREMENT protocol,
		 * as soon as it can take more packets (usually immediately)
		 */

		if (((unsigned int)tv.tv_usec - oldus) > 50000) {
			libwebsocket_callback_on_writable_all_protocol(&protocols[PROTOCOL_DUMB_INCREMENT]);
			oldus = tv.tv_usec;

		}


		/*
		 * If libwebsockets sockets are all we care about,
		 * you can use this api which takes care of the poll()
		 * and looping through finding who needed service.
		 *
		 * If no socket needs service, it'll return anyway after
		 * the number of ms in the second argument.
		 */

		n = libwebsocket_service(context, 50);

                loop(); // call loop as we have took over the execution flow :[


	}



	libwebsocket_context_destroy(context);

	lwsl_notice("libwebsockets-test-server exited cleanly\n");

	closelog();


	return 0;
}





void setup() {
  // put your setup code here, to run once:
  system("/home/root/startAP");
  //delay(5000);  
  ip[0] = IPAddress(192, 168, 0, 11);
  ip[1] = IPAddress(192, 168, 0, 12);
  ip[2] = IPAddress(192, 168, 0, 13) ;
  
  for (int i=0; i < N_PANNEL; i++)
  {
    udp[i].begin(8888+i);
    //pthread_mutex_init(&mutex[i],0); //init the mutex for UDP sending;  
  }
 // pthread_mutex_init(&color_mutex,0);
  
  _udpIn.begin(3333);
  _udpIn.listen();
  
  //pthread magic !
   pthread_t oscThread;
   pthread_create(&oscThread,NULL,osc_loop,0);
  
  //pthread magic end
    
  initWebsocket();  
    
    
}


int getNextMessage(OSCBundle &b, int* from)
{
  
  int bytes  = _udpIn.parsePacket();
  if (bytes)
  {
    unsigned char *buffer = new unsigned char[bytes];
    bytes = _udpIn.read(buffer,bytes);
    //Serial.println(bytes);
    if (bytes > 1024) { 
      Serial.println("Overload warning !");
      Serial.println(bytes);
    }
    
    *from = -1;
    
    for (int i=0; i < N_PANNEL; i++)
    {
        if (ip[i] == _udpIn.remoteIP()) { *from = i; break; }
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


//This is an hackish way of processing OSC Message
// There is no synchronization between the main thread and this one
// So... If it looks bad on the phone, don't use it :)
void *osc_loop(void *dummy)
{
  
  while (!force_exit) {
  OSCBundle b;
  OSCMessage *m;
  
  int from = -1;
  while (getNextMessage(b,&from))
  {
    if (m = b.getOSCMessage(0))
    {
      from = m->getInt(0);
      if (from != -1) { 
      
       int index = 8*from;
      sprintf(colors + index+1,"%06x",m->getInt(1));
      if (from != N_PANNEL -1) colors[index+7] = ',';
     // sleep(5);
      //Serial.println((char*) colors);
       
      }
    }
    b.empty();
  }
  
  sleep(1);
  }
  
  return 0;
}


void loop()
{
  
  /*
  OSCBundle b;
  OSCMessage *m;
  
  int from = -1;
  while (getNextMessage(b,&from))
  {
    if (m = b.getOSCMessage(0))
    {
      from = m->getInt(0);
      if (from != -1) { 
      
       int index = 8*from;
      sprintf(colors + index+1,"%06x",m->getInt(1));
      if (from != N_PANNEL -1) colors[index+7] = ',';
      //Serial.println((char*) colors);
       
      }
    }
    b.empty();
  }
    */
  
}

