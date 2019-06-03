#include <Bela.h>
#include "I2C_TPA2016.h"

// Digital pin
int softwareResetPin = 0;

// I2C bus number
const int i2c_bus = 1;

// Reset has completed
bool isReset = false;

// In seconds, how long to apply reset signal
float delay = 0.5; 

// Ellapsed samples
int count = 0;

bool setup(BelaContext *context, void *userData)
{
	// Pin to reset TPA2016 registers. SDZ (Shutdown terminal) is active low, so restore normal state
	// Before running this program, with the right pin assembly, the TPA2016 has very low quiescent current consumption
	pinMode(context, 0, softwareResetPin, OUTPUT);
	digitalWrite(context, 0, softwareResetPin, 1); 
	// Open I2C on bus number 1
	I2C_TPA2016 tpa(i2c_bus);
	
	tpa.enableChannels(false, false);
	return true;
}

void render(BelaContext *context, void *userData)
{
	
}


void cleanup(BelaContext *context, void *userData)
{
	// Reset registers
	digitalWrite(context, 0, softwareResetPin, 0); 
}