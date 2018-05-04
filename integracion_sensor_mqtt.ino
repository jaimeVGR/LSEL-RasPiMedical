#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>

// Librerias de MQTT
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

// Constantes MQTT
const char* ssid = "Raspy_jaime";
const char* password = "MUISE_LSEL";
const char* mqtt_server = "192.168.4.1";

WiFiClient espClient;
PubSubClient client(espClient);

// FIN MQTT

#define REPORTING_PERIOD_MS     1000
// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

// Variables a enviar mqtt
float temperatura = 0;
int pulso = 0;
int oxigeno = 0;
char msg[50];

// Variables sensores Nacho y Jaime
int variable, variable1 = 0;
float tempdedo = 26;
int aux, aux1 = 0;
uint32_t captura_spo2, captura_heart = 0;    

uint32_t tsLastReport = 0;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// COnfiguracion Wifi para MQTT
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Linea extra para solucionar error:
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    //Serial.println("Beat!");
}

void setup()
{
    // Configuracion de mensajes por monitor serie
    Serial.begin(115200);

    // Configuracion de Wifi y MQTT
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    // Funcion callback de recepcion de datos
    //client.setCallback(callback);

    
    //Serial.print("Initializing pulse oximeter..");

    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    /*
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    
    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
    */
    
    Serial.println("Adafruit MLX90614 test");
    mlx.begin();
}

int aux2 = 0;
int aux_2 = 0;
int aux3 =0;
uint32_t division = 10;

void jaime_sensor()
{

while(aux1 == 0)
{

  if(variable1 > 20)
  {
    Serial.print("Heart rate:");
    Serial.print(captura_heart/aux2);
    Serial.print("bpm / SpO2:");
    Serial.print(captura_spo2/aux3);
    Serial.println("%");
    aux1 = 1;
  } 
  else{
    pox.update();
   
    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
     if (aux_2 > 10){
         
      if(pox.getSpO2()>94){
      captura_spo2 = pox.getSpO2() + captura_spo2; 
      aux3++;
      }
      if (pox.getHeartRate()>60){
      captura_heart = pox.getHeartRate() + captura_heart ;
      aux2++;
      }
      
      /*
      Serial.print("Heart rate:");
      Serial.print(pox.getHeartRate());
      Serial.print("bpm / SpO2:");
      Serial.print(pox.getSpO2());
      Serial.println("%");
      */
     }
     tsLastReport = millis(); 
      variable1++;
      aux_2++;
      //delay(1000);
    }
    
  }
    
    
}
}

float nacho_sensor()
{
  float temp_media = 0;
  //while (aux == 0) 
  while(1)
    {
      if (variable > 24) 
      {
        temp_media = tempdedo / 25;
        Serial.print("*C\tCorporal = "); Serial.print(temp_media); Serial.println("*C");
        //aux = 1;
        variable = 0;
        tempdedo = 0;
        return temp_media;
      } 
      else 
      {
      tempdedo = tempdedo + (mlx.readObjectTempC());
      variable++;
      delay(200);
      }
    }
}

// Funcion para reconectar a la red
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Medic_System/Wemos/Temperatura", "Enviando el primer mensaje");
      // ... and resubscribe
      // ESTA PARTE TODAVIA NO LA HACEMOS; ENVIAR RASPY A WEMOS
      //client.subscribe("Sistema/Raspy");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    /*
    // Make sure to call update as fast as possible 
    PulseOximeter = jaime_sensor();

    // Envio pulso
    snprintf (msg, 75, "Pulso: %ld", PulseOximeter[0]);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Sistema/Wemos/Pulso", msg);

    //Envio Oxigeno
    snprintf (msg, 75, "Oxigeno: %ld", PulseOximeter[1]);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Sistema/Wemos/Oxigeno", msg);
    */
    
    temperatura = nacho_sensor();

    // Envio temperatura
    snprintf (msg, 75, "Temperatura: %f", temperatura);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Sistema/Wemos/Temperatura", msg);
}
