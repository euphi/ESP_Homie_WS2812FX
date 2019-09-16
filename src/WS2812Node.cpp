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

//2812Node::WS2812Node(const char* name, uint8_t _mode, neoPixelType type, int8_t pin, int16_t count):
WS2812Node::WS2812Node(const char* id, const char* name, uint8_t _mode, neoPixelType type, int8_t pin, int16_t count):
		HomieNode(id, name, "LED_FX"),
		customPin(pin),
		customCount(count),
		ws2812fx(LED_COUNT, LED_PIN, type),
		dirtBrightness(true), dirtMode(true), dirtSpeed(true),
		runtimeBrightness(BRIGHTNESS_MAX), runtimeMode(_mode), runtimeSpeed(210) {
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
	ws2812fx.setMode(_mode);
	advertise("mode").settable();
	advertise("speed").settable();
	advertise("brightness").settable();
}

void WS2812Node::setup() {
	  ws2812fx.setPin(customPin==-1 ? wsPin.get(): customPin);
	  ws2812fx.setLength(customCount==-1 ? wsNumber.get() : customCount);
	  ws2812fx.init();
	  pinMode(ws2812fx.getPin(), OUTPUT);
      //ws2812fx.setColor(255, 160,5);
	  ws2812fx.start();
      ws2812fx.service();
      //setRunLoopDisconnected(true);
}

void WS2812Node::loop() {
	static uint_fast16_t count = 0;
	if (count==0) Serial.print("Loop");
	if ((count++ % 5000) == 0)
	{
		Serial.print(".");
	}
	if (dirtBrightness) {
		ws2812fx.setBrightness(runtimeBrightness);
		dirtBrightness = false;
		setProperty("brightness").send(String(ws2812fx.getBrightness()));
	}
	if (dirtMode) {
		ws2812fx.setMode(runtimeMode);
		dirtMode = false;
		setProperty("mode").send(ws2812fx.getModeName(ws2812fx.getMode()));
	}
	if (dirtSpeed) {
		ws2812fx.setSpeed(runtimeSpeed);
		dirtSpeed = false;
		uint8_t speed_per = 100-((ws2812fx.getSpeed()-SPEED_MIN) * 100 / (SPEED_MAX-SPEED_MIN));
		setProperty("speed").send(String(speed_per));
	}
	ws2812fx.service();
}

void WS2812Node::onReadyToOperate() {
	setProperty("mode").send(ws2812fx.getModeName(ws2812fx.getMode()));
}

bool WS2812Node::handleInput(const HomieRange& range, const String& property, const String& value) {
	//LN.logf("WS2812::handleInput", LoggerNode::DEBUG, "new input: %s, %s", property.c_str(), value.c_str());
	if (property.equals("mode")) {
		uint8_t new_mode = 0;
		if (value.equals("next")) {
			new_mode = ws2812fx.getMode() + 1;
		} else if (value.equals("prev")) {
			new_mode = ws2812fx.getMode() - 1;;
		} else {
		    new_mode = value.toInt();
		    if (new_mode==0) {
		    	LN.log("WS2812::handleInput", LoggerNode::WARNING, "Invalid mode received");
		    	return false;
		    }
		}
		if (new_mode >= MODE_COUNT) new_mode = 1;
		if (new_mode < 1) new_mode = MODE_COUNT-1;
//		LN.logf("WS2812::handleInput", LoggerNode::DEBUG, "New mode: %x", new_mode);
		runtimeMode = new_mode;
		dirtMode = true;
//		switch (ws2812fx.getMode()) {
//			case FX_MODE_BREATH: // Breath mode
//				LN.log("WS2812::handleInput", LoggerNode::INFO, "Breath mode");
////				ws2812fx.setColor(255,200,9);
////				ws2812fx.setSpeed(200);
//				break;
//			case FX_MODE_FIRE_FLICKER:
//			case FX_MODE_FIRE_FLICKER_SOFT:  	// Fire flicker
//			case FX_MODE_FIRE_FLICKER_INTENSE:  //    (intense)
//				LN.log("WS2812::handleInput", LoggerNode::INFO, "Fire flicker mode");
//				//ws2812fx.setColor(255, 69,3);
//				//ws2812fx.setColor(255, 160,5);
//				//ws2812fx.setSpeed(210);
//				break;
//			default: // nothing to do
//				break;
//		}
		return true;
	}
	if (property.equals("brightness")) {
		uint8_t new_brightness = value.toInt();
		if ((new_brightness < 0) || (new_brightness > 100)) {
	    	LN.log("WS2812::handleInput", LoggerNode::WARNING, "Invalid brightness received");
			return false;
		}
		runtimeBrightness = new_brightness;
		dirtBrightness = true;

	}
	if (property.equals("speed")) {
		int8_t new_speed = value.toInt();
		if (new_speed < 0 || new_speed > 100) {
			LN.logf(__PRETTY_FUNCTION__, LoggerNode::WARNING, "Invalid speed [%s] received", value.c_str());
			return false;
		}
		runtimeSpeed = (100-new_speed) * ((SPEED_MAX-SPEED_MIN)/100) + SPEED_MIN;
		LN.logf("WS2812Node::Input_Speed", LoggerNode::DEBUG, "New speed %d [%d]", runtimeSpeed, new_speed);
		dirtSpeed = true;
	}
	//TODO: Color
	return false;
}
