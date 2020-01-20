#include <Arduino.h>
#include <Homie.h>
#include <WS2812Node.h>
#include <LoggerNode.h>

#define FW_NAME "homie-wx2812fx"
#define FW_VERSION "0.3.12"

// first WS2812 Strip with PIN loaded from config.json
//WS2812Node ws2812Node("Feuer1",  FX_MODE_FIRE_FLICKER_SOFT,NEO_GRB + NEO_KHZ800, -1, 8);

// second WS2812 Strip with custom pin (2 == D4 (Wemos))
//WS2812Node ws2812Node2("Feuer2", FX_MODE_FIREWORKS_RANDOM, NEO_GRB + NEO_KHZ800, 2, 8);

// third WS2812 Strip with custom pin (4 == D2 (Wemos))
WS2812Node ws2812Node3("strip", "1st Strip", FX_MODE_RAINBOW_CYCLE, NEO_GRBW + NEO_KHZ800, 4, 429);
LoggerNode LN;

void setup() {
  Serial.begin(74880);
  Serial.println("Start");
  Serial.flush();

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.disableLedFeedback();
  Homie.disableResetTrigger();
  Homie.setLoggingPrinter(&Serial);
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
