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
	WS2812Node();

protected:
	virtual void setup() override;
	virtual void loop() override;
	virtual void onReadyToOperate() override;
	virtual bool handleInput(const String& property, const HomieRange& range, const String& value) override;

private:
	// HomieNode
	static HomieSetting<long> wsPin;
	static HomieSetting<long> wsNumber;
	static HomieSetting<bool> ws800kHz;

	WS2812FX ws2812fx;
	uint8_t mode = 1;

};

#endif /* SRC_WS2812NODE_H_ */