#include <Bela.h>
#include "I2C_TPA2016.h"
#include <assert.h>

bool setup(BelaContext *context, void *userData)
{
	// Open I2C on bus number 1
	I2C_TPA2016 tpa(1);
	for(unsigned int n = 0; n < context->digitalFrames; n++) {
		//Avoid having residual values
		for(unsigned int channel = 0; channel < context->digitalChannels; channel++) {
			digitalWrite(context, n, channel, 1);
		}
	}
	return true;
}

void render(BelaContext *context, void *userData)
{

}


void cleanup(BelaContext *context, void *userData)
{
 
}