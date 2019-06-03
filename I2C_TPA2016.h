/*
 * I2C_TPA2016.h
 *
 * Port of the Arduino library for the TPA2016D2 stereo amplifier plus a few corrections/additions :
 * - See here for the product : https://learn.adafruit.com/adafruit-tpa2016-2-8w-agc-stereo-audio-amplifier for the product
 * - See here for the original libray : https://github.com/adafruit/Adafruit-TPA2016-Library
 *
 * This class allows to read and write the 8-bits registers of the TPA2016D2 via I2C.
 * Examples of parameters which can be changed via I2C : AGC (Automatic Gain Control) activation, compression ratio, limiter level...
 * The datasheet for the TPA2016D2 can be found here : http://www.adafruit.com/datasheets/TPA2016D2.pdf
 *
 * We use SMBus protocol as it will allow to use either an I2C or SMBus adapter, and as we only use basic features of I2C (read/write).
 * See https://www.kernel.org/doc/Documentation/i2c/smbus-protocol for more explanations.
 *
 * The constants below are arbitrary. They give :
 *	- The address of the different registers
 *	- For each register, the value needed to set a specific parameter to a specific value
 *
 * So, in order to set multiple parameters at once, you just need to do the binary OR
 * of the differents constants which refer to the same register, and write the value in the register.
 */

#ifndef I2CTPA2016_H_
#define I2CTPA2016_H_

#include <Bela.h>
#include <I2c.h>
#include <iostream>
#include <errno.h>
#include <string.h>

// Register 1 : function control
#define TPA2016_SETUP 0x1
// Enables right amplifier (bit 7)
#define TPA2016_SETUP_R_EN 0x80
// Enables left amplifier (bit 6)
#define TPA2016_SETUP_L_EN 0x40
// Software shutdown of control, bias and oscillator (bit 5)
#define TPA2016_SETUP_SWS 0x20
// Changes to a 1 when there is a short on the right channel. Reset by writing a 0 (bit 4)
#define TPA2016_SETUP_R_FAULT 0x10
// Changes to a 1 when there is a short on the left channel. Reset by writing a 0 (bit 3)
#define TPA2016_SETUP_L_FAULT 0x08
// Changes to a 1 when die temperature is above 150°C (bit 2)
#define TPA2016_SETUP_THERMAL 0x04
// Bit 1 is unused
// Enables Noise Gate function (bit 0)
#define TPA2016_SETUP_NOISEGATE 0x01

/* Register 2 : AGC Attack control. Used to set AGC attack time, i.e. the minimum time between gain decreases.
Only the first 6 bits are used. */
#define TPA2016_ATK 0x2

/* Register 3 : AGC Release control. Used to set AGC release time, i.e. the minimum time between gain increases.
Only the first 6 bits are used. */
#define TPA2016_REL 0x3

/* Register 4 : AGC Hold control. Used to set AGC hold time, i.e. the minimum time between a gain decrease (attack) and a gain increase (release).
Only the first 6 bits are used. Set to 0 to disable hold function. */
#define TPA2016_HOLD 0x4

/* Register 5 : AGC Fixed gain control.
If the Compression is enabled, fixed gain is adjustable from –28dB to 30dB.
If the Compression is disabled, fixed gain is adjustable from 0dB to 30dB.
Only the first 5 bits are used. Two's compliment. */
#define TPA2016_GAIN 0x5

// Register 6 : AGC Control (1/2). The first 5 bits are used to set the output limiter level.
#define TPA2016_LIMITER 0x6
// Disables the output limiter function. Can only be disabled when the AGC compression ratio is 1:1 (bit 7)
#define TPA2016_LIMITER_DISABLE 0x80
/* NoiseGate Thresholds (only functional when the AGC compression ratio is not 1:1).
Disable gain increases below the threshold specified in mili volts in the constant name (bits 5 and 6) */
#define TPA2016_LIMITER_NOISEGATE_1MV 0x00 // (0000 0000)
#define TPA2016_LIMITER_NOISEGATE_4MV 0x20 // (0010 0000)
#define TPA2016_LIMITER_NOISEGATE_10MV 0x40 // (0100 0000)
#define TPA2016_LIMITER_NOISEGATE_20MV 0x60 // (0110 0000)

/* Register 7 : AGC Control (2/2)
Bits 2 and 3 are unused*/
#define TPA2016_AGC 0x7
// Set the maximum gain the AGC can achieve at 30dB (bits 4 to 7)
#define TPA2016_AGC_GAIN_MAX 0x0C
// Set the compression ratio (bit 0 and 1)
// AGC compression ratio 1:1
#define TPA2016_AGC_1 0x00
// AGC compression ratio 1:2
#define TPA2016_AGC_2 0x01
// AGC compression ratio 1:4
#define TPA2016_AGC_4 0x02
// AGC compression ratio 1:8
#define TPA2016_AGC_8 0x03

// Defaut I2C address
#define TPA2016_I2CADDR 0x58

class I2C_TPA2016 : public I2c
{
public:
	// Constructor and destructor
	I2C_TPA2016(uint8_t bus, uint8_t address = TPA2016_I2CADDR);
	~I2C_TPA2016();

	void resetAmplifier();

	// Register 1
	void enableChannels(bool right, bool left);
	bool rightEnabled();
	bool leftEnabled();
	void softwareShutdown(bool shutdown);
  bool ready();
	void resetShort(bool right, bool left);
	bool rightShorted();
	bool leftShorted();
	bool tooHot();
	void enableNoiseGate(bool noiseGate);
	bool noiseGateEnabled();

	// Register 2
	void setAttackTime(uint8_t attack);
	uint8_t attackTime();

	// Register 3
	void setReleaseTime(uint8_t release);
	uint8_t releaseTime();

	// Register 4
	void setHoldTime(uint8_t hold);
	uint8_t holdTime();
	void disableHoldControl();
	bool holdControlEnabled();

	// Register 5
	void setGain(int8_t gain);
	int8_t gain();

	// Register 6
	void enableLimiter(bool limiter);
	bool limiterEnabled();
	void setLimiterLevel(uint8_t limit);
	uint8_t limiterLevel();
	void setNoiseGateThreshold(uint8_t threshold);
	uint8_t noiseGateThreshold();

	// Register 7
	void setCompressionRatio(uint8_t x);
	uint8_t compressionRatio();
	void setMaxGain(uint8_t x);
	uint8_t maxGain();
	void disableAGC();

	uint8_t readI2C(uint8_t regAddress);
	void writeI2C(uint8_t regAddress, uint8_t value);
private:
	uint8_t bus;
	uint8_t address;
	int readI2C();
	void boolWrite(uint8_t reg, uint8_t bit, bool enable);
};

#endif /* I2CTPA2016_H_ */
