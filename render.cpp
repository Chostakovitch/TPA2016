#include <Bela.h>
#include "I2C_TPA2016.h"

bool setup(BelaContext *context, void *userData)
{
	// Open I2C on bus number 1
	I2C_TPA2016 i2c(1);
	// Configure amplifier
	

	return true;
}

void render(BelaContext *context, void *userData)
{

}


void cleanup(BelaContext *context, void *userData)
{

}