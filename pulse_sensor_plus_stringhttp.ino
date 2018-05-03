/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;



 
 volatile int Signal;  
 
 unsigned long last_time_signal_high=0;
 unsigned long time_signal_high=0;
int delta_time=0;
int buffer_array=0;
int  BPM[10];
int flag=0;

 int sum_BPM=0;
   int PBPM=0;
   int temperature=0;
   int spo2=0;

   String string_pulse;
   String string_temperature;
   String string_spo2;
     

int flag_button=0;

void setup() {

 

    USE_SERIAL.begin(115200);

    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("spitfire", "supermarine");

         pinMode(D3, INPUT);

           USE_SERIAL.println("MODULO ENCENDIDO");
  USE_SERIAL.println("PULSE BOTON PARA RITMO CARDIACO");
      

}


 // delay(2000);

void loop() {
    

  int buttonState = digitalRead(D3); 
  if (buttonState==LOW && flag_button==0)
 {
  pulse_sensor();
    send_wifi("Pulso(PPM)",string_pulse);
  flag_button=1;

 }

   buttonState =digitalRead(D3); 
  if (buttonState==LOW && flag_button==1)
  {
  temperature_sensor();
    send_wifi("Temperatura(C)", string_temperature);
  flag_button+=1;
  }

  buttonState = digitalRead(D3); 
  if (buttonState==LOW && flag_button==2)
  {
  spo2_sensor();
   send_wifi("Oxigeno(%)", string_spo2);
  flag_button=0;
  }
  


    delay(100);
}


int pulse_sensor()
{

 //USE_SERIAL.println("COLOQUE EL DEDO INDICE SOBRE EL SENSOR Y ESPERE A QUE EL LED ENCIENDA 10 VECES");
 delay(3000);
  /*

 while(buffer_array<10)
  {
    
   Signal = analogRead(A0);
    
  if (Signal > 600 && flag==0 && buffer_array<10)
{
  flag=1;
   time_signal_high=millis();
  digitalWrite(LED_BUILTIN, LOW);
 
delta_time= (time_signal_high - last_time_signal_high);



BPM[buffer_array]=((60000/delta_time));
buffer_array+=1;


if (delta_time>900)
{
  buffer_array=0;
}

last_time_signal_high = time_signal_high; 

}

else if (Signal < 600 && flag==1 && buffer_array<10)
{
    flag=0;
    digitalWrite(LED_BUILTIN, HIGH);
  }

delay(5);
  }

    for(int i=3;i<8;i++)
    {
    sum_BPM+=BPM[i];
    }
    PBPM=(sum_BPM/5);


  Serial.println("PRUEBA TERMINADA:");
      Serial.println("BPM:");
      Serial.println(PBPM);
       
      buffer_array=0;
    sum_BPM=0;
*/

  PBPM=100;
string_pulse=String(PBPM);
  
   USE_SERIAL.println("PRUEBA TERMINADA:");
      USE_SERIAL.println("BPM:");
     USE_SERIAL.println(PBPM);
    
 }

 void temperature_sensor()
 {
  delay(3000);
  temperature=28;
  string_temperature=String(temperature);
   USE_SERIAL.println("PRUEBA TERMINADA:");
      USE_SERIAL.println("TEMP:");
     USE_SERIAL.println(temperature);
 }
 
void spo2_sensor()
{
   delay(3000);
  spo2=95;
    string_spo2=String(spo2);
     USE_SERIAL.println("PRUEBA TERMINADA:");
      USE_SERIAL.println("spo2:");
     USE_SERIAL.println(spo2);
}

//void send_wifi()
void send_wifi(String sensor_name,String data_sensor)
{

// wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
      //  http.begin("http://192.168.137.2/test2.php?sensor=pulso&heartbeats="+ string_pulse +""); //HTTP
          http.begin("http://192.168.137.2/test2.php?sensor="+ sensor_name +"&heartbeats="+ data_sensor +""); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
}











