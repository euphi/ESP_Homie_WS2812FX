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
	WS2812Node(const char* name, uint8_t _mode = 0,   neoPixelType type = NEO_GBR + NEO_KHZ800, int8_t pin=-1, int16_t count=1);

protected:
	virtual void setup() override;
	virtual void loop() override;
	virtual void onReadyToOperate() override;
	virtual bool handleInput(const String& property, const HomieRange& range, const String& value) override;

private:
	// HomieNode
	static HomieSetting<long> wsPin;
	static HomieSetting<long> wsNumber;
	//static HomieSetting<bool> ws800kHz;
	static bool settingsInitialized;

	int8_t customPin;
	int16_t customCount;


	WS2812FX ws2812fx;
};

#endif /* SRC_WS2812NODE_H_ */
