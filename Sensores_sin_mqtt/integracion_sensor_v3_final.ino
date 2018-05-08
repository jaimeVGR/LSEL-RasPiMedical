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

int pulsox_sensor_read_complete, temp_sensor_read_complete = 0;
float tempdedo = 0.0;
uint32_t captura_spo2, captura_heart,samples_pulsox_sensor,samples_temp_sensor =0 ;

uint32_t tsLastReport = 0;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


void setup()
{
  Serial.begin(115200);

  //Serial.print("Initializing pulse oximeter..");

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip

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



void loop()
{
pulsox_sensor();
temp_sensor();
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

void temp_sensor()
{
  while (temp_sensor_read_complete == 0)
  {
   
    if (samples_temp_sensor == 20)
    {
      Serial.print("*C\tCorporal = "); Serial.print(tempdedo / 20); Serial.println("*C");
      temp_sensor_read_complete = 1;
    }
    else
    {
      Serial.println(mlx.readObjectTempC());//muestro las 20 leccturas del sensor
      tempdedo += (mlx.readObjectTempC());
      samples_temp_sensor++;
      delay(200);
    }
  }
}

