/*
 * WS2812Node.cpp
 *
 *  Created on: 19.09.2017
 *      Author: ian
 */

#include <WS2812Node.h>
#include <LoggerNode.h>
#include <Homie.hpp>

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
		usewhite(!(((type >> 4) & 3) == ((type >> 6 & 3)))),  // For Pixeltypes without white, the WHITE offset is same as RED offset (see Adafruit_NeoPixel.h:79)
		dirtBright(true), dirtMode(true), dirtSpeed(true),
		dirtColor{true, true, true},
		dirtWhite{usewhite, usewhite, usewhite},
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
	advertise("mode").setDatatype("enum").setFormat("0:59, next, prev").setName("FX Mode").settable();
	advertise("speed").setDatatype("integer").setUnit("%").setName("FX Speed").settable();
	advertise("brightness").setDatatype("integer").setUnit("%").setName("FX Brightness").settable();
	advertise("color").setDatatype("color").setFormat("rgb").setName("FX Primary Color").settable();
	advertise("color2").setDatatype("color").setFormat("rgb").setName("FX Secondary Color").settable();
	advertise("color3").setDatatype("color").setFormat("rgb").setName("FX Tertiary Color").settable();
	if (usewhite) {
		advertise("white").setDatatype("integer").setUnit("%").setFormat("0:100").setName("FX Primary White").settable();
		advertise("white2").setDatatype("integer").setUnit("%").setFormat("0:100").setName("FX Secondary White").settable();
		advertise("white3").setDatatype("integer").setUnit("%").setFormat("0:100").setName("FX Tertiary White").settable();
	}
}

void WS2812Node::setup() {
	  ws2812fx.setPin(customPin==-1 ? wsPin.get(): customPin);
	  ws2812fx.setLength(customCount==-1 ? wsNumber.get() : customCount);
	  ws2812fx.init();
	  pinMode(ws2812fx.getPin(), OUTPUT);
	  ws2812fx.start();
      ws2812fx.service();
      setRunLoopDisconnected(true);
}

void WS2812Node::loop() {
	static uint_fast16_t count = 0;
	if (count==0) Serial.print("Loop");
	if ((count++ % 4096) == 0)
	{
		Serial.print(".");
	}
	bool connected = Homie.isConnected();
	if (dirtBright) {
		ws2812fx.setBrightness(runtimeBrightness);
		dirtBright = false;
		if (connected) setProperty("brightness").send(String(ws2812fx.getBrightness()));
	}
	if (dirtMode) {
		ws2812fx.setMode(runtimeMode);
		dirtMode = false;
		if (connected) setProperty("mode").send(ws2812fx.getModeName(ws2812fx.getMode()));
	}
	if (dirtSpeed) {
		ws2812fx.setSpeed(runtimeSpeed);
		dirtSpeed = false;
		uint8_t speed_per = 100-((ws2812fx.getSpeed()-SPEED_MIN) * 100 / (SPEED_MAX-SPEED_MIN));
		if (connected) setProperty("speed").send(String(speed_per));
	}
	for (uint_fast8_t i = 0; i < 3; i++) {
		if (dirtColor[i] || dirtWhite[i]) {
			uint32_t ca[3];
			for (uint_fast8_t j = 0; j < 3; j++) ca[j] = (((uint32_t) color[j][W] << 24) | ((uint32_t) color[j][R] << 16) | ((uint32_t) color[j][G] << 8) | ((uint32_t) color[j][B]));
			ws2812fx.setColors(0, ca);
			if (connected) {
				LN.logf("WS2812Node::loop", LoggerNode::DEBUG, "WS2812FX Colors: %x, %x, %x", ca[0], ca[1], ca[2]);
				if (dirtColor[i]) {
					String rgb_str(color[i][R]);
					String prop_str("color");
					rgb_str += ", ";
					rgb_str += color[i][G];
					rgb_str += ",";
					rgb_str += color[i][B];
					prop_str += (i+1);
					setProperty(prop_str).send(rgb_str);
					dirtColor[i] = false;
				} else if (dirtWhite[i]) {
					String prop_str("white");
					prop_str += (i+1);
					setProperty(prop_str).send(String(color[i][W]));
					dirtWhite[i] = false;
				}
			}
		}
	}
	ws2812fx.service();
	yield();
}

void WS2812Node::onReadyToOperate() {
	dirtBright = true;
	dirtMode   = true;
	dirtSpeed  = true;
	dirtColor[0] = true;
	dirtColor[1] = true;
	dirtColor[2] = true;
	dirtWhite[0] = true;
	dirtWhite[1] = true;
	dirtWhite[2] = true;
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
		runtimeMode = new_mode;
		dirtMode = true;
		return true;
	}
	if (property.equals("brightness")) {
		uint16_t new_brightness = value.toInt();
		if ((new_brightness < 0) || (new_brightness > 100)) {
	    	LN.log("WS2812::handleInput", LoggerNode::WARNING, "Invalid brightness received");
			return false;
		}
		runtimeBrightness = new_brightness*255/100;
		dirtBright = true;
		return true;
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
		return true;
	}
	if (property.startsWith("color")) {
		uint_fast8_t no = 0;
		if (property.endsWith("2")) no = 1;
		if (property.endsWith("3")) no = 2;
		uint_fast8_t r_end = value.indexOf(',');
		uint_fast8_t g_end = value.lastIndexOf(',');
		int new_r = value.substring(0, r_end).toInt() * 100 / 255;
		int new_g = value.substring(r_end+1, g_end).toInt() * 100 / 255;
		int new_b = value.substring(g_end+1).toInt() * 100 / 255;
		if (new_r >= 0 && new_r <= 100) color[no][R] = new_r;
		if (new_g >= 0 && new_g <= 100) color[no][G] = new_g;
		if (new_b >= 0 && new_b <= 100) color[no][B] = new_b;
		dirtColor[no] = true;
		LN.logf("WS2812Node::Color", LoggerNode::DEBUG, "New color%d dimmer value %d, %d, %d", no+1, new_r, new_g, new_b);


	}
	if (property.startsWith("white")) {
		uint_fast8_t no = 0;
		if (property.endsWith("2")) no = 1;
		if (property.endsWith("3")) no = 2;
		int value_int = value.toInt();
		color[no][W] = value_int;
		dirtWhite[no] = true;
		LN.logf("WS2812Node::White", LoggerNode::DEBUG, "New white%d dimmer value %d", no+1, value_int);

	}
	return false;
}
