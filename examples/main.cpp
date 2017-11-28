#include <Arduino.h>
#include <Homie.h>
#include <WS2812Node.h>
#include <LoggerNode.h>

#define FW_NAME "homie-wx2812fx"
#define FW_VERSION "0.0.3"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

WS2812Node ws2812Node;

void setup() {
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.disableLedFeedback();
  Homie.disableResetTrigger();
  Homie.setLoggingPrinter(&Serial);
  Serial.begin(74880);
  Serial.println("Start");
  Serial.flush();
  Homie.setup();
}

void loop() {
	Homie.loop();
}
