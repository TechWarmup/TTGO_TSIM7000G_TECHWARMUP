
/* Code By : VIKAS CHOUDHARY "TECH WARMUP" */

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#define RXD2 26    //VVM501 MODULE RXD INTERNALLY CONNECTED
#define TXD2 27    //VVM501 MODULE TXD INTERNALLY CONNECTED
#define powerPin 4 ////VVM501 MODULE ESP32 PIN D4 CONNECTED TO POWER PIN OF A7000 CHIPSET, INTERNALLY CONNECTED

// Select your modem:
#define TINY_GSM_MODEM_SIM7000   //SIMA7000 Compatible with SIM7000 AT instructions

// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
//#define BLYNK_HEARTBEAT 30


#include <BlynkSimpleTinyGSM.h>
#include <DHT.h>
#include <Arduino.h>
#include <Wire.h>
int rx = -1;
#define SerialAT Serial1
String rxString;
int _timeout;
String _buffer;

BlynkTimer timer;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "";
char user[] = "";
char pass[] = "";



bool reply = false;

TinyGsm modem(SerialAT);
#define Sensor_PIN 32  //D15-DHT22, REFER CONNECTION DIAGRAM AT TOP OF THIS PAGE
#define DHTTYPE DHT11
DHT  dht(Sensor_PIN, DHTTYPE);



void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println("%");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("*C");
  SerialAT.print("Humidity: ");
  SerialAT.print(h);
  SerialAT.println("%");
  SerialAT.print("Temperature: ");
  SerialAT.print(t);
  SerialAT.println("*C");
  delay(1000);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  Blynk.virtualWrite(V0,t); // WRITING TEMPERATURE VALUE TO VIRTUAL PIN V0 IN BLYNK
  Blynk.virtualWrite(V1,h); // WRITING HUMIDITY VALUE TO VIRTUAL PIN V1 IN BLYNK

}




void setup() {
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  delay(1000);
  digitalWrite(powerPin, LOW);
  delay(1000);
  Serial.begin(115200);
  delay(100);
  SerialAT.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(1000);
  dht.begin();
  delay(2000);
  Serial.println("Modem Reset, Please Wait");
  SerialAT.println("AT+CRESET");
  delay(1000);
  SerialAT.println("AT+CRESET");
  delay(2000);
  SerialAT.flush();

  Serial.println("Echo Off");
  SerialAT.println("ATE0");   //120s
  delay(1000);
  SerialAT.println("ATE0");   //120s
  rxString = SerialAT.readString();
  Serial.print("Got: ");
  Serial.println(rxString);
  rx = rxString.indexOf("OK");
  if (rx != -1)
    Serial.println("Modem Ready");
  delay(1000);

  Serial.println("SIM card check");
  SerialAT.println("AT+CPIN?"); //9s
  rxString = SerialAT.readString();
  Serial.print("Got: ");
  Serial.println(rxString);
  rx = rxString.indexOf("+CPIN: READY");
  if (rx != -1)
    Serial.println("SIM Card Ready");
  delay(1000);

  String name = modem.getModemName();
  delay(500);
  Serial.println("Modem Name: " + name);
  Serial.println("Hello from TECHWARMUP/ Subscribe the Channel ");

  Blynk.begin(auth, modem, apn, user, pass);
  // Setup a function to be called every second
  timer.setInterval(2000L, sendSensor);
}

void loop()
{

  Blynk.run();
  timer.run();

}
