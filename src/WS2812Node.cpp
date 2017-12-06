/*
 * WS2812Node.cpp
 *
 *  Created on: 19.09.2017
 *      Author: ian
 */

#include <WS2812Node.h>
#include <LoggerNode.h>

#define LED_COUNT 24
#define LED_PIN   12

// vVv - initialize static variables - vVv

HomieSetting<long> WS2812Node::wsPin("WS2812N_Pin", "Output pin for WS2812 strip");
HomieSetting<long> WS2812Node::wsNumber("WS2812N_Count", "Count of connected WS2812 LED");
//HomieSetting<bool> WS2812Node::ws800kHz("WS2812N_800k", "Enable 800kHz Mode");
bool WS2812Node::settingsInitialized(false);

// ^^^ end of static part ^^^

WS2812Node::WS2812Node(const char* name, int8_t pin) :
		HomieNode(name, "WS-LED-Strip"),
		customPin(pin),
		ws2812fx(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800) {
	if (!settingsInitialized) {
		settingsInitialized = true;
		wsPin.setDefaultValue(LED_PIN).setValidator([] (long candidate) {
			return (candidate >= 0 && candidate < 16);
		});
		wsNumber.setDefaultValue(LED_COUNT).setValidator([] (long candidate) {
			return (candidate > 0 && candidate < 255);
		});
	//	ws800kHz.setDefaultValue(true);
	}
	advertise("mode").settable();
	advertise("speed").settable();
	advertise("brightness").settable();
}

void WS2812Node::setup() {
	  ws2812fx.setPin(customPin==-1 ? wsPin.get(): customPin);
	  ws2812fx.setLength(wsNumber.get());
	  ws2812fx.init();
	  ws2812fx.setBrightness(BRIGHTNESS_MAX);
	  ws2812fx.setSpeed(100);
	  ws2812fx.setMode(FX_MODE_BREATH);
	  ws2812fx.start();
}

void WS2812Node::loop() {
	ws2812fx.service();
}

void WS2812Node::onReadyToOperate() {
	ws2812fx.setMode(FX_MODE_STROBE_RAINBOW);
	setProperty("mode").send(ws2812fx.getModeName(ws2812fx.getMode()));
}

bool WS2812Node::handleInput(const String& property, const HomieRange& range, const String& value) {
	Serial.printf("handleInput: %s, %s", property.c_str(), value.c_str());
	if (property.equals("mode")) {
		uint8_t new_mode = 0;
		if (value.equals("next")) {
			new_mode = ws2812fx.getMode() + 1;
		} else if (value.equals("prev")) {
			new_mode = ws2812fx.getMode() - 1;;
		} else {
		    new_mode = value.toInt();
		    if (new_mode==0) return false; // TODO: Log error
		}

		if (new_mode >= MODE_COUNT) new_mode = 1;
		if (new_mode < 1) new_mode = MODE_COUNT-1;
		Serial.printf("New mode: %x", new_mode); // TODO: Log success
		ws2812fx.setMode(new_mode);
		setProperty("mode").send(ws2812fx.getModeName(ws2812fx.getMode()));
		switch (ws2812fx.getMode()) {
			case 2: // Breath mode
				LN.log("WS2812::handleInput", LoggerNode::INFO, "Breath mode");
				ws2812fx.setColor(255,200,9);
				ws2812fx.setSpeed(200);
				break;
			case 45:
			case 46:  // Fire flicker
			case 47:  //    (intense)
			case 48:
				LN.log("WS2812::handleInput", LoggerNode::INFO, "Fire flicker mode");
				//ws2812fx.setColor(255, 69,3);
				ws2812fx.setColor(255, 120,3);
				ws2812fx.setSpeed(210);
				break;
			default: // nothing to do
				break;
		}
		return true;
	}
	if (property.equals("brightness")) {
		uint8_t new_brightness = value.toInt();
		if ((new_brightness < 0) || (new_brightness > 100)) {
			//TODO: Log ERROR
			return false;
		}
		ws2812fx.setBrightness(new_brightness);
		setProperty("brightness").send(String(ws2812fx.getBrightness()));

	}
	if (property.equals("speed")) {
		uint8_t new_speed = value.toInt();
		if (new_speed < 10) {
			LN.logf(__PRETTY_FUNCTION__, LoggerNode::ERROR, "Cannot set new speed value [%s]", value.c_str()); //TODO: Log ERROR
			return false;
		}
		ws2812fx.setSpeed(new_speed);
		setProperty("speed").send(String(ws2812fx.getSpeed()));
	}
	return false;
}
