#include <catch.hpp>
#include "../I2C_TPA2016.h"

// Test of default values
SCENARIO("Normal values of the amplifier") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("The amplifier starts") {
			THEN("Channels are enabled") {
				REQUIRE(tpa.rightEnabled());
				REQUIRE(tpa.leftEnabled());
			}
			THEN("Amplifier is ready") {
				REQUIRE(tpa.ready());
			}
			THEN("No short circuit on left and right amplifier") {
				REQUIRE(!tpa.rightShorted());
				REQUIRE(!tpa.leftShorted());
			}
			THEN("Circuit is not too hot") {
				REQUIRE(!tpa.tooHot());
			}
			THEN("Noise gate is enabled") {
				REQUIRE(tpa.noiseGateEnabled());
			}
			THEN("Attack time is 6.4ms/6dB") {
				REQUIRE(tpa.attackTime() == 5);
			}
			THEN("Release time is 1.81s/6dB") {
				REQUIRE(tpa.releaseTime() == 11);
			}
			THEN("Hold time is disabled") {
				REQUIRE(tpa.holdTime() == 0);
			}
			THEN("Fixed gain is 6dB") {
				REQUIRE(tpa.gain() == 6);
			}
			THEN("Limiter is enabled") {
				REQUIRE(tpa.limiterEnabled());
			}
			THEN("NoiseGate Treshold is 4mV RMS") {
				REQUIRE(tpa.noiseGateThreshold() == 1);
			}
			THEN("Output limiter level is 6.5dBV") {
				REQUIRE(tpa.limiterLevel() == 26);
			}
			THEN("Max gain is 30dB") {
				REQUIRE(tpa.maxGain() == 12);
			}
			THEN("Compression ratio is 4:1") {
				REQUIRE(tpa.compressionRatio() == TPA2016_AGC_4);
			}
		}
	}
}

// Test of registers modifications
SCENARIO("Modification of the registers") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("The right and the left channel are turned off then on") {
			tpa.enableChannels(false, false);
			THEN("Their status change to off") {
				REQUIRE(!tpa.rightEnabled());
				REQUIRE(!tpa.leftEnabled());
			}
			tpa.enableChannels(true, true);
			THEN("Their status change to on") {
				REQUIRE(tpa.rightEnabled());
				REQUIRE(tpa.leftEnabled());
			}
		}

		WHEN("The right channel is turned on and the left channel is turned off") {
			tpa.enableChannels(true, false);
			THEN("The right channel is on and the left channel is off") {
				REQUIRE(tpa.rightEnabled());
				REQUIRE(!tpa.leftEnabled());
			}
		}
		WHEN("We request software shutdown") {
			tpa.softwareShutdown(true);
			THEN("Amplifier is not ready anymore") {
				REQUIRE(!tpa.ready());
				WHEN("Software shutdown is disabled") {
					tpa.softwareShutdown(false);
					THEN("Amplifier becomes ready") {
						REQUIRE(tpa.ready());
					}
				}
			}
		}
		WHEN("We reset short circuit status") {
			tpa.resetShort(true, true);
			THEN("Status remains unshorted") {
				REQUIRE(!tpa.rightShorted());
				REQUIRE(!tpa.leftShorted());
			}
		}

		WHEN("Noise gate is turned on") {
			tpa.enableNoiseGate(true);
			THEN("Noise gate is enabled") {
				REQUIRE(tpa.noiseGateEnabled());
				WHEN("Noise gate is turned off") {
					tpa.enableNoiseGate(false);
					THEN("Noise gate is disabled") {
						REQUIRE(!tpa.noiseGateEnabled());
					}
				}
			}
		}

	}
}
