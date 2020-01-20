/*
 * WS2812Node.h
 *
 *  Created on: 19.09.2017
 *      Author: ian
 */

#ifndef SRC_WS2812NODE_H_
#define SRC_WS2812NODE_H_

#include <WS2812FX.h>
#include <HomieNode.hpp>

class WS2812Node: public HomieNode {
public:
	//WS2812Node(const char* name, uint8_t _mode = 0, neoPixelType type = NEO_GBR + NEO_KHZ800, int8_t pin=-1, int16_t count=1);
	WS2812Node(const char* id, const char* name, uint8_t _mode = 0, neoPixelType type = NEO_GBR + NEO_KHZ800, int8_t pin=-1, int16_t count=1);

	enum RGB_MAP {
		R = 0, G, B, W, RGB_MAP_SIZE
	};

protected:
	virtual void setup() override;
	virtual void loop() override;
	virtual void onReadyToOperate() override;
	virtual bool handleInput(const HomieRange& range, const String& property, const String& value)override;


private:
	// HomieNode
	static HomieSetting<long> wsPin;
	static HomieSetting<long> wsNumber;
	//static HomieSetting<bool> ws800kHz;
	static bool settingsInitialized;

	int8_t customPin;
	int16_t customCount;

	WS2812FX ws2812fx;
	bool dirtBright, dirtMode, dirtSpeed, dirtColor[3], dirtWhite[3];
	uint8_t runtimeBrightness, runtimeMode;
	uint16_t runtimeSpeed;
	 // 3 colors as defined in WS2812FX.h NUMCOLORS - don't use the macro definition here, because number is hardcoded in MQTT Homie advertisment and it's handleInput()
	uint8_t color[3][RGB_MAP_SIZE];
};

#endif /* SRC_WS2812NODE_H_ */
