#include "I2C_TPA2016.h"

I2C_TPA2016::I2C_TPA2016(uint8_t bus, uint8_t address) {
	this->bus = bus;
	this->address = address;
	int code;

	// Open I2C device
	if((code = initI2C_RW(bus, address, 0)) != 0) {
		throw std::runtime_error("Unable to initialize I2C");
	}

	/* Check if all features used by this code are available, here :
		- Reading and writing bytes
		- Combined read/write transaction without stop in between (used by i2c_smbus_read_byte_data).
	* See https://www.kernel.org/doc/Documentation/i2c/functionality for details */
	uint64_t availableFuncs;
	if (ioctl(i2C_file, I2C_FUNCS, &availableFuncs) < 0) {
		throw std::runtime_error("Unable to check I2C adapter functionalities");
	}

	if (!(availableFuncs & (I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_I2C))) {
		throw std::runtime_error("Desired functionality is not available");
	}

	// Activate all features of the amplifier
	softwareShutdown(false);
}

I2C_TPA2016::~I2C_TPA2016() {
	// Disable most features
	softwareShutdown(true);

	int code;
	if((code = closeI2C()) != 0) {
		throw std::runtime_error("Unable to close I2C");
	}
}

void I2C_TPA2016::writeI2C(uint8_t regAddress, uint8_t value) {
	if(i2c_smbus_write_byte_data(i2C_file, regAddress, value) < 0)
	{
		throw std::runtime_error(strerror(errno));
	}
}

uint8_t I2C_TPA2016::readI2C(uint8_t regAddress) {
	uint8_t res;
	if((res = i2c_smbus_read_byte_data(i2C_file, regAddress)) < 0)
	{
		throw std::runtime_error(strerror(errno));
	}
	return res;
}

void I2C_TPA2016::boolWrite(uint8_t reg, uint8_t bit, bool enable) {
	uint8_t reg_value = readI2C(reg);
	if(enable)
		reg_value |= bit;
	else
		reg_value &= ~bit;
	writeI2C(reg, reg_value);
}

void I2C_TPA2016::enableChannels(bool right, bool left) {
	boolWrite(TPA2016_SETUP, TPA2016_SETUP_R_EN, right);
	boolWrite(TPA2016_SETUP, TPA2016_SETUP_L_EN, left);
}

bool I2C_TPA2016::rightEnabled() {
	return readI2C(TPA2016_SETUP) & TPA2016_SETUP_R_EN;
}

bool I2C_TPA2016::leftEnabled() {
	return readI2C(TPA2016_SETUP) & TPA2016_SETUP_L_EN;
}

void I2C_TPA2016::softwareShutdown(bool shutdown) {
	boolWrite(TPA2016_SETUP, TPA2016_SETUP_SWS, shutdown);
}

bool I2C_TPA2016::ready() {
	// TPA2016_SETUP_SWS is shutdown enabled, negate to get readiness
	return ~(readI2C(TPA2016_SETUP) & TPA2016_SETUP_SWS);
}

void I2C_TPA2016::resetShort(bool right, bool left) {
	boolWrite(TPA2016_SETUP, TPA2016_SETUP_R_FAULT, right);
	boolWrite(TPA2016_SETUP, TPA2016_SETUP_L_FAULT, left);
}

bool I2C_TPA2016::rightShorted() {
	return readI2C(TPA2016_SETUP) & TPA2016_SETUP_R_FAULT;
}

bool I2C_TPA2016::leftShorted() {
	return readI2C(TPA2016_SETUP) & TPA2016_SETUP_L_FAULT;
}

bool I2C_TPA2016::tooHot() {
	return readI2C(TPA2016_SETUP) & TPA2016_SETUP_THERMAL;
}

void I2C_TPA2016::enableNoiseGate(bool noiseGate) {
	boolWrite(TPA2016_SETUP, TPA2016_SETUP_NOISEGATE, noiseGate);
}

bool I2C_TPA2016::noiseGateEnabled() {
  return readI2C(TPA2016_SETUP) & TPA2016_SETUP_NOISEGATE;
}

void I2C_TPA2016::setAttackTime(float attack) {
	writeI2C(TPA2016_ATK, convertAttackTime(attack));
}

float I2C_TPA2016::attackTime() {
	// Mask off last two bits and multiply by increment step
	return (readI2C(TPA2016_ATK) & ~(0xC0)) * TPA2016_ATTACK_STEP;
}

void I2C_TPA2016::setReleaseTime(float release) {
	writeI2C(TPA2016_REL, convertReleaseTime(release));
}

float I2C_TPA2016::releaseTime() {
	// Mask off last two bits and multiply by increment step
	return (readI2C(TPA2016_REL) & ~(0xC0)) * TPA2016_RELEASE_STEP;
}

void I2C_TPA2016::setHoldTime(float hold) {
	writeI2C(TPA2016_HOLD, convertHoldTime(hold));
}

float I2C_TPA2016::holdTime() {
	// Mask off last two bits and multiply by increment step
	return (readI2C(TPA2016_HOLD) & ~(0xC0)) * TPA2016_HOLD_STEP;
}

void I2C_TPA2016::disableHoldControl() {
	writeI2C(TPA2016_HOLD, 0);
}

bool I2C_TPA2016::holdControlEnabled() {
	// Mask off 2 last bits : if 6 first bits are at 0, hold control is disabled
	return readI2C(TPA2016_HOLD) & ~(0xC0);
}

void I2C_TPA2016::setGain(int8_t gain) {
	if(gain > 30 || gain < -28) {
		throw std::out_of_range("Illegal gain value : must be between -28 and 30");
	}
	/*
	 * int8_t follows two's compliment notation. So any 5-bits number will be "left padded" with ones on bits 5, 6, 7 (remember, flip bits and add one).
	 * Therefore, even if gain is "8-bits two's compliment", it will work for a "6-bits two's compliment".
	 */
	writeI2C(TPA2016_GAIN, gain);
}

int8_t I2C_TPA2016::gain() {
	uint8_t gain = readI2C(TPA2016_GAIN);
	/*
	 * We get a 6-bits two's compliment. If bit 6 is 1, the value is negative
	 * so left pad with ones (| 0xC0) so we have a true 8-bits two's compliment.
	 */
	return (gain & 0x20) ? gain | 0xC0 : gain;
}

void I2C_TPA2016::enableLimiter(bool limiter) {

}

bool I2C_TPA2016::limiterEnabled() {
	return false;
}

void I2C_TPA2016::setLimiterLevel(uint8_t limit) {

}

uint8_t I2C_TPA2016::limiterLevel() {
	return 0;
}

void I2C_TPA2016::setNoiseGateThreshold(uint8_t threshold) {

}

uint8_t I2C_TPA2016::noiseGateThreshold() {
	return 0;
}

void I2C_TPA2016::setCompressionRatio(uint8_t x) {

}

uint8_t I2C_TPA2016::compressionRatio() {
	return 0;
}

void I2C_TPA2016::setMaxGain(uint8_t x) {

}

uint8_t I2C_TPA2016::maxGain() {
	return 0;
}

void I2C_TPA2016::disableAGC() {

}

uint8_t I2C_TPA2016::convertAttackTime(float attackTime) {
	if(attackTime > 80.66f || attackTime < 1.28f) {
		throw std::out_of_range("Illegal attack time value");
	}
	// The quotient of the division gives us how many TPA2016_ATTACK_STEP are in attackTime, this is the reg value
	return static_cast<uint8_t>(attackTime / TPA2016_ATTACK_STEP);
}

uint8_t I2C_TPA2016::convertReleaseTime(float releaseTime) {
	if(releaseTime > 10.36f || releaseTime < 0.1644f) {
		throw std::out_of_range("Illegal release time value");
	}
	return static_cast<uint8_t>(releaseTime / TPA2016_RELEASE_STEP);
}

uint8_t I2C_TPA2016::convertHoldTime(float holdTime) {
	if(holdTime > 0.8631f || holdTime < 0) {
		throw std::out_of_range("Illegal hold time value");
	}
	return static_cast<uint8_t>(holdTime / TPA2016_HOLD_STEP);
}

// We do not provide reading without register adress, as we already have SMBus method to write register adress then read value.
int I2C_TPA2016::readI2C() {
	throw std::logic_error("Function not implemented");
}
