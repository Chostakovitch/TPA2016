#include <catch.hpp>
#include "../I2C_TPA2016.h"

SCENARIO("Channels can be turned on and off") {
	GIVEN("An I2C connection on bus 1") {
		I2C_TPA2016 tpa(1);
		WHEN("The right and the left channel are turned off") {
			tpa.enableChannels(false, false);
			THEN("Their status change to off") {
				REQUIRE(!tpa.rightEnabled());
				REQUIRE(!tpa.leftEnabled());
			}
		}
		WHEN("The right and the left channel if turned on") {
			tpa.enableChannels(true, true);
			THEN("Their status change to on") {
				REQUIRE(tpa.rightEnabled());
				REQUIRE(tpa.leftEnabled());
			}
		}
	}
}