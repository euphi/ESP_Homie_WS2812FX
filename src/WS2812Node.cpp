/*
 * WS2812Node.cpp
 *
 *  Created on: 19.09.2017
 *      Author: ian
 */

#include <WS2812Node.h>

#define LED_COUNT 24
#define LED_PIN   12

WS2812Node::WS2812Node() :
		HomieNode("WS2812FX", "WS-LED-Strip"),
		ws2812fx(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800) {
	advertise("mode").settable();
	advertise("speed").settable();
	advertise("brightness").settable();
}

void WS2812Node::setup() {
	  ws2812fx.init();
	  ws2812fx.setBrightness(100);
	  ws2812fx.setSpeed(100);
	  mode = FX_MODE_BREATH;
	  ws2812fx.setMode(mode);
	  ws2812fx.start();
}

void WS2812Node::loop() {
	ws2812fx.service();
}

void WS2812Node::onReadyToOperate() {
	setProperty("mode").send(ws2812fx.getModeName(mode));
	mode = FX_MODE_STROBE_RAINBOW;
	ws2812fx.setMode(mode);
	setProperty("mode").send(ws2812fx.getModeName(mode));
}

bool WS2812Node::handleInput(const String& property, const HomieRange& range, const String& value) {
	Serial.printf("handleInput: %s, %s", property.c_str(), value.c_str());
	if (property.equals("mode")) {
		if (value.equals("next")) {
			mode++;
		}
		if (value.equals("prev")) {
			mode--;
		}
		if (mode >= MODE_COUNT) mode = 1;
		if (mode < 1) mode = MODE_COUNT-1;
		Serial.printf("New mode: %x", mode);
	}
	ws2812fx.setMode(mode);
	setProperty("mode").send(ws2812fx.getModeName(mode));
	return true;
}
