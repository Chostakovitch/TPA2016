/*
 * I2C_TPA2016.h
 *
 * Port of the Arduino library for the TPA2016D2 stereo amplifier plus a few corrections/additions :
 * - See here for the product : https://learn.adafruit.com/adafruit-tpa2016-2-8w-agc-stereo-audio-amplifier for the product
 * - See here for the original libray : https://github.com/adafruit/Adafruit-TPA2016-Library
 *
 * This class allows to read and write the 8-bits registers of the TPA2016D2 via I2C.
 * Examples of parameters which can be changed via I2C : AGC (Automatic Gain Control) activation, compression ratio, limiter level...
 * The datasheet for the TPA2016D2 can be found here : https://www.ti.com/product/TPA2016D2/technicaldocuments
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
#include <bitset>

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

// Register 2 : AGC Attack control. Only the first 6 bits are used.
#define TPA2016_ATK 0x2

// Register 3 : AGC Release control. Only the first 6 bits are used.
#define TPA2016_REL 0x3

/* Register 4 : AGC Hold control. Only the first 6 bits are used.
Set to 0 to disable hold function. */
#define TPA2016_HOLD 0x4

// Increment step in ms/6dB value of attack time
#define TPA2016_ATTACK_STEP 1.28f
// Increment step in sec/6dB of release time
#define TPA2016_RELEASE_STEP 0.1644f
// Increment step in sec/step of hold time
#define TPA2016_HOLD_STEP 0.0137f

/* Register 5 : AGC Fixed gain control.
If the Compression is enabled, fixed gain is adjustable from –28dB to 30dB.
If the Compression is disabled, fixed gain is adjustable from 0dB to 30dB.
Only the first 5 bits are used. Two's compliment. */
#define TPA2016_GAIN 0x5

// Register 6 : AGC Control (1/2). The first 5 bits are used to set the output limiter level.
#define TPA2016_LIMITER 0x6
// Increment step in dBV for output limiter level
#define TPA2016_LIMITER_STEP 0.5f
// Disables the output limiter function. Can only be disabled when the AGC compression ratio is 1:1 (bit 7)
#define TPA2016_LIMITER_DISABLE 0x80

// Register 7 : AGC Control (2/2) : Bits 2 and 3 are unused
#define TPA2016_AGC 0x7

// Defaut I2C address
#define TPA2016_I2CADDR 0x58

enum class TPA2016_LIMITER_NOISEGATE: uint8_t {
	_1MV = 0x00,
	_4MV = 0x20,
	_10MV = 0x40,
	_20MV = 0x60
};

enum class TPA2016_COMPRESSION_RATIO: uint8_t {
	 _1_1 = 0x00, // 1:1
	 _1_2 = 0x01, // 1:2
	 _1_4 = 0x02, // 1:4
	 _1_8 = 0x03 // 1:8
};

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
	/**
	 * Changes the minimum time between gain decreases.
	 * @param attack Attack time in ms/6dB
	 */
	void setAttackTime(float attack);
	float attackTime();

	// Register 3
	/**
	 * Changes the minimum time between gain increases.
	 * @param release Release time in sec/6dB
	 */
	void setReleaseTime(float release);
	float releaseTime();

	// Register 4
	/**
	 * Changes the minimum time between a gain decrease (attack) and a gain increase (release)
	 * @param hold Hold time in second (per step)
	 */
	void setHoldTime(float hold);
	float holdTime();
	void disableHoldControl();
	bool holdControlEnabled();

	// Register 5
	void setGain(int8_t gain);
	int8_t gain();

	// Register 6
	void enableLimiter(bool limiter);
	bool limiterEnabled();
	void setLimiterLevel(int8_t limit);
	int8_t limiterLevel();
	void setNoiseGateThreshold(TPA2016_LIMITER_NOISEGATE threshold);
	TPA2016_LIMITER_NOISEGATE noiseGateThreshold();

	// Register 7
	void setCompressionRatio(TPA2016_COMPRESSION_RATIO x);
	TPA2016_COMPRESSION_RATIO compressionRatio();
	void setMaxGain(uint8_t maxGain);
	uint8_t maxGain();
private:
	uint8_t bus;
	uint8_t address;
	uint8_t readI2C(uint8_t regAddress);
	void writeI2C(uint8_t regAddress, uint8_t value);
	int readI2C();
	/**
	 * Small helper to avoid code duplication.
	 * Are there is a lot of "toggle-bit" functions which basically does the same thing, modulo register address and bit position, this should replace boilerplate code.
	 * @param reg    Address of the 8-bit register to write
	 * @param bit    Bitmask corresponding to "true" for the feature
	 * @param enable If the feature should be enabled
	 */
	void boolWrite(uint8_t reg, uint8_t bit, bool enable);
};

#endif /* I2CTPA2016_H_ */
