#include <Wire.h>
#include <math.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>

// Librerias RFID
#include <SPI.h>
#include <MFRC522.h>

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

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Variables a enviar mqtt
int flag_temp, flag_ox = 0;
int flag_autentic, flag_msg_rec = 0;
float temperatura[2];
float oximetria[4];
char msg[50];


// Variables boton
int estado_boton_encendido=0;
int estado_boton_prueba=0;

//*******************************************variables boton de usuario
//int flag_button=0;
//int interface_button = digitalRead(D3);
int button_temp = digitalRead(D3);
int on_off_button_detector = digitalRead(D0);
int button_pulse=0;
//*******************************************

//********************************************* variables rfid
#define RST_PIN  D3 
#define SS_PIN  D8  
MFRC522 mfrc522(SS_PIN, RST_PIN); 
byte ActualUID[4]; 
int autenticacion=0;
//********************************************

//**************************************inicio lcd
LiquidCrystal_I2C lcd(0x27,16,2);
//***************************************

//variables auto off
unsigned long last_measure_time = 0;   
unsigned long last_measure_timeout = 100000;
//int on_off_button_detector = digitalRead(D0);

// Configuracion Wifi para MQTT
void setup_wifi() {

  delay(50);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Linea extra para solucionar error:
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Conectando WIFI");
    
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(50);
    Serial.print(".");

    // Ponemos Autoapagado por si se bloquea al no encontrar WIFI
    if((millis() >last_measure_timeout))
    {
      lcd.setCursor(0,1);
      lcd.print("Error WIFI");
      delay(3000);
      digitalWrite(D4, HIGH); //gate mosfet  
    }

    on_off_button_detector =digitalRead(D0); 
    if (on_off_button_detector==HIGH && (millis()>3000))
   {
      digitalWrite(D4, HIGH); //gate mosfet       
   }

    
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0,1);
  lcd.print("    WIFI OK");
  delay(1000);
}

//*********************************** callback de recepcion MQTT
void callback(char* topic, byte* payload, unsigned int length) {

  flag_msg_rec = 1;
  char dataArray[length];
  for (int i=0;i<length;i++) {
    dataArray[i] = (char)payload[i];
  }
  if (dataArray[0] == '1'){
    flag_autentic = 1;
  }
    // else para indicar ERRORES
}



void setup()
{
  // Realimentación encendido
  pinMode(D4, OUTPUT);//pin gate mosfet  
  digitalWrite(D4, LOW); //retroalimentacion wemos

  lcd.init(); 
  lcd.backlight();

  Serial.begin(115200);

  lcd.setCursor(0,0);
  lcd.print("  BIENVENIDOS   ");
  lcd.setCursor(0,1);
  lcd.print("  RASPIMEDIKAL  ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  PULSA BOTON   ");
  lcd.setCursor(0,1);
  lcd.print("  PARA INICIAR  ");
    
  
  // Configuracion de Wifi y MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Funcion callback de recepcion de datos
  client.setCallback(callback);

  //****************************************setup boton de usuario
  pinMode(D3, INPUT); //pin interface temp
  //********************************************

  pinMode(D0, INPUT);//pin button on off


  //configuracion RFID
  SPI.begin();       
  mfrc522.PCD_Init();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   ESPERANDO");
  lcd.setCursor(0,1);
  lcd.print(" IDENTIFICACION");

  // Aqui codigo para autenticar!!!
   while(flag_autentic != 1)
  {
    if ( mfrc522.PICC_IsNewCardPresent()) {  
       if ( mfrc522.PICC_ReadCardSerial()) {
          for (byte i = 0; i < mfrc522.uid.size; i++){
             ActualUID[i]=mfrc522.uid.uidByte[i];  
           }
           mfrc522.PICC_HaltA();

           // Compruebo conexion Mqtt (y resuscribo a topics)
           if (!client.connected()) 
           {
             reconnect();
           }
           client.loop();
           
           //Envio del TAG por Mqtt
           snprintf (msg, 75, "Card UID:%X%X%X%X ", ActualUID[0], ActualUID[1],ActualUID[2],ActualUID[3]);
           client.publish("Sistema/Wemos/RFID", msg);
           //delay(1000); No se porque esta el delay?

           //recepcion en funcion interrupcion

           
                      
       }
    }

    // Compruebo conexion Mqtt (y resuscribo a topics)
     if (!client.connected()) 
     {
       reconnect();
     }
     client.loop();

     // Ponemos Autoapagado por si se bloquea al no identificar
      if((millis() >last_measure_timeout))
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   IDENTIDAD ");
        lcd.setCursor(0,1);
        lcd.print("   ERRONEA ");
        delay(2000);
        digitalWrite(D4, HIGH); //gate mosfet  
      }
  
      on_off_button_detector =digitalRead(D0); 
      if (on_off_button_detector==HIGH && (millis()>3000))
      {
        digitalWrite(D4, HIGH); //gate mosfet
      }
      

      // Si llega mensaje pero no afirmativo, 
       if (flag_msg_rec == 1 && flag_autentic != 1){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("   IDENTIDAD ");
          lcd.setCursor(0,1);
          lcd.print("   ERRONEA ");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("   ESPERANDO");
          lcd.setCursor(0,1);
          lcd.print(" IDENTIFICACION"); 
       }
       flag_msg_rec = 0;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   IDENTIDAD ");
  lcd.setCursor(0,1);
  lcd.print("   CORRECTA");
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  int button_pulse = digitalRead(D8);
  pinMode(D8, INPUT); //pin interface pulse
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  delay(2000);
  
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
      client.subscribe("Sistema/Raspy");
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
  last_measure_timeout=last_measure_time+30000;   
}



void pulsox_sensor(float oximetria[4])
{

  int samples_total_sensor = 0;
  int samples_validas_sensor = 0;
  int suma_captura_heart =0 ;
  int suma_captura_spo2 = 0;
  int captura_heart;
  int captura_spo2;
  unsigned long timeout;
  uint32_t tsLastReport = 0;
  float heartrate_media=0;
  float heartrate_varianza=0;
  float spo2_media=0;
  float spo2_varianza=0;
  float dif_cuadrados_heartrate= 0;
  float dif_cuadrados_spo2=0;
  float dataheartrate[15] ; 
  float dataspo2[15];

  captura_heart = 0;
  captura_spo2 = 0;
  
  //inicializar sensor
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_24MA); // Somos conservadores
  
  timeout = millis() + 20000;
  
  while (1)
  {

    if (samples_validas_sensor == 15)
    {
      //Media y varianza (s^2)
      
      heartrate_media = suma_captura_heart/samples_validas_sensor;
      spo2_media= suma_captura_spo2/samples_validas_sensor;
      for (int i=0; i < samples_validas_sensor; i++){
        dif_cuadrados_heartrate = (dataheartrate[i]-heartrate_media)*(dataheartrate[i]-heartrate_media) + dif_cuadrados_heartrate;
        dif_cuadrados_spo2 = (dataspo2[i]-spo2_media)*(dataspo2[i]-spo2_media) + dif_cuadrados_spo2;
      }
      
      heartrate_varianza = dif_cuadrados_heartrate / (samples_validas_sensor - 1.0);
      oximetria[0]= heartrate_media;
      oximetria[1]= heartrate_varianza;
      spo2_varianza = dif_cuadrados_spo2 / (samples_validas_sensor - 1);
      oximetria[2]= spo2_media;
      oximetria[3]= spo2_varianza;
      
      Serial.print("Ritmo Cardiaco: Media="); Serial.print(oximetria [0]); Serial.println("  Varianza="); Serial.print(oximetria[1]); Serial.print("\n");
      Serial.print("Oxigeno: Media="); Serial.print(oximetria [2]); Serial.println("  Varianza="); Serial.print(oximetria [3]); Serial.print("\n");
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
        captura_heart = pox.getHeartRate();
        captura_spo2 = pox.getSpO2();

        if ((captura_heart < 50) || (captura_spo2 < 50)){
          
          // Si se pasa del timeout paso a la siguiente prueba
          if (timeout < millis()){  
            oximetria[0]= -1;
            oximetria[1]= -1;
            oximetria[2]= -1;
            oximetria[3]= -1;
            pox.shutdown();
            return;
          }
        }
        else {
          
          if (samples_total_sensor >= 15) {
            dataheartrate[samples_validas_sensor]=captura_heart;
            dataspo2[samples_validas_sensor]=captura_spo2;
            suma_captura_spo2 += captura_spo2;
            suma_captura_heart += captura_heart;
            Serial.print(captura_heart);//muestro las 20 lecturas del sensor de pulso y oxigeno
            Serial.print(",");
            Serial.println(captura_spo2);
            samples_validas_sensor++;
          }
          tsLastReport = millis();

          // Actualizo timeout si dato valido
          timeout = 10000 + millis();
          samples_total_sensor++;
          
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(captura_heart));
          lcd.setCursor(0,1);
          lcd.print(String(captura_spo2));
        }
        
        
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
  unsigned long  timeout;
  float tempdedo = 0;
  float temp_media = 0;
  float temp_varianza=0;
  float temp_varianza_2 = 0;
  float dif_cuadrados = 0;
  float dif_cuadrados_2 = 0;
  float data[20] ; 
  float out[2];

  timeout = millis() + 20000;
  
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
      data[samples_temp_sensor]= mlx.readObjectTempC()+ 2;  // El offset se añade aqui, primero era de 5, luego lo baje a 2
      
      if (data[samples_temp_sensor] > mlx.readAmbientTempC() && data[samples_temp_sensor] < 50){
        Serial.println(data[samples_temp_sensor]);//muestro las 20 leccturas del sensor
        tempdedo += (data[samples_temp_sensor]);
        samples_temp_sensor++;
        timeout = 10000 + millis();
        delay(500);
      }
      else{
        Serial.print("Dato despreciado, dedo no detectado: ");Serial.println(data[samples_temp_sensor]);
        if (timeout < millis()){ 
          temperatura [0] = -1;
          temperatura [1]= -1;
          return;
        }
        delay(10);
      }
    }
  }
}


void loop()
{

  //times to auto off

  if((millis() >last_measure_timeout))
    {
      digitalWrite(D4, HIGH); //gate mosfet  
    }
/*
  on_off_button_detector = digitalRead(D0);
  if(on_off_button_detector==HIGH && (millis()>10000))
   {
     pinMode(D6, OUTPUT);
     digitalWrite(D6, LOW); //gate mosfet
   }
*/   

  if (!client.connected()) {
      reconnect();
  }
  client.loop();

/*
  else
  {
     //lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("   PULSA PARA   ");
     lcd.setCursor(0,1);
     lcd.print("SIGUIENTE PRUEBA"); 
   }
*/  

  button_pulse=digitalRead(D8);
      if (button_pulse==HIGH)
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
          
          if (oximetria[0]== -1){
            
            lcd.clear();
            lcd.setCursor(0,0); 
            lcd.print(" MEDIDA ERRONEA"); 
            lcd.setCursor(0,1);
            lcd.print("SIGUIENTE PRUEBA");
            delay(2000);
          }
          else {
            flag_ox = 1;
      
            lcd.clear();
            lcd.setCursor(0,0); 
            lcd.print("PULSO: "); 
            lcd.print(String(oximetria[0])); 
            lcd.print(" PPM");
            lcd.setCursor(0,1);
            lcd.print("VAR: ");
            lcd.print(String(oximetria[1])); 
            delay(2000); 
            
            lcd.clear();
            lcd.setCursor(0,0); 
            lcd.print("OXIG: ");
            lcd.print(String(oximetria[2])); 
            lcd.print(" %");
            lcd.setCursor(0,1);
            lcd.print("VAR: ");
            lcd.print(String(oximetria[3])); 
            delay(2000); 
          }
      
          time_update();   // Inicio timer para apagado

        }

  
  else {
    
      button_temp=digitalRead(D3); 
      if (button_temp==LOW)
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
      
          /*temperatura[0]= -1;
          temperatura[1] = -1;*/
          
          

      if (temperatura[0]== -1){
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print(" MEDIDA ERRONEA"); 
        lcd.setCursor(0,1);
        lcd.print("SIGUIENTE PRUEBA"); 
        delay(2000);
      }
      else{
        flag_temp = 1;
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("TEMP: "); 
        lcd.print(String(temperatura[0])); 
        delay(3000);
      }
      
      time_update(); // Inicio timer para apagado
    }
}
  

  on_off_button_detector =digitalRead(D0); 
    if (on_off_button_detector==HIGH && (millis()>3000))
   {
      digitalWrite(D4, HIGH); //gate mosfet
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
