#include <catch.hpp>
#include "../I2C_TPA2016.h"

// Test of default values
SCENARIO("Amplifier default values are expected") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("The amplifier starts") {
			THEN("Channels are enabled") {
				CHECK(tpa.rightEnabled());
				CHECK(tpa.leftEnabled());
			}
			THEN("Amplifier is ready") {
				CHECK(tpa.ready());
			}
			THEN("No short circuit on left and right amplifier") {
				CHECK(!tpa.rightShorted());
				CHECK(!tpa.leftShorted());
			}
			THEN("Circuit is not too hot") {
				CHECK(!tpa.tooHot());
			}
			THEN("Noise gate is enabled") {
				CHECK(tpa.noiseGateEnabled());
			}
			THEN("Attack time is 6.4ms/6dB") {
				CHECK(tpa.attackTime() == 5);
			}
			THEN("Release time is 1.81s/6dB") {
				CHECK(tpa.releaseTime() == 11);
			}
			THEN("Hold time is disabled") {
				CHECK(tpa.holdTime() == 0);
			}
			THEN("Fixed gain is 6dB") {
				CHECK(tpa.gain() == 6);
			}
			THEN("Limiter is enabled") {
				CHECK(tpa.limiterEnabled());
			}
			THEN("NoiseGate Treshold is 4mV RMS") {
				CHECK(tpa.noiseGateThreshold() == TPA2016_LIMITER_NOISEGATE::_4MV);
			}
			THEN("Output limiter level is 6.5dBV") {
				CHECK(tpa.limiterLevel() == 26);
			}
			THEN("Max gain is 30dB") {
				CHECK(tpa.maxGain() == 12);
			}
			THEN("Compression ratio is 1:4") {
				CHECK(tpa.compressionRatio() == TPA2016_COMPRESSION_RATIO::_1_4);
			}
		}
	}
}

// Test of registers modifications
SCENARIO("Toggle-bit features") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("The right and the left channel are turned off then on") {
			tpa.enableChannels(false, false);
			THEN("Their status first changes to off") {
				CHECK(!tpa.rightEnabled());
				CHECK(!tpa.leftEnabled());
				tpa.enableChannels(true, true);
				THEN("Their status then changes to on") {
					CHECK(tpa.rightEnabled());
					CHECK(tpa.leftEnabled());
				}
			}
		}

		WHEN("The right channel is turned on and the left channel is turned off") {
			tpa.enableChannels(true, false);
			THEN("The right channel is on and the left channel is off") {
				CHECK(tpa.rightEnabled());
				CHECK(!tpa.leftEnabled());
			}
		}

		WHEN("We request software shutdown") {
			tpa.softwareShutdown(true);
			THEN("Amplifier is not ready anymore") {
				CHECK(!tpa.ready());
				WHEN("Software shutdown is disabled") {
					tpa.softwareShutdown(false);
					THEN("Amplifier becomes ready") {
						CHECK(tpa.ready());
					}
				}
			}
		}
		WHEN("We reset short circuit status") {
			tpa.resetShort(true, true);
			THEN("Status remains unshorted") {
				CHECK(!tpa.rightShorted());
				CHECK(!tpa.leftShorted());
			}
		}

		WHEN("Noise gate is turned on then offs") {
			tpa.enableNoiseGate(true);
			THEN("Noise gate is first enabled") {
				CHECK(tpa.noiseGateEnabled());
				tpa.enableNoiseGate(false);
				THEN("Noise gate is then disabled") {
					CHECK(!tpa.noiseGateEnabled());
				}
			}
		}

		WHEN("Output limiter is turn off then on") {
			tpa.enableLimiter(false);
			THEN("Limiter is first disabled") {
				CHECK(!tpa.limiterEnabled());
				tpa.enableLimiter(true);
				THEN("Limiter is then enabled") {
					CHECK(tpa.limiterEnabled());
				}
			}
		}
	}
}

SCENARIO("Attack time modification") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		float releaseTime = 0.6576;
		float releaseTimeRound = 4;
		float releaseTimeInvalid = -5;
		float holdTime = 0.0274;
		float holdTimeRound = 5;
		float holdTimeInvalid = 2;
		WHEN("We set an attack time of 1.28ms/6dB") {
			tpa.setAttackTime(1.28);
			THEN("Attack time should report 1.28ms/6dB") {
				CHECK(tpa.attackTime() == 1.28f);
			}
		}
		WHEN("We set an attack time of 3ms/6dB (non TPA2016_ATTACK_STEP multiple)") {
			tpa.setAttackTime(3);
			THEN("Attack time should report 2.56ms/6dB") {
				CHECK(tpa.attackTime() == 2.56f);
			}
		}
		WHEN("We set an attack time of 200ms/6dB (invalid value)") {
			THEN("An out-of-range exception should be thrown") {
				CHECK_THROWS_AS(tpa.setAttackTime(200), std::out_of_range);
			}
		}
	}
}

SCENARIO("Release time modification") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("We set an release time of 0.1644sec/6dB") {
			tpa.setReleaseTime(0.1644);
			THEN("Release time should report 0.1644sec/6dB") {
				CHECK(tpa.releaseTime() == 0.1644f);
			}
		}
		WHEN("We set an release time of 4sec/6dB (non TPA2016_RELEASE_STEP multiple)") {
			tpa.setReleaseTime(4);
			THEN("Release time should report 3.9456sec/6dB") {
				CHECK(tpa.releaseTime() == 3.9456f);
			}
		}
		WHEN("We set an release time of 12sec/6dB (invalid value)") {
			THEN("An out-of-range exception should be thrown") {
				CHECK_THROWS_AS(tpa.setReleaseTime(12), std::out_of_range);
			}
		}
	}
}

SCENARIO("Hold time modification") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("We set an hold time of 0.0274sec/step") {
			tpa.setHoldTime(0.0274);
			THEN("Hold time should report 0.0274sec/step") {
				CHECK(tpa.holdTime() == 0.0274f);
			}
		}
		WHEN("We set an hold time of 0.05sec/step (non TPA2016_HOLD_STEP multiple)") {
			tpa.setHoldTime(0.05);
			THEN("Hold time should report 0.0411sec/step") {
				CHECK(tpa.holdTime() == 0.0411f);
			}
		}
		WHEN("We set an hold time of 2sec/step (invalid value)") {
			THEN("An out-of-range exception should be thrown") {
				CHECK_THROWS_AS(tpa.setHoldTime(2), std::out_of_range);
			}
		}
	}
}

SCENARIO("Maximum gain modification") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("We set the maximum gain to 18dB") {
			tpa.setMaxGain(18);
			THEN("Maximum gain should report 18dB") {
				CHECK(tpa.maxGain() == 18);
			}
		}
		WHEN("We set the maximum gain to 25.5dB") {
			tpa.setMaxGain(25.5);
			THEN("Maximum gain should report 25dB") {
				CHECK(tpa.maxGain() == 25);
			}
		}
		WHEN("We set the maximum gain to 31dB (invalid value)") {
			THEN("An out-of-range exception should be thrown") {
				CHECK_THROWS_AS(tpa.setMaxGain(31), std::out_of_range);
			}
		}
	}
}

SCENARIO("Fixed gain modification") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("We set the gain to -28dB") {
			tpa.setGain(-28);
			THEN("Gain should report -28dBdB") {
				CHECK(tpa.gain() == -28);
			}
		}
		WHEN("We set the gain to 25.5dB") {
			tpa.setGain(25.5);
			THEN("Gain should report 25dB") {
				CHECK(tpa.gain() == 25);
			}
		}
		WHEN("We set the gain to 31dB (invalid value)") {
			THEN("An out-of-range exception should be thrown") {
				CHECK_THROWS_AS(tpa.setGain(31), std::out_of_range);
			}
		}
	}
}

SCENARIO("Noise gate threshold") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("We set the noise gate threshold to 4mVrms") {
			tpa.setNoiseGateThreshold(TPA2016_LIMITER_NOISEGATE::_4MV);
			THEN("Reported noise gate threshold is 4mVrms") {
				CHECK(tpa.noiseGateThreshold() == TPA2016_LIMITER_NOISEGATE::_4MV);
			}
		}
	}
}

SCENARIO("Output limiter level") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("We set the limiter level to -6.5dBV") {
			tpa.setLimiterLevel(-6.5);
			THEN("Limiter level should report -6.5dBV") {
				CHECK(tpa.limiterLevel() == -6.5f);
			}
		}
		WHEN("We set the limiter level to 7.3dBV") {
			tpa.setLimiterLevel(7.3);
			THEN("Limiter level should report 7dBV") {
				CHECK(tpa.limiterLevel() == 7);
			}
		}
		WHEN("We set the limiter level to 10dBV") {
			THEN("An out-of-range exception should be thrown") {
				CHECK_THROWS_AS(tpa.setLimiterLevel(10), std::out_of_range);
			}
		}
	}
}

SCENARIO("Compression ratio") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("We set the compression ratio to 1:1 (off)") {
			tpa.setCompressionRatio(TPA2016_COMPRESSION_RATIO::_1_1);
			THEN("Reporter compression ratio is 1:1") {
				CHECK(tpa.compressionRatio() == TPA2016_COMPRESSION_RATIO::_1_1);
			}
		}
	}
}
