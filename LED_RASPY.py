                    

import RPi.GPIO as GPIO # Import Raspberry Pi GPIO library
from time import sleep # Import the sleep function from the time modu$

GPIO.setwarnings(False) # Ignore warning for now
GPIO.setmode(GPIO.BOARD) # Use physical pin numbering
GPIO.setup(11, GPIO.OUT, initial=GPIO.LOW) # Set pin 8 to be an outpu$
GPIO.setup(13, GPIO.OUT, initial=GPIO.LOW) # Set pin 8 to be an outpu$


print "Ejecucion iniciaca..."
iteracion = 0
while iteracion < 30: # Run forever
     GPIO.output(11, GPIO.HIGH) # Turn on
     GPIO.output(13, GPIO.LOW)
     sleep(0.02) # Sleep for 1 second
     GPIO.output(11, GPIO.LOW) # Turn off
     GPIO.output(13, GPIO.HIGH)
     sleep(0.02) # Sleep for 1 second
     iteracion = iteracion + 2
print "Ejecucion finalizada"
GPIO.cleanup()

