#define BLYNK_TEMPLATE_ID "TMPL3jmQE7Wkh"
#define BLYNK_TEMPLATE_NAME "SmartCraddle"
#define BLYNK_AUTH_TOKEN "FhprJZVdQ_4BL5QXuLSDv24_IP1m6xux"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h>

//wifi credentials
const char* ssid = "vivo Y33T";
const char* pass = "varshi11";
const char* auth ="FhprJZVdQ_4BL5QXuLSDv24_IP1m6xux";
#define SOUND_SENSOR 35 // Sound sensor pin
#define SERVO_PIN 23  // Servo motor pin
#define MOISTURE_SENSOR 34 // Moisture sensor pin
#define DHTPIN 5       // DHT sensor connected to GPIO 4
#define RELAY_PIN 21    // Relay control pin
#define DHTTYPE DHT11   // Change to DHT22 if using that model

DHT dht(5,DHT11);
Servo motor;
int moisture=34;

int soundThreshold = 100; // Adjust based on sensitivity
int moistureThreshold = 60; // Adjust based on baby pee detection 

//BLYNK_WRITE(V5){
//int state=param.asInt();
//if(state==1)
//digitalWrite(Fan,1);
//else
//digitalWrite(Fan,0);
//}
BLYNK_WRITE(V0){
  int state=param.asInt();
  if(state==1)
    rockcradle();
  
}
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Blynk.begin(auth, ssid, pass);
dht.begin();
motor.attach(23);
pinMode(moisture,INPUT);
pinMode(RELAY_PIN, OUTPUT);
digitalWrite(RELAY_PIN, LOW);  // Ensure relay is OFF initially
}

void loop() {
// put your main code here, to run repeatedly:
//rockcradle();
checkmoisture();
checkenvironment();
fancontrol();
detectcry();
}

void rockcradle()
{
  for(int i=0;i<=10;i++)
  {
  motor.write(35);
  delay(1000);
  motor.write(0);
  delay(1000);
}
}

void detectcry()
{
    int soundLevel = analogRead(SOUND_SENSOR);
    Serial.println(soundLevel);
    if (soundLevel > soundThreshold) {
        rockcradle();
        Blynk.logEvent("Baby_cry","Baby is crying! Cradle rocking activated.");
}
}
void checkmoisture() 
{
float m=analogRead(moisture);
m=4095-m;
float p=(m/4095)*100;
Serial.print(p);
Serial.println(" %wet");
Blynk.virtualWrite(V1,p);
if (p > moistureThreshold) {
  Blynk.logEvent("Dampness_alert","Diaper needs changing! Moisture detected.");
//delay(7000);

}
}
void checkenvironment()
{
  float h=dht.readHumidity();
  float t=dht.readTemperature();
  if(isnan(h)||isnan(t))
  {
    Serial.println("DHT error");
    return;
  }
  Serial.print("H: ");
  Serial.print(h);
  Serial.print(",T: ");
  Serial.println(t);
   if (!isnan(t) && !isnan(h)) {
        Blynk.virtualWrite(V3, t);
        Blynk.virtualWrite(V2, h);
  delay(2000);
}
}
void fancontrol()
{
  float t= dht.readTemperature();  // Read temperature in Celsius

  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }

  //Serial.print("Temperature: ");
  Serial.print(t);
  //Serial.println(" °C");
   

  if (t > 28) {  // Change threshold as needed
    digitalWrite(RELAY_PIN, HIGH);
    //Blynk.virtualWrite(V1, on);*   // Turn OFF relay LED (Green LED OFF)
    //Serial.println("Relay OFF - Green LED OFF");
  } else {
   digitalWrite(RELAY_PIN, LOW);  // Turn ON relay LED (Green LED ON)
    //Serial.println("Relay ON - Green LED On");

  }

  delay(2000);  // Delay for stable readings
}
