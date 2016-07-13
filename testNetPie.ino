/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <MicroGear.h>
//#include "CmmcMonoLedMartix.h"
#include "LedControl.h"

const char* ssid     = "ESPERT-002";
const char* password = "espertap";

#define APPID   "Led8x8Mono"
#define KEY     "1tTpLMgmm8804hg"
#define SECRET  "Eame0VMT3fqlzTlzMDQpGvn6U"
#define ALIAS   "Led8x8"

WiFiClient client;
AuthClient *authclient;

/***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.

  LedControl lc=LedControl(12,11,10,1);
*/
LedControl lc = LedControl(15, 0, 13, 1);

int data[8] = {0, 252, 114, 243, 127, 243, 114, 252};
int data2D[8][8];
int data2D0R[8][8];
int data2D90R[8][8];
int data2D180R[8][8];
int data2D270R[8][8];
//CmmcMonoLedMartix ledX;

int timer = 0, count, showT = 0;
MicroGear microgear(client);
String stringTwo = "";
String datatoSend = "", aliasTx = "";
char cstr[100];
char cstr2[100];

//
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;






#define LED_PIN 2
#define ACCELEROMETER_SENSITIVITY 16384

bool blinkState = false;
unsigned long time_now, time_prev_control;
float accelRaw[3];
//





void convertTo2D();
void   rotea90();
void   to1D();
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
      if (y == 8) {
        Serial.print("  last   --> ");
        for (int x = i + 1; x < msglen; x++) {
          char inChar = (char)msg[x];
          b += inChar;
        }
        Serial.println(b);
        b.toCharArray(cstr, 100);
        datatoSend = ALIAS;
        datatoSend = String(datatoSend + " receive data: " + (char *)msg); // concatenating two strings
        datatoSend.toCharArray(cstr2, 100);
        microgear.chat(cstr, "100,19");
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

  convertTo2D();//keep in data2D[][]
  
  for (int i = 0; i < 8; i++) {
    for (int j = 8 - 1; j >= 0; j--) {
      data2D0R[i][j] = data2D[i][j];

    }
  }
  // make pictrue 90' in  data2D90R
  // 180' in  data2D90R
  //270' in  data2D270R
  for (int bo = 0; bo < 3 ; bo++) {
    rotea90();
    for (int i = 0; i < 8; i++) {
      for (int j = 8 - 1; j >= 0; j--) {
        if (bo == 0) {
          data2D90R[i][j] = data2D[i][j];
        }
        if (bo == 1) {
          data2D180R[i][j] = data2D[i][j];
        }
        if (bo == 2) {
          data2D270R[i][j] = data2D[i][j];
        }
      }
    }
  }
  // showT++;
  //for (int i = 0; i < showT; i++) {
  //  rotea90();
  // }
  //to1D();
  //Serial.println(cstr);
  Serial.println(data[0]);
  Serial.println(data[1]);
  Serial.println(data[2]);
  Serial.println(data[3]);
  Serial.println(data[4]);
  Serial.println(data[5]);
  Serial.println(data[6]);
  Serial.println(data[7]);

  //

  //show data
  //  lc.setRow(0, 0, (byte)data[0]);
  //  lc.setRow(0, 1, (byte)data[1]);
  //  lc.setRow(0, 2, (byte)data[2]);
  //  lc.setRow(0, 3, (byte)data[3]);
  //  lc.setRow(0, 4, (byte)data[4]);
  //  lc.setRow(0, 5, (byte)data[5]);
  //  lc.setRow(0, 6, (byte)data[6]);
  //  lc.setRow(0, 7, (byte)data[7]);
  for (int col = 0; col < 8; col++) {
    for (int row = 0; row < 8; row++) {
      lc.setLed(0, col, row, data2D0R[col][row]);
    }
  }

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

  //Serial.begin(115200);
  //


  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(115200);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");



  // configure Arduino LED for
  pinMode(LED_PIN, OUTPUT);



  //
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

  //ledX.begin(0x20);

  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);
  //
  //  lc.setRow(0, 0, (byte)data[0]);
  //  lc.setRow(0, 1, (byte)data[1]);
  //  lc.setRow(0, 2, (byte)data[2]);
  //  lc.setRow(0, 3, (byte)data[3]);
  //  lc.setRow(0, 4, (byte)data[4]);
  //  lc.setRow(0, 5, (byte)data[5]);
  //  lc.setRow(0, 6, (byte)data[6]);
  //  lc.setRow(0, 7, (byte)data[7]);
  convertTo2D();//keep in data2D[][]
  for (int i = 0; i < 8; i++) {
    for (int j = 8 - 1; j >= 0; j--) {
      data2D0R[i][j] = data2D[i][j];

    }
  }
  //
  for (int col = 0; col < 8; col++) {
    for (int row = 0; row < 8; row++) {
      lc.setLed(0, col, row, data2D0R[col][row]);
    }
  }

  //
  //

  for (int bo = 0; bo < 3 ; bo++) {
    rotea90();
    for (int i = 0; i < 8; i++) {
      for (int j = 8 - 1; j >= 0; j--) {
        if (bo == 0) {
          data2D90R[i][j] = data2D[i][j];
        }
        if (bo == 1) {
          data2D180R[i][j] = data2D[i][j];
        }
        if (bo == 2) {
          data2D270R[i][j] = data2D[i][j];
        }
      }
    }
  }


}

void loop() {
  /* To check if the microgear is still connected */
  if (microgear.connected()) {
    //Serial.println("connected");

    /* Call this method regularly otherwise the connection may be lost */
    microgear.loop();
    //
    // read raw accel/gyro measurements from device
    time_now = millis();

    if (time_now - time_prev_control >= 100) {
      time_prev_control = time_now;
      accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

      accelRaw[0] = ax - 700;
      accelRaw[1] = ay - 100;
      accelRaw[2] = az + 1300;

      accelRaw[0]  /= ACCELEROMETER_SENSITIVITY;
      accelRaw[1] /= ACCELEROMETER_SENSITIVITY;
      accelRaw[2] /= ACCELEROMETER_SENSITIVITY;

      Serial.print(accelRaw[0]); Serial.print("\t");
      Serial.print(accelRaw[1]); Serial.print("\t");
      Serial.print(accelRaw[2]); Serial.println("\t");
      //check rotate led8x8 by accelRaw
      /*
       * state   
       * 
       * angle   | accelRaw[0] | accelRaw[1] | accelRaw[2]
       *         |             |             |
       *  0      |      1      |       0     |      0
       *         |             |             |
       *  90'    |      0      |       1     |      0
       *         |             |             |
       *  180'   |      -1     |       0     |      0
       *         |             |             |
       *  270'   |       0     |      -1     |      0
       *         |             |             |
       *         |             |             |
       *  
       */
      if ((accelRaw[0] < 0.1) && (accelRaw[0] > -0.25) && (accelRaw[1] > 0.90) && (accelRaw[2] < 0.25) && (accelRaw[2] > -0.25)) {//90แก้ไข วางMPU6050บนPCBกลับหัว
        for (int col = 0; col < 8; col++) {
          for (int row = 0; row < 8; row++) {
            lc.setLed(0, col, row, data2D90R[col][row]);
          }
        }
      } else if ((accelRaw[0] < -0.8) && (accelRaw[1] <  0.25) && (accelRaw[1] > -0.25) && (accelRaw[2] < 0.25) && (accelRaw[2] > -0.25)) {//180  แก้ไข วางMPU6050บนPCBกลับหัว
        for (int col = 0; col < 8; col++) {
          for (int row = 0; row < 8; row++) {
            lc.setLed(0, col, row, data2D0R[col][row]);
          }
        }
      } else if ((accelRaw[0] < 0.1) && (accelRaw[0] >  -0.25) && (accelRaw[1] < -0.80) && (accelRaw[2] < 0.25) && (accelRaw[2] > -0.25)) { //270 แก้ไข วางMPU6050บนPCBกลับหัว
        for (int col = 0; col < 8; col++) {
          for (int row = 0; row < 8; row++) {
            lc.setLed(0, col, row, data2D270R[col][row]);
          }
        }
      } else if ((accelRaw[0] > 0.8) && (accelRaw[1] <  0.25) && (accelRaw[1] > -0.25) && (accelRaw[2] < 0.25) && (accelRaw[2] > -0.25)) {// 0แก้ไข วางMPU6050บนPCBกลับหัว
        for (int col = 0; col < 8; col++) {
          for (int row = 0; row < 8; row++) {
            lc.setLed(0, col, row, data2D180R[col][row]);
          }
        }
      }


      // blink LED to indicate activity
      blinkState = !blinkState;
      digitalWrite(LED_PIN, blinkState);
    }

    //ledX.showPic8bit(data);
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
    //if (timer >= 5000) {
    microgear.connect(APPID);
    //timer = 0;
    // }
    //else timer += 100;
  }
  // delay(100);
}

void convertTo2D() {


  for (int j = 0; j < 8; j++) {
    //#####

    for (int i = 0; i < 8; i++) {

      data2D[j][i] = bitRead(data[j], 7 - i);

      //

    }//i
  }//j

}

void rotea90() {
  //  for (int i = 0; i < 8; i++) {
  //    for (int j = 8 - 1; j >= 0; j--) {
  //      data2D[i][j] = data2D[j][i];
  //    }
  //  }
  //

  //  for (int j = 0; j < 8; ++j) {
  //        for (int i = 0; i < 8; ++i) {
  //            data2D[j][i] = data2D[7-i][j];
  //        }
  //    }

  // Transpose the matrix
  for ( int i = 0; i < 8; i++ ) {
    for ( int j = i + 1; j < 8; j++ ) {
      int tmp = data2D[i][j];
      data2D[i][j] = data2D[j][i];
      data2D[j][i] = tmp;
    }
  }

  // Swap the columns
  for ( int i = 0; i < 8; i++ ) {
    for ( int j = 0; j < 8 / 2; j++ ) {
      int tmp = data2D[i][j];
      data2D[i][j] = data2D[i][8 - 1 - j];
      data2D[i][8 - 1 - j] = tmp;
    }
  }
  //

  //
}//


void to1D() {
  for (int j = 0; j < 8; j++) {
    //#####

    for (int i = 0; i < 8; i++) {
      int ff = pow(2, i);
      //data2D[j][i] = bitRead(data[j], i);
      data[j] += data2D[j][i] * ff;

      //

    }//i
  }//j
}

