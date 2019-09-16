#include <Arduino.h>
#include <Homie.h>
#include <WS2812Node.h>
#include <LoggerNode.h>

#define FW_NAME "homie-wx2812fx"
#define FW_VERSION "0.3.11"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

// first WS2812 Strip with PIN loaded from config.json
//WS2812Node ws2812Node("Feuer1",  FX_MODE_FIRE_FLICKER_SOFT,NEO_GRB + NEO_KHZ800, -1, 8);

// second WS2812 Strip with custom pin (2 == D4 (Wemos))
//WS2812Node ws2812Node2("Feuer2", FX_MODE_FIREWORKS_RANDOM, NEO_GRB + NEO_KHZ800, 2, 8);

// third WS2812 Strip with custom pin (4 == D2 (Wemos))
WS2812Node ws2812Node3("strip", "1st Strip", FX_MODE_CIRCUS_COMBUSTUS, NEO_GRBW + NEO_KHZ800, 4, 429);
LoggerNode LN;

void setup() {
  Serial.begin(74880);
  Serial.println("Start");
  Serial.flush();
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.disableLedFeedback();
  Homie.disableResetTrigger();
  Homie.setLoggingPrinter(&Serial);
  Serial.println("Setup");
  Homie.setup();
}

void loop() {
	static uint32_t last = 0;
	Homie.loop();
	if ((last + 5000) < millis()) {
		last = millis();
		LN.logf(__PRETTY_FUNCTION__, LoggerNode::DEBUG, "Free heap: %d bytes", ESP.getFreeHeap());
	}

}
