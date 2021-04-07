#define BLYNK_PRINT Serial
#include <SPI.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>
#include <DHT.h>


#define DHTPIN 2          // What digital pin we're connected to (PIN 2)
#define DHTTYPE DHT11     // DHT 11

// Get Auth Token in the Blynk App.
char auth[] = "";

// WiFi credentials.
char ssid[] = "";
char pass[] = "";


// Declare variables and constants
int sensorPin=A1;
int mSensor;
int hSensor;
float tSensor;


const int RELAY_PIN = A5;    // the Arduino pin A5, which connects to the IN pin of relay


BlynkTimer timer;            //Initialize BlynkTimer Library
DHT dht(DHTPIN, DHTTYPE);    //Initialize DHT Library



void setup()
{
  // Debug console
  Serial.begin(9600);        // Initialize serial
  Blynk.begin(auth, ssid, pass);
  
  dht.begin();
  
  pinMode(RELAY_PIN, OUTPUT);   // initialize analog pin A5 as an output.
  digitalWrite(RELAY_PIN,HIGH);  //initialize relay state as OFF

  // Setup a function to be called every m milliseconds defined. 5000 represents 5 seconds
  timer.setInterval(6000, TempHum); //DHT 11
  timer.setInterval(5000, Moisture); //Moisture Sensor
  timer.setInterval(120000, Things); //ThingSpeak delay 2 min
 
}


 //=============== Humidity and Temperature ===============
 
void TempHum()
{
  // Reads sensor value on Digital Pin 2 
  // Stores the 2 values in their respective variable. Humidity - hSensor. Temperature - tSensor.
  hSensor = dht.readHumidity(); 
  tSensor = dht.readTemperature();

 
  if (isnan(hSensor) || isnan(tSensor)) { // In case of abnormal readings from the sensor (not a number)
    Serial.println("Failed to read from DHT sensor!"); // Display error message on serial monitor
  }
  else{
  // Display readings on serial monitor
  Serial.println (String ("Humidity is : ") + hSensor); 
  Serial.println (String ("Temperature is : ") + tSensor);


  Blynk.virtualWrite(V4, hSensor); /* Sends Humidity reading stored in variable hSensor 
                                      to Virtual Pin V4 defined in the Blynk App*/
  Blynk.virtualWrite(V5, tSensor); /* Sends Temperature reading stored in variable tSensor 
                                      to Virtual Pin V5 defined in the Blynk App*/

    
  } 
}


 //=============== Soil Moisture ===============

void Moisture(){ 
  
  mSensor = analogRead(sensorPin); //Read sensor value on analog PIN A1
  mSensor = map(mSensor, 0, 1023, 1023, 0); // Mapping used for sensor calibration ( 0V=0 , +5V=1023 )

  if (isnan(mSensor)) { // In case of abnormal readings from the sensor (not a number)
  Serial.println("Failed to read from Soil Moisture sensor!"); // Display error message on serial monitor
  }
  else{
  
  Serial.println (String ("Moisture is : ") + mSensor); // Print moisture reading on serial monitor
  Blynk.virtualWrite(V6, mSensor); // Sends Moisture reading stored in variable sensorValue to Virtual Pin V6 (GAUGE) 
  }

  //=============== Automatic Irrigation Conditions ===============
  
  if(mSensor <=250){
  Blynk.notify("Plant needs water.. Activating water pump"); // Notification widget triggers notification message
  digitalWrite(RELAY_PIN, LOW); // Pump ON
  delay(3000); //Supply water for 3 seconds then turn off pump
  digitalWrite(RELAY_PIN, HIGH); // Pump OFF  
  }
  else if(mSensor >300){
    digitalWrite(RELAY_PIN, HIGH); // Pump OFF
  }
  
}

 //=============== Blynk to ThingSpeak ===============

void Things(){
  Blynk.virtualWrite(V0, hSensor, tSensor, mSensor); /* Virtual Pin V0 assigned to WebHook widget send data 
                                                        to ThingSpeak channel*/
}


void loop()
{
 Blynk.run(); 
 timer.run(); // initiates BlynkTimer
 
}
