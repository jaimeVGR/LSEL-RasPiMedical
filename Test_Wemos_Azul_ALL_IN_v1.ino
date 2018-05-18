#include <Wire.h>
#include <math.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>

// Librerias de MQTT
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

//*******************************************libreria lcd
#include <LiquidCrystal_I2C.h>
//*******************************************

// Constantes MQTT
const char* ssid = "RaspyMedical";
const char* password = "RaspyMedikal";
const char* mqtt_server = "192.168.4.1";

WiFiClient espClient;
PubSubClient client(espClient);

// FIN MQTT

#define REPORTING_PERIOD_MS     1000

PulseOximeter pox;



// Variables sensores Nacho y Jaime
int pulsox_sensor_read_complete = 0;




uint32_t tsLastReport = 0;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Variables a enviar mqtt
int flag_temp, flag_ox = 0;
float temperatura[2];
float oximetria[4];
char msg[50];

// Variables boton
int estado_boton_encendido=0;
int estado_boton_prueba=0;

//*******************************************variables boton de usuario
int flag_button=0;
int interface_button = digitalRead(D3);
//*******************************************

//**************************************inicio lcd
LiquidCrystal_I2C lcd(0x27,16,2);
//***************************************

//variables auto off
unsigned long last_measure_time,actual_time=0;   
unsigned long last_measure_timeout = 10000;
int on_off_button_detector = digitalRead(D0);

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

  /*
  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  */

  //****************************************setup boton de usuario
  pinMode(D3, INPUT); //pin interface button
  //********************************************
  
  //***************************lcd:
  lcd.init(); 
  lcd.backlight();
  //***********************************
  
  //********************************lcd mensaje inicio
  lcd.setCursor(0,0);
  lcd.print("  BIENVENIDOS   ");
  lcd.setCursor(0,1);
  lcd.print("  RASPIMEDIKAL  ");
  delay(2000);
  lcd.clear();
  //**********************************  


  //setup auto off
  pinMode(D0, INPUT);//pin on off
  pinMode(D7, OUTPUT);//pin gate mosfet  
  digitalWrite(D7, HIGH); //retroalimentacion wemos
  // Quizas hacer aqui un timeupdate?
  
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
      client.publish("Sistema/Wemos", "Conexión correcta: Inicio tranmisión datos\n");
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


void lcd_interface()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  DEDO INDICE   ");
  lcd.setCursor(0,1);
  lcd.print("SOBRE EL SENSOR ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("        3       "); 
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("        2       "); 
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("        1       "); 
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("PRUEBA EN CURSO ");   
}



void time_update()
{
  last_measure_time=millis();
  last_measure_timeout=last_measure_time+10000;   
}



void pulsox_sensor(float oximetria[4])
{

  //inicializar sensor
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_24MA); // Somos conservadores
  
  int samples_total_sensor = 0;
  int samples_validas_sensor = 0;
  int captura_heart =0 ;
  int captura_spo2 = 0;
  float heartrate_media=0;
  float heartrate_varianza=0;
  float spo2_media=0;
  float spo2_varianza=0;
  float dif_cuadrados_heartrate= 0;
  float dif_cuadrados_spo2=0;
  float dataheartrate[10] ; 
  float dataspo2[10];
  

  while (1)
  {

    if (samples_total_sensor == 20)
    {
      //Media y varianza (s^2)
      heartrate_media = captura_heart/10.0;
      spo2_media=  captura_spo2/10.0;
      for (int i=0; i < 10; i++){
        dif_cuadrados_heartrate = (dataheartrate[i]-heartrate_media)*(dataheartrate[i]-heartrate_media) + dif_cuadrados_heartrate;
        dif_cuadrados_spo2 = (dataspo2[i]-spo2_media)*(dataspo2[i]-spo2_media) + dif_cuadrados_spo2;
      }
      heartrate_varianza = dif_cuadrados_heartrate / (10 - 1);
      oximetria[0]= heartrate_media;
      oximetria[1]= heartrate_varianza;
      spo2_varianza = dif_cuadrados_spo2 / (10 - 1);
      oximetria[2]= spo2_media;
      oximetria[3]= spo2_varianza;
      Serial.print("Ritmo Cardiaco: Media="); Serial.print(oximetria [0]); Serial.println("  Varianza="); Serial.print(oximetria[1]); Serial.print("\n");
      Serial.print("Oxigeno: Media="); Serial.print(oximetria [2]); Serial.println("  Varianza="); Serial.print(oximetria [3]); Serial.print("\n");
      pulsox_sensor_read_complete = 1;
      pox.shutdown();
      //time_update();
      return;
    }
    
    else {
    
      pox.update();
      // Asynchronously dump heart rate and oxidation levels to the serial
      // For both, a value of 0 means "invalid"
      if (millis() - tsLastReport > REPORTING_PERIOD_MS) 
      {
        if (samples_total_sensor >= 10) 
        {
            dataheartrate[samples_validas_sensor]=pox.getHeartRate();
            dataspo2[samples_validas_sensor]=pox.getSpO2();
            captura_spo2 += pox.getSpO2();
            captura_heart += pox.getHeartRate();
            Serial.print(pox.getHeartRate());//muestro las 20 lecturas del sensor de pulso y oxigeno
            Serial.print(",");
            Serial.println(pox.getSpO2());
            samples_validas_sensor++;
        }
        tsLastReport = millis();
        samples_total_sensor++;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(String(pox.getHeartRate()));
        lcd.setCursor(0,1);
        lcd.print(String(pox.getSpO2()));
        
      }

    }
  }
}

void temp_sensor(float temperatura[2])
{
 
  //inicializar sensor
  mlx.begin();
  
  //variables temperatura
  int samples_temp_sensor = 0;
  float tempdedo = 0;
  float temp_media = 0;
  float temp_varianza=0;
  float temp_varianza_2 = 0;
  float dif_cuadrados = 0;
  float dif_cuadrados_2 = 0;
  float data[20] ; 
  float out[2];
  
  while (1)
  {
    if (samples_temp_sensor == 20)
    {
      // Media y varianza (s^2)
      temp_media = tempdedo / 20;
      for (int i=0; i < 20; i++){
        dif_cuadrados = (data[i]-temp_media)*(data[i]-temp_media) + dif_cuadrados;
      }
      temp_varianza = dif_cuadrados / (samples_temp_sensor-1);
      Serial.print("Varianza =");Serial.println(temp_varianza);
      temperatura [0] = temp_media;
      temperatura [1]= temp_varianza;
      Serial.print("Temperatura Corporal: Media="); Serial.print(temperatura [0]); Serial.print("*C,  Varianza="); Serial.println(temperatura [1]);

      //time_update(); 
      return;
    }
    else
    {
      data[samples_temp_sensor]= mlx.readObjectTempC()+ 5;
      
      if (data[samples_temp_sensor] > mlx.readAmbientTempC() && data[samples_temp_sensor] < 50){
        Serial.println(data[samples_temp_sensor]);//muestro las 20 leccturas del sensor
        tempdedo += (data[samples_temp_sensor]);
        samples_temp_sensor++;
        delay(500);
      }
      else{
        Serial.print("Dato despreciado de temperatura: ");Serial.println(data[samples_temp_sensor]);
        delay(50);
      }
    }
  }
}


void loop()
{

  //times to auto off
  actual_time=millis();

  if((actual_time>last_measure_timeout))
    {
      digitalWrite(D7, LOW); //gate mosfet  
    }

  on_off_button_detector = digitalRead(D0);
  if(on_off_button_detector==HIGH && (millis()>3000))
   {
     digitalWrite(D7, LOW); //gate mosfet
   }

  if (!client.connected()) {
      reconnect();
  }
  client.loop();

  if (flag_button==0)
  {
     //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  PULSA BOTON   ");
    lcd.setCursor(0,1);
    lcd.print("  PARA INICIAR  ");
  } 

  else
  {
     //lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("   PULSA PARA   ");
     lcd.setCursor(0,1);
     lcd.print("SIGUIENTE PRUEBA"); 
   }
  
  interface_button =digitalRead(D3);
  
  if (interface_button==LOW && flag_button==0)
  {
    //salida lcd
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SENSOR DE PULSO ");
    delay(2000);
    lcd_interface();
    
    pulsox_sensor(oximetria);
    /*oximetria[0]= -1 ;
    oximetria[1]= -1 ;
    oximetria[2]= -1 ;
    oximetria[3]= -1 ;
    */
    flag_button=1;
    flag_ox = 1;

    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("PULSO: "); 
    lcd.print(String(oximetria[0])); 
    lcd.print(" PPM");
    lcd.setCursor(0,1);
    lcd.print("OXIG: ");
    lcd.print(String(oximetria[2])); 
    delay(3000); 
    time_update();   // Inicio timer para apagado
    
  }
  else {
    //interface_button =digitalRead(D3); 
    if (interface_button==LOW && flag_button==1)
    {
      //salida lcd
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   SENSOR DE    ");
      lcd.setCursor(0,1);
      lcd.print("  TEMPERATURA   ");
      delay(2000);
      lcd_interface();
      
      temp_sensor( temperatura );
      flag_button=0;
      flag_temp = 1;

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("TEMP: "); 
      lcd.print(String(temperatura[0])); 
      lcd.print(" C");
      delay(3000);
      time_update(); // Inicio timer para apagado
    }
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (flag_ox == 1){
    
    // Envio pulso
    snprintf (msg, 75, "Ritmo Cardiaco: Media =%.1f Varianza =%.1f", oximetria[0], oximetria[1]);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Sistema/Wemos/Pulso", msg);
    
    //Envio Oxigeno
    snprintf (msg, 75, "Oxigeno: Media =%.1f Varianza =%.1f", oximetria[2], oximetria[3]);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Sistema/Wemos/Oxigeno", msg);
    flag_ox = 0;
  }
  
  // Aqui pondriamos un if para comprobar la varianza
  
  // Envio temperatura
  if( flag_temp == 1){
    
    snprintf (msg, 75, "Temperatura: Media =%.1f Varianza =%.1f", temperatura[0], temperatura[1]);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Sistema/Wemos/Temperatura", msg);
    flag_temp=0;
  }
  
}
