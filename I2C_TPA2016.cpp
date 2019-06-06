#include "I2C_TPA2016.h"

I2C_TPA2016::I2C_TPA2016(uint8_t bus, uint8_t address) {
	this->bus = bus;
	this->address = address;

	// Open I2C device
	char filename[MAX_BUF_NAME];
	char error[MAX_BUF_ERROR];
	snprintf(filename, sizeof(filename), "/dev/i2c-%d", bus);
	if((fd = open(filename, O_RDWR)) < 0) {
		snprintf(error, sizeof(error), "Failed to initialize I2C on bus %d", bus);
		throw std::runtime_error(error);
	}

	if (ioctl(fd, I2C_SLAVE, address) < 0){
		snprintf(error, sizeof(error), "Failed to target TPA as a slave (address %#x)", address);
		throw std::runtime_error(error);
	}

	/* Check if all features used by this code are available, i.e. :
		- Reading and writing bytes
		- Combined read/write transaction without stop bit in between (used by i2c_smbus_read_byte_data and needed by the TPA2016D2 to read a register).
	* See https://www.kernel.org/doc/Documentation/i2c/functionality for details */
	uint64_t availableFuncs;
	if (ioctl(fd, I2C_FUNCS, &availableFuncs) < 0) {
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
	if((code = close(fd)) != 0) {
		throw std::runtime_error("Unable to close I2C");
	}
}

void I2C_TPA2016::writeI2C(uint8_t regAddress, uint8_t value) {
	if(i2c_smbus_write_byte_data(fd, regAddress, value) < 0)
	{
		throw std::runtime_error(strerror(errno));
	}
}

uint8_t I2C_TPA2016::readI2C(uint8_t regAddress) {
	uint8_t res;
	if((res = i2c_smbus_read_byte_data(fd, regAddress)) < 0)
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
	return !(readI2C(TPA2016_SETUP) & TPA2016_SETUP_SWS);
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
	if(noiseGate && compressionRatio() == TPA2016_COMPRESSION_RATIO::_1_1) {
		throw std::logic_error("Noise Gate cannot be enabled when compression ratio is 1:1");
	}
	boolWrite(TPA2016_SETUP, TPA2016_SETUP_NOISEGATE, noiseGate);
}

bool I2C_TPA2016::noiseGateEnabled() {
  return readI2C(TPA2016_SETUP) & TPA2016_SETUP_NOISEGATE;
}

void I2C_TPA2016::setAttackTime(float attack) {
	if(attack > 80.66f || attack < 1.28f) {
		throw std::out_of_range("Illegal attack time value : must be between 1.28ms/6dB and 80.66ms/6dB");
	}
	// Apply conversion table specified in datasheet for the attack time
	writeI2C(TPA2016_ATK, static_cast<uint8_t>(attack / TPA2016_ATTACK_STEP));
}

float I2C_TPA2016::attackTime() {
	// Mask off last two bits and multiply by increment step
	return (readI2C(TPA2016_ATK) & ~(0xC0)) * TPA2016_ATTACK_STEP;
}

void I2C_TPA2016::setReleaseTime(float release) {
	if(release > 10.36f || release < 0.1644f) {
		throw std::out_of_range("Illegal release time value : must be between 0.01644s/6dB and 10.36s/6dB");
	}
	writeI2C(TPA2016_REL, static_cast<uint8_t>(release / TPA2016_RELEASE_STEP));
}

float I2C_TPA2016::releaseTime() {
	// Mask off last two bits and multiply by increment step
	return (readI2C(TPA2016_REL) & ~(0xC0)) * TPA2016_RELEASE_STEP;
}

void I2C_TPA2016::setHoldTime(float hold) {
	if(hold > 0.8631f || hold < 0) {
		throw std::out_of_range("Illegal hold time value : must be between 0 and 0.8631s/step");
	}
	writeI2C(TPA2016_HOLD, static_cast<uint8_t>(hold / TPA2016_HOLD_STEP));
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
	if(compressionRatio() == TPA2016_COMPRESSION_RATIO::_1_1 && gain < 0) {
		throw std::out_of_range("Illegal gain value : cannot be negative when compression ratio is 1:1");
	}
	if(gain > 30 || gain < -28) {
		throw std::out_of_range("Illegal gain value : must be between -28dB and 30dB");
	}
	/*
	 * int8_t follows two's compliment notation. So any 5-bits number will be "left padded" with ones on bits 5, 6, 7 (remember, flip bits and add one).
	 * Therefore, even if gain is "8-bits two's compliment", it will also be a "6-bits two's compliment".
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
	if(!limiter && compressionRatio() != TPA2016_COMPRESSION_RATIO::_1_1) {
		throw std::logic_error("Limiter cannot be disabled when compression ratio is not 1:1");
	}
	boolWrite(TPA2016_LIMITER, TPA2016_LIMITER_DISABLE, !limiter);
}

bool I2C_TPA2016::limiterEnabled() {
	return !(readI2C(TPA2016_LIMITER) & TPA2016_LIMITER_DISABLE);
}

void I2C_TPA2016::setLimiterLevel(float limit) {
	if(limit > 9 || limit < -6.5) {
		throw std::out_of_range("Illegal limiter level value : must be between -6.5dBV and 9dBV");
	}
	// 0x00 is -6.5dBV
	limit += 6.5f;
	// Mask-off first 5 bits
	uint8_t reg_value = readI2C(TPA2016_LIMITER) & ~0x1F;
	reg_value |= static_cast<uint8_t>(limit / TPA2016_LIMITER_STEP);
	writeI2C(TPA2016_LIMITER, reg_value);
}

float I2C_TPA2016::limiterLevel() {
	// Get only the first 5 bits and compensate offset
	return (readI2C(TPA2016_LIMITER) & 0x1F) * TPA2016_LIMITER_STEP - 6.5f;
}

void I2C_TPA2016::setNoiseGateThreshold(TPA2016_LIMITER_NOISEGATE threshold) {
	if(compressionRatio() == TPA2016_COMPRESSION_RATIO::_1_1) {
		throw std::logic_error("Noise Gate threshold cannot be changed when compression ratio is 1:1");
	}
	// Mask-off bit 5 and 6
	uint8_t reg_value = readI2C(TPA2016_LIMITER) & ~0x60;
	reg_value |= static_cast<uint8_t>(threshold);
	writeI2C(TPA2016_LIMITER, reg_value);
}

TPA2016_LIMITER_NOISEGATE I2C_TPA2016::noiseGateThreshold() {
	// Get only bit 5 and 6
	uint8_t threshold = readI2C(TPA2016_LIMITER) & 0x60;
	switch(threshold) {
		case 0x00:
			return TPA2016_LIMITER_NOISEGATE::_1MV;
		case 0x20:
			return TPA2016_LIMITER_NOISEGATE::_4MV;
		case 0x40:
			return TPA2016_LIMITER_NOISEGATE::_10MV;
		case 0x60:
			return TPA2016_LIMITER_NOISEGATE::_20MV;
		default:
			throw std::out_of_range("Unknown noise gate threshold value");
	}
}

void I2C_TPA2016::setCompressionRatio(TPA2016_COMPRESSION_RATIO ratio) {
	// Mask-off bits 0 and 1
	uint8_t reg_value = readI2C(TPA2016_AGC) & ~0x03;
	reg_value |= static_cast<uint8_t>(ratio);
	writeI2C(TPA2016_AGC, reg_value);
}

TPA2016_COMPRESSION_RATIO I2C_TPA2016::compressionRatio() {
	// Get only bit 0 and 1
	uint8_t ratio = readI2C(TPA2016_AGC) & 0x03;
	switch(ratio) {
		case 0x00:
			return TPA2016_COMPRESSION_RATIO::_1_1;
		case 0x01:
			return TPA2016_COMPRESSION_RATIO::_1_2;
		case 0x02:
			return TPA2016_COMPRESSION_RATIO::_1_4;
		case 0x03:
			return TPA2016_COMPRESSION_RATIO::_1_8;
		default:
			throw std::out_of_range("Unknown compression ratio value");
	}
}

void I2C_TPA2016::setMaxGain(uint8_t maxGain) {
		if(maxGain > 30 || maxGain < 18) {
			throw std::out_of_range("Illegal max gain value : should be between 18dB and 30dB");
		}
		uint8_t reg_value = readI2C(TPA2016_AGC);
		// "0" is 18dB.
		maxGain -= 18;
		// Let the first 4 bits stay the same and change 4 last bits if needed
		reg_value = (reg_value & 0xF) | (maxGain << 4);
		writeI2C(TPA2016_AGC, reg_value);
}

uint8_t I2C_TPA2016::maxGain() {
	// Don't forget to compensate the 18dB offset
	return (readI2C(TPA2016_AGC) >> 4) + 18;
}
