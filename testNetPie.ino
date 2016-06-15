/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "CmmcMonoLedMartix.h"

const char* ssid     = "ESPERT-002";
const char* password = "espertap";

#define APPID   "Led8x8Mono"
#define KEY     "1tTpLMgmm8804hg"
#define SECRET  "Eame0VMT3fqlzTlzMDQpGvn6U"
#define ALIAS   "Led8x8"

WiFiClient client;
AuthClient *authclient;

int data[8] = {0, 252, 114, 243, 127, 243, 114, 252};

CmmcMonoLedMartix ledX;

int timer = 0, count;
MicroGear microgear(client);
String stringTwo = "";
String datatoSend = "", aliasTx = "";
char cstr[100];
char cstr2[100];

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  //Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  //Serial.println((char *)msg);
  Serial.print("  topic --> ");
  Serial.println((char *)topic);
  String b = "";
  Serial.print("Incoming message --> ");
  Serial.println((char *)msg);

  //    data[0] = b.substring((b.indexOf('a') + 1), b.indexOf('b')).toInt();
  //    data[1] = b.substring((b.indexOf('b') + 1), b.indexOf('c')).toInt();
  //    data[2] = b.substring((b.indexOf('c') + 1), b.indexOf('d')).toInt();
  //    data[3] = b.substring((b.indexOf('d') + 1), b.indexOf('e')).toInt();
  //
  //    data[4] = b.substring((b.indexOf('e') + 1), b.indexOf('f')).toInt();
  //    data[5] = b.substring((b.indexOf('f') + 1), b.indexOf('g')).toInt();
  //    data[6] = b.substring((b.indexOf('g') + 1), b.indexOf('h')).toInt();
  //    data[7] = b.substring((b.indexOf('h') + 1), b.indexOf('i')).toInt();
  int y = 0;
  for (int i = 0; i < msglen; i++) {
    char inChar = (char)msg[i];
    b += inChar;
    if (msg[i] ==  44 ) {
      if (y < 8) {
        data[y] = b.toInt();
        b = "";
      }
      y++;
      // try sendback to who sending
      if (y > 7) {
        Serial.print("  last   --> ");
        for (int x = i+1; x < msglen; x++) {
          char inChar = (char)msg[x];
          b += inChar;
        }
        Serial.println(b);
        b.toCharArray(cstr, 100);
        datatoSend = ALIAS;
        datatoSend = String(datatoSend + " receive data"); // concatenating two strings
        datatoSend.toCharArray(cstr2, 100);
        microgear.chat(cstr,cstr2);
        b = "";
      }




      //      if (y == 8) {
      //        b.toCharArray(cstr, 100);
      //        microgear.chat(cstr, "ok");
      //        //y = 0;
      //        b = "";
      //      }


      //      if (y == 9) {
      //        y = 0;
      //      }
    }
  }

  //  data[0] = (int)msg[0];
  //  data[1] = (int)msg[1];
  //  data[2] = (int)msg[2];
  //  data[3] = (int)msg[3];
  //
  //  data[4] = (int)msg[4];
  //  data[5] = (int)msg[5];
  //  data[6] = (int)msg[6];
  //  data[7] = (int)msg[7];

  //aliasTx = b.substring((b.indexOf('i') + 1), b.indexOf('j'));

  //respond to who send income
  //b.toCharArray(cstr,100);
  //aliasTx.toCharArray(cstr2,100);

  //Serial.println(cstr);
  Serial.println(data[0]);
  Serial.println(data[1]);
  Serial.println(data[2]);
  Serial.println(data[3]);
  Serial.println(data[4]);
  Serial.println(data[5]);
  Serial.println(data[6]);
  Serial.println(data[7]);


  //microgear.chat(, cstr);
}
/*
   if Found new member in APPLICATION
*/
void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}
/*
   If some menber in APPLICATION  offline
*/
void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Connected to NETPIE...");
  Serial.print("     attribute");
  Serial.print((char*)attribute);
  Serial.print("     uint8_t* msg");
  Serial.println((char*)msg);
  /* Set the alias of this microgear ALIAS */
  microgear.setName(ALIAS);
}


void setup() {
  /* Add Event listeners */

  /* Call onMsghandler() when new message arraives */
  microgear.on(MESSAGE, onMsghandler);

  /* Call onFoundgear() when new gear appear */
  microgear.on(PRESENT, onFoundgear);

  /* Call onLostgear() when some gear goes offline */
  microgear.on(ABSENT, onLostgear);

  /* Call onConnected() when NETPIE connection is established */
  microgear.on(CONNECTED, onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");

  /* Initial WIFI, this is just a basic method to configure WIFI on ESP8266.                       */
  /* You may want to use other method that is more complicated, but provide better user experience */
  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);

  ledX.begin(0x20);
}

void loop() {
  /* To check if the microgear is still connected */
  if (microgear.connected()) {
    //Serial.println("connected");

    /* Call this method regularly otherwise the connection may be lost */
    microgear.loop();
    ledX.showPic8bit(data);
    //        if (timer >= 1000) {
    //            Serial.println("Publish...");
    //            count++;
    //             stringTwo = String(count);
    //             datatoSend = String(stringTwo + " of Hello"); // concatenating two strings
    //             datatoSend.toCharArray(cstr,16);
    //
    //            /* Chat with the microgear named ALIAS which is myself */
    //            microgear.chat("myX",cstr);
    //            timer = 0;
    //        }
    //        else timer += 100;
  }/* end if (microgear.connected())*/
  else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }
  // delay(100);
}
