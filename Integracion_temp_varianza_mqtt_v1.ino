#include <Wire.h>
#include <math.h>
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

PulseOximeter pox;

// Variables sensores Nacho y Jaime
int pulsox_sensor_read_complete, temp_sensor_read_complete = 0;
float tempdedo = 0.0;
uint32_t captura_spo2, captura_heart,samples_pulsox_sensor =0 ;

uint32_t tsLastReport = 0;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Variables a enviar mqtt
float temperatura[2];
int pulso = 0;
int oxigeno = 0;
char msg[50];

// Configuracion Wifi para MQTT
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

void setup()
{
   // Configuracion de mensajes por monitor serie
  Serial.begin(115200);

  // Configuracion de Wifi y MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  // Funcion callback de recepcion de datos
  //client.setCallback(callback);


  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
 

  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  Serial.println("Adafruit MLX90614 test");
  mlx.begin();
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





void pulsox_sensor()
{

  while (pulsox_sensor_read_complete == 0)
  {

    if (samples_pulsox_sensor == 20)
    {
      //Serial.println(captura_spo2);
      Serial.print("Heart rate:");
        Serial.print(captura_heart / 10);
        Serial.print("bpm / SpO2:");
        Serial.print(captura_spo2 / 10); 
        Serial.println("%");
      pulsox_sensor_read_complete = 1;
    }

    
    else {
      pox.update();

      // Asynchronously dump heart rate and oxidation levels to the serial
      // For both, a value of 0 means "invalid"
      if (millis() - tsLastReport > REPORTING_PERIOD_MS) 
      {
         Serial.print(pox.getHeartRate());//muestro las 20 lecturas del sensor de pulso y oxigeno
         Serial.print(",");
         Serial.println(pox.getSpO2());
         
     
        if (samples_pulsox_sensor >= 10) 
        {
            captura_spo2 += pox.getSpO2();
            captura_heart += pox.getHeartRate();
        }
        tsLastReport = millis();
        samples_pulsox_sensor++;
      }

    }
  }
}

void temp_sensor(float temperatura[2])
{
  int samples_temp_sensor = 0;
  float temp_media;
  float temp_varianza;
  float dif_cuadrados = 0;
  float data[20] ; 
  float out[2];
  
  temp_media = 0;
  temp_varianza = 0;
  
  while (1)
  {
    if (samples_temp_sensor == 20)
    {
      // Media y varianza (s^2)
      temp_media = tempdedo / samples_temp_sensor;
      for (int i=0; i < 20; i++){
        dif_cuadrados += (data[i]-temp_media)*(data[i]-temp_media);
      }
      temp_varianza = dif_cuadrados / (samples_temp_sensor-1);
      temperatura [0] = temp_media;
      temperatura [1]= temp_varianza;
      Serial.print("Temperatura Corporal: Media="); Serial.print(temperatura [0]); Serial.println("*C,  Varianza="); Serial.print(temperatura [1]); Serial.print("\n");
      //temp_sensor_read_complete = 1;
      return;
    }
    else
    {
      data[samples_temp_sensor] = mlx.readObjectTempC();
      
      if (data[samples_temp_sensor] > mlx.readAmbientTempC() && data[samples_temp_sensor] < 50){
        Serial.println(data[samples_temp_sensor]);//muestro las 20 leccturas del sensor
        tempdedo += (data[samples_temp_sensor]);
        samples_temp_sensor++;
        delay(200);
      }
      else{
        Serial.println("Dato despreciado de temperatura");
        delay(10);
      }
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
  pulsox_sensor();
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
  
 temp_sensor( temperatura );

  // Aqui pondriamos un if para comprobar la varianza
  
  // Envio temperatura
  snprintf (msg, 75, "Temperatura: Media =%.1f Varianza =%.1f", temperatura[0], temperatura[1]);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("Sistema/Wemos/Temperatura", msg);
}
