#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>



#define REPORTING_PERIOD_MS     1000
// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

int variable, variable1 = 0;
float tempdedo = 26;
int aux, aux1 = 0;
uint32_t captura_spo2, captura_heart = 0;    

uint32_t tsLastReport = 0;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    //Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);

    //Serial.print("Initializing pulse oximeter..");

    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
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

void nacho_sensor()
{
  while (aux == 0) 
    {
      if (variable > 24) 
      {
        Serial.print("*C\tCorporal = "); Serial.print(tempdedo / 25); Serial.println("*C");
        aux = 1;
      } 
      else 
      {
      tempdedo = tempdedo + (mlx.readObjectTempC());
      variable++;
      delay(200);
      }
    }
}

void loop()
{
    // Make sure to call update as fast as possible
   jaime_sensor();
    nacho_sensor();
    
}
