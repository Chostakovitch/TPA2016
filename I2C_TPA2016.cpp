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

void I2C_TPA2016::enableChannels(bool right, bool left) {
	uint8_t setup = readI2C(TPA2016_SETUP);
	if(right)
		setup |= TPA2016_SETUP_R_EN;
	else
		setup &= ~TPA2016_SETUP_R_EN;
	if(left)
		setup |= TPA2016_SETUP_L_EN;
	else
		setup &= ~TPA2016_SETUP_L_EN;
	writeI2C(TPA2016_SETUP, setup);
}

bool I2C_TPA2016::rightEnabled() {
	return readI2C(TPA2016_SETUP) & TPA2016_SETUP_R_EN;
}

bool I2C_TPA2016::leftEnabled() {
	return readI2C(TPA2016_SETUP) & TPA2016_SETUP_L_EN;
}

void I2C_TPA2016::softwareShutdown(bool shutdown) {

}

bool I2C_TPA2016::ready() {
	return false;
}

void I2C_TPA2016::resetShort(bool right, bool left) {

}

bool I2C_TPA2016::rightShorted() {
	return true;
}

bool I2C_TPA2016::leftShorted() {
	return true;
}

bool I2C_TPA2016::tooHot() {
	return true;
}

void I2C_TPA2016::enableNoiseGate(bool noiseGate) {

}

bool I2C_TPA2016::noiseGateEnabled() {
		return false;
}

void I2C_TPA2016::setAttackTime(uint8_t attack) {

}

uint8_t I2C_TPA2016::attackTime() {
	return 0;
}

void I2C_TPA2016::setReleaseTime(uint8_t release) {

}

uint8_t I2C_TPA2016::releaseTime() {
	return 0;
}

void I2C_TPA2016::setHoldTime(uint8_t hold) {

}

uint8_t I2C_TPA2016::holdTime() {
	return 0;
}

void I2C_TPA2016::disableHoldControl() {

}

bool I2C_TPA2016::holdControlEnabled() {
	return false;
}

void I2C_TPA2016::setGain(int8_t gain) {

}

int8_t I2C_TPA2016::gain() {
	return 0;
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


// We do not provide reading without register adress, as we already have SMBus method to write register adress then read value.
int I2C_TPA2016::readI2C() {
	throw std::logic_error("Function not implemented");
}
